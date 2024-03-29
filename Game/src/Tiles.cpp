#include "stdafx.h"
#include "Tiles.h"


Tiles::Tiles(int width, int height)
{

    for (int y = 0; y < TILE_COUNT_Y; ++y)
    {
        for (int x = 0; x < TILE_COUNT_X; ++x)
        {
            auto max = Vec2(
                static_cast<float>((std::min)((x + 1) * TILE_SIZE_X, width)), 
                static_cast<float>((std::min)((y + 1) * TILE_SIZE_Y, height))
            );
            auto min = Vec2(
                static_cast<float>((std::min)(x * TILE_SIZE_X, width)), 
                static_cast<float>((std::min)(y * TILE_SIZE_Y, height))
            );
            auto t = Tile(min, max);
            TilesArray.push_back(t);
        }
    }
}
