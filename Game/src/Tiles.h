#pragma once
#include "Resource.h"
#include "Tile.h"

class Tiles : public Resource
{
public:
    Tiles(int width, int height);


    void ResetResource() override
    {
        for (int i = 0; i < tiles.size(); i++)
        {
            tiles[i].Clear();
        }
    }

    std::vector<Tile> tiles;
};
