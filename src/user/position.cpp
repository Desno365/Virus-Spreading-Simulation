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

tuple<float,float> Position::getCoordinates(){
    return {x,y};
}

tuple<float,float> Position::getDirections(){
    return {dirX,dirY};
}

void Position::updateDirections(float newDirx, float newDirY){
    dirX = newDirx;
    dirY = newDirY;
}

//NOTE: follow this link https://www.codeproject.com/Articles/42727/Equation-Solver-in-C
void Position::goBackToIntersection(float coefX, float coefY, float noteTerm){
    //Rewrite the input equaiton as ax + by + c = 0
    int a = coefX;
    int b = coefY;
    int c = noteTerm;
    //Rewrite the moving direction of out point as dx + ey + f = 0
    int d = 1/dirX;
    int e = -1/dirY;
    int f = y/dirY - x/dirX;
    //The solution is given by:
    float denominator = (a*e-d*b);
    x = (f*b-c*e) / denominator;
    y = (c*d-f*a) / denominator; 
}