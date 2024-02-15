#include "stdafx.h"

#include "Tile.h"

// the 4 corners of the tile
Vec2 Tile::CornerIndex[4] = {Vec2(0, 0), Vec2(TILE_SIZE_X, 0), Vec2(0, TILE_SIZE_Y), Vec2(TILE_SIZE_X, TILE_SIZE_Y)};