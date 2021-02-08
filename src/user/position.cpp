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

tuple<int,int> Position::getCoordinates(){
    return {x,y};
}

tuple<int,int> Position::getDirections(){
    return {dirX,dirY};
}