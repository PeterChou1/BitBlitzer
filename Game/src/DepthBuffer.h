#pragma once
#include "SIMD.h"
#include <memory>


class DepthBuffer
{
public:
    DepthBuffer() : m_height(0), m_width(0)
    {
    }

    DepthBuffer(int width, int height) : m_height(height), m_width(width)
    {
        m_DepthBuffer = std::shared_ptr<float[]>(new float[(height + 1) * (width + 1)]);
    }

    void SetBuffer(int x, int y, float depth)
    {
        assert(0 <= x && x <= m_width && 0 <= y && y <= m_height && "out of bounds index");
        m_DepthBuffer[y * m_width + x] = depth;
    }

    float GetBuffer(int x, int y)
    {
        assert(0 <= x && x <= m_width && 0 <= y && y <= m_height && "out of bounds index");
        return m_DepthBuffer[y * m_width + x];
    }

    SIMDFloat GetBufferSIMD(SIMDPixel& p)
    {
        return SIMDFloat(&m_DepthBuffer[p.position.y[0] * m_width + p.position.x[0]]);
    }

    void ClearBuffer()
    {
        for (int i = 0; i < m_height * m_width; i++)
        {
            m_DepthBuffer[i] = 0.0f;
        }
    }

private:
    int m_height;
    int m_width;
    std::shared_ptr<float[]> m_DepthBuffer;
};

class DepthBufferSIMD
{
public:
    DepthBufferSIMD() : m_height(0), m_width(0)
    {
    }


    DepthBufferSIMD(int width, int height) : m_height(height / SIMDPixel::PIXEL_HEIGHT),
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


    SIMDFloat GetBuffer(int x, int y)
    {
        const int xAlign = x / SIMDPixel::PIXEL_WIDTH;
        const int yAlign = y / SIMDPixel::PIXEL_HEIGHT;
        return m_DepthBuffer[yAlign * m_width + xAlign];
    }

    void ClearBuffer()
    {
        for (int i = 0; i < m_DepthBuffer.size(); i++)
        {
            m_DepthBuffer[i] = SIMDFloat(0.0f);
        }
    }

private:
    int m_height;
    int m_width;
    std::vector<SIMDFloat> m_DepthBuffer;
};
