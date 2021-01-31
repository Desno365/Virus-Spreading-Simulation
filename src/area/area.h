#ifndef AREA_H
#define AREA_H

#include "user/user.h"
#include <map>

using namespace std;

class Area
{
    public:
        Area(int id);
        void addUser(int userId);
    private:
        int id;
        map<int,User> userInArea;
    protected:

};

#endif