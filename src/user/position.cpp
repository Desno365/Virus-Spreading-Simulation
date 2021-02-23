#include "position.h"

Position::Position(float x, float y, float vel, float dirX, float dirY):x(x),y(y),vel(vel),dirX(dirX),dirY(dirY){}

void Position::updatePosition(int deltaTime){
    this->x += ((float)deltaTime) * this->vel * dirX;
    this->y += ((float)deltaTime) * this->vel * dirY;
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
    //Rewrite the input equation as ax + by + c = 0
    float a = coefX;
    float b = coefY;
    float c = noteTerm;
    //Rewrite the moving direction of out point as dx + ey + f = 0
    float d = 1/dirX;
    float e = -1/dirY;
    float f = y/dirY - x/dirX;
    //The solution is given by:
    float denominator = (a*e-d*b);
    x = (f*b-c*e) / denominator;
    y = (c*d-f*a) / denominator; 
}

bool Position::setCoordinatesInsideGlobalArea(int maxX, int maxY){
    bool hasChanged = false;
    if(x>(float)maxX){
        x=maxX;
        hasChanged=true;
    }else if(x<0.0f){
        x=0.0f;
        hasChanged=true;
    }

    if(y>(float)maxY){
        y=maxY;
        hasChanged=true;
    }else if(y<0.0f){
        y=0.0f;
        hasChanged=true;
    }

    return hasChanged;
}