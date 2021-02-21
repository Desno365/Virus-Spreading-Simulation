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
#include "utility/mainUtility.h"


using namespace std;
//Is the number of parameters that can be passed when the program execute
#define NUMBER_OF_PARAMETERS 10
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

//Populate the map with the vector for the areas.
template<typename T>
static map<int,vector<shared_ptr<T>>> * getPopulateMapOfUsersVector(vector<int> ids);

//Reset the vectors inside a previously populated maps.
template<typename T>
static void resetPopulateMapOfUsersVector(map<int,vector<shared_ptr<T>>> * map);

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
    //Check that the number of users is greater than the number of individuals intially infected.
    if(N<I){
        if(my_rank==0)
            cout << "\nThe number of user has to be greater than the number of individuals intially infected.\n";
        MPI_Finalize();
        return -1;
    }

    //Set a random seed for all the next computation.
    srand (time(NULL));

    //Register the required MPI types. 
    // MPI_Datatype mpi_user = User::getMPIType();
    user_struct user_t;
    MPI_Datatype mpi_user;
    //Set the structure length.
    int struct_len = 8;
    int block_lens[struct_len];
    //Compute the displacement of elements in the struct.
    MPI_Datatype types[struct_len];
    MPI_Aint displacements[struct_len];
    //Add id.
    block_lens[0] = 1;
    types[0] = MPI_INT;
    displacements[0] = (size_t) &(user_t.id) - (size_t) &user_t;
    //Add the position of the user.
    //Add x.
    block_lens[1] = 1;
    types[1] = MPI::FLOAT;
    displacements[1] = (size_t) &(user_t.x) - (size_t) &user_t;
    //Add y.
    block_lens[2] = 1;
    types[2] = MPI::FLOAT;
    displacements[2] = (size_t) &(user_t.y) - (size_t) &user_t;
    //Add dirX.
    block_lens[3] = 1;
    types[3] = MPI::FLOAT;
    displacements[3] = (size_t) &(user_t.dirX) - (size_t) &user_t;
    //Add dirY
    block_lens[4] = 1;
    types[4] = MPI::FLOAT;
    displacements[4] = (size_t) &(user_t.dirY) - (size_t) &user_t;
    //Add the infection state of the user.
    block_lens[5] = 1;
    types[5] = MPI_C_BOOL;
    displacements[5] = (size_t) &(user_t.infected) - (size_t) &user_t;
    //Add the immune time of the user.
    block_lens[6] = 1;
    types[6] = MPI_INT;
    displacements[6] = (size_t) &(user_t.immuneTime) - (size_t) &user_t;
    //Add the time.
    block_lens[7] = 1;
    types[7] = MPI_INT;
    displacements[7] = (size_t) &(user_t.timeCounter) - (size_t) &user_t;
    int res = MPI_Type_create_struct(struct_len, block_lens, displacements, types, &mpi_user);
    if(res!=MPI_SUCCESS){
        cout<<"Error in generating mpi datatype "<<res<<"\n";
    }
    res = MPI_Type_commit(&mpi_user);
    if(res!=MPI_SUCCESS){
        cout<<"Error in generating mpi datatype "<<res<<"\n";
    }

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
    vector<shared_ptr<Area>> processor_areas = getArea(number_of_areas,my_rank,world_size,getStride(W,w),d,t);

    //Create the user associated to each processor(we split the number of user in an equal way for each area
    //also the infected users are equally).
    int user_per_area = N / number_of_areas;
    //Assing an extra user at the first processors, if N is not a multiple of number_of_areas.
    int user_left_out = N % number_of_areas;
    if(my_rank<user_left_out) user_per_area++;
    //Compute the infected users per area.
    int infected_users_area = I / number_of_areas;
    int infected_left_out = I % number_of_areas;
    if(my_rank<infected_left_out) infected_users_area++;
    for(shared_ptr<Area> area:processor_areas){
        int lowerX = w * area->getCol();
        int lowerY = l * (getStrideVertical(L,l)-1-area->getRow());
        int higherX = lowerX + w;
        int higherY = lowerY + l;
        area->setBoundaries(lowerX,lowerY,higherX,higherY);
        
        for(int i=0;i<user_per_area;i++){
            //Generate random coordinates inside this region.
            float userX,userY;
            tie(userX,userY) = area->getRadomCoordinates();
            //Generate a random direction vector.
            float userDirX,userDirY;
            tie(userDirX,userDirY) = area->getRadomDirection();
            shared_ptr<Position> userPos = make_shared<Position>(userX,userY,v,userDirX,userDirY);
            //Compute a unique user ID
            int userID = my_rank * N + i;
            //Set infected the first generated users per area
            bool isAlreadyInfected = i<infected_users_area ? true : false;
            shared_ptr<User> newUser = make_shared<User>(userID, userPos, isAlreadyInfected);
            area->addUser(newUser);
        }
    }

    //NOTE: users starts to infect other users the step next to the one in which they've become infected.
    //This is the total duration of the computation.
    int total_seconds = D * SECONDS_IN_DAY;
    //Is the last day on which we have print the status.
    int next_day_print_status = 0;

    MPI_Barrier(MPI_COMM_WORLD);

    //For each processor gets its outputFile:
    string fileName = "./outputs/results-" + to_string(my_rank) +".csv";
    char * str = fromStringToCharName(fileName);
    FILE *fptr = fopen(str,"w");
    if(fptr == NULL)
    {
        printf("Error in opening %s!",str);   
        exit(1);             
    }
    free(str);

    //Setup data structure for the main loop:
    //Compute vector with the ids of the processor's areas.
    vector<int> area_ids;
    for(shared_ptr<Area> area : processor_areas){
        area_ids.push_back(area->getID());
    }
    map<int,vector<shared_ptr<User>>> * mapOutOfAreaUsersToAreaLocal = getPopulateMapOfUsersVector<User>(area_ids);
    map<int,vector<shared_ptr<User>>> * mapNearBorderUsersToAreaLocal = getPopulateMapOfUsersVector<User>(area_ids);
    //Get id of the other processors.
    vector<int> processor_ids;
    for(int i=0; i<world_size ; i++){
        if(i!=my_rank) processor_ids.push_back(i);
    }
    map<int,vector<shared_ptr<user_struct>>> * mapOutOfAreaUsersToAreaRemote = getPopulateMapOfUsersVector<user_struct>(processor_ids);
    map<int,vector<shared_ptr<user_struct>>> * mapNearBorderUsersToAreaRemote = getPopulateMapOfUsersVector<user_struct>(processor_ids);

    //Now starts the main loop.
    for(int elapsedTime = 0; elapsedTime < total_seconds; elapsedTime+=t){
        if ( (elapsedTime / SECONDS_IN_DAY) >= next_day_print_status ){
            if(my_rank==0) cout<<"Printing spreading infections status at day " << next_day_print_status << "\n";
            string recap = "The state of the infection spreading at day "+ to_string(next_day_print_status) + " of the computation is:\n";
            str = fromStringToCharName(recap);
            fprintf(fptr,"%s", str);
            free(str);
            for(shared_ptr<Area> area:processor_areas){
                area->printActualState(fptr);
            }
            str = fromStringToCharName("\n");
            fprintf(fptr,"%s", str);
            fflush(fptr);
            free(str);
            next_day_print_status++;
        }
        //Wait fo everyone to reach this point.
        MPI_Barrier(MPI_COMM_WORLD);

        //First exchange and update informations about users out of area.
        for(shared_ptr<Area> area:processor_areas){
            area->computeOutOfAreaUserMap();
            //Collects out of area users locally
            map<int,shared_ptr<vector<shared_ptr<User>>>> outOfAreasUserLocallyOnThisArea = area->getOutOfAreaUsersLocal();
            for(auto outOfAreaUserVectors=outOfAreasUserLocallyOnThisArea.begin(); outOfAreaUserVectors!=outOfAreasUserLocallyOnThisArea.end() ; ++outOfAreaUserVectors){
                vector<shared_ptr<User>> * previousUsers = &(mapOutOfAreaUsersToAreaLocal->at(outOfAreaUserVectors->first));
                previousUsers->insert(previousUsers->end(),outOfAreaUserVectors->second->begin(),outOfAreaUserVectors->second->end());
            }
            //Collects now users out of area that goes to a remote location.
            map<int,shared_ptr<vector<shared_ptr<user_struct>>>> outOfAreasUserRemotelyOnThisArea = area->getOutOfAreaUsersRemote();
            for(auto outOfAreaUserVectors=outOfAreasUserRemotelyOnThisArea.begin(); outOfAreaUserVectors!=outOfAreasUserRemotelyOnThisArea.end() ; ++outOfAreaUserVectors){
                vector<shared_ptr<user_struct>> * previousUsers = &(mapOutOfAreaUsersToAreaRemote->at(outOfAreaUserVectors->first));
                previousUsers->insert(previousUsers->end(),outOfAreaUserVectors->second->begin(),outOfAreaUserVectors->second->end());
            }
        }
        //Dispatch out of area users among local area.
        for(shared_ptr<Area> area : processor_areas){
            area->addUsers(mapOutOfAreaUsersToAreaLocal->at(area->getID()));
        }
        //Dispatch out of area users among remote areas.
        //Now in order based on the processor id performs gathers operations to get from each processors informations about the out of area users:
        // - First the at the processor's turn waits for and int for each processors that specify how mush user's structs it will send;
        // - Allocate a buffer of a proportional dimension in order to receives such structures.
        //NOTE: follow this link https://www.mcs.anl.gov/research/projects/mpi/mpi-standard/mpi-report-1.1/node70.htm or here https://stackoverflow.com/questions/31890523/how-to-use-mpi-gatherv-for-collecting-strings-of-diiferent-length-from-different.
        for(int i=0; i<world_size ; i++){
            //Waits for everyone to reach this point
            MPI_Barrier(MPI_COMM_WORLD);
            int n_OutOfAreaUsers;
            int *gather_buffer_sizes = NULL;
            user_struct *gather_buffer_structs = NULL;
            //Is the offset relative to the receiving buffer to which data incoming from the relative processors should be placed.
            int * displays = NULL;
            if(i==my_rank){
                //If this is my turn then I have to gather information from other nodes.
                n_OutOfAreaUsers = 0;
                gather_buffer_sizes = (int *) malloc(sizeof(int) * world_size);
                displays = (int *) malloc(sizeof(int) * world_size);
            }else{
                n_OutOfAreaUsers = mapOutOfAreaUsersToAreaRemote->at(i).size();
            }
            MPI_Gather(&n_OutOfAreaUsers, 1, MPI_INT, gather_buffer_sizes, 1, MPI_INT, i, MPI_COMM_WORLD);

            //Now we can receive from the remote location the user struct of the user out of areas.
            //Is the datatype for the vector of user struct that has to be sent.
            user_struct send_vector[n_OutOfAreaUsers];
            int receiving_size = 0;
            if(i==my_rank){
                //If this is my turn then I have to gather information from other nodes.
                for(int i=0; i<world_size ; i++){//TODO error when i==my_rank???
                    displays[i] = receiving_size;
                    receiving_size += gather_buffer_sizes[i];
                }
                gather_buffer_structs = (user_struct *) malloc(sizeof(user_struct) * receiving_size);
            }else{
                int index = 0;
                for(shared_ptr<user_struct> user_t : mapOutOfAreaUsersToAreaRemote->at(i)){
                    if(i==0 && (user_t.get()->x<0 || user_t.get()->x>100 || user_t.get()->y<50 || user_t.get()->y>100)){
                        cout<< "Error in sending to 0 from "<< my_rank<<"\n";
                    }else if(i==1 && (user_t.get()->x<100 || user_t.get()->x>200 || user_t.get()->y<50 || user_t.get()->y>100)){
                        cout<< "Error in sending to 1 from "<< my_rank<<"\n";
                    }else if(i==2 && (user_t.get()->x<0 || user_t.get()->x>100 || user_t.get()->y<0 || user_t.get()->y>50)){
                        cout<< "Error in sending to 2 from "<< my_rank<<"\n";
                    }else if(i==3 && (user_t.get()->x<100 || user_t.get()->x>200 || user_t.get()->y<0 || user_t.get()->y>50)){
                        cout<< "Error in sending to 3 from "<< my_rank<<"\n";
                    }
                    user_struct user = *(user_t.get());
                    send_vector[index] = user;   
                    index++;
                }
            }
            MPI_Gatherv(&send_vector[0], n_OutOfAreaUsers, mpi_user, gather_buffer_structs, gather_buffer_sizes, displays,mpi_user, i, MPI_COMM_WORLD);
            
            //Now convert the user_structs into Users.
            if(i==my_rank){
                vector<shared_ptr<User>> newUsers;
                for(int i=0; i<receiving_size ; i++){
                    newUsers.push_back(make_shared<User>(&(gather_buffer_structs[i]),v));
                }
                //Pass the vector with the new user to the different areas.
                for(shared_ptr<Area> area : processor_areas){
                    area->getNewUserFromRemoteLocation(&newUsers);
                }
                //Check that such vector is now empty
                if(!newUsers.empty()){
                    //TODO put the users inside the nearest area.
                    cout << "Error in processor " << my_rank << " since the vector new received new users from remote locations is not empty!\n";
                }
            }
            //Free the buffer used during the comunication.
            if(gather_buffer_sizes!=NULL) free(gather_buffer_sizes);
            if(gather_buffer_structs!=NULL) free(gather_buffer_structs);  
            if(displays!=NULL) free(displays);  
        }

        //Next steps consists in computing user near border in order to exchange such information with other local areas,
        //or with remote location.
        //Compute the data structure that contains for each area the user structs of users near the border.
        for(shared_ptr<Area> area : processor_areas){
            area->computeNearBorderUserMap();
            //Collects users near border locally.
            map<int,shared_ptr<vector<shared_ptr<User>>>> nearBorderUserLocallyOnThisArea = area->getNearBorderUsersLocal();
            for(auto nearBorderUserVectors=nearBorderUserLocallyOnThisArea.begin(); nearBorderUserVectors!=nearBorderUserLocallyOnThisArea.end() ; ++nearBorderUserVectors){
                vector<shared_ptr<User>> * previousUsers = &(mapNearBorderUsersToAreaLocal->at(nearBorderUserVectors->first));
                previousUsers->insert(previousUsers->end(),nearBorderUserVectors->second->begin(),nearBorderUserVectors->second->end());
            }
            //Collects now users near border which information will go remotely.
            map<int,shared_ptr<vector<shared_ptr<user_struct>>>> nearBorderUserRemotelyOnThisArea = area->getNearBorderUsersRemote();
            for(auto nearBorderUserVectors=nearBorderUserRemotelyOnThisArea.begin(); nearBorderUserVectors!=nearBorderUserRemotelyOnThisArea.end() ; ++nearBorderUserVectors){
                vector<shared_ptr<user_struct>> * previousUsers = &(mapNearBorderUsersToAreaRemote->at(nearBorderUserVectors->first));
                previousUsers->insert(previousUsers->end(),nearBorderUserVectors->second->begin(),nearBorderUserVectors->second->end());
            }
        }
        //Dispatch User on the border information locally.
        for(shared_ptr<Area> area : processor_areas){
            area->addNearbyUsersLocal(mapNearBorderUsersToAreaLocal->at(area->getID()));
        }
        //Dispatch the user struct to the remote location interested.
        //NOTE: the comunication is carried out in the same way as before.
        for(int i=0; i<world_size ; i++){
            //Waits for everyone to reach this point
            MPI_Barrier(MPI_COMM_WORLD);
            int n_nearbyUsersToRemote;
            int *gather_buffer_sizes = NULL;
            user_struct *gather_buffer_structs = NULL;
            //Is the offset relative to the receiving buffer to which data incoming from the relative processors should be placed.
            int * displays = NULL;
            if(i==my_rank){
                //If this is my turn then I have to gather information from other nodes.
                n_nearbyUsersToRemote = 0;
                gather_buffer_sizes = (int *) malloc(sizeof(int) * world_size);
                displays = (int *) malloc(sizeof(int) * world_size);
            }else{
                n_nearbyUsersToRemote = mapNearBorderUsersToAreaRemote->at(i).size();
            }
            MPI_Gather(&n_nearbyUsersToRemote, 1, MPI_INT, gather_buffer_sizes, 1, MPI_INT, i, MPI_COMM_WORLD);

            //Now we can receive from the remote location the user struct of the user out of areas.
            //Is the datatype for the vector of user struct that has to be sent.
            user_struct * send_vector=NULL;
            int receiving_size = 0;
            if(i==my_rank){
                //If this is my turn then I have to gather information from other nodes.
                for(int i=0; i<world_size ; i++){
                    displays[i] = receiving_size;
                    receiving_size += gather_buffer_sizes[i];
                }
                gather_buffer_structs = (user_struct *) calloc(receiving_size,sizeof(user_struct));
            }else{
                int index = 0;
                send_vector = (user_struct *) calloc(n_nearbyUsersToRemote,sizeof(user_struct));
                for(shared_ptr<user_struct> user_t : mapNearBorderUsersToAreaRemote->at(i)){
                    user_struct user = *(user_t.get());//TODO the user_struct is automatically destroyed at the end of the loop??
                    send_vector[index] = user;
                    index++;
                }
            }
            MPI_Gatherv(send_vector, n_nearbyUsersToRemote, mpi_user, gather_buffer_structs, gather_buffer_sizes, displays,mpi_user, i, MPI_COMM_WORLD);

            //Now convert the user_structs into shared pointer of user struct.
            if(i==my_rank){
                vector<shared_ptr<user_struct>> users_t;
                for(int i=0; i<receiving_size ; i++){
                    users_t.push_back(User::getSharedFromStruct(gather_buffer_structs[i]));
                }
                //Pass the vector with the user structs to the different areas.
                for(shared_ptr<Area> area : processor_areas){
                    area->addNearbyUsersRemote(users_t);
                }
            }
            //Free the buffer used during the comunication.
            if(send_vector!=NULL) free(send_vector);
            if(gather_buffer_sizes!=NULL) free(gather_buffer_sizes);
            if(gather_buffer_structs!=NULL) free(gather_buffer_structs);  
            if(displays!=NULL) free(displays);  
        }

        //After this every area has a global vision of the map so it can update the infected status of the user.
        for(shared_ptr<Area> area:processor_areas){
            area->updateUserInfectionStatus();
        }

        //Update the position of all the users in the various area.
        for(shared_ptr<Area> &area:processor_areas){
            area->updateUserPositions();
        }

        //Erase the content of the previous interation inside the data structure.
        resetPopulateMapOfUsersVector(mapOutOfAreaUsersToAreaLocal);
        resetPopulateMapOfUsersVector(mapOutOfAreaUsersToAreaRemote);
        resetPopulateMapOfUsersVector(mapNearBorderUsersToAreaLocal);
        resetPopulateMapOfUsersVector(mapNearBorderUsersToAreaRemote);
    }

    //Print the state at the end of the computation.
    MPI_Barrier(MPI_COMM_WORLD);
    string recap = "The state of the infection spreading at the end of the computation is:\n";
    str = fromStringToCharName(recap);
    fprintf(fptr,"%s", str);
    free(str);
    for(shared_ptr<Area> area:processor_areas){
        area->printActualState(fptr);
    }

    //Closed the opened file.
    fclose(fptr);

    //TearDown the data structure used for the main loop;
    mapOutOfAreaUsersToAreaLocal->clear();
    delete mapOutOfAreaUsersToAreaLocal;
    mapOutOfAreaUsersToAreaRemote->clear();
    delete mapOutOfAreaUsersToAreaRemote;
    mapNearBorderUsersToAreaLocal->clear();
    delete mapNearBorderUsersToAreaLocal;
    mapNearBorderUsersToAreaRemote->clear();
    delete mapNearBorderUsersToAreaRemote;

    MPI_Finalize();
    return 0;
}

template<typename T>
map<int,vector<shared_ptr<T>>> * getPopulateMapOfUsersVector(vector<int> ids){
    map<int,vector<shared_ptr<T>>> * mapPopulate = new map<int,vector<shared_ptr<T>>>;
    for(int ids : ids){
        vector<shared_ptr<T>> * users = new vector<shared_ptr<T>>;
        mapPopulate->insert( {ids,*users} );
    }
    return mapPopulate;
}

template<typename T>
void resetPopulateMapOfUsersVector(map<int,vector<shared_ptr<T>>> * map){
    for(auto mapElements = map->begin(); mapElements != map->end(); ++mapElements){
        mapElements->second.clear();
    }
}