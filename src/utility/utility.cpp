#include "utility.h"
using namespace std;

char * fromStringToCharName(string string){
    char * name_as_chars = (char *)malloc( sizeof(char)* (string.length()+1) );
    strcpy(name_as_chars,string.c_str());
    return name_as_chars;
}

tuple<int,int> fromDirectionToVector(Direction direction){
    switch (direction)
    {
    case North:
    case NorthEast:
    case NorthWest:
    case South:
    case SouthEast:
    case SouthWest:
        return {0,1};    
    case West:
    case East:
        return {1,0};    
    default:
        cout<<"Error in asking direction from vector "<< direction<< "\n";
        return {0,0};
    }
}

//NOTE: follow this link https://www.delftstack.com/howto/cpp/how-to-generate-random-float-number-in-cpp/

float generateRandomFloat(float max, float min){
    random_device rd;
    default_random_engine eng(rd());
    uniform_real_distribution<float> distr(min,max);

    return distr(eng);
}

