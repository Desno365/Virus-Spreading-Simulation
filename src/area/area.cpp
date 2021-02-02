#include "area.h"

Area::Area(int col,int row):col(col),row(row){}

void Area::addUser(User user){
    this->usersInArea.insert({user.getId(),user});
}

void Area::setNeighborArea(NeighborArea neighborArea,Direction direction){
    this->neighborAreas.insert( {direction,neighborArea});
}

NeighborArea Area::getNeighborArea(Direction direction){
    return this->neighborAreas.at(direction);
}

vector<user_struct> Area::getUsersToSend(){
    vector<user_struct> users_t;
    //TODO compute the value of the users that has to be sent to another process.
    return users_t;
}

tuple<int,int> Area::getCoordinate(){
    return {this->col,this->row};
}