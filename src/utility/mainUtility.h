#ifndef MAINUTILITY_H
#define MAINUTILITY_H

#include <iostream>
#include <cstring>
#include <tuple>
#include <random>
#include <map> 
#include <memory>

#include "area/area.h"
#include "area/neighborArea.h"
#include "user/user.h"

using namespace std;

//Returns the total number of areas. It returns 0 if they are not multiple.
int getNumberOfAreas(int W,int L, int w,int l);

//Returns the length of row in which the map is divide when generating the areas.
int getStride(int W,int w);

//Returns the length of column in which the map is divide when generating the areas.
int getStrideVertical(int L,int l);

//Returns the vector of Area associated to the provided processor_rank.
vector<shared_ptr<Area>> getArea(int numberOfAreas, int processor_rank, int world_size,int stride, float infectionDistance, int deltaTime);

//Sets correctly the boundaries of the area given the values used to create it.
void setCorrectBoundariesForArea(shared_ptr<Area> area, int w, int l, int L);

#endif