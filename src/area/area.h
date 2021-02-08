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
        Area(int col, int row,int id);
        //Remove all the references to local objects.
        ~Area();
        //It add a new user to this area when it enters inside its boundaries.
        //NOTE: if the user is on the border, than it is managed by the previous area.
        //NOTE: this method will then sort the user into the different maps based on its position.
        void addUser(shared_ptr<User> user, int infectionDistance);
        //It adds the provided vector of user_struct to the list of users that
        //are near but outside of this area inside remote areas.
        void addNearbyUsersRemote(vector<shared_ptr<user_struct>> nearbyUsersRemote);
        //It adds the provided vector of users to the list of users that
        //are near but outside of this area in local area.
        //It deletes the vector(not its content).
        void addNearbyUsersLocal(vector<shared_ptr<User>> nearbyUsersLocal);
        //Sets the as provided are as a neighbor of this area in the direction.
        void setNeighborArea(shared_ptr<NeighborArea> neighborArea,Direction direction);
        //Returns a Map that associated the local Area to which the vector of user has to be 
        //added.
        map<int,vector<shared_ptr<User>>> getOutOfAreaUsersLocal();
        //Returns a Map that associated the remote processor to which the vector of user_struct has to be 
        //added. The user object will be desotroyed in this method.
        map<int,vector<shared_ptr<user_struct>>> getOutOfAreaUsersRemote();
        //Compute the content of the maps that contains the user or the user_struct of the users that have leave the area, returned 
        //by the two previous methods.
        void computeOutOfAreaUserMap(int my_processor_rank);
        //Returns a Map that associated the local Area interested in knowing the users near the border that are infected(user pointer are shared).
        map<int,vector<shared_ptr<User>>> getNearBorderUsersLocal();
        //Returns a Map that associated the remote processor interested in knowing the users near the border that are infected.
        map<int,vector<shared_ptr<user_struct>>> getNearBorderUsersRemote();
        //Computes the two maps that contains the users near the border. Use the previous getter to get them.
        //NOTE: it assumes that it is called after updaetUserPosition() so the map of the user near the border is up to date.
        void computeNearBorderUserMap(int infectedDistance, int my_processor_rank);
        //Returns a tuple with the (col,row) of this area.
        tuple<int,int> getCoordinate();
        //Returns the column of this area, on the total area division.
        int getCol();
        //Returns the row of this area, on the total area division.
        int getRow();
        //Update the position of all the users inside this area.
        void updateUserPositions(int deltaTime, int infectionDistance);
        //Update the infection status of the users.
        void updateUserInfectionStatus(int deltaTime, int infectionDistance);
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
        //Prints on the standard output the actual state of the area.
        void printActualState(FILE *ptr);
        //Return the id of this area.
        int getID();
    private:
        //Is the id of the area.
        int id;
        //Is the posisition of the area on the global area.
        int col,row;
        //Are the boundaries of the area with respect to the global area.
        int lowerX,lowerY,higherX,higherY;
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
        //deleted and trasnformed into the user struct if it has to go to another processor. OUT
        vector<shared_ptr<User>> outOfAreaUsers;
        //Contains the neighbor areas.
        map<Direction,shared_ptr<NeighborArea>> neighborAreas;
        //Before each update of the position of the user the state of this area is reset.
        void resetState();
        //Set the user inside the correct list/map at each iteration.
        void sortUser(shared_ptr<User> user, int infectionDistance);

        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the area
        //to which the infected user has to be sent locally.
        map<int,vector<shared_ptr<User>>>  mapAreasToUsersLocal;
        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the process
        //to which the infected user has to be sent remotly.
        map<int,vector<shared_ptr<user_struct>>>  mapAreasToUsersRemote;
        //Add a user to one of the previous maps, based on the neighbor area, it will only work on infected user.
        void addUserNear(shared_ptr<User> user, shared_ptr<NeighborArea> neighborArea, int my_processor_rank);

        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the area
        //to which the infected user has to be sent locally.
        map<int,vector<shared_ptr<User>>>  mapOutOfAreasToUsersLocal;
        //It is recomputed each time computeNearBorderUserMap(), and contains the association of the id of the process
        //to which the infected user has to be sent remotly.
        map<int,vector<shared_ptr<user_struct>>>  mapOutOfAreasToUsersRemote;

        //Returns a tuple in which the first elements is the number of users actually infected in this area, while
        //the second is the number of users actually immune.
        tuple<int,int> actuallyInfectedAndImmuneUser();
    protected:
};

#endif