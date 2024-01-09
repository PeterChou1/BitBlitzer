#pragma once
#include <cassert>
#include <vector>

#include "Resource.h"
#include "SIMDPixel.h"

class DepthBuffer : public Resource 
{
public:

    DepthBuffer(int width, int height) : m_height(height / SIMDPixel::PIXEL_HEIGHT),
        m_width(width / SIMDPixel::PIXEL_WIDTH)
    {
        m_DepthBuffer.resize(m_height * m_width);
    }

    SIMDFloat DepthTest(int x, int y, SIMDFloat& depth, SIMDFloat& mask)
    {
        SIMDFloat& curDepth = m_DepthBuffer[y * m_width + x];
        const SIMDFloat visible = depth > curDepth;
        const SIMDFloat updateMask = mask & visible;
        return updateMask;
    }


    void UpdateBuffer(int x, int y, SIMDFloat& mask, SIMDFloat& depth)
    {
        SIMDFloat& curDepth = m_DepthBuffer[y * m_width + x];
        curDepth = SIMD::Select(mask, curDepth, depth);
    }


    SIMDFloat GetBuffer(int x, int y) const
    {
        const int xAlign = x / SIMDPixel::PIXEL_WIDTH;
        const int yAlign = y / SIMDPixel::PIXEL_HEIGHT;
        return m_DepthBuffer[yAlign * m_width + xAlign];
    }

    SIMDFloat GetBuffer(int index) const
    {
        assert(0 <= index && index < m_DepthBuffer.size());
        return m_DepthBuffer[index];
    }

    void ResetResource() override
    {
        for (auto& depth : m_DepthBuffer)
        {
            depth = SIMDFloat(0.0f);
        }
    }

private:
    int m_height;
    int m_width;
    std::vector<SIMDFloat> m_DepthBuffer;
};