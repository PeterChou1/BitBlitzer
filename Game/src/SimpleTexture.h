#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "Vec3.h"

class SimpleTexture
{
public:
    SimpleTexture() : texture(nullptr), mtexWidth(0), mtexHeight(0)
    {
    };
    SimpleTexture(const char* fileName, Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);
    void Sample(float u, float v, float& r, float& g, float& b);

    Vec3 ambient{};
    Vec3 diffuse{};
    Vec3 specular{};
    float highlight{};

private:
    bool LoadTexture(const char* filename);
    int mtexWidth;
    int mtexHeight;
    std::shared_ptr<unsigned char[]> texture;
};


/*
 * /brief stores every single texture in the game
 */
struct TextureList
{
    std::unordered_map<std::string, size_t> textureIDs;
    std::vector<SimpleTexture> textureList;
};
