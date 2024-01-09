#pragma once
#include <thread>
#include <vector>

#include "Resource.h"
#include "Triangle.h"

class ClippedTriangleBuffer : public Resource 
{
public:
    ClippedTriangleBuffer()
    {
        buffer.resize(std::thread::hardware_concurrency());
    }


    void ResetResource() override
    {
        for (auto& triangles : buffer)
        {
            triangles.clear();
        }
    }

    std::vector<std::vector<Triangle>> buffer;
};
