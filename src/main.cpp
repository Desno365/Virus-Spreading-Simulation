#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <vector>
#include <stdlib.h> 
#include <list>
     
#include "user/user.h"

using namespace std;
//Is the number of parameters that can be passed when the program execute
#define NUMBER_OF_PARAMETERS 9

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

//Returns the list of ID of the user associated to processor_rank.
list<int> getAreaIDs(int numberOfAreas, int processor_rank, int world_size);

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
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
    list<int> processor_areas_IDs = getAreaIDs(number_of_areas,my_rank,world_size);

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

list<int> getAreaIDs(int numberOfAreas, int processor_rank, int world_size){
    list<int> areas_ID;
    for(int i=processor_rank; i<numberOfAreas;i+=world_size){
        areas_ID.push_back(i);
    }
    return areas_ID;
}