#include "mainUtility.h"

using namespace std;

//Is the size of the matrix that is considered as a neighborhoods an Area.
//NOTE: it has to be ODD.
#define NEIGHBOR_DISTANCE 3

int getNumberOfAreas(int W,int L, int w,int l){
    if(W%w==0 && L%l==0){
        return W/w * L/l;
    }else
        return 0;
}

int getStride(int W,int w){
    return W/w;
}

int getStrideVertical(int L,int l){
    return L/l;
}

//NOTE: the ids of the areas are assigned from left to right, top to bottom in increasing order.
vector<shared_ptr<Area>> getArea(int numberOfAreas, int processor_rank, int world_size,int stride, float infectionDistance, int deltaTime){
    vector<shared_ptr<Area>> areas;
    int minAreasForProcessor = numberOfAreas / world_size;
    int maxAreasForProcessor = numberOfAreas % world_size == 0 ? minAreasForProcessor : minAreasForProcessor + 1;
    int leftOutAreas = numberOfAreas % world_size;
    int startingAreaID,endingAreaID;
    if(processor_rank<leftOutAreas){
        startingAreaID = maxAreasForProcessor*(processor_rank);
        endingAreaID = startingAreaID+maxAreasForProcessor;
    }else{
        startingAreaID = maxAreasForProcessor*leftOutAreas + minAreasForProcessor*(processor_rank-leftOutAreas);
        endingAreaID = startingAreaID+minAreasForProcessor;
    }
    for(int i=startingAreaID; i<endingAreaID;i++){
        shared_ptr<Area> newArea = make_shared<Area>(i%stride,i/stride,i,infectionDistance, deltaTime, processor_rank);
        areas.push_back(newArea);
    }
    for(shared_ptr<Area> area:areas){
        tuple<int,int> areaCor = area->getCoordinate();
        int neighborStartCol = get<0>(areaCor)-(int)(NEIGHBOR_DISTANCE/2);
        int neighborStartRow = get<1>(areaCor)-(int)(NEIGHBOR_DISTANCE/2);
        //Starts from the top and go down to the right
        for(int i=0;i<NEIGHBOR_DISTANCE;i++){//i is the Row
            for (int j = 0; j < NEIGHBOR_DISTANCE; j++)//j is the Col
            {
                int neighborAreaCol = neighborStartCol+j;
                int neighborAreaRow = neighborStartRow+i;
                if(neighborAreaCol>=0 && neighborAreaRow>=0 && neighborAreaCol<stride && neighborAreaRow<numberOfAreas/stride && (neighborAreaCol!=get<0>(areaCor) || neighborAreaRow!=get<1>(areaCor))){
                    int neighborID = neighborAreaCol+neighborAreaRow*stride;
                    int processorArea;
                    if(neighborID<maxAreasForProcessor*(leftOutAreas)){
                        processorArea = neighborID/maxAreasForProcessor;
                    }else{
                        int temp = neighborID-leftOutAreas*maxAreasForProcessor;
                        processorArea = leftOutAreas + temp/minAreasForProcessor;
                    }
                    //TODO check the value of the neighborID.
                    shared_ptr<NeighborArea> neighborArea = make_shared<NeighborArea>(processorArea,neighborID);
                    area->setNeighborArea(neighborArea,(Direction)(i*NEIGHBOR_DISTANCE+j));
                }
            }            
        }
    }
    return areas;
}

void setCorrectBoundariesForArea(shared_ptr<Area> area, int w, int l, int L) {
    int lowerX = w * area->getCol();
    int lowerY = l * (getStrideVertical(L,l)-1-area->getRow());
    int higherX = lowerX + w;
    int higherY = lowerY + l;
    area->setBoundaries(lowerX,lowerY,higherX,higherY);
}


