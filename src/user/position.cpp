#include "position.h"

Position::Position(int x, int y, int vel, int dirX, int dirY):x(x),y(y),vel(vel),dirX(dirX),dirY(dirY){}

void Position::setDirection(int dirX, int dirY){
    this->dirX = dirX;
    this->dirY = dirY;
}

void Position::setVelocity(int vel){
    this->vel = vel;
}

void Position::updatePosition(int deltaTime){
    //TODO update the position of the user
}