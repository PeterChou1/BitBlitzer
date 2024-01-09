#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "SIMD.h"
#include "Vec3.h"

class Material
{
public:

    Material(Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);

    Material(const char* fileName, Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);

    void SampleSIMD(SIMDVec2& tex, SIMDFloat& r, SIMDFloat& g, SIMDFloat& b) const;

    Vec3 ambient{};
    Vec3 diffuse{};
    Vec3 specular{};
    float highlight{};
    bool hasTexture;

    static Material DefaultMaterial;

private:
    bool LoadTexture(const char* filename);
    int mtexWidth{};
    int mtexHeight{};
    std::shared_ptr<unsigned char[]> texture;
};


/*
 * /brief stores every single texture in the game
 */
struct MaterialList
{
    std::unordered_map<std::string, size_t> textureIDs;
    std::vector<Material> textureList;
};
