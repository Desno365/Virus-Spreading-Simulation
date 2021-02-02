#include "user.h"

using namespace std;

User::User(int id, Position position):id(id),pos(position){
    infected = false;
}

void User::setInfected(bool isInfected){
    this->infected = isInfected;
}

bool User::isInfected(){
    return this->infected;
}

shared_ptr<user_struct> User::getStruct(){
    shared_ptr<user_struct> user_t = make_shared<user_struct>();
    user_t->id = this->id;
    user_t->pos_t = *(this->pos.getStruct().get());
    return user_t;
}

User::~User(){

}

MPI_Datatype User::getMPIType(vector<MPI_Datatype> requiredDatatypes){
    user_struct user_t;
    MPI_Datatype mpi_user;
    //Set the structure lenght.
    int struct_len = 2;
    int block_lens[struct_len];
    //Compute the displacment of elements in the struct.
    MPI_Datatype types[struct_len];
    MPI_Aint displacements[struct_len];
    MPI_Aint current_displacement = 0;
    //Add id.
    block_lens[0] = 1;
    types[0] = MPI_INT;
    displacements[0] = (size_t) &(user_t.id) - (size_t) &user_t;
    //Add y.
    block_lens[1] = 1;
    types[1] = requiredDatatypes[0];
    displacements[1] = (size_t) &(user_t.pos_t) - (size_t) &user_t;
    MPI_Type_create_struct(struct_len, block_lens, displacements, types, &mpi_user);
    MPI_Type_commit(&mpi_user);
    return mpi_user;
}

int User::getId(){
    return this->id;
}

