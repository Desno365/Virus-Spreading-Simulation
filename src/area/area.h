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
        //Returns a Map that associated the local Area to which the vector of user has to be 
        //added.
        map<int,vector<User>> getOutOfAreaUsersLocal();
        //Returns a Map that associated the remote processor to which the vector of user_struct has to be 
        //added.
        map<int,vector<user_struct>> getOutOfAreaUsersRemote();
        //Returns a Map that associated the local Area tinterested in knowing the users near the border.
        map<int,vector<User>> getNearBorderUsersLocal();
        //Returns a Map that associated the remote processor interested in knowing the users near the border.
        map<int,vector<user_struct>> getNearBorderUsersRemote();
        tuple<int,int> getCoordinate();
        int getCol();
        int getRow();
        void addNearbyUser(User user);
        void recomputePositions(int deltaTime);
        void setBoundaries(int lowerX, int lowerY, int higherX, int higherY);
        //Return a tuple with the value of (X,Y) that are inside the requested area. The values will be:
        //- lowerX < X < higherX ; 
        //- lowerY < Y < highery ; 
        //X is considered aligned with the Column, while Y with the Row of the global map.
        //The method expects to receive the global coordinates.
        tuple<int,int> getRadomCoordinates();
        //Return a tuple with the value of (X,Y) that represents the random direction. The values will be:
        //X is considered aligned with the Column, while Y with the Row of the global map.
        tuple<int,int> getRadomDirection(); 
        void recomputeInfectionState();
    private:
        int col,row;
        int lowerX,lowerY,higherX,higherY;
        map<int,User> usersInArea;
        map<int,User> usersNearby;
        map<int,User> userNearInternalBorders;
        vector<User> outOfAreaUsers;
        map<Direction,NeighborArea> neighborAreas;
        void resetState();
        //Set the user inside the correct list/map at each iteration.
        void sortUser(User user);
    protected:
};

#endif