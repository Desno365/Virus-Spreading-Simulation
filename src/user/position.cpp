#include "position.h"

Position::Position(float x, float y, float vel, float dirX, float dirY):x(x),y(y),vel(vel),dirX(dirX),dirY(dirY){}

void Position::setDirection(float dirX, float dirY){
    this->dirX = dirX;
    this->dirY = dirY;
}

void Position::updatePosition(int deltaTime){
    //TODO change this formula if you use direction with float
    this->x += deltaTime * this->vel * dirX;
    this->y += deltaTime * this->vel * dirY;
}

Position::~Position(){

}

shared_ptr<position_struct> Position::getStruct(){
    shared_ptr<position_struct> pos_t = make_shared<position_struct>();
    pos_t->x = this->x;
    pos_t->y = this->y;
    pos_t->dirX = this->dirX;
    pos_t->dirY = this->dirY;
    return pos_t;
}

MPI_Datatype Position::getMPIType(vector<MPI_Datatype> requiredDatatypes){
    position_struct post_t;
    MPI_Datatype mpi_position;
    //Set the structure lenght.
    int struct_len = 4;
    int block_lens[struct_len];
    //Compute the displacment of elements in the struct.
    MPI_Datatype types[struct_len];
    MPI_Aint displacements[struct_len];
    MPI_Aint current_displacement = 0;
    //Add x.
    block_lens[0] = 1;
    types[0] = MPI_FLOAT;
    displacements[0] = (size_t) &(post_t.x) - (size_t) &post_t;
    //Add y.
    block_lens[1] = 1;
    types[1] = MPI_FLOAT;
    displacements[1] = (size_t) &(post_t.y) - (size_t) &post_t;
    //Add dirX.
    block_lens[2] = 1;
    types[2] = MPI_FLOAT;
    displacements[2] = (size_t) &(post_t.dirX) - (size_t) &post_t;
    //Add dirY
    block_lens[3] = 1;
    types[3] = MPI_FLOAT;
    displacements[3] = (size_t) &(post_t.dirY) - (size_t) &post_t;
    MPI_Type_create_struct(struct_len, block_lens, displacements, types, &mpi_position);
    MPI_Type_commit(&mpi_position);
    return mpi_position;
}