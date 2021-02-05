#ifndef NEIGHBORAREA_H
#define NEIGHBORAREA_H

#include "utility.h"
using namespace std;

class NeighborArea{
    public:
        NeighborArea(int processor_rank, int id);
        bool isLocal(int my_processor_rank);
        int getID();
    private:
        int processor_rank;
        int id;
};

#endif