#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <stdlib.h> 
#include <list>
#include <tuple>
#include <unistd.h> //For debugging
#include <time.h>  
#include <stdio.h>

#include "user/user.h"
#include "area/area.h"
#include "utility/utility.h"

using namespace std;
//Is the number of parameters that can be passed when the program execute
#define NUMBER_OF_PARAMETERS 10
//Is the size of the matrix that is considered as a neighborhoodof an Area.
//NOTE: it has to be ODD.
#define NEIGHBOR_DISTANCE 3
//Is the number of seconds in a day. Is used in order to print the status only after a day.
#define SECONDS_IN_DAY 86400

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
vector<shared_ptr<Area>> getArea(int numberOfAreas, int processor_rank, int world_size,int stride);

int main(int argc, char** argv) {

    MPI_Init(NULL, NULL);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //Used only for debugging comment this part below if not interested
    char * parameter = getCmdOption(argv, argv + argc, "--debug");
    if(parameter)
    {    
        //The ID of the processor required to be debuged.
        int debuged_processor = atoi(parameter);
        if(my_rank==debuged_processor){
            volatile int ifl = 0;//Set this variable to 0 in order to resume the computation
            char hostname[256];
            gethostname(hostname, sizeof(hostname));
            printf("PID %d\n", getpid());
            fflush(stdout);
            while(ifl==0){
                sleep(1);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //Used only for debugging comment this if not interested
    string requiredParameters[NUMBER_OF_PARAMETERS] = {"-N","-I","-W","-L","-w","-l","-v","-d","-t","-D"};
    if(my_rank==0 && cmdOptionExists(argv, argv+argc, "-h"))
    {
        cout << "Usage:\n"
               " - h get help\n"
               " - N number of individuals\n"
               " - I number of individuals that are initially infected\n"
               " - W, L width and length of the rectangular area where individuals move (in meters as integer)\n"
               " - w, l width and length of each country (in meters as integer)\n"
               " - v moving speed for an individual(in meters per second as float)\n"
               " - d maximum spreading distance (in meters as float)\n"
               " - D simulation durations (in days as integer, has to be a multiple of t)\n"
               " - t time step (in seconds as integer)\n"
               " - d start debug mode and the value is used to know on which processor to start the debug\n"
               "All these fields are mandatory.\n";
        MPI_Finalize();
        return 0;
    }
    int i;
    int N,I,W,L,w,l,t,D;
    float v,d;
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
                    v = atof(parameter);
                }else if(requiredParameters[i]=="-d"){
                    d = atof(parameter);
                }else if(requiredParameters[i]=="-t"){
                    t = atoi(parameter);
                }else if(requiredParameters[i]=="-D"){
                    D = atoi(parameter);
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
    //Check that the number of users is greater than the number of indiiduals intially infected.
    if(N<I){
        if(my_rank==0)
            cout << "\nThe number of user has to be greater than the number of individuals intially infected.\n";
        MPI_Finalize();
        return -1;
    }

    //Set a random seed for all the next computation.
    srand (time(NULL));

    //Register the required MPI types. 
    MPI_Datatype mpi_user = User::getMPIType();


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
    vector<shared_ptr<Area>> processor_areas = getArea(number_of_areas,my_rank,world_size,getStride(W,w));

    //Create the user associated to each processor(we split the number of user in an equal way for each area
    //also the infected users are equally).
    int user_per_area = N / number_of_areas;
    //Assing an extra user at the first processors, if N is not a multiple of number_of_areas.
    int user_left_out = N % number_of_areas;
    if(my_rank<user_left_out) user_per_area++;
    //Compute the infected users per area.
    int infected_users_area = I / number_of_areas;
    int infected_left_out = i % number_of_areas;
    if(my_rank<infected_left_out) infected_users_area++;
    for(shared_ptr<Area> area:processor_areas){
        int lowerX = w * area->getCol();
        int lowerY = l * area->getRow();
        int higherX = lowerX + w;
        int higherY = lowerY + l;
        area->setBoundaries(lowerX,lowerY,higherX,higherY);
        
        for(int i=0;i<user_per_area;i++){
            //Generate random coordinates inside this region.
            int userX,userY;
            tie(userX,userY) = area->getRadomCoordinates();
            //Generate a random direction vector.
            int userDirX,userDirY;
            tie(userDirX,userDirY) = area->getRadomDirection();
            shared_ptr<Position> userPos = make_shared<Position>(userX,userY,v,userDirX,userDirX);
            //Compute a unique user ID
            int userID = my_rank * N + i;
            //Set infected the first generated users per area
            bool isAlreadyInfected = i<infected_users_area ? true : false;
            shared_ptr<User> newUser = make_shared<User>(userID, userPos, isAlreadyInfected);
            area->addUser(newUser,d);
        }
    }

    //NOTE: users starts to infect other users the step next to the one in which they've becomed infected.
    //This is the total duration of the computation.
    int total_seconds = D * SECONDS_IN_DAY;
    //Is the last day on which we have print the status.
    int next_day_print_status = 0;

    MPI_Barrier(MPI_COMM_WORLD);

    //For each processor gets its outputFile:
    string fileName = "./outputs/results-" + to_string(my_rank) +".csv";
    FILE *fptr = fopen(fromStringToCharName(fileName),"w");
    if(fptr == NULL)
    {
        printf("Error in opening %s!",fromStringToCharName(fileName));   
        exit(1);             
    }

    //Now starts the main loop.
    for(int elapsedTime = 0; elapsedTime < total_seconds; elapsedTime+=t){
        //Wait fo everyone to reach this point.
        MPI_Barrier(MPI_COMM_WORLD);
        if ( (elapsedTime / SECONDS_IN_DAY) > next_day_print_status ){
            string recap = "The state of the infection spreading at day "+ to_string(next_day_print_status) + " of the computation is:\n";
            fprintf(fptr,"%s", fromStringToCharName(recap));
            for(shared_ptr<Area> area:processor_areas){
                area->printActualState(fptr);
            }
            fprintf(fptr,"%s", fromStringToCharName("\n"));
            next_day_print_status++;
            MPI_Barrier(MPI_COMM_WORLD);
        }

        //TODO start by exchaning information about users since you have already started by placing them
        //Do the comunication in turn with the gather for each node:
        //- each node will send to the same node the size of the list with the struct of all the interesting user for that Area
        //- based on this the receiver che allocate enough memory in the buffer
        //- at this point each node can send the users structs
        //Thi is repeated two times:
        //- In the first part the user out of area are moved,
        //- The when everything is up to date we move the user struct for updating the global state

        //After this every area has a global vision of the map so it can update the infected status of the user.
        for(shared_ptr<Area> area:processor_areas){
            area->updateUserInfectionStatus(t,d);
        }

        //Update the position of all the users in the various area.
        for(shared_ptr<Area> &area:processor_areas){
            area->updateUserPositions(t,d);
        }
    }

    //Print the state at the end of the computation.
    MPI_Barrier(MPI_COMM_WORLD);
    string recap = "The state of the infection spreading at the end of the computation is:\n";
    fprintf(fptr,"%s", fromStringToCharName(recap));
    for(shared_ptr<Area> area:processor_areas){
        area->printActualState(fptr);
    }

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
vector<shared_ptr<Area>> getArea(int numberOfAreas, int processor_rank, int world_size,int stride){
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
        shared_ptr<Area> newArea = make_shared<Area>(i%stride,i/stride,startingAreaID+i);
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
                        neighborID-=leftOutAreas*maxAreasForProcessor;
                        processorArea = leftOutAreas + neighborID/minAreasForProcessor;
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