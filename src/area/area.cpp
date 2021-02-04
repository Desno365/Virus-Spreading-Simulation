#include <stdlib.h> 

#include "area.h"

//Is the maximum number used by rand() to generates coordinates.
#define RAND_MAX_DIRECTION 3

Area::Area(int col,int row):col(col),row(row){}

void Area::addUser(User user){
    this->usersInArea.insert({user.getId(),user});
}

void Area::setNeighborArea(NeighborArea neighborArea,Direction direction){
    this->neighborAreas.insert({direction,neighborArea});
}

NeighborArea Area::getNeighborArea(Direction direction){
    return this->neighborAreas.at(direction);
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
    this->outOfAreaUsers.clear();
    this->outOfAreaUsers.clear();
    this->userNearInternalBorders.clear();
    this->usersNearbyLocal.clear();
}

void Area::updateUserPositions(int deltaTime){
    this->resetState();
    for ( auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it  )
    {
        it->second.pos.updatePosition(deltaTime);
        this->sortUser(it->second);
        //TODO see if the user has gone out of the area sort it in the right queue.
    } 
}

void Area::updateUserInfectionStatus(int deltaTime){
    for ( auto it = this->usersInArea.begin(); it != this->usersInArea.end(); ++it  )
    {
        map<int,User> nearbyUser;
        //TODO computer the nearbyuser
        it->second.updateUserInfectionState(nearbyUser, deltaTime);
        //TODO see if the user has gone out of the area sort it in the right queue.
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

map<int,vector<User>> Area::getOutOfAreaUsersLocal(){
    map<int,vector<User>> usersMap;
    //TODO compute the value of the users that has to be sent to another process.
    return usersMap;
}

map<int,vector<user_struct>> Area::getOutOfAreaUsersRemote(){
    map<int,vector<user_struct>> usersMap;
    //TODO compute the value of the users that has to be sent to another process.
    return usersMap;
}

map<int,vector<User>> Area::getNearBorderUsersLocal(){
    //TODO
}

map<int,vector<user_struct>> Area::getNearBorderUsersRemote(){
    //TODO
}

void Area::sortUser(User user){
    //TODO sort user also change direction if they exit the area
}

void Area::setBoundaries(int lowerX, int lowerY, int higherX, int higherY){
    this->lowerX = lowerX;
    this->lowerY = lowerY;
    this->higherX = higherX; 
    this->higherY = higherY;
}

void Area::addNearbyUsersRemote(vector<user_struct> nearbyUsersRemote){
    for(user_struct &nearbyUser:nearbyUsersRemote){
        this->usersNearbyRemote.insert( { nearbyUser.id , nearbyUser } );
    }
}

void Area::addNearbyUsersLocal(vector<User> nearbyUsersLocal){
    for(User &nearbyUser:nearbyUsersLocal){
        this->usersNearbyLocal.insert({ nearbyUser.getId() , nearbyUser });
    }
}

void Area::printActualState(){
    //TODO print the stats of this area
}