#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "./../src/user/user.h"

// 1) Install Catch2 (it can be installed for example through homebrew with the command "brew install catch2");
// 2) Run the following command: g++ -std=c++11 -I./src -Wall -o ./test.0 ./test/test.cpp && ./test.0 --success

static int Factorial( int number ) {
    return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
    // return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
}

TEST_CASE( "User isNear" ) {
    shared_ptr<Position> userPos = make_shared<Position>(1,1,1,1,1);
    shared_ptr<User> newUser = make_shared<User>(0, userPos, false);
    REQUIRE( newUser->isNear(2, 2, 1000000) == true );
}

TEST_CASE( "Factorials of 1 and higher are computed (pass)", "[single-file]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}