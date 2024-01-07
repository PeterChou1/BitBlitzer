#pragma once
#include "Entity.h"

template<typename T>
struct TypeID {
    static const ComponentTypeID value; // Unique value for each type
};


inline ComponentTypeID GenerateComponentID()
{
    static auto counter = ComponentTypeID(0);
    return counter++;
}

template<typename T>
const std::size_t TypeID<T>::value = GenerateComponentID();
