//---------------------------------------------------------------------------------
// Material.h
//---------------------------------------------------------------------------------
// 
// A Material is a class representing the loaded .mtl material of an .obj object
// Depending on the material it may contain a texture or not
//
#pragma once

#include <memory>

#include "SIMD.h"
#include "Vec3.h"

class Material
{
public:
    /**
     * \brief Loads an Material without a texture
     */
    Material(Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);

    /**
     * \brief Loads a texture from filename + Phong Model terms
     */
    Material(const char* fileName, Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);

    /**
     * \brief Samples 8 pixels at once 
     * \param tex UV coordinates of the 8 pixels
     * \param r
     * \param g 
     * \param b 
     */
    void SampleSIMD(SIMDVec2& tex, SIMDFloat& r, SIMDFloat& g, SIMDFloat& b) const;


    Vec3 ambient{};
    Vec3 diffuse{};
    Vec3 specular{};
    float highlight{};
    bool hasTexture;

    // Default Material used if a .obj model has no texture information
    static Material DefaultMaterial;

private:
    bool LoadTexture(const char* filename);
    int mtexWidth{};
    int mtexHeight{};
    std::shared_ptr<unsigned char[]> texture;
};
