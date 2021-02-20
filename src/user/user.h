#ifndef USER_H
#define USER_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <map>
#include <mpi.h>
#include "position.h"

using namespace std;

//Is the struct associated to the user used for serialization.
typedef struct user_struct_t {
    //Is the id of the user.
    int id;
    //Is the position struct of the user.
    float x,y,dirX,dirY;
    //Indicates if the user is actually infected.
    bool infected;
    //Is the remaining immune time of the user
    int immuneTime;
    //Is the infected time if the user if infected otherwise is the time near at least one infected user.
    int timeCounter; 
} user_struct;

//Represent an agent that can move on the global area.
class User
{
    public:
        //Construct an instance of a user.
        User(int id,shared_ptr<Position> pos, bool isAlreadyInfected);
        //Create an instance of the user from its struct and the provided velocity.
        //After this the struct is destroyed.
        User(user_struct *user_t, float vel);
        //Is the posisiton associated to the user
        shared_ptr<Position> pos;
        //Updates the position associated to this user.
        void updateUserPosition(int deltaTime);
        //Updates the direction associated to this user.
        void updateUserDirection(float newDirX, float newDirY);
        //It requires the elapsed delta time to update eventually the counter and a boolean that says 
        //if it is actually near an infected user: this field is discarded if the user is immune or infected.
        //New infected user will start to infect other users only at the next step.
        void updateUserInfectionState(bool isNearAnInfected, int deltaTime);
        //Set the position to a previous passed set of points based on the intersection with the hyperplane
        //described by the arguments of the function.
        void goBackToIntersection(float coefX, float coefY, float noteTerm);
        //Returns true if the user is infected.
        bool isInfected();
        //Returns the id of the user.
        int getId();
        //Destructs this instance of the user.
        ~User();
        //Returns true if the user is immune.
        bool isImmune();
        //Returns the struct associated to the user used for serialization.
        shared_ptr<user_struct> getStruct();
        //It returns the datatype that can be used in order to send serialize this object in the struct and send it.
        static MPI_Datatype getMPIType();
        //Returns true if this user is at a distance that is lower of infection distance w.r.t (x,y).
        //NOTE: is a <= comparison between the two distances.
        bool isNear(float x, float y, float infectionDistance);
        //Returns a shared pointer of a user struct starting from the pointer of another user struct. I performs a clone.
        static shared_ptr<user_struct> getSharedFromStruct(user_struct &user_t);
    private:
        //The unique id of the user.
        int id;
        //Indicates id the user is actually infected.
        bool infected;
        //Is the remaining immune time of the user, if 0 the user can be infected.
        int immuneTime;
        //It is the consecutive time spent near at least one infected user,
        //so it starts from 0 and it goes to 10 days.
        int timeNearInfected;
        //Is the remaining time that the user wiil remain infected, it is arrives to 0 after 10 days.
        int infectedTime;
        //Is the user_struct associated to this user recompute every time the position is updated.
        shared_ptr<user_struct> user_saved_struct;
        //Update the struct associated to this user.
        //NOTE: at the moment the shared pointer is substituted, so all the previous reference are not update with the new
        //values.
        void updateStruct();
    protected:
};

#endif