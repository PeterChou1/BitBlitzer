#include "stdafx.h"
#include "../stb_image/stb_image.h"
#include "Material.h"
#include <utility>
#include <cassert>
#include <iostream>
#include "Material.h"


// Custom deleter function for stbi_image_free
struct STBImageDeleter
{
    void operator()(unsigned char* data) const
    {
        stbi_image_free(data);
    }
};

Material Material::DefaultMaterial = Material({ 0, 0, 0 }, { 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0 }, 10.0);

Material::Material(Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight)
    : ambient(ambient),
      diffuse(diffuse),
      specular(specular),
      highlight(highlight),
      hasTexture(false)
{
}

Material::Material(const char* fileName,
                             Vec3 ambient,
                             Vec3 diffuse,
                             Vec3 specular,
                             float highlight)
    : ambient(ambient),
      diffuse(diffuse),
      specular(specular),
      highlight(highlight),
      hasTexture(true)
{
    const bool texLoaded = LoadTexture(fileName);
    assert(texLoaded && "Failed to Load Material");
}


void Material::SampleSIMD(SIMDVec2& tex, SIMDFloat& r, SIMDFloat& g, SIMDFloat& b) const
{
    if (!hasTexture)
    {
        r = 255 * diffuse[0];
        g = 255 * diffuse[1];
        b = 255 * diffuse[2];
        return;
    }

    SIMDFloat u = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, tex.X);
    SIMDFloat v = SIMD::Clamp(SIMD::ZERO, SIMD::ONE, tex.Y);

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

bool Material::LoadTexture(const char* filename)
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

