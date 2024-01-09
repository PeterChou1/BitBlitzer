#pragma once
#include <vector>

#include "Resource.h"
#include "Vertex.h"


class VertexBuffer : public Resource
{
public:

    void ResetResource() override
    {
        buffer.clear();
    }

    std::vector<Vertex> buffer;
};
