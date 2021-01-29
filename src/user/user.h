#ifndef USER_H
#define USER_H

#include <iostream>
#include <algorithm>
#include <vector>
#include "position.h"

using namespace std;

class User
{
    public:
        User(int id,Position pos);
        Position pos;
    private:
        int id;
    protected:
};


#endif