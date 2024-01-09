#pragma once
#include <vector>

#include "Resource.h"


class IndexBuffer : public Resource
{
public:

    void ResetResource() override
    {
        buffer.clear();
    }

    std::vector<std::uint32_t> buffer;
};
