#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <memory>
#include <mpi.h>

using namespace std;

template <typename T>
class Serializable
{
    public:
        virtual ~Serializable() = 0;
        virtual shared_ptr<T> getStruct() = 0;
};

template<typename T>
Serializable<T>::~Serializable(){

}

#endif