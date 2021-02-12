#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "./../src/user/user.h"
#include "./../src/area/area.h"
#include "./testUtility.cpp"

using namespace std;

// 1) Install Catch2 (it can be installed for example through homebrew with the command "brew install catch2");
// 2) Run the following command: make test -B


TEST_CASE("Area") {

    SECTION("sortUser") {
        // Initialize test area with users with 0 velocity.
        shared_ptr<Area> area = getTestAreaWithoutNearbyUsersRemote(0.0);

        // Make tests on the created area.
        map<int,shared_ptr<User>> userNearInternalBorders1 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers1 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 1 );
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
        REQUIRE( outOfAreaUsers1.size() == 1 );
        REQUIRE( infectedUser == 1 );
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 1 );
        REQUIRE( infectedUser == 1 );
        REQUIRE( immuneUser == 0 );

        area->updateUserInfectionStatus();

        map<int,shared_ptr<User>> userNearInternalBorders3 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers3 = area->getOutOfAreaUsers();
        tie(infectedUser,immuneUser) = area->actuallyInfectedAndImmuneUser();
        REQUIRE( userNearInternalBorders1.size() == 2 );
        REQUIRE( outOfAreaUsers1.size() == 1 );
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
        REQUIRE( outOfAreaUsers1.size() == 1 );

        area->updateUserPositions();

        map<int,shared_ptr<User>> userNearInternalBorders2 = area->getUserNearInternalBorders();
        vector<shared_ptr<User>> outOfAreaUsers2 = area->getOutOfAreaUsers();
        REQUIRE( userNearInternalBorders2.size() == 0 );
        REQUIRE( outOfAreaUsers2.size() == 5 );
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
