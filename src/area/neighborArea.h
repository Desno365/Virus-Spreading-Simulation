#ifndef NEIGHBORAREA_H
#define NEIGHBORAREA_H

#include "utility/utility.h"

using namespace std;

//This object represents a bordering area.
class NeighborArea{
    public:
        //Constructs an instance of a bordering area, it requires the rank of the processor that manages this
        //bordering area, and the id of such an area.
        NeighborArea(int processor_rank, int id);
        //Returns true if the area is local, so the processor managing the calling area and this bordering area
        //is the same.
        bool isLocal(int my_processor_rank);
        //Is the id of this area.
        int getID();
        //Is the rank of the processor managing this bordering area.
        int getOtherProcessorRank();
    private:
        int processor_rank;
        int id;
};

#endif