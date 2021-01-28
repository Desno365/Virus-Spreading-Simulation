#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <vector>

using namespace std;
#define NUMBER_OF_PARAMETERS 9

char* getCmdOption(char ** begin, char ** end, const string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const string& option)
{
    return find(begin, end, option) != end;
}

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
    //TODO create are manager that manages a certain area based on the number of process(create object area)
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    MPI_Finalize();
    return 0;
}
