#include "neighborArea.h"

NeighborArea::NeighborArea(int processor_rank, int id):processor_rank(processor_rank),id(id){}

bool NeighborArea::isLocal(int my_processor_rank){
    return my_processor_rank==this->processor_rank;
}

int NeighborArea::getID(){
    return id;
}