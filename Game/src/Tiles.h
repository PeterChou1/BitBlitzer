//---------------------------------------------------------------------------------
// Tiles.h
//---------------------------------------------------------------------------------
//
// Container classes for all tiles see Tile.h for more information
//
#pragma once
#include "Resource.h"
#include "Tile.h"

class Tiles : public Resource
{
public:
    Tiles(int width, int height);


    void ResetResource() override
    {
        for (int i = 0; i < TilesArray.size(); i++)
        {
            TilesArray[i].Clear();
        }
    }

    std::vector<Tile> TilesArray;
};
