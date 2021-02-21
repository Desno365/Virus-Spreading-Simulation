#include <stdlib.h> 
#include <sstream>
#include <stdio.h>

#include "area.h"

Area::Area(int col,int row, int id, float infectionDistance, int deltaTime, int my_processor_rank):id(id),col(col),row(row),infectionDistance(infectionDistance),deltaTime(deltaTime),my_processor_rank(my_processor_rank){}

Area::~Area(){
    this->resetState();
}

void Area::addUser(shared_ptr<User> user){
    this->usersInArea.insert({user->getId(), user});
    sortUser(user);
}

void Area::addUsers(vector<shared_ptr<User>> users){
    for(shared_ptr<User> user : users){
        addUser(user);
    }
}

void Area::getNewUserFromRemoteLocation(vector<shared_ptr<User>> *newUsers){
    vector<int> selectedUsers;
    for(int i=0; i<(int)newUsers->size() ; i++){
        shared_ptr<User> user = newUsers->at(i);
        float x  = user->pos->getX();
        float y  = user->pos->getY();
        //Check if the user is inside this area, if is the case take it.
        if(x>=lowerX && x<=higherX && y>=lowerY && y<=higherY){
            addUser(user);
            selectedUsers.push_back(user->getId());
        }
    }
    //Remove the selected user from the input vector.
    for(int i : selectedUsers){
        for(auto newUser = newUsers->begin(); newUser != newUsers->end(); ++newUser)
            if(newUser->get()->getId() == i){
                newUsers->erase(newUsers->begin()+i);
                break;
            }
    }
}

void Area::setNeighborArea(shared_ptr<NeighborArea> neighborArea,Direction direction){
    this->neighborAreas.insert({direction,neighborArea});
}

tuple<int,int> Area::getCoordinate(){
    return {this->col,this->row};
}

int Area::getCol(){
    return this->col;
}

int Area::getRow(){
    return this->row;
}

void Area::resetState(){
    mapOutOfAreasToUsersRemote.clear();
    mapAreasToUsersRemote.clear();
    outOfAreaUsers.clear();
    mapOutOfAreasToUsersLocal.clear();
    mapAreasToUsersLocal.clear();
    usersNearbyLocal.clear();
    usersNearbyRemote.clear();
    userNearInternalBorders.clear();
}

void Area::updateUserPositions(){
    this->resetState();
    for ( auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it  )
    {
        it->second->updateUserPosition(deltaTime);
        this->sortUser(it->second);
    } 
}

void Area::updateUserInfectionStatus(){
    map<int,bool> usersNearInfected;
    for(auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it){
        shared_ptr<User> user = it->second;
        bool isNearInfectedUser = false;
        //If is not the case that the user is infected and not immune, we see if it has infected user nearby.
        if(!user->isInfected() && !user->isImmune()){
            //For everyone checks if near him there exist a user that is infected.
            for(auto entryUsersInArea = this->usersInArea.begin(); entryUsersInArea != this->usersInArea.end() && !isNearInfectedUser ; ++entryUsersInArea){
                shared_ptr<User> otherUser = entryUsersInArea->second;
                if(otherUser->isInfected() && user->isNear(otherUser->pos->getX(),otherUser->pos->getY(),infectionDistance)){
                    isNearInfectedUser = true;
                }
            }
            if(!isNearInfectedUser && userNearInternalBorders.count(user->getId())>0){//The user is near a border and it is not already near an infected user we have to check also for the neraby users.
                for(auto entryUsersNearbyLocal = this->usersNearbyLocal.begin(); entryUsersNearbyLocal != this->usersNearbyLocal.end() && !isNearInfectedUser ; ++entryUsersNearbyLocal){
                    shared_ptr<User> otherUser = entryUsersNearbyLocal->second;
                    //Here is not necessary to check if the user is infected since the map contains only the infected users from other areas.
                    if(user->isNear(otherUser->pos->getX(),otherUser->pos->getY(),infectionDistance)){
                        isNearInfectedUser = true;
                    }
                }
                if(!isNearInfectedUser){//Check if the previous for has already found a nearby infected user.
                    //Check now that the user is not near an infected user from a remote source.
                    //Here is not necessary to check if the user is infected since the map contains only the infected users from other areas.
                    for(auto entryUsersNearbyRemote = this->usersNearbyRemote.begin(); entryUsersNearbyRemote != this->usersNearbyRemote.end() && !isNearInfectedUser ; ++entryUsersNearbyRemote){
                        shared_ptr<user_struct> otherUser = entryUsersNearbyRemote->second;
                        if(user->isNear(otherUser->x,otherUser->y,infectionDistance)){
                            isNearInfectedUser = true;
                        }
                    }
                }
            }
        }
        //Save the boolean inside the map for the late update of the status.
        usersNearInfected.insert({user->getId(),isNearInfectedUser});
    } 

    //Now updates all the users at once.
    for(auto it = usersInArea.begin(); it != usersInArea.end(); ++it){
        shared_ptr<User> user = it->second;
        if(usersNearInfected.count(it->first)>0) user->updateUserInfectionState(usersNearInfected.at(it->first),deltaTime);
        else cout << "Error in area.cpp in doing map::at";
    }
}

tuple<float,float> Area::getRadomCoordinates(){
    return { generateRandomFloat(higherX,lowerX), generateRandomFloat(higherY,lowerY)};
}

tuple<float,float> Area::getRadomDirection(){
    float rand1,rand2;
    while( (rand1 = generateRandomFloat(1,-1))==0.0 );
    while( (rand2 = generateRandomFloat(1,-1))==0.0 );
    float term = sqrt( pow(rand1,2) + pow(rand2,2) );
    return { rand1/term, rand2/term};
}

map<int,shared_ptr<vector<shared_ptr<User>>>> Area::getOutOfAreaUsersLocal(){
    return mapOutOfAreasToUsersLocal;
}

map<int,shared_ptr<vector<shared_ptr<user_struct>>>> Area::getOutOfAreaUsersRemote(){
    return mapOutOfAreasToUsersRemote;
}

map<int,shared_ptr<vector<shared_ptr<User>>>> Area::getNearBorderUsersLocal(){
    return mapAreasToUsersLocal;
}

map<int,shared_ptr<vector<shared_ptr<user_struct>>>> Area::getNearBorderUsersRemote(){
    return mapAreasToUsersRemote;
}

void Area::sortUser(shared_ptr<User> user){
    if(user->pos->getX()<=higherX && user->pos->getX()>=lowerX && user->pos->getY()<=higherY && user->pos->getY()>=lowerY){//The user is still inside the region
        if(user->pos->getX()>=(higherX-infectionDistance) || user->pos->getX()<=(lowerX+infectionDistance) || user->pos->getY()>=(higherY-infectionDistance) || user->pos->getY()<=(lowerY+infectionDistance))
            userNearInternalBorders.insert({ user->getId() , user });
    }else{//The user has gone too faraway
        outOfAreaUsers.push_back(user);
    }
}

void Area::setBoundaries(int lowerX, int lowerY, int higherX, int higherY){
    this->lowerX = lowerX;
    this->lowerY = lowerY;
    this->higherX = higherX; 
    this->higherY = higherY;
}

void Area::addNearbyUsersRemote(vector<shared_ptr<user_struct>> nearbyUsersRemote){
    for(shared_ptr<user_struct> nearbyUser:nearbyUsersRemote){
        if(nearbyUser->x<=(higherX+infectionDistance) && nearbyUser->x>=(lowerX-infectionDistance) && nearbyUser->y<=(higherY+infectionDistance) && nearbyUser->y>=(lowerY+infectionDistance))
            this->usersNearbyRemote.insert( { nearbyUser->id , nearbyUser } );
    }
}

void Area::addNearbyUsersLocal(vector<shared_ptr<User>> nearbyUsersLocal){
    for(shared_ptr<User> nearbyUser:nearbyUsersLocal){
        this->usersNearbyLocal.insert({ nearbyUser->getId() , nearbyUser });
    }
}

void Area::printActualState(FILE *ptr){
    int infectedUser,immuneUser;
    tie(infectedUser,immuneUser) = actuallyInfectedAndImmuneUser();
    ostringstream stream;
    stream << "Area ID: " << id << "\n"
            "   Actual population: " << usersInArea.size() << "\n" 
            "   Actually infected: " << infectedUser << "\n" 
            "   Actually immune: " << immuneUser << "\n";
    char * string = fromStringToCharName(stream.str());
    fprintf(ptr,"%s", string);
    free(string);
}

tuple<int,int> Area::actuallyInfectedAndImmuneUser(){
    int infectedUser = 0;
    int immuneUser = 0;
    for(auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it){
        shared_ptr<User> user = it->second;
        if(user->isInfected()) infectedUser++;
        else if(user->isImmune()) immuneUser++; 
    }
    return {infectedUser,immuneUser};
}


void Area::computeNearBorderUserMap(){
    //Check the various user based on the border to which they are near.
    //NOTE: we check only the users inside userNearInternalBorders since it has been updated after the recompute position.
    for(auto entriesUsersInternalNearBorder = userNearInternalBorders.begin(); entriesUsersInternalNearBorder != userNearInternalBorders.end(); ++entriesUsersInternalNearBorder){
        shared_ptr<User> user = entriesUsersInternalNearBorder->second;
        //Do this computation only if the user is infected.
        if(user->isInfected()){
            if(user->pos->getX() <= (lowerX + infectionDistance)){
                //Check West
                if(neighborAreas.count(West)>0)
                    addUserNear(user,neighborAreas.at(West));
                //Check North-West and also North
                if(user->pos->getY()>=(higherY-infectionDistance)){
                    //Check North-West
                    if(neighborAreas.count(NorthWest)>0)
                        addUserNear(user,neighborAreas.at(NorthWest));
                    //Check North
                    if(neighborAreas.count(North)>0)
                        addUserNear(user, neighborAreas.at(North));
                }else if(user->pos->getY()<=(lowerY+infectionDistance)){ //Check South and South-West
                    //Check South-West
                    if(neighborAreas.count(SouthWest)>0)
                        addUserNear(user,neighborAreas.at(SouthWest));
                    //Check South
                    if(neighborAreas.count(South)>0)
                        addUserNear(user, neighborAreas.at(South));
                }
            }else if(user->pos->getX()>=(higherX-infectionDistance)){
                //Check East
                if(neighborAreas.count(East)>0)
                    addUserNear(user,neighborAreas.at(East));
                //Check North-East and also North
                if(user->pos->getY()>=(higherY-infectionDistance)){
                    //Check North-East
                    if(neighborAreas.count(NorthEast)>0)
                        addUserNear(user,neighborAreas.at(NorthEast));
                    //Check North
                    if(neighborAreas.count(North)>0)
                        addUserNear(user, neighborAreas.at(North));
                }else if(user->pos->getY()<=(lowerY+infectionDistance)){ //Check South and South-East
                    //Check South-East
                    if(neighborAreas.count(SouthEast)>0)
                        addUserNear(user,neighborAreas.at(SouthEast));
                    //Check South
                    if(neighborAreas.count(South)>0)
                        addUserNear(user, neighborAreas.at(South));
                }     
            }else if(user->pos->getY()>=(higherX-infectionDistance)){
                //Since we have previously check this has to be sent only to north.
                //Check North
                if(neighborAreas.count(North)>0)
                    addUserNear(user, neighborAreas.at(North));
            }else if(user->pos->getY()<=(lowerY+infectionDistance)){
                //Since we have previously check this has to be sent only to south.
                //Check South
                if(neighborAreas.count(South)>0)
                    addUserNear(user, neighborAreas.at(South));
            }
        }
    }
}

//NOTE: locally the user in the list cannot be repeated. Instead we need to check for remote destination.
void Area::addUserNear(shared_ptr<User> user,  shared_ptr<NeighborArea> neighborArea){
    if(neighborArea->isLocal(my_processor_rank)){
        shared_ptr<vector<shared_ptr<User>>> users;
        if(mapAreasToUsersLocal.count(neighborArea->getID())) users = mapAreasToUsersLocal.at(neighborArea->getID());
        else{
            //No entries is already present so we insert such a vector.
            users = make_shared<vector<shared_ptr<User>>>();
            mapAreasToUsersLocal.insert({neighborArea->getID(), users});
        }
        users->push_back(user);
    }else{
        //We have to send the information remotely.
        shared_ptr<vector<shared_ptr<user_struct>>> users_t;
        if(mapAreasToUsersRemote.count(neighborArea->getOtherProcessorRank())) users_t = mapAreasToUsersRemote.at(neighborArea->getOtherProcessorRank());
        else{
            //No entries is already present so we insert such a vector.
            users_t = make_shared<vector<shared_ptr<user_struct>>>();
            mapAreasToUsersRemote.insert({neighborArea->getOtherProcessorRank(), users_t});
        }
        bool isAlreadyPresent = false;
        for(auto it = users_t->begin(); it != users_t->end() && !isAlreadyPresent; ++it){
            shared_ptr<user_struct> user_t = *it;
            if(user_t->id==user->getId())
                isAlreadyPresent = true;
        }
        if(!isAlreadyPresent){
            //If no user_struct with the same id is present than we can insert a new user_struct.
            shared_ptr<user_struct> user_t = user->getStruct();
            users_t->push_back(user_t);
        }
    }
}

//NOTE: If the user has gone out of the global area, based on the direction he is move to the border and it changes direction
//based on the border.
//NOTE: It removes the user from the users in area map, if they have not reached a border.
void Area::computeOutOfAreaUserMap(){
    //Analyze the position of each user that has gone to far away.
    for(shared_ptr<User> user_ptr: outOfAreaUsers){
        Direction areaOutDirection;
        int borderCoordinates;
        //First check from which direction the user has gone out.
        if(user_ptr->pos->getY()>higherY){
            borderCoordinates = higherY;
            if(user_ptr->pos->getX()<lowerX) areaOutDirection = NorthWest;
            else if(user_ptr->pos->getX()>higherX) areaOutDirection = NorthEast;
            else areaOutDirection = North;
        } else if(user_ptr->pos->getY()<lowerY){
            borderCoordinates = lowerY;
            if(user_ptr->pos->getX()<lowerX) areaOutDirection = SouthWest;
            else if(user_ptr->pos->getX()>higherX) areaOutDirection = SouthEast;
            else areaOutDirection = South;
        }else if(user_ptr->pos->getX()<lowerX){
            borderCoordinates = lowerX;
            areaOutDirection = West;
        }else if(user_ptr->pos->getX()>higherX){
            borderCoordinates = higherX;
            areaOutDirection = East;
        }else{
            cout << "Error in Area " << id << " managed by processor " << my_processor_rank << " with user with (X,Y): (" << user_ptr->pos->getX() << "," << user_ptr->pos->getY() << ")\n";
            throw -1;
        }
        addUserOutOfArea(areaOutDirection,user_ptr,borderCoordinates);
    }
}

void Area::addUserOutOfArea(Direction direction, shared_ptr<User> user, float borderCoordinates){
    //Check if a neighorbArea exists
    if(neighborAreas.count(direction)>0){
        //Remove the user from the list of the user in area.
        usersInArea.erase(user->getId());
        //Get the correspondent neighbor area and see if it is local or remote.
        shared_ptr<NeighborArea> neighborArea = neighborAreas[direction];
        if(neighborArea->isLocal(my_processor_rank)){
            //The user changes area but it remains in the same processor,
            //so we populate the map mapOutOfAreasToUsersLocal.
            shared_ptr<vector<shared_ptr<User>>> users;
            if(mapOutOfAreasToUsersLocal.count(neighborArea->getID())){
                users = mapOutOfAreasToUsersLocal.at(neighborArea->getID());
            }else{
                users = make_shared<vector<shared_ptr<User>>>();
                mapOutOfAreasToUsersLocal.insert( {neighborArea->getID(), users});
            }
            users->push_back(user);
        }else{
            //The user changes area and goes to a different processor,
            //so we populate the map mapOutOfAreasToUsersRemote.
            shared_ptr<vector<shared_ptr<user_struct>>> users_t;
            if(mapOutOfAreasToUsersRemote.count(neighborArea->getOtherProcessorRank())){
                users_t = mapOutOfAreasToUsersRemote.at(neighborArea->getOtherProcessorRank());
            }else{
                users_t = make_shared<vector<shared_ptr<user_struct>>>();
                mapOutOfAreasToUsersRemote.insert( {neighborArea->getOtherProcessorRank(), users_t});
            }
            users_t->push_back(user->getStruct());
        }
    }else{
        //If the are isn't present than the user changes direction and remains in this area.
        //The user's coordinates are also updated to the nearest border.
        if(direction==NorthEast) user->pos->setCoordinates(higherX,higherY);
        else if(direction==NorthWest) user->pos->setCoordinates(lowerX,higherY);
        else if(direction==SouthEast) user->pos->setCoordinates(higherX,lowerY);
        else if(direction==SouthWest) user->pos->setCoordinates(lowerX,lowerY);
        else{
            //Set the user coordinates to be on the nearest border, by doing the intersection between plane.
            int coefX,coefY;
            tie(coefX,coefY) = fromDirectionToVector(direction);
            if(borderCoordinates==0){
                user->goBackToIntersection(coefX, coefY,0);
            }else
                user->goBackToIntersection(coefX/borderCoordinates, coefY/borderCoordinates,-1);
        }

        //Now update the direction of the user such that at the next iteration it will remains inside this region.
        float newDirX, newDirY;
        float nextX,nextY;
        float x,y;
        tie(x,y) = user->pos->getCoordinates();
        float vel = user->pos->getVel();
        do
        {
            tie(newDirX,newDirY) = getRadomDirection();
            nextX = x + deltaTime * vel * newDirX;
            nextY = y + deltaTime * vel * newDirY;
        } while (!checkIfCoordinatesAreOKWithDirection(direction,nextX,nextY));
        user->updateUserDirection(newDirX,newDirY);     

        //NOTE: is not necessary to add the user to the list of neighbor users since they will be computed
        //after the exchange of all the user out of areas.
    }
}

bool Area::checkIfCoordinatesAreOKWithDirection(Direction direction, float x, float y){
    switch (direction)
    {
    case NorthWest:
        return x>=lowerX && y<=higherY;
    case North:
        return y<=higherY;
    case NorthEast:
        return x<=higherX && y<=higherY;
    case West:
        return x>=lowerX;
    case East:
        return x<=higherX;
    case SouthWest:
        return x>=lowerX && y>=lowerY;
    case South:
        return y>=lowerY;
    case SouthEast:
        return x<=higherX && y>=lowerY;
    default:
        return false;
    }
}

// METHODS FOR TESTS:
map<int,shared_ptr<User>> Area::getUserNearInternalBorders(){
    return userNearInternalBorders;
}
vector<shared_ptr<User>> Area::getOutOfAreaUsers() {
    return outOfAreaUsers;
}