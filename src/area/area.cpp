#include <stdlib.h> 

#include "area.h"

//Is the maximum number used by rand() to generates coordinates.
#define RAND_MAX_DIRECTION 3

Area::Area(int col,int row, int id):col(col),row(row),id(id){}

Area::~Area(){
    for ( auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it  )
    {
        delete it->second;
    } 
}

void Area::addUser(User &user, int infectionDistance){
    this->usersInArea.insert({user.getId(),&user});
    sortUser(user, infectionDistance);
}

void Area::setNeighborArea(NeighborArea &neighborArea,Direction direction){
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
    //Clear the created user struct used to sending information to other regions and delete them.
    for ( auto it = this->mapOutOfAreasToUsersRemote.begin(); it != this->mapOutOfAreasToUsersRemote.end(); ++it  )
    {
        for ( auto it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
        {
            delete &it2;
        } 
        //Destroy the vector.
        delete &it->second;
    } 
    for ( auto it = this->mapAreasToUsersRemote.begin(); it != this->mapAreasToUsersRemote.end(); ++it  )
    {
        for ( auto it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
        {
            delete &it2;
        } 
        //Destroy the vector.
        delete &it->second;
    }
    mapOutOfAreasToUsersRemote.clear();
    mapAreasToUsersRemote.clear();

    //At the end it will contains the user that has been translated into user struct and so can be safely destroyed.
    outOfAreaUsers.clear();

    //Reset the state of the various vector.
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
        this->sortUser(*(it->second), infectionDistance);
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

    //Now updates all the new infected user at once.
    for(auto it = usersNearInfected.begin(); it != usersNearInfected.end(); ++it){
        User user = *(usersInArea.at(it->first));
        user.updateUserInfectionState(it->second,deltaTime);
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

map<int,vector<User*>> Area::getOutOfAreaUsersLocal(){
    return mapOutOfAreasToUsersLocal;
}

map<int,vector<user_struct*>> Area::getOutOfAreaUsersRemote(){
    return mapOutOfAreasToUsersRemote;
}

map<int,vector<User*>> Area::getNearBorderUsersLocal(){
    return mapAreasToUsersLocal;
}

map<int,vector<user_struct*>> Area::getNearBorderUsersRemote(){
    return mapAreasToUsersRemote;
}

void Area::sortUser(User &user, int infectionDistance){
    if(user.pos.getX()<=higherX && user.pos.getX()>=lowerX && user.pos.getY()<=higherY && user.pos.getY()>=lowerY){//The user is still inside the region
        if(user.pos.getX()>=(higherX-infectionDistance) && user.pos.getX()<=(lowerX+infectionDistance) && user.pos.getY()>=(higherY-infectionDistance) && user.pos.getY()<=(lowerY+infectionDistance))
            userNearInternalBorders.insert({ user.getId() , &user });
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

void Area::addNearbyUsersRemote(vector<user_struct*> &nearbyUsersRemote){
    for(user_struct *nearbyUser:nearbyUsersRemote){
        this->usersNearbyRemote.insert( { nearbyUser->id , nearbyUser } );
    }
    delete &nearbyUsersRemote;
}

void Area::addNearbyUsersLocal(vector<User*> &nearbyUsersLocal){
    for(User * nearbyUser:nearbyUsersLocal){
        this->usersNearbyLocal.insert({ nearbyUser->getId() , nearbyUser });
    }
    delete &nearbyUsersLocal;
}

void Area::printActualState(){
    //TODO print the stats of this area
}

void Area::computeNearBorderUserMap(int infectedDistance, int my_processor_rank){
    //Check the various user based on the border to which they are near.
    //NOTE: we check only the users inside userNearInternalBorders since it has been updated after the recompute position.
    for(auto entriesUsersInternalNearBorder = userNearInternalBorders.begin(); entriesUsersInternalNearBorder != userNearInternalBorders.end(); ++entriesUsersInternalNearBorder){
        User user = *(entriesUsersInternalNearBorder->second);
        //Do this computation only if the user is infected.
        if(user.isInfected()){
            if(user.pos.getX() >= (lowerX - infectedDistance)){
                //Check West
                if(neighborAreas.count(West)>0)
                    addUserNear(user,neighborAreas.at(West),my_processor_rank);
                //Check North-West and also North
                if(user.pos.getY()<=(higherY-infectedDistance)){
                    //Check North-West
                    if(neighborAreas.count(NorthWest)>0)
                        addUserNear(user,neighborAreas.at(NorthWest),my_processor_rank);
                    //Check North
                    if(neighborAreas.count(North)>0)
                        addUserNear(user,neighborAreas.at(North),my_processor_rank);
                }else if(user.pos.getY()){
                    
                }

            }
        }
    }
}

//NOTE: there can ne duplicated in the vector going to a remote location, locally instead we send information to the same areas, so no problem.
//NOTE: check the users behavior for the other processor global staet when they are sent remotly.
void Area::addUserNear(User &user,  NeighborArea neighborArea, int my_processor_rank){
    if(neighborArea.isLocal(my_processor_rank)){
        vector<User> users;
        // if(mapAreasToUsersLocal.count(neighborArea.getID())) users = neighborAreas.at(neighborArea.getID());
        // else users = new vector<User>;
        //TODO finish here
    }
}

void Area::computeOutOfAreaUserMap(int my_processor_rank){
    //TODO compute the map with the user out of map and their destination, leaves inside the out of area users only the one that has to be deleted.
}