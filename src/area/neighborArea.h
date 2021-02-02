#ifndef NEIGHBORAREA_H
#define NEIGHBORAREA_H

#include "utility.h"
using namespace std;

class NeighborArea{
    public:
        NeighborArea(int processor_rank);
        bool isLocal(int my_processor_rank);
    private:
        int processor_rank;
};

#endif