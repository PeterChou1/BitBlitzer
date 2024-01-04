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
