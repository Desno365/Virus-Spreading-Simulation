#ifndef POSITION_H
#define POSITION_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include "serializable.h"

using namespace std;

//Represents the position of the user
class Position
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
        //Returns X.
        int getX() { return x; };
        //Returns Y.
        int getY() { return y; };
        //Returns a tuple with (X,Y);
        tuple<int,int> getCoordinates();
        //Returns a tuple with (dirX,dirY).
        tuple<int,int> getDirections();
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