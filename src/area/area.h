#ifndef AREA_H
#define AREA_H

#include "user/user.h"
#include "area/neighborArea.h"
#include "utility.h"
#include <map>
#include <iostream>
#include <vector>
#include <tuple>

//It represents a country.
class Area
{
    public:
        //Constructs an Area based on its row and column.
        Area(int col, int row);
        //It add a new user to this area when it enters inside its boundaries.
        //NOTE: if the user is on the border, than it is managed by the previous area.
        void addUser(User user);
        //It adds the provided vector of user_struct to the list of users that
        //are near but outside of this area inside remote areas.
        void addNearbyUsersRemote(vector<user_struct> nearbyUsersRemote);
        //It adds the provided vector of users to the list of users that
        //are near but outside of this area in local area.
        void addNearbyUsersLocal(vector<User> nearbyUsersLocal);
        //Sets the as provided are as a neighbor of this area in the direction.
        void setNeighborArea(NeighborArea neighborArea,Direction direction);
        NeighborArea getNeighborArea(Direction direction);
        //Returns a Map that associated the local Area to which the vector of user has to be 
        //added.
        map<int,vector<User>> getOutOfAreaUsersLocal();
        //Returns a Map that associated the remote processor to which the vector of user_struct has to be 
        //added. The user object will be desotroyed in this method.
        map<int,vector<user_struct>> getOutOfAreaUsersRemote();
        //Returns a Map that associated the local Area tinterested in knowing the users near the border(user pointer are shared).
        map<int,vector<User>> getNearBorderUsersLocal();
        //Returns a Map that associated the remote processor interested in knowing the users near the border.
        map<int,vector<user_struct>> getNearBorderUsersRemote();
        //Returns a tuple with the (col,row) of this area.
        tuple<int,int> getCoordinate();
        //Returns the column of this area, on the total area division.
        int getCol();
        //Returns the row of this area, on the total area division.
        int getRow();
        //Update the position of all the users inside this area.
        void updateUserPositions(int deltaTime);
        //Update the infection status of the users.
        void updateUserInfectionStatus(int deltaTime);
        //Sets the boundaries given to this area, with respect to the total size.
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
        //Recomputes the infection state of each user based on the global information provided by the system.
        void recomputeInfectionState();
        //Prints on the standard output the actual state of the area.
        void printActualState();
    private:
        //Is the posisition of the area on the global area.
        int col,row;
        //Are the boundaries of the area with respect to the global area.
        int lowerX,lowerY,higherX,higherY;
        //Is the map that associated the id of the users with the user present inside this area.
        map<int,User> usersInArea;
        //Maps the user_struct of the nearby users from the remote location with their ids.
        map<int,user_struct> usersNearbyRemote;
        //Maps the state of the nearby users from other local areas with their ids.
        map<int,User> usersNearbyLocal;
        //Is the map that contains the user inside this area that are near the border.
        map<int,User> userNearInternalBorders;
        //Is the map that contains the users that has gone out of this area. This user will be send
        //to their area of managment that will have to recompute their infection status
        vector<User> outOfAreaUsers;
        //Contains the neighbor areas.
        map<Direction,NeighborArea> neighborAreas;
        //Before each update of the position of the user the state of this area is reset.
        void resetState();
        //Set the user inside the correct list/map at each iteration.
        void sortUser(User user);
    protected:
};

#endif