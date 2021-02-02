#ifndef AREA_H
#define AREA_H

#include "user/user.h"
#include "area/neighborArea.h"
#include "utility.h"
#include <map>
#include <iostream>
#include <vector>
#include <tuple>

class Area
{
    public:
        Area(int col, int row);
        void addUser(User user);
        void setNeighborArea(NeighborArea neighborArea,Direction direction);
        NeighborArea getNeighborArea(Direction direction);
        vector<user_struct> getUsersToSend();
        tuple<int,int> getCoordinate();
    private:
        int col,row;
        map<int,User> usersInArea;
        map<Direction,NeighborArea> neighborAreas;
    protected:
};

#endif