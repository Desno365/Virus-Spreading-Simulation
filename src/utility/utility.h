#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <cstring>
#include <tuple>
#include <random>
#include <map> 
#include <memory>

using namespace std;

//This is the list of possible direction with the vector used to compute the intersections points
//when users go out of an area. for the corner one the equations of north and south are used.
enum Direction {
    NorthWest,
    North,
    NorthEast,
    West,
    Center,
    East,
    SouthWest,
    South,
    SouthEast
    };

//Returns a pointer to a char from a string.
char * fromStringToCharName(string string);

//Returns the direction vector of the given direction.
tuple<int,int> fromDirectionToVector(Direction direction);

//Generates a random float number between max and min.
float generateRandomFloat(float max, float min);

#endif