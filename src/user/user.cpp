#include "user.h"
#include <cmath>

//Immune time of the user express as 3(month) * 30(days per month) * 24(hours) * 60(minutes) * 60(seconds)
#define IMMUNE_TIME 7776000
//Infected time of the user express as 10(days per month) * 24(hours) * 60(minutes) * 60(seconds)
#define INFECTED_TIME 864000
//Is the time that a user has to remains close to another infected user in order to become infected, it is 
//computed as 10(minutes) * 60(seconds)
#define TIME_NEAR_INFECTED 600

using namespace std;

User::User(int id, shared_ptr<Position> position, bool isAlreadyInfected):pos(position),id(id),infected(isAlreadyInfected){
    this->updateStruct();
    this->infected = false;
    this->immuneTime = 0;
    this->timeNearInfected = 0;
    if(isAlreadyInfected) this->infectedTime = INFECTED_TIME;
    else this->infectedTime = 0;
}


User::User(user_struct *user_t, float vel){
    this->pos = make_shared<Position>(user_t->x,user_t->y,vel,user_t->dirX,user_t->dirY);
    this->immuneTime = user_t->immuneTime;
    this->id = user_t->id;
    this->infected = user_t->infected;
    this->infectedTime = this->infected ? user_t->timeCounter : 0 ;
    this->timeNearInfected = !this->infected ? user_t->timeCounter : 0 ;
    this->updateStruct();
    
}


bool User::isInfected(){
    return this->infected;
}

shared_ptr<user_struct> User::getStruct(){
    return user_saved_struct;
}

void User::updateStruct(){
    user_saved_struct = make_shared<user_struct>();
    user_saved_struct->id = this->id;
    user_saved_struct->infected = this->infected;
    user_saved_struct->immuneTime = this->immuneTime;
    user_saved_struct->timeCounter = this->infected ? this->infectedTime : this->timeNearInfected;
    tie(user_saved_struct->x,user_saved_struct->y) = pos->getCoordinates();
    tie(user_saved_struct->dirX,user_saved_struct->dirY) = pos->getDirections();
}

User::~User(){
    //The shared pointer of the position destructor is automatically invoked upond deletion.
}

MPI_Datatype User::getMPIType(){
    user_struct user_t;
    MPI_Datatype mpi_user;
    //Set the structure lenght.
    int struct_len = 8;
    int block_lens[struct_len];
    //Compute the displacment of elements in the struct.
    MPI_Datatype types[struct_len];
    MPI_Aint displacements[struct_len];
    //Add id.
    block_lens[0] = 1;
    types[0] = MPI_INT;
    displacements[0] = (size_t) &(user_t.id) - (size_t) &user_t;
    //Add the position of the user.
    //Add x.
    block_lens[1] = 1;
    types[1] = MPI_FLOAT;
    displacements[1] = (size_t) &(user_t.x) - (size_t) &user_t;
    //Add y.
    block_lens[2] = 1;
    types[2] = MPI_FLOAT;
    displacements[2] = (size_t) &(user_t.y) - (size_t) &user_t;
    //Add dirX.
    block_lens[3] = 1;
    types[3] = MPI_FLOAT;
    displacements[0] = (size_t) &(user_t.dirX) - (size_t) &user_t;
    //Add dirY
    block_lens[4] = 1;
    types[4] = MPI_FLOAT;
    displacements[4] = (size_t) &(user_t.dirY) - (size_t) &user_t;
    //Add the infection state of the user.
    block_lens[5] = 1;
    types[5] = MPI_C_BOOL;
    displacements[5] = (size_t) &(user_t.infected) - (size_t) &user_t;
    //Add the immune time of the user.
    block_lens[6] = 1;
    types[6] = MPI_INT;
    displacements[6] = (size_t) &(user_t.immuneTime) - (size_t) &user_t;
    //Add the time.
    block_lens[7] = 1;
    types[7] = MPI_INT;
    displacements[7] = (size_t) &(user_t.timeCounter) - (size_t) &user_t;
    MPI_Type_create_struct(struct_len, block_lens, displacements, types, &mpi_user);
    MPI_Type_commit(&mpi_user);
    return mpi_user;
}

int User::getId(){
    return this->id;
}


bool User::isImmune(){
    if (this->immuneTime>0)
    {
        return true;
    }
    return false;
}

void User::updateUserPosition(int deltaTime){
    this->pos->updatePosition(deltaTime);
    this->updateStruct();
}

void User::updateUserInfectionState(bool isNearAnInfected, int deltaTime){
    if(this->infected){
        this->infectedTime = max( this->infectedTime-deltaTime , 0 );
        if( this->infectedTime == 0 ){
            this->infected = false;
            this->immuneTime = IMMUNE_TIME;
        }
    }else if (this->immuneTime>0){
        this->immuneTime = max( this->immuneTime-deltaTime , 0 );
    }else if(isNearAnInfected){//If is not already infected, is not immune and it is nearby an infected user, that it will increase its time near an infected user
        this->timeNearInfected+=deltaTime;
        if(this->timeNearInfected>TIME_NEAR_INFECTED){
            this->timeNearInfected = 0;
            this->infected = true;
            this->infectedTime = INFECTED_TIME;
        }
    }else{//If is not near an infected user, and it is not infected or immune, reset its time near an infected user
        this->timeNearInfected = 0;
    }
}

bool User::isNear(float x, float y, float infectionDistance){
    float distanceX = pos->getX() - x;
    float distanceY = pos->getY() - y;
    float distance = sqrt(pow(distanceX,2) + pow(distanceY,2)); 
    return distance<=infectionDistance;
}

void User::goBackToIntersection(float coefX, float coefY, float noteTerm){
    this->pos->goBackToIntersection(coefX, coefY, noteTerm);
    this->updateStruct();
}

shared_ptr<user_struct> User::getSharedFromStruct(user_struct &user_t){
    shared_ptr<user_struct> returned_user_t = make_shared<user_struct>();
    returned_user_t->id = user_t.id;
    returned_user_t->infected = user_t.infected;
    returned_user_t->immuneTime = user_t.immuneTime;
    returned_user_t->timeCounter = user_t.timeCounter;
    returned_user_t->x = user_t.x;
    returned_user_t->y = user_t.y;
    returned_user_t->dirX = user_t.dirX;
    returned_user_t->dirY = user_t.dirY;
    return returned_user_t;
}
