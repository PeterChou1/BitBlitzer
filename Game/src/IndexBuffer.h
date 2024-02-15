//---------------------------------------------------------------------------------
// IndexBuffer.h
//---------------------------------------------------------------------------------
//
// The IndexBuffer is used to store all the indices from the vertex buffer that
// form the meshes in the current scene
//
#pragma once

#include <vector>

#include "Resource.h"


class IndexBuffer : public Resource
{
public:

    void ResetResource() override
    {
        Buffer.clear();
    }

    std::vector<std::uint32_t> Buffer;
};
