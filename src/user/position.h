#ifndef POSITION_H
#define POSITION_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include "serializable.h"

using namespace std;

//It is the struct of the Position object used for serialization during comuinication.
//It does not have the velocity parameters since the velocity is the same among
//all processor.
typedef struct position_struct_t {
  float x,y;
  float dirX,dirY;
} position_struct;

//Represents the position of the user
class Position : public Serializable<position_struct>
{
    public:
        //Constructor of Position, it requires the x coordinate, the 
        //y coordinate, the vel velocity of movement of the user,
        //and the component X and Y of the direction vector. 
        Position(float x, float y, float vel, float dirX, float dirY);
        //Sets the direction to the vector represented by dirX and dirY.
        void setDirection(float dirX, float dirY);
        //Update the actual value on the Position at each call. It advance of 1 second
        //since the time step is alwasy the same.
        void updatePosition(int deltaTime);
        //It returns the struct of a Position that can be used for serialization.
        shared_ptr<position_struct> getStruct();
        //The destructor of the Position class.
        ~Position();
        //It returns the datatype that can be used in order to send serialize this object in the struct and send it.
        static MPI_Datatype getMPIType(vector<MPI_Datatype> requiredDatatypes);
    private:
        //x and y coordinates of the position.
        float x,y{0};
        //velocity associated to the user movement
        float vel{0};
        //direction of movement of the user
        float dirX,dirY{0};
    protected:
};

#endif