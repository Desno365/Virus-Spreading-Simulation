#include "neighborArea.h"

NeighborArea::NeighborArea(int processor_rank):processor_rank(processor_rank){}

bool NeighborArea::isLocal(int my_processor_rank){
    return my_processor_rank==this->processor_rank;
}