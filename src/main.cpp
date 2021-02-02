#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <stdlib.h> 
#include <list>
#include <tuple>
#include <unistd.h> //For debugging
     
#include "user/user.h"
#include "area/area.h"

using namespace std;
//Is the number of parameters that can be passed when the program execute
#define NUMBER_OF_PARAMETERS 9
//Is the size of the matrix that is considered as a neighborhoodof an Area.
//NOTE: it has to be ODD.
#define NEIGHBOR_DISTANCE 3

//Retuns the value associated to the option between the two pointers
char* getCmdOption(char ** begin, char ** end, const string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

//Retuns true if in the between the two pointer the option string has been found
bool cmdOptionExists(char** begin, char** end, const string& option)
{
    return find(begin, end, option) != end;
}

//Returns the total number of areas. It returns 0 if they are not multiple.
int getNumberOfAreas(int W,int L, int w,int l);

//Returns the lenght of row in which the map is divide when generating the areas.
int getStride(int W,int w);

//Returns the vector of Area associated to the provided processor_rank.
vector<Area> getArea(int numberOfAreas, int processor_rank, int world_size,int stride);


static int prova;
int main(int argc, char** argv) {



    MPI_Init(NULL, NULL);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //Used only for debugging comment this part below if not interested
    if(my_rank==0){//User for debugging removed this if not interested.
        volatile int ifl = 0;
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        printf("PID %d\n", getpid());
        fflush(stdout);
        while (0 == ifl)
            sleep(5);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //Used only for debugging comment this if not interested

    string requiredParameters[NUMBER_OF_PARAMETERS] = {"-N","-I","-W","-L","-w","-l","-v","-d","-t"};
    if(my_rank==0 && cmdOptionExists(argv, argv+argc, "-h"))
    {
        cout << "Usage:\n"
               " - h get help\n"
               " - N number of individuals\n"
               " - I number of individuals that are initially infected\n"
               " - W, L width and length of the rectangular area where individuals move (in meters)\n"
               " - w, l width and length of each country (in meters)\n"
               " - v moving speed for an individual\n"
               " - d maximum spreading distance (in meters)\n"
               " - t time step (in seconds)\n"
               "All these fields are mandatory.\n";
        MPI_Finalize();
        return 0;
    }
    int i;
    int N,I,W,L,w,l,v,d,t;
    for(i=0;i<NUMBER_OF_PARAMETERS;i++){
        try{
            char * parameter = getCmdOption(argv, argv + argc, requiredParameters[i]);
            if(parameter){
                if(requiredParameters[i]=="-N"){
                    N = atoi(parameter);
                }else if(requiredParameters[i]=="-I"){
                    I = atoi(parameter);
                }else if(requiredParameters[i]=="-W"){
                    W = atoi(parameter);
                }else if(requiredParameters[i]=="-L"){
                    L = atoi(parameter);
                }else if(requiredParameters[i]=="-w"){
                    w = atoi(parameter);
                }else if(requiredParameters[i]=="-l"){
                    l = atoi(parameter);
                }else if(requiredParameters[i]=="-v"){
                    v = atoi(parameter);
                }else if(requiredParameters[i]=="-d"){
                    d = atoi(parameter);
                }else if(requiredParameters[i]=="-t"){
                    t = atoi(parameter);
                }
            }else{
                if(my_rank==0)
                    cout << "\nMissing parameter " + requiredParameters[i] + ". Use [-h] for help.";
                MPI_Finalize();
                return -1;
            }
        }catch(...){
            cout << "Error with parameter " << requiredParameters[i];
        }
    }

    //Register the required MPI types. 
    vector<MPI_Datatype> mpi_datatypes;
    MPI_Datatype mpi_position = Position::getMPIType(mpi_datatypes);
    mpi_datatypes.push_back(mpi_position);
    MPI_Datatype mpi_user = User::getMPIType(mpi_datatypes);


    //Get the number of processors in the world
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Compute the number of areas that has to be managed
    int number_of_areas = getNumberOfAreas(W,L,w,l);
    if(!number_of_areas && my_rank==0){
        cout << "The provided dimension are not multiple!" << requiredParameters[i];
        MPI_Finalize();
        return -1;
    }

    //Assign different users to different processor
    vector<Area> processor_areas = getArea(number_of_areas,my_rank,world_size,getStride(W,w));

    //Create the user associated to each processor
    //TODO later associated user to the various area
    // map<int,User> process_users;
    // for(int i : processor_user_IDs){
    //     Position userPosition(rand(),rand(),v,rand(),rand());
    //     User newUser(i,userPosition);
    //     process_users.insert({ i ,newUser});
    // }
    // cout << process_users.size();

    MPI_Finalize();
    return 0;
}


int getNumberOfAreas(int W,int L, int w,int l){
    if(W%w==0 && L%l==0){
        return W/w * L/l;
    }else
        return 0;
}

int getStride(int W,int w){
    return W/w;
}

//NOTE: the ids of the areas are assigned from left to right, top to bottom in increasing order.
vector<Area> getArea(int numberOfAreas, int processor_rank, int world_size,int stride){
    vector<Area> areas;
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
        Area newArea(i%stride,i/stride);
        areas.push_back(newArea);
    }
    for(Area &area:areas){
        tuple<int,int> areaCor = area.getCoordinate();
        int neighborStartCol = get<0>(areaCor)-(int)(NEIGHBOR_DISTANCE/2);
        int neighborStartRow = get<1>(areaCor)-(int)(NEIGHBOR_DISTANCE/2);
        //Starts from the top and go down to the right
        for(int i=0;i<NEIGHBOR_DISTANCE;i++){//i is the Row
            for (int j = 0; j < NEIGHBOR_DISTANCE; j++)//j is the Col
            {
                int neighborAreaCol = neighborStartCol+j;
                int neighborAreaRow = neighborStartRow+i;
                if(neighborAreaCol>=0 && neighborAreaRow>=0 && (neighborAreaCol!=get<0>(areaCor) || neighborAreaRow!=get<1>(areaCor))){
                    int neighborID = neighborAreaCol+neighborAreaRow*stride;
                    int processorArea;
                    if(neighborID<maxAreasForProcessor*(leftOutAreas)){
                        processorArea = neighborID/maxAreasForProcessor;
                    }else{
                        neighborID-=leftOutAreas*maxAreasForProcessor;
                        processorArea = leftOutAreas + neighborID/minAreasForProcessor;
                    }
                    NeighborArea neighborArea(processorArea);
                    area.setNeighborArea(neighborArea,(Direction)(i*NEIGHBOR_DISTANCE+j));
                }
            }            
        }
    }
    return areas;
}