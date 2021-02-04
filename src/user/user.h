#ifndef USER_H
#define USER_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include <map>
#include "position.h"
#include "serializable.h"

using namespace std;

typedef struct user_struct_t {
    int id;
    position_struct pos_t;
    bool infected;
    int immuneTime;
    //Is the infected time if the user if infected otherwise is the time near at least one infected user.
    int timeCounter; 
} user_struct;

class User: public Serializable<user_struct>
{
    public:
        User(int id,Position pos);
        //Create an instance of the user from its struct and the provided velocity.
        User(user_struct user_t, int vel);
        Position pos;
        //Updates the position associated to this user.
        void updateUserPosition(int deltaTime);
        //When the vector with the nearby user is ready, than updates the infection status of the user.
        //User will infect other users only at the next step.
        void updateUserInfectionState(map<int,User> nearbyUser, int deltaTime);
        bool isInfected();
        int getId();
        ~User();
        bool isImmune();
        shared_ptr<user_struct> getStruct();
        //It returns the datatype that can be used in order to send serialize this object in the struct and send it.
        static MPI_Datatype getMPIType(vector<MPI_Datatype> requiredDatatypes);
    private:
        int id;
        bool infected;
        //Is the remaining immune time of the user, if 0 the user can be infected.
        int immuneTime;
        //It is consequtive time spent near at least one infected user,
        //so it starts from 0 and it goes to 10 days.
        int timeNearInfected;
        //Is the remaining time that the user wiil remain infected, it is arrives to 0 after 10 days.
        int infectedTime;
        //Sets the user to be infected, if it is not already infected, it doesn't keep track of the near infected user.
        void setInfected(bool);
    protected:
};

#endif