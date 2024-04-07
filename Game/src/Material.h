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

    Material() = default;

    /**
     * \brief Loads a Material without a texture
     */
    Material(Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);

    /**
     * \brief Loads a Material with texture + Phong Model terms
     */
    Material(const char* fileName, Vec3 ambient, Vec3 diffuse, Vec3 specular, float highlight);


    /**
     * \brief Load a new texture deleting the old one
     * \param fileName 
     */
    void LoadMaterial(const char* fileName);

    /**
     * \brief Reset material clearing texture and lighting terms
     */
    void ResetMaterial();

    /**
     * \brief Samples 8 pixels at once 
     * \param tex UV coordinates of the 8 pixels
     * \param r output red channel
     * \param g output green channel
     * \param b output blue channel
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
