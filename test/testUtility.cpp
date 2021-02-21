#include "./../src/user/user.h"
#include "./../src/area/area.h"
#include "./../src/area/neighborArea.h"
#include "./../src/utility/utility.h"

#define DEFAULT_INFECTION_DISTANCE 1.5
#define DEFAULT_DELTA_TIME 500
#define MY_PROCESSOR_RANK 1
#define DEFAULT_AREA_ID 0

shared_ptr<Area> getEmptyArea() {
    shared_ptr<Area> area = make_shared<Area>(0,0,DEFAULT_AREA_ID, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, MY_PROCESSOR_RANK);
    area->setBoundaries(0,0,5,5);
    return area;
}

shared_ptr<Area> getTestAreaWithoutNearbyUsersRemote(float velocity) {
    // Create users.
    // +6   C  |        6        B
    // +5      |---------------
    // +4   8  |              |
    // +3      |        4     |  5
    // +2      |     2        |
    // +1      |  1  3        |
    // +0 -----|---------------------
    // -1   9  |  7              A
    // --2--1--|--1--2--3--4--5--6--7
    // User 3 is infected.

    shared_ptr<Position> userPos1 = make_shared<Position>(1,1,velocity,0.174,0.985);
    shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, false);

    shared_ptr<Position> userPos2 = make_shared<Position>(2,2,velocity,0.174,0.985);
    shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, false);

    shared_ptr<Position> userPos3 = make_shared<Position>(2,1,velocity,0.174,0.985);
    shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, true);

    shared_ptr<Position> userPos4 = make_shared<Position>(3,3,velocity,0.174,0.985);
    shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, false);

    shared_ptr<Position> userPos5 = make_shared<Position>(6,3,velocity,0.985,0.174);
    shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, false);

    shared_ptr<Position> userPos6 = make_shared<Position>(3,6,velocity,0.174,0.985);
    shared_ptr<User> newUser6 = make_shared<User>(6, userPos6, false);

    shared_ptr<Position> userPos7 = make_shared<Position>(1,-1,velocity,0.174,-0.985);
    shared_ptr<User> newUser7 = make_shared<User>(7, userPos7, false);

    shared_ptr<Position> userPos8 = make_shared<Position>(-1,4,velocity,-0.985,0.174);
    shared_ptr<User> newUser8 = make_shared<User>(8, userPos8, false);

    shared_ptr<Position> userPos9 = make_shared<Position>(-1,-1,velocity,0.174,0.985);
    shared_ptr<User> newUser9 = make_shared<User>(9, userPos9, false);

    shared_ptr<Position> userPosA = make_shared<Position>(6,-1,velocity,0.174,0.985);
    shared_ptr<User> newUserA = make_shared<User>(10, userPosA, false);

    shared_ptr<Position> userPosB = make_shared<Position>(6,6,velocity,0.174,0.985);
    shared_ptr<User> newUserB = make_shared<User>(11, userPosB, false);

    shared_ptr<Position> userPosC = make_shared<Position>(-1,6,velocity,0.174,0.985);
    shared_ptr<User> newUserC = make_shared<User>(12, userPosC, false);

    // Create area and add users.
    shared_ptr<Area> area = make_shared<Area>(0,0,DEFAULT_AREA_ID, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, MY_PROCESSOR_RANK);
    area->setBoundaries(0,0,5,5);
    area->addUser(newUser1);
    area->addUser(newUser2);
    area->addUser(newUser3);
    area->addUser(newUser4);
    area->addUser(newUser5);
    area->addUser(newUser6);
    area->addUser(newUser7);
    area->addUser(newUser8);
    area->addUser(newUser9);
    area->addUser(newUserA);
    area->addUser(newUserB);
    area->addUser(newUserC);


    // Create neighbors.
    shared_ptr<NeighborArea> neighborArea1 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,1);
    shared_ptr<NeighborArea> neighborArea2 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,2);
    shared_ptr<NeighborArea> neighborArea3 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,3);
    shared_ptr<NeighborArea> neighborArea4 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,4);
    shared_ptr<NeighborArea> neighborArea6 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,6);
    shared_ptr<NeighborArea> neighborArea7 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,7);
    shared_ptr<NeighborArea> neighborArea8 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,8);
    shared_ptr<NeighborArea> neighborArea9 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,9);

    // Add neighbors.
    area->setNeighborArea(neighborArea1, SouthWest);
    area->setNeighborArea(neighborArea2, South);
    area->setNeighborArea(neighborArea3, SouthEast);
    area->setNeighborArea(neighborArea4, West);
    area->setNeighborArea(neighborArea6, East);
    area->setNeighborArea(neighborArea7, NorthWest);
    area->setNeighborArea(neighborArea8, North);
    area->setNeighborArea(neighborArea9, NorthEast);


    return area;
}

shared_ptr<Area> getTestAreaWithoutNearbyUsersRemoteAndWithExtraRegionUsers(float velocity) {
    // Create users.
    // +6   C  |        6        B
    // +5      |---------------
    // +4   8  |              |
    // +3      |        4     |  5
    // +2      |     2        |
    // +1      |  1  3        |
    // +0 -----|---------------------
    // -1   9  |  7              A
    // --2--1--|--1--2--3--4--5--6--7
    // User 3 is infected.

    shared_ptr<Position> userPos1 = make_shared<Position>(1,1,velocity,0.174,0.985);
    shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, false);

    shared_ptr<Position> userPos2 = make_shared<Position>(2,2,velocity,0.174,0.985);
    shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, false);

    shared_ptr<Position> userPos3 = make_shared<Position>(2,1,velocity,0.174,0.985);
    shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, true);

    shared_ptr<Position> userPos4 = make_shared<Position>(3,3,velocity,0.174,0.985);
    shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, false);

    shared_ptr<Position> userPos5 = make_shared<Position>(6,3,velocity,0.985,0.174);
    shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, false);

    shared_ptr<Position> userPos6 = make_shared<Position>(3,6,velocity,0.174,0.985);
    shared_ptr<User> newUser6 = make_shared<User>(6, userPos6, false);

    shared_ptr<Position> userPos7 = make_shared<Position>(1,-1,velocity,0.174,-0.985);
    shared_ptr<User> newUser7 = make_shared<User>(7, userPos7, false);

    shared_ptr<Position> userPos8 = make_shared<Position>(-1,4,velocity,-0.985,0.174);
    shared_ptr<User> newUser8 = make_shared<User>(8, userPos8, false);

    shared_ptr<Position> userPos9 = make_shared<Position>(-1,-1,velocity,0.174,0.985);
    shared_ptr<User> newUser9 = make_shared<User>(9, userPos9, false);

    shared_ptr<Position> userPosA = make_shared<Position>(6,-1,velocity,0.174,0.985);
    shared_ptr<User> newUserA = make_shared<User>(10, userPosA, false);

    shared_ptr<Position> userPosB = make_shared<Position>(6,6,velocity,0.174,0.985);
    shared_ptr<User> newUserB = make_shared<User>(11, userPosB, false);

    shared_ptr<Position> userPosC = make_shared<Position>(-1,6,velocity,0.174,0.985);
    shared_ptr<User> newUserC = make_shared<User>(12, userPosC, false);

    // Create area and add users.
    shared_ptr<Area> area = make_shared<Area>(0,0,DEFAULT_AREA_ID, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, MY_PROCESSOR_RANK);
    area->setBoundaries(0,0,5,5);
    area->addUser(newUser1);
    area->addUser(newUser2);
    area->addUser(newUser3);
    area->addUser(newUser4);
    area->addUser(newUser5);
    area->addUser(newUser6);
    area->addUser(newUser7);
    area->addUser(newUser8);
    area->addUser(newUser9);
    area->addUser(newUserA);
    area->addUser(newUserB);
    area->addUser(newUserC);


    // Create neighbors.
    shared_ptr<NeighborArea> neighborArea6 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,6);
    shared_ptr<NeighborArea> neighborArea8 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,8);
    shared_ptr<NeighborArea> neighborArea9 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,9);

    // Add neighbors.
    area->setNeighborArea(neighborArea6, East);
    area->setNeighborArea(neighborArea8, North);
    area->setNeighborArea(neighborArea9, NorthEast);


    return area;
}

shared_ptr<Area> getTestAreaWithNearbyUsersRemote(float velocity) {
    // Create users.
    // +6      |        5
    // +5      |--------4------
    // +4      |              |
    // +3      |              |
    // +2      |     2        |
    // +1      |  1  3        |
    // +0 -----|---------------------
    // -1      |
    // --2--1--|--1--2--3--4--5--6--7
    // User 5 is infected.

    shared_ptr<Position> userPos1 = make_shared<Position>(1,1,velocity,0.174,0.985);
    shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, false);

    shared_ptr<Position> userPos2 = make_shared<Position>(2,2,velocity,0.174,0.985);
    shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, false);

    shared_ptr<Position> userPos3 = make_shared<Position>(2,1,velocity,0.174,0.985);
    shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, false);

    shared_ptr<Position> userPos4 = make_shared<Position>(3,4.99,velocity,0.174,0.985);
    shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, false);

    shared_ptr<Position> userPos5 = make_shared<Position>(3,6,velocity,0.174,0.985);
    shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, true);

    // Create area and add users.
    shared_ptr<Area> area = make_shared<Area>(0,0,DEFAULT_AREA_ID, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, MY_PROCESSOR_RANK);
    area->setBoundaries(0,0,5,5);
    area->addUser(newUser1);
    area->addUser(newUser2);
    area->addUser(newUser3);
    area->addUser(newUser4);
    vector<shared_ptr<user_struct>> userStructs;
    userStructs.push_back(newUser5->getStruct());
    area->addNearbyUsersRemote(userStructs);


    // Create neighbors.
    shared_ptr<NeighborArea> neighborArea1 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,1);
    shared_ptr<NeighborArea> neighborArea2 = make_shared<NeighborArea>(1,2);
    shared_ptr<NeighborArea> neighborArea3 = make_shared<NeighborArea>(1,3);

    // Add neighbors.
    area->setNeighborArea(neighborArea1, East);
    area->setNeighborArea(neighborArea2, North);
    area->setNeighborArea(neighborArea3, NorthEast);

    return area;
}

shared_ptr<Area> getTestAreaWithNearBorderUsers(float velocity) {
    // Create users.
    // +6      |
    // +5      |---------------
    // +4      |        6     |
    // +3      |  3  4     5  |
    // +2      |        2     |
    // +1      |        1     |
    // +0 -----|---------------------
    // -1      |
    // --2--1--|--1--2--3--4--5--6--7
    // All users are infected.

    shared_ptr<Position> userPos1 = make_shared<Position>(3,1,velocity,0.174,0.985);
    shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, true);

    shared_ptr<Position> userPos2 = make_shared<Position>(3,2,velocity,0.174,0.985);
    shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, true);

    shared_ptr<Position> userPos3 = make_shared<Position>(1,3,velocity,0.174,0.985);
    shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, true);

    shared_ptr<Position> userPos4 = make_shared<Position>(2,3,velocity,0.174,0.985);
    shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, true);

    shared_ptr<Position> userPos5 = make_shared<Position>(4,3,velocity,0.174,0.985);
    shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, true);

    shared_ptr<Position> userPos6 = make_shared<Position>(3,4,velocity,0.174,0.985);
    shared_ptr<User> newUser6 = make_shared<User>(6, userPos6, true);


    // Create area and add users.
    shared_ptr<Area> area = make_shared<Area>(0,0,DEFAULT_AREA_ID, DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME, MY_PROCESSOR_RANK);
    area->setBoundaries(0,0,5,5);
    area->addUser(newUser1);
    area->addUser(newUser2);
    area->addUser(newUser3);
    area->addUser(newUser4);
    area->addUser(newUser5);
    area->addUser(newUser6);


    // Create neighbors.
    shared_ptr<NeighborArea> neighborArea1 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,1);
    shared_ptr<NeighborArea> neighborArea2 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,2);
    shared_ptr<NeighborArea> neighborArea3 = make_shared<NeighborArea>(MY_PROCESSOR_RANK-1,3);
    shared_ptr<NeighborArea> neighborArea4 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,4);
    shared_ptr<NeighborArea> neighborArea6 = make_shared<NeighborArea>(MY_PROCESSOR_RANK,6);
    shared_ptr<NeighborArea> neighborArea7 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,7);
    shared_ptr<NeighborArea> neighborArea8 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,8);
    shared_ptr<NeighborArea> neighborArea9 = make_shared<NeighborArea>(MY_PROCESSOR_RANK+1,9);

    // Add neighbors.
    area->setNeighborArea(neighborArea1, SouthWest);
    area->setNeighborArea(neighborArea2, South);
    area->setNeighborArea(neighborArea3, SouthEast);
    area->setNeighborArea(neighborArea4, West);
    area->setNeighborArea(neighborArea6, East);
    area->setNeighborArea(neighborArea7, NorthWest);
    area->setNeighborArea(neighborArea8, North);
    area->setNeighborArea(neighborArea9, NorthEast);


    return area;
}
