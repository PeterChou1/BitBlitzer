//---------------------------------------------------------------------------------
// Entity.h
//---------------------------------------------------------------------------------
// 
// This file define useful constants and types for the ECS system
//

#pragma once

#include <cstdint>
#include <bitset>

// Id used to Identify an Entity Essentially an Index into the Entity Array Managed by the Entity Manager
using Entity = std::uint32_t;
// Max Amount of Components that can registered at once defines the size
// of arrays of Entity Tracked by EntityManager
constexpr Entity MAX_ENTITIES = 5000;
// Used to identify components
using ComponentTypeID = std::size_t;
// Max Amount of Component that can registered at once
constexpr ComponentTypeID MAX_COMPONENTS = 32;
// A Signature is used to by the ECS system to identify which Entity Own which Component
using Signature = std::bitset<MAX_COMPONENTS>;