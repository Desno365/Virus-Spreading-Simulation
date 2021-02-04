#include "user.h"

//Immune time of the user express as 3(month) * 30(days per month) * 24(hours) * 60(minutes) * 60(seconds)
#define IMMUNE_TIME 3 * 30 * 24 * 60 * 60 
//Infected time of the user express as 10(days per month) * 24(hours) * 60(minutes) * 60(seconds)
#define INFECTED_TIME 10 * 24 * 60 * 60

using namespace std;

User::User(int id, Position position):id(id),pos(position){
    infected = false;
}


User::User(user_struct user_t, int vel):pos(user_t.pos_t,vel){
    this->immuneTime = user_t.immuneTime;
    this->id = user_t.id;
    this->infected = user_t.infected;
    this->infectedTime = this->infected ? user_t.timeCounter : 0 ;
    this->timeNearInfected = !this->infected ? user_t.timeCounter : 0 ;
}


bool User::isInfected(){
    return this->infected;
}

shared_ptr<user_struct> User::getStruct(){
    shared_ptr<user_struct> user_t = make_shared<user_struct>();
    user_t->id = this->id;
    user_t->pos_t = *(this->pos.getStruct().get());
    user_t->infected = this->infected;
    user_t->immuneTime = this->immuneTime;
    user_t->timeCounter = this->infected ? this->infectedTime : this->timeNearInfected;
    return user_t;
}

User::~User(){

}

MPI_Datatype User::getMPIType(vector<MPI_Datatype> requiredDatatypes){
    user_struct user_t;
    MPI_Datatype mpi_user;
    //Set the structure lenght.
    int struct_len = 6;
    int block_lens[struct_len];
    //Compute the displacment of elements in the struct.
    MPI_Datatype types[struct_len];
    MPI_Aint displacements[struct_len];
    MPI_Aint current_displacement = 0;
    //Add id.
    block_lens[0] = 1;
    types[0] = MPI_INT;
    displacements[0] = (size_t) &(user_t.id) - (size_t) &user_t;
    //Add the position of the user.
    block_lens[1] = 1;
    types[1] = requiredDatatypes[0];
    displacements[1] = (size_t) &(user_t.pos_t) - (size_t) &user_t;
    //Add the infection state of the user.
    block_lens[2] = 1;
    types[2] = MPI_C_BOOL;
    displacements[2] = (size_t) &(user_t.infected) - (size_t) &user_t;
    //Add the immune time of the user.
    block_lens[3] = 1;
    types[3] = MPI_INT;
    displacements[3] = (size_t) &(user_t.immuneTime) - (size_t) &user_t;
    //Add the time.
    block_lens[4] = 1;
    types[4] = MPI_INT;
    displacements[4] = (size_t) &(user_t.timeCounter) - (size_t) &user_t;
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
    this->pos.updatePosition(deltaTime);
    //Update time counter associated to the user.
    if(this->immuneTime!=0) this->immuneTime-=deltaTime;
    if(this->infected) this->infectedTime-=deltaTime;
}

void User::updateUserInfectionState(map<int,User> nearbyUser, int deltaTime){
    if(this->infected){
        this->infectedTime = min( this->infectedTime-deltaTime , 0 );
        if( this->infectedTime == 0 ){
            this->infected = false;
            this->immuneTime = IMMUNE_TIME;
        }
    }else if (this->immuneTime>0){
        this->immuneTime = min( this->immuneTime-deltaTime , 0 );
    }
    //TODO checks if the user has been infected and update the nearbyInfectedUser

}

void User::setInfected(bool isInfected){
    if(!this->infected){
        this->infected = isInfected;
        this->infectedTime = INFECTED_TIME;
    }
}