#pragma once
#include <cassert>
#include "SIMD.h"
#include "Mesh.h"


class DepthBufferSIMD;
/*
* /brief sets of 8 pixels (4 x 2)
*/
class SIMDPixel
{
public:
    SIMDPixel(const SIMDVec2& position, const SIMDFloat& depth, SIMDFloat alpha, SIMDFloat beta, SIMDFloat gamma,
        int binId, int binIndex) :
        position(position), alpha(alpha), beta(beta), gamma(gamma), depth(depth), binId(binId), binIndex(binIndex)
    {
    }

    void Interpolate(Triangle& t)
    {
        auto t1 = SIMDVec2(t.verts[0].uv);
        auto t2 = SIMDVec2(t.verts[1].uv);
        auto t3 = SIMDVec2(t.verts[2].uv);
        auto n1 = SIMDVec3(t.verts[0].normal);
        auto n2 = SIMDVec3(t.verts[1].normal);
        auto n3 = SIMDVec3(t.verts[2].normal);

        textureCoord = (t1 * alpha + t2 * beta + t3 * gamma) / depth;
        normal = (n1 * alpha + n2 * beta + n3 * gamma) / depth;
    }

    static constexpr int PIXEL_WIDTH = 4;
    static constexpr int PIXEL_HEIGHT = 2;
    static SIMDFloat PixelOffsetX;
    static SIMDFloat PixelOffsetY;
    SIMDFloat mask;
    SIMDVec2 position{}, textureCoord{};
    SIMDVec3 normal{};
    // barycentric coordinates
    SIMDFloat alpha{}, beta{}, gamma{};
    // depth buffer values of current pixels
    SIMDFloat depth{};
    int binId;
    int binIndex;
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

    void Clear()
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

class SIMDPixelBuffer
{
public:

    SIMDPixelBuffer() = default;

    SIMDPixelBuffer(int width, int height) : m_width(width / SIMDPixel::PIXEL_WIDTH),
        m_height(height / SIMDPixel::PIXEL_HEIGHT)
    {
        m_Pixelbuffer.resize(m_height * m_width);
    }

    void SetBuffer(int x, int y, const SIMDPixel& pixel)
    {
        m_Pixelbuffer[y * m_width + x].push_back(pixel);
    }

    /**
     * \brief Once pixel has been rasterized accumulate the pixel so we can distribute the pixels evenly across threads
     * \param depth
     */
    void AccumulatePixel(const DepthBufferSIMD& depth)
    {
        for (int i = 0; i < m_Pixelbuffer.size(); i++)
        {
            for (SIMDPixel& pixel : m_Pixelbuffer[i])
            {
                //SIMDFloat mask = pixel.depth == depth.GetBuffer(i);
                //if (SIMD::Any(mask))
                //{
                //    pixel.mask = mask;
                    m_PixelScreenSpace.push_back(pixel);
                //}
            }
        }
    }

    std::vector<SIMDPixel>::iterator begin()
    {
        return m_PixelScreenSpace.begin();
    }

    std::vector<SIMDPixel>::iterator end()
    {
        return m_PixelScreenSpace.end();
    }

    void Clear()
    {
        for (int i = 0; i < m_Pixelbuffer.size(); i++)
        {
            m_Pixelbuffer[i].clear();
        }
        m_PixelScreenSpace.clear();
    }

private:
    std::vector<std::vector<SIMDPixel>> m_Pixelbuffer;
    std::vector<SIMDPixel> m_PixelScreenSpace;
    int m_width{};
    int m_height{};
};

