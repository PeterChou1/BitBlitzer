#pragma once
#include <cstdint>
#include <vector>
#include "Vec4.h"
#include "Mesh.h"

constexpr uint8_t LEFT_PLANE = 1 << 0, RIGHT_PLANE = 1 << 1, DOWN_PLANE = 1 << 2, UP_PLANE = 1 << 3, NEAR_PLANE = 1 << 4, FAR_PLANE = 1 << 5;


std::vector<Triangle> Clip(Triangle& clip);
