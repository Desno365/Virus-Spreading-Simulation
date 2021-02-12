#include "./../src/user/user.h"
#include "./../src/area/area.h"

#define DEFAULT_INFECTION_DISTANCE 1.5
#define DEFAULT_DELTA_TIME 1000

shared_ptr<Area> getTestArea(float velocity) {
    // Create users.
    // 6|
    // 5|---------------
    // 4|              |
    // 3|        4     |  5
    // 2|     2        |
    // 1|  1  3        |
    // 0|---------------------
    // 0|--1--2--3--4--5--6--7

    shared_ptr<Position> userPos1 = make_shared<Position>(1,1,velocity,0,1);
    shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, false);

    shared_ptr<Position> userPos2 = make_shared<Position>(2,2,velocity,0,1);
    shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, false);

    shared_ptr<Position> userPos3 = make_shared<Position>(2,1,velocity,0,1);
    shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, true);

    shared_ptr<Position> userPos4 = make_shared<Position>(3,3,velocity,0,1);
    shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, false);

    shared_ptr<Position> userPos5 = make_shared<Position>(6,3,velocity,0,1);
    shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, false);

    // Create area and add users.
    shared_ptr<Area> area = make_shared<Area>(0,0,0, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, 0);
    area->setBoundaries(0,0,5,5);
    area->addUser(newUser1);
    area->addUser(newUser2);
    area->addUser(newUser3);
    area->addUser(newUser4);
    area->addUser(newUser5);

    return area;
}
