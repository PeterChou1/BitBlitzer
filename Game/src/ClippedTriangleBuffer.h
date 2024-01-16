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
        Buffer.resize(std::thread::hardware_concurrency());
    }


    void ResetResource() override
    {
        for (auto& triangles : Buffer)
        {
            triangles.clear();
        }
    }

    std::vector<std::vector<Triangle>> Buffer;
};
