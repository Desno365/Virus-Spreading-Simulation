#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "./../src/user/user.h"
#include "./../src/area/area.h"
#include "utility/mainUtility.h"
#include "utility/utility.h"
#include "./testUtility.cpp"

using namespace std;

// 1) Install Catch2 (it can be installed for example through homebrew with the command "brew install catch2");
// 2) Run the following command: make test -B

TEST_CASE("Main Utility") {
    SECTION("getArea") {
        int W = 10;
        int L = 10;
        int w = 5;
        int l = 5;
        int number_of_areas = getNumberOfAreas(W,L,w,l); // W must be multiple of w. L must be multiple of l.

        // Test areas of processor 0.
        vector<shared_ptr<Area>> processorAreas0 = getArea(number_of_areas, 0, 4, getStride(W,w), DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME);
        REQUIRE( processorAreas0.size() == 1 );

        shared_ptr<Area> area0 = processorAreas0[0];
        setCorrectBoundariesForArea(area0, w, l, L);
        float x0, y0;
        tie(x0,y0) = area0->getRadomCoordinates();
        map<Direction,shared_ptr<NeighborArea>> neighborAreas0 = area0->getNeighborAreas();
        REQUIRE( area0->getRow() == 0 );
        REQUIRE( area0->getCol() == 0 );
        REQUIRE( (x0 >= 0.0 && x0 <= 5.0 && y0 >= 5.0 && y0 <= 10.0) == true );
        REQUIRE( neighborAreas0.size() == 3 );
        REQUIRE( neighborAreas0.at(East)->getOtherProcessorRank() == 1 );
        REQUIRE( neighborAreas0.at(South)->getOtherProcessorRank() == 2 );
        REQUIRE( neighborAreas0.at(SouthEast)->getOtherProcessorRank() == 3 );

        // Test areas of processor 1.
        vector<shared_ptr<Area>> processorAreas1 = getArea(number_of_areas, 1, 4, getStride(W,w), DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME);
        REQUIRE( processorAreas1.size() == 1 );

        shared_ptr<Area> area1 = processorAreas1[0];
        setCorrectBoundariesForArea(area1, w, l, L);
        float x1, y1;
        tie(x1,y1) = area1->getRadomCoordinates();
        map<Direction,shared_ptr<NeighborArea>> neighborAreas1 = area1->getNeighborAreas();
        REQUIRE( area1->getRow() == 0 );
        REQUIRE( area1->getCol() == 1 );
        REQUIRE( (x1 >= 5.0 && x1 <= 10.0 && y1 >= 5.0 && y1 <= 10.0) == true );
        REQUIRE( neighborAreas1.size() == 3 );
        REQUIRE( neighborAreas1.at(West)->getOtherProcessorRank() == 0 );
        REQUIRE( neighborAreas1.at(South)->getOtherProcessorRank() == 3 );
        REQUIRE( neighborAreas1.at(SouthWest)->getOtherProcessorRank() == 2 );

        // Test areas of processor 2.
        vector<shared_ptr<Area>> processorAreas2 = getArea(number_of_areas, 2, 4, getStride(W,w), DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME);
        REQUIRE( processorAreas2.size() == 1 );

        shared_ptr<Area> area2 = processorAreas2[0];
        setCorrectBoundariesForArea(area2, w, l, L);
        float x2, y2;
        tie(x2,y2) = area2->getRadomCoordinates();
        map<Direction,shared_ptr<NeighborArea>> neighborAreas2 = area2->getNeighborAreas();
        REQUIRE( area2->getRow() == 1 );
        REQUIRE( area2->getCol() == 0 );
        REQUIRE( (x2 >= 0.0 && x2 <= 5.0 && y2 >= 0.0 && y2 <= 5.0) == true );
        REQUIRE( neighborAreas2.size() == 3 );
        REQUIRE( neighborAreas2.at(East)->getOtherProcessorRank() == 3 );
        REQUIRE( neighborAreas2.at(North)->getOtherProcessorRank() == 0 );
        REQUIRE( neighborAreas2.at(NorthEast)->getOtherProcessorRank() == 1 );

        // Test areas of processor 3.
        vector<shared_ptr<Area>> processorAreas3 = getArea(number_of_areas, 3, 4, getStride(W,w), DEFAULT_INFECTION_DISTANCE, DEFAULT_DELTA_TIME);
        REQUIRE( processorAreas3.size() == 1 );

        shared_ptr<Area> area3 = processorAreas3[0];
        setCorrectBoundariesForArea(area3, w, l, L);
        float x3, y3;
        tie(x3,y3) = area3->getRadomCoordinates();
        map<Direction,shared_ptr<NeighborArea>> neighborAreas3 = area3->getNeighborAreas();
        REQUIRE( area3->getRow() == 1 );
        REQUIRE( area3->getCol() == 1 );
        REQUIRE( (x3 >= 5.0 && x3 <= 10.0 && y3 >= 0.0 && y3 <= 5.0) == true );
        REQUIRE( neighborAreas3.size() == 3 );
        REQUIRE( neighborAreas3.at(West)->getOtherProcessorRank() == 2 );
        REQUIRE( neighborAreas3.at(North)->getOtherProcessorRank() == 1 );
        REQUIRE( neighborAreas3.at(NorthWest)->getOtherProcessorRank() == 0 );
    }
}


TEST_CASE("Area") {

    SECTION("printArea") {
        // Initialize test area with users with near 0 velocity.
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(0.1);

        string fileName = "./outputs/print-area/test.txt";
        char * str = fromStringToCharName(fileName);
        FILE *fptr = fopen(str,"w");
        if(fptr == NULL){
            printf("Error in opening %s!",str);
            exit(1);
        }
        free(str);

        area->printArea(fptr, 0);
    }

    SECTION("getNewUserFromRemoteLocation") {
        // Initialize empty area.
        shared_ptr<Area> area = getEmptyArea();

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders1.size() == 0 );
        REQUIRE( outOfAreaUsers1.size() == 0 );

        // Create newUsers vector.
        vector<shared_ptr<User>> newUsers;

        // User not inside.
        shared_ptr<Position> userPos1 = make_shared<Position>(6,6,0.1,0.174,0.985);
        shared_ptr<User> newUser1 = make_shared<User>(1, userPos1, false);
        newUsers.push_back(newUser1);

        // User inside.
        shared_ptr<Position> userPos2 = make_shared<Position>(1,1,0.1,0.174,0.985);
        shared_ptr<User> newUser2 = make_shared<User>(2, userPos2, false);
        newUsers.push_back(newUser2);

        // User not inside.
        shared_ptr<Position> userPos3 = make_shared<Position>(6,6,0.1,0.174,0.985);
        shared_ptr<User> newUser3 = make_shared<User>(3, userPos3, false);
        newUsers.push_back(newUser3);

        // User inside.
        shared_ptr<Position> userPos4 = make_shared<Position>(1,1,0.1,0.174,0.985);
        shared_ptr<User> newUser4 = make_shared<User>(4, userPos4, false);
        newUsers.push_back(newUser4);

        // User not inside.
        shared_ptr<Position> userPos5 = make_shared<Position>(6,6,0.1,0.174,0.985);
        shared_ptr<User> newUser5 = make_shared<User>(5, userPos5, false);
        newUsers.push_back(newUser5);

        // Make tests on the created vector.
        REQUIRE( newUsers.size() == 5 );

        area->getNewUserFromRemoteLocation(&newUsers);

        // Make tests on the area.
        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders2.size() == 2 );
        REQUIRE( outOfAreaUsers2.size() == 0 );

        // Make tests on the vector.
        REQUIRE( newUsers.size() == 3 );
        REQUIRE( newUsers[0]->getId() == 1 );
        REQUIRE( newUsers[1]->getId() == 3 );
        REQUIRE( newUsers[2]->getId() == 5 );
    }

    SECTION("sortUser") {
        // Initialize test area with users with 0 velocity.
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(0.0);

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 8 );
    }

    SECTION("updateUserInfectionStatus without nearby users remote") {
        int infectedUser,immuneUser;

        // Initialize test area with users with 0 velocity.
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(0.0); // Using area without nearby users remote!

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 8 );
        REQUIRE( infectedUser == 1 );
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 8 );
        REQUIRE( infectedUser == 1 );
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders3 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers3 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 8 );
        REQUIRE( infectedUser == 3 );
        REQUIRE( immuneUser == 0 );
    }

    SECTION("updateUserInfectionStatus with nearby users remote") {
        int infectedUser,immuneUser;

        // Initialize test area with users with 0 velocity.
        shared_ptr<Area> area = getTestAreaWithNearbyUsersRemote(0.0); // Using area with nearby users remote!

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 3 );
        REQUIRE( outOfAreaUsers1.size() == 0 );
        REQUIRE( infectedUser == 0 ); // The only infected user is outside the area.
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 3 );
        REQUIRE( outOfAreaUsers1.size() == 0 );
        REQUIRE( infectedUser == 0 ); // The only infected user is outside the area.
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders3 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers3 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 3 );
        REQUIRE( outOfAreaUsers1.size() == 0 );
        REQUIRE( infectedUser == 1 );
        REQUIRE( immuneUser == 0 );
    }

    SECTION("updateUserPositions") {
        // Initialize test area with users with very big velocity (so they exit the area).
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(9999999.9);

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 8 );

        area->updateUserPositions();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders2.size() == 0 );
        REQUIRE( outOfAreaUsers2.size() == 12 );
    }

    SECTION("computeOutOfAreaUserMap, without extra region users") {
        // Initialize test area with users with near 0 velocity (note: velocity can't be zero otherwise exiting from the area is impossible and causes an infinite loop).
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(0.1);

        // Make tests on the created area.
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> outOfAreaUsersLocal1 = area->getOutOfAreaUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> outOfAreaUsersRemote1 = area->getOutOfAreaUsersRemote();
        REQUIRE( outOfAreaUsers1.size() == 8 );
        REQUIRE( outOfAreaUsersLocal1.size() == 0 );
        REQUIRE( outOfAreaUsersRemote1.size() == 0 );

        area->computeOutOfAreaUserMap();

        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> outOfAreaUsersLocal2 = area->getOutOfAreaUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> outOfAreaUsersRemote2 = area->getOutOfAreaUsersRemote();
        REQUIRE( outOfAreaUsers2.size() == 8 );
        REQUIRE( outOfAreaUsersLocal2.size() == 2 );
        REQUIRE( outOfAreaUsersRemote2.size() == 2 );
        REQUIRE( outOfAreaUsersRemote2.at(MY_PROCESSOR_RANK-1)->size() == 3 );
        REQUIRE( outOfAreaUsersRemote2.at(MY_PROCESSOR_RANK+1)->size() == 3 );
    }

    SECTION("computeOutOfAreaUserMap, with extra region users") {
        // Initialize test area with users with near 0 velocity (note: velocity can't be zero otherwise exiting from the area is impossible and causes an infinite loop).
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemoteAndWithExtraRegionUsers(0.1);

        // Make tests on the created area.
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> outOfAreaUsersLocal1 = area->getOutOfAreaUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> outOfAreaUsersRemote1 = area->getOutOfAreaUsersRemote();
        REQUIRE( outOfAreaUsers1.size() == 8 );
        REQUIRE( outOfAreaUsersLocal1.size() == 0 );
        REQUIRE( outOfAreaUsersRemote1.size() == 0 );

        area->computeOutOfAreaUserMap();

        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> outOfAreaUsersLocal2 = area->getOutOfAreaUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> outOfAreaUsersRemote2 = area->getOutOfAreaUsersRemote();
        REQUIRE( outOfAreaUsers2.size() == 8 );
        REQUIRE( outOfAreaUsersLocal2.size() == 1 );
        REQUIRE( outOfAreaUsersRemote2.size() == 1 );
        REQUIRE( outOfAreaUsersRemote2.at(MY_PROCESSOR_RANK+1)->size() == 2 );
    }

    SECTION("computeNearBorderUserMap, without extra region users") {
        // Initialize test area with users with near 0 velocity (note: velocity can't be zero otherwise exiting from the area is impossible and causes an infinite loop).
        shared_ptr<Area> area = getTestAreaWithNearBorderUsers(0.1);

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> nearBorderUsersLocal1 = area->getNearBorderUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> nearBorderUsersRemote1 = area->getNearBorderUsersRemote();
        REQUIRE( userNearInternalBorders1.size() == 4 );
        REQUIRE( outOfAreaUsers1.size() == 0 );
        REQUIRE( nearBorderUsersLocal1.size() == 0 );
        REQUIRE( nearBorderUsersRemote1.size() == 0 );

        area->computeNearBorderUserMap();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        map<int,shared_ptr<vector<shared_ptr<User>>>> nearBorderUsersLocal2 = area->getNearBorderUsersLocal();
        map<int,shared_ptr<vector<shared_ptr<user_struct>>>> nearBorderUsersRemote2 = area->getNearBorderUsersRemote();
        REQUIRE( userNearInternalBorders2.size() == 4 );
        REQUIRE( outOfAreaUsers2.size() == 0 );
        REQUIRE( nearBorderUsersLocal2.size() == 2 );
        REQUIRE( nearBorderUsersRemote2.size() == 2 );
        REQUIRE( nearBorderUsersRemote2.at(MY_PROCESSOR_RANK-1)->size() == 1 );
        REQUIRE( nearBorderUsersRemote2.at(MY_PROCESSOR_RANK+1)->size() == 1 );
    }

}

TEST_CASE("User") {

    SECTION("updateUserInfectionState") {
        // Create user not infected.
        shared_ptr<Position> userPos = make_shared<Position>(0,0,1,0,0);
        shared_ptr<User> newUser = make_shared<User>(0, userPos, false);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User alone.
        newUser->updateUserInfectionState(false, 1);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User near infected for a short time.
        newUser->updateUserInfectionState(true, 500);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User alone.
        newUser->updateUserInfectionState(false, 1);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User near infected for a short time.
        newUser->updateUserInfectionState(true, 500);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User still near infected, should become infected (surpassed TIME_NEAR_INFECTED 600).
        newUser->updateUserInfectionState(true, 200);
        REQUIRE( newUser->isInfected() == true );
        REQUIRE( newUser->isImmune() == false );

        // User still near infected, should remain infected.
        newUser->updateUserInfectionState(true, 1);
        REQUIRE( newUser->isInfected() == true );
        REQUIRE( newUser->isImmune() == false );

        // User not near infected, should remain infected.
        newUser->updateUserInfectionState(false, 1);
        REQUIRE( newUser->isInfected() == true );
        REQUIRE( newUser->isImmune() == false );

        // User not near infected for a long period, should become immune (surpassed INFECTED_TIME 864000).
        newUser->updateUserInfectionState(false, 864999);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == true );

        // User not near infected, should remain immune.
        newUser->updateUserInfectionState(false, 100);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == true );

        // User near infected for a long time, should remain immune (surpassed TIME_NEAR_INFECTED 600).
        newUser->updateUserInfectionState(true, 650);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == true );

        // User not near infected for a very long time, should stop being immune (surpassed IMMUNE_TIME 7776000).
        newUser->updateUserInfectionState(false, 7776999);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User alone.
        newUser->updateUserInfectionState(false, 1);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User near infected for a short time.
        newUser->updateUserInfectionState(true, 500);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User alone.
        newUser->updateUserInfectionState(false, 1);
        REQUIRE( newUser->isInfected() == false );
        REQUIRE( newUser->isImmune() == false );

        // User near infected for a long time (surpassed TIME_NEAR_INFECTED 600).
        newUser->updateUserInfectionState(true, 699);
        REQUIRE( newUser->isInfected() == true );
        REQUIRE( newUser->isImmune() == false );
    }

    SECTION("isNear true") {
        shared_ptr<Position> userPos = make_shared<Position>(0,0,1,1,0);
        shared_ptr<User> newUser = make_shared<User>(0, userPos, false);
        REQUIRE( newUser->isNear(5, 5, 8) == true );
        REQUIRE( newUser->isNear(-5, -5, 8) == true );
    }
    SECTION("isNear false") {
        shared_ptr<Position> userPos = make_shared<Position>(0,0,1,1,0);
        shared_ptr<User> newUser = make_shared<User>(0, userPos, false);
        REQUIRE( newUser->isNear(5, 5, 7) == false );
        REQUIRE( newUser->isNear(-5, -5, 7) == false );
    }

    SECTION("updateUserPosition and getStruct") {
        // Initialize user1.
        shared_ptr<Position> userPos1 = make_shared<Position>(0,0,1,1,0);
        shared_ptr<User> newUser1 = make_shared<User>(0, userPos1, false);

        // Update user1 position.
        newUser1->updateUserPosition(2);
        REQUIRE( newUser1->getStruct()->x == 2 );
        REQUIRE( newUser1->getStruct()->y == 0 );

        // Update user1 position.
        newUser1->updateUserPosition(2);
        REQUIRE( newUser1->getStruct()->x == 4 );
        REQUIRE( newUser1->getStruct()->y == 0 );

        // Initialize user2.
        shared_ptr<Position> userPos2 = make_shared<Position>(0,0,1,0,1);
        shared_ptr<User> newUser2 = make_shared<User>(0, userPos2, false);

        // Update user2 position.
        newUser2->updateUserPosition(1);
        REQUIRE( newUser2->getStruct()->x == 0 );
        REQUIRE( newUser2->getStruct()->y == 1 );

        // Update user2 position.
        newUser2->updateUserPosition(1);
        REQUIRE( newUser2->getStruct()->x == 0 );
        REQUIRE( newUser2->getStruct()->y == 2 );
    }
}
