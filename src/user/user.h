#ifndef USER_H
#define USER_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>
#include "position.h"
#include "serializable.h"

using namespace std;

typedef struct user_struct_t {
  int id;
  bool infected;
  position_struct pos_t;
} user_struct;

class User: public Serializable<user_struct>
{
    public:
        User(int id,Position pos);
        Position pos;
        void setInfected(bool);
        bool isInfected();
        int getId();
        ~User();
        shared_ptr<user_struct> getStruct();
        //It returns the datatype that can be used in order to send serialize this object in the struct and send it.
        static MPI_Datatype getMPIType(vector<MPI_Datatype> requiredDatatypes);
    private:
        int id;
        bool infected;
    protected:
};

#endif