#pragma once
#include <cassert>
#include <string>

// define graphics assets


enum ObjAsset
{
    Spot,
    Furina
};


inline std::string LookUpFilePath(ObjAsset asset)
{
    switch (asset)
    {
    case Spot:
        return "./Assets/spot.obj";
    case Furina:
        return "./Assets/furina.obj";
    default:
        assert(false && "No possible");
    }
}