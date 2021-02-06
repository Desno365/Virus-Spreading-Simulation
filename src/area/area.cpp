#include <stdlib.h> 

#include "area.h"

//Is the maximum number used by rand() to generates coordinates.
#define RAND_MAX_DIRECTION 3

Area::Area(int col,int row, int id):col(col),row(row),id(id){}

Area::~Area(){
    this->resetState();
}

void Area::addUser(shared_ptr<User> user, int infectionDistance){
    this->usersInArea.insert({user->getId(), user});
    sortUser(user, infectionDistance);
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

void Area::updateUserPositions(int deltaTime, int infectionDistance){
    this->resetState();
    for ( auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it  )
    {
        it->second->pos.updatePosition(deltaTime);
        this->sortUser(it->second, infectionDistance);
    } 
}

void Area::updateUserInfectionStatus(int deltaTime, int infectionDistance){
    map<int,bool> usersNearInfected;
    for(auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it){
        User user = *(it->second);
        bool isNearInfectedUser = false;
        //If is not the case that the user is infected and not immune, we see if it has infected user nearby.
        if(!user.isInfected() && !user.isImmune()){
            //For everyone checks if near him there exsist a user that is infected.
            for(auto entryUsersInArea = this->usersInArea.begin(); entryUsersInArea != this->usersInArea.end() && !isNearInfectedUser ; ++entryUsersInArea){
                User otherUser = *(entryUsersInArea->second);
                if(otherUser.isInfected() && user.isNear(otherUser.pos.getX(),otherUser.pos.getY(),infectionDistance)){
                    isNearInfectedUser = true;
                }
            }
            if(!isNearInfectedUser && !userNearInternalBorders.count(user.getId())>0){//The user is near a border and it is not already near an infected user we have to check also for the neraby users.
                for(auto entryUsersNearbyLocal = this->usersNearbyLocal.begin(); entryUsersNearbyLocal != this->usersNearbyLocal.end() && !isNearInfectedUser ; ++entryUsersNearbyLocal){
                    User otherUser = *(entryUsersNearbyLocal->second);
                    //Here is not necessary to check if the user is infected since the map contains only the infected users from other areas.
                    if(user.isNear(otherUser.pos.getX(),otherUser.pos.getY(),infectionDistance)){
                        isNearInfectedUser = true;
                    }
                }
                if(!isNearInfectedUser){//Check if the preivous for has alreadya found a nearby infected user.
                    //Check now that the user is not near an infected user from a remote source.
                    //Here is not necessary to check if the user is infected since the map contains only the infected users from other areas.
                    for(auto entryUsersNearbyRemote = this->usersNearbyRemote.begin(); entryUsersNearbyRemote != this->usersNearbyRemote.end() && !isNearInfectedUser ; ++entryUsersNearbyRemote){
                        user_struct otherUser = *(entryUsersNearbyRemote->second);
                        if(user.isNear(otherUser.pos_t.x,otherUser.pos_t.y,infectionDistance)){
                            isNearInfectedUser = true;
                        }
                    }
                }
            }
        }
        //Save the boolean inside the map for the late update of the status.
        usersNearInfected.insert({user.getId(),isNearInfectedUser});
    } 

    //Now updates all the users at once.
    for(auto it = usersInArea.begin(); it != usersInArea.end(); ++it){
        User user = *(it->second.get());
        user.updateUserInfectionState(usersNearInfected.at(it->first),deltaTime);
    }
}

tuple<int,int> Area::getRadomCoordinates(){

    int randomX = rand() % (this->higherX - this->lowerX);
    if(randomX==0) randomX++;

    int randomY = rand() % (this->higherY - this->lowerY);
    if(randomY==0) randomY++;

    return { this->lowerX+randomX , this->lowerY+randomY};
}

tuple<int,int> Area::getRadomDirection(){
    return { (rand() % RAND_MAX_DIRECTION) - (int)RAND_MAX_DIRECTION/2, (rand() % RAND_MAX_DIRECTION) - (int)RAND_MAX_DIRECTION/2 };
}

map<int,vector<shared_ptr<User>>> Area::getOutOfAreaUsersLocal(){
    return mapOutOfAreasToUsersLocal;
}

map<int,vector<shared_ptr<user_struct>>> Area::getOutOfAreaUsersRemote(){
    return mapOutOfAreasToUsersRemote;
}

map<int,vector<shared_ptr<User>>> Area::getNearBorderUsersLocal(){
    return mapAreasToUsersLocal;
}

map<int,vector<shared_ptr<user_struct>>> Area::getNearBorderUsersRemote(){
    return mapAreasToUsersRemote;
}

void Area::sortUser(shared_ptr<User> user, int infectionDistance){
    if(user->pos.getX()<=higherX && user->pos.getX()>=lowerX && user->pos.getY()<=higherY && user->pos.getY()>=lowerY){//The user is still inside the region
        if(user->pos.getX()>=(higherX-infectionDistance) && user->pos.getX()<=(lowerX+infectionDistance) && user->pos.getY()>=(higherY-infectionDistance) && user->pos.getY()<=(lowerY+infectionDistance))
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
        this->usersNearbyRemote.insert( { nearbyUser->id , nearbyUser } );
    }
}

void Area::addNearbyUsersLocal(vector<shared_ptr<User>> nearbyUsersLocal){
    for(shared_ptr<User> nearbyUser:nearbyUsersLocal){
        this->usersNearbyLocal.insert({ nearbyUser->getId() , nearbyUser });
    }
}

void Area::printActualState(){
    //TODO print the stats of this area
}

void Area::computeNearBorderUserMap(int infectedDistance, int my_processor_rank){
    //Check the various user based on the border to which they are near.
    //NOTE: we check only the users inside userNearInternalBorders since it has been updated after the recompute position.
    for(auto entriesUsersInternalNearBorder = userNearInternalBorders.begin(); entriesUsersInternalNearBorder != userNearInternalBorders.end(); ++entriesUsersInternalNearBorder){
        shared_ptr<User> user = entriesUsersInternalNearBorder->second;
        //Do this computation only if the user is infected.
        if(user->isInfected()){
            if(user->pos.getX() <= (lowerX + infectedDistance)){
                //Check West
                if(neighborAreas.count(West)>0)
                    addUserNear(user,neighborAreas.at(West).get(),my_processor_rank);
                //Check North-West and also North
                if(user->pos.getY()>=(higherY-infectedDistance)){
                    //Check North-West
                    if(neighborAreas.count(NorthWest)>0)
                        addUserNear(user,neighborAreas.at(NorthWest).get(),my_processor_rank);
                    //Check North
                    if(neighborAreas.count(North)>0)
                        addUserNear(user, neighborAreas.at(North).get(),my_processor_rank);
                }else if(user->pos.getY()<=(lowerY+infectedDistance)){ //Check South and South-West
                    //Check South-West
                    if(neighborAreas.count(SouthWest)>0)
                        addUserNear(user,neighborAreas.at(SouthWest).get(),my_processor_rank);
                    //Check South
                    if(neighborAreas.count(South)>0)
                        addUserNear(user, neighborAreas.at(South).get(),my_processor_rank);
                }
            }else if(user->pos.getX()>=(higherX-infectedDistance)){
                //Check East
                if(neighborAreas.count(East)>0)
                    addUserNear(user,neighborAreas.at(East).get(),my_processor_rank);
                //Check North-East and also North
                if(user->pos.getY()>=(higherY-infectedDistance)){
                    //Check North-East
                    if(neighborAreas.count(NorthEast)>0)
                        addUserNear(user,neighborAreas.at(NorthEast).get(),my_processor_rank);
                    //Check North
                    if(neighborAreas.count(North)>0)
                        addUserNear(user, neighborAreas.at(North).get(),my_processor_rank);
                }else if(user->pos.getY()<=(lowerY+infectedDistance)){ //Check South and South-East
                    //Check South-East
                    if(neighborAreas.count(SouthEast)>0)
                        addUserNear(user,neighborAreas.at(SouthEast).get(),my_processor_rank);
                    //Check South
                    if(neighborAreas.count(South)>0)
                        addUserNear(user, neighborAreas.at(South).get(),my_processor_rank);
                }     
            }else if(user->pos.getY()>=(higherX-infectedDistance)){
                //Since we have previously check this has to be sent only to north.
                //Check North
                if(neighborAreas.count(North)>0)
                    addUserNear(user, neighborAreas.at(North).get(),my_processor_rank);
            }else if(user->pos.getY()<=(lowerY+infectedDistance)){
                //Since we have previously check this has to be sent only to south.
                //Check South
                if(neighborAreas.count(South)>0)
                    addUserNear(user, neighborAreas.at(South).get(),my_processor_rank);
            }
        }
    }
}

//NOTE: locally the user in the list cannot be reapted. Instead we need to check for remote destination.
void Area::addUserNear(shared_ptr<User> user,  NeighborArea *neighborArea, int my_processor_rank){
    if(neighborArea->isLocal(my_processor_rank)){
        vector<shared_ptr<User>> * users;
        if(mapAreasToUsersLocal.count(neighborArea->getID())) users = &(mapAreasToUsersLocal.at(neighborArea->getID()));
        else{
            //No entries is already present so we insert such a vector.
            users = new vector<shared_ptr<User>>;
            mapAreasToUsersLocal.insert({neighborArea->getID(), *users});
        }
        users->push_back(user);
    }else{
        //We have to send the information remotly.
        vector<shared_ptr<user_struct>> * users_t;
        if(mapAreasToUsersRemote.count(neighborArea->getID())) users_t = &(mapAreasToUsersRemote.at(neighborArea->getID()));
        else{
            //No entries is already present so we insert such a vector.
            users_t = new vector<shared_ptr<user_struct>>;
            mapAreasToUsersRemote.insert({neighborArea->getID(), *users_t});
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

void Area::computeOutOfAreaUserMap(int my_processor_rank){
    //TODO compute the map with the user out of map and their destination, leaves inside the out of area users only the one that has to be deleted.
}