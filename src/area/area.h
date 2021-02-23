#ifndef AREA_H
#define AREA_H

#include "user/user.h"
#include "area/neighborArea.h"
#include "utility/utility.h"
#include <map>
#include <iostream>
#include <vector>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>

//It represents a country.
class Area
{
    public:
        //Constructs an Area based on its row and column.
        Area(int col, int row,int id, float infectionDistance, int deltaTime, int my_processor_rank);
        //Remove all the references to local objects.
        ~Area();
        //It add a new user to this area when it enters inside its boundaries.
        //NOTE: if the user is on the border, than it is managed by the previous area.
        //NOTE: this method will then sort the user into the different maps based on its position.
        void addUser(shared_ptr<User> user);
        //Add to the map the provided content of the vector users.
        void addUsers(vector<shared_ptr<User>> users);
        //After exchanging users out of a certain area a vector with new users is passed around, if an area 
        //find a new user inside its border it removes it from the vector and it adds it locally. The first area that identify
        //the user on his border will take it.
        //TODO check that the user is removed not from the copy but byt the actual location.
        void getNewUserFromRemoteLocation(vector<shared_ptr<User>> *newUsers);
        //It gets the vector that contains all the received user struct after the comunication, and this area
        //will take reference to the shared pointer of interest.
        void addNearbyUsersRemote(vector<shared_ptr<user_struct>> nearbyUsersRemote);
        //It adds the provided vector of users to the list of users that
        //are near but outside of this area in local area.
        //It deletes the vector(not its content).
        void addNearbyUsersLocal(vector<shared_ptr<User>> nearbyUsersLocal);
        //Sets the as provided are as a neighbor of this area in the direction.
        void setNeighborArea(shared_ptr<NeighborArea> neighborArea,Direction direction);
        //Returns a Map that associated the local Area to which the vector of user has to be 
        //added.
        map<int,shared_ptr<vector<shared_ptr<User>>>> getOutOfAreaUsersLocal();
        //Returns a Map that associated the remote processor to which the vector of user_struct has to be 
        //added. The user object will be destroyed in this method.
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> getOutOfAreaUsersRemote();
        //Compute the content of the maps that contains the user or the user_struct of the users that have leave the area, returned 
        //by the two previous methods.
        void computeOutOfAreaUserMap();
        //Returns a Map that associated the local Area interested in knowing the users near the border that are infected(user pointer are shared).
        map<int,shared_ptr<vector<shared_ptr<User>>>> getNearBorderUsersLocal();
        //Returns a Map that associated the remote processor interested in knowing the users near the border that are infected.
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> getNearBorderUsersRemote();
        //Computes the two maps that contains the users near the border. Use the previous getter to get them.
        //NOTE: it assumes that it is called after updateUserPosition() so the map of the user near the border is up to date.
        void computeNearBorderUserMap();
        //Returns a tuple with the (col,row) of this area.
        tuple<int,int> getCoordinate();
        //Returns the column of this area, on the total area division.
        int getCol();
        //Returns the row of this area, on the total area division.
        int getRow();
        //Update the position of all the users inside this area.
        //NOTE: it also resets the previous state of the area.
        void updateUserPositions();
        //Update the infection status of the users.
        void updateUserInfectionStatus();
        //Sets the boundaries given to this area, with respect to the total size.
        void setBoundaries(int lowerX, int lowerY, int higherX, int higherY);
        //Return a tuple with the value of (X,Y) that are inside the requested area. The values will be:
        //- lowerX < X < higherX ; 
        //- lowerY < Y < highery ; 
        //X is considered aligned with the Column, while Y with the Row of the global map.
        //The method expects to receive the global coordinates.
        tuple<float,float> getRadomCoordinates();
        //Return a tuple with the value of (X,Y) that represents the random direction. The values will be:
        //X is considered aligned with the Column, while Y with the Row of the global map.
        tuple<float,float> getRadomDirection(); 
        //Prints on the standard output the actual state of the area.
        void printActualState(FILE *ptr);
        //Prints on the standard output a representation of the area.
        void printArea(FILE *ptr, int timestamp);
        //Returns a tuple in which the first elements is the number of users actually infected in this area, while
        //the second is the number of users actually immune.
        tuple<int,int> actuallyInfectedAndImmuneUser();
        //Return the id of this area.
        int getID() { return id; };
        //Returns the coordinates of the center point of the area as (Col,Row), the same as (x,y).
        tuple<float,float> getAreaCenter();
        //Sets the maximum values of x and y of the global area.
        void setMaxValue(int maxX, int maxY){ this->maxX=maxX; this->maxY=maxY; };

        // METHODS FOR TESTS:
        map<int,shared_ptr<User>> getUserNearInternalBorders();
        vector<shared_ptr<User>> getOutOfAreaUsers();
        map<Direction,shared_ptr<NeighborArea>> getNeighborAreas();
    private:
        //Is the id of the area.
        int id;
        //Is the position of the area on the global area.
        int col,row;
        //Is the distance to which the user can infect each other.
        float infectionDistance;
        //Is the time to which the computation advance at each step.
        int deltaTime;
        //Is the rank of the processor that is managing this area.
        int my_processor_rank;
        //Are the boundaries of the area with respect to the global area.
        int lowerX,lowerY,higherX,higherY;
        //Are the values of the max x and y values inside the global area, they are used if a user goes beyond them
        //NOTE: the lower value is supposed to be 0 for both x and y.
        int maxX, maxY;
        //Is the map that associated the id of the users with the user present inside this area(no matter if they are inside
        //or if they are near the internal borders).
        map<int,shared_ptr<User>> usersInArea;
        //Maps the user_struct of the nearby INFECTED users from the remote location with their ids. IN
        map<int,shared_ptr<user_struct>> usersNearbyRemote;
        //Maps the state of the nearby INFECTED users from other local areas with their ids. IN
        map<int,shared_ptr<User>> usersNearbyLocal;
        //Is the map that contains the user inside this area that are near the border.OUT
        map<int,shared_ptr<User>> userNearInternalBorders;
        //Is the map that contains the users that has gone out of this area. User objects will be
        //deleted and transformed into the user struct if it has to go to another processor. OUT
        vector<shared_ptr<User>> outOfAreaUsers;
        //Contains the neighbor areas.
        map<Direction,shared_ptr<NeighborArea>> neighborAreas;
        //Before each update of the position of the user the state of this area is reset.
        void resetState();
        //Set the user inside the correct list/map at each iteration.
        void sortUser(shared_ptr<User> user);

        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the area
        //to which the infected user has to be sent locally.
        map<int,shared_ptr<vector<shared_ptr<User>>>>  mapAreasToUsersLocal;
        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the process
        //to which the infected user has to be sent remotely.
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>>  mapAreasToUsersRemote;
        //Add a user to one of the previous maps, based on the neighbor area, it will only work on infected user.
        void addUserNear(shared_ptr<User> user, shared_ptr<NeighborArea> neighborArea);

        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the area
        //to which the infected user has to be sent locally.
        map<int,shared_ptr<vector<shared_ptr<User>>>>  mapOutOfAreasToUsersLocal;
        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the process
        //to which the infected user has to be sent remotely.
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>>  mapOutOfAreasToUsersRemote;
        //Add the user to one of the previous map( if the user has not reached a border) based on the neighbor area
        //that is present inside the neighboar areas map at the correspondent direction.
        void addUserOutOfArea(Direction direction, shared_ptr<User> user, float borderCoordinates);

        //Returns true if the given coordinates are opposite to the given direction.
        bool checkIfCoordinatesAreOKWithDirection(Direction direction, float x, float y);

        // Returns the number of users at the integer coordinates, so it rounds up (ceil) the position of the user to do this check.
        int howManyUsersAtIntCoordinates(int x, int y);
    protected:
};

#endif