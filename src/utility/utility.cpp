#include "utility.h"

char * fromStringToCharName(string string){
    char * name_as_chars = (char *)malloc( sizeof(char)* (string.length()+1) );
    strcpy(name_as_chars,string.c_str());
    return name_as_chars;
}