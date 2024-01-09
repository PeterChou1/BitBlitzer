#pragma once
#include <vector>

#include "Resource.h"
#include "DepthBuffer.h"
#include "SIMDPixel.h"

class PixelBuffer : public Resource
{
public:

    PixelBuffer(int width, int height) : m_width(width / SIMDPixel::PIXEL_WIDTH),
        m_height(height / SIMDPixel::PIXEL_HEIGHT)
    {
        m_Pixelbuffer.resize(m_height * m_width);
    }

    void SetBuffer(int x, int y, const SIMDPixel& pixel, SIMDFloat& mask)
    {
        if (SIMD::All(mask)) {
            m_Pixelbuffer[y * m_width + x].clear();
        }
        m_Pixelbuffer[y * m_width + x].push_back(pixel);
    }

    /**
     * \brief Once pixel has been rasterized accumulate the pixel so we can distribute the pixels evenly across threads
     * \param depth
     */
    void AccumulatePixel(const DepthBuffer& depth)
    {
        for (int i = 0; i < m_Pixelbuffer.size(); i++)
        {
            for (SIMDPixel& pixel : m_Pixelbuffer[i])
            {
                SIMDFloat mask = pixel.depth == depth.GetBuffer(i);
                if (SIMD::Any(mask))
                {
                    pixel.mask = mask;
                    m_PixelScreenSpace.push_back(pixel);
                }
            }
        }
    }

    size_t size()
    {
        return m_PixelScreenSpace.size();
    }

    SIMDPixel& operator[] (int i)
    {
        return m_PixelScreenSpace[i];
    }

    std::vector<SIMDPixel>::iterator begin()
    {
        return m_PixelScreenSpace.begin();
    }

    std::vector<SIMDPixel>::iterator end()
    {
        return m_PixelScreenSpace.end();
    }

    void ResetResource() override
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
