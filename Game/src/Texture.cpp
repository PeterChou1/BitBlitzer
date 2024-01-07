#include "stdafx.h"
#include "../stb_image/stb_image.h"
#include "Texture.h"
#include <utility>
#include <cassert>
#include <iostream>


// Custom deleter function for stbi_image_free
struct STBImageDeleter
{
    void operator()(unsigned char* data) const
    {
        stbi_image_free(data);
    }
};


Texture::Texture(const char* fileName,
                             Vec3 ambient,
                             Vec3 diffuse,
                             Vec3 specular,
                             float highlight)
    : ambient(ambient)
      , diffuse(diffuse), specular(specular), highlight(highlight)
{
    const bool texLoaded = LoadTexture(fileName);
    assert(texLoaded && "Failed to Load Texture");
}


void Texture::Sample(float u, float v, float& r, float& g, float& b)
{
    u = std::min(1.0f, std::max(0.0f, u));
    v = std::min(1.0f, std::max(0.0f, v));
    // Convert u and v to pixel coordinates
    const int x = static_cast<int>(u * (mtexWidth - 1));
    const int y = static_cast<int>(v * (mtexHeight - 1));

    // Calculate the offset for the pixel
    const int offset = (y * mtexWidth + x) * 4; // 4 channels per pixel (RGBA)
    // Extract the RGB values
    r = texture[offset];
    g = texture[offset + 1];
    b = texture[offset + 2];
}

void Texture::SampleSIMD(SIMDVec2& tex, SIMDFloat& r, SIMDFloat& g, SIMDFloat& b)
{
    SIMDFloat u = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, tex.x);
    SIMDFloat v = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, tex.y);

    const SIMDFloat x = (u * (mtexWidth - 1)).floor();
    const SIMDFloat y = (v * (mtexHeight - 1)).floor();
    SIMDFloat offset = (y * mtexWidth + x) * 4;

    auto loadRGB = [&](SIMDFloat& color, SIMDFloat off)
    {
        color[0] = texture[static_cast<int>(off[0])];
        color[1] = texture[static_cast<int>(off[1])];
        color[2] = texture[static_cast<int>(off[2])];
        color[3] = texture[static_cast<int>(off[3])];
        color[4] = texture[static_cast<int>(off[4])];
        color[5] = texture[static_cast<int>(off[5])];
        color[6] = texture[static_cast<int>(off[6])];
        color[7] = texture[static_cast<int>(off[7])];
    };

    loadRGB(r, offset);
    loadRGB(g, offset + 1);
    loadRGB(b, offset + 2);

}

bool Texture::LoadTexture(const char* filename)
{
    int channels;
    unsigned char* data = stbi_load(filename, &mtexWidth, &mtexHeight, &channels, 4);
    if (data)
    {
        texture = std::shared_ptr<unsigned char[]>(data, STBImageDeleter());
        return true;
    }
    return false;
}
