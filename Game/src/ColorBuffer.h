#pragma once
#include <vector>
#include <cassert>
#include "Concurrent.h"

struct Color
{
    unsigned char r, g, b;
};

class ColorBuffer
{
public:
    ColorBuffer()
    {
    }

    ColorBuffer(int width, int height) : m_height(height), m_width(width)
    {
        // RBG
        m_buffer.resize((width + 1) * (height + 1) * 3);
    }

    void SetColor(int x, int y, unsigned char r, unsigned char g, unsigned char b)
    {
        assert(0 <= x && x <= m_width && 0 <= y && y <= m_height && "out of bounds index");
        m_buffer[(y * m_width + x) * 3] = r;
        m_buffer[(y * m_width + x) * 3 + 1] = g;
        m_buffer[(y * m_width + x) * 3 + 2] = b;
    }

    void Clear()
    {
        Concurrent::ForEach(m_buffer.begin(), m_buffer.end(), [&](unsigned char& color)
        {
            color = 0.0f;
        });
    }


    unsigned char* GetBuffer()
    {
        return m_buffer.data();
    }

private:
    int m_height{};
    int m_width{};
    std::vector<unsigned char> m_buffer;
};
