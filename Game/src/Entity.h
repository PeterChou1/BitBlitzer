//---------------------------------------------------------------------------------
// Entity.h
//---------------------------------------------------------------------------------
// 
// This file define useful constants and types for the ECS system
//

#pragma once
#include <cstdint>
#include <bitset>

using Entity = std::uint32_t;
// Used to define the size of arrays later on
constexpr Entity MAX_ENTITIES = 5000;
// Simple ID used to 
using ComponentType = std::uint8_t;
// used to identify components
using ComponentTypeID = std::size_t;
// Used to define the size of arrays later on
constexpr ComponentType MAX_COMPONENTS = 32;
// A Signature is used to by the ECS system to identify which Entity Own which Component
using Signature = std::bitset<MAX_COMPONENTS>;

using Index = uint32_t;
