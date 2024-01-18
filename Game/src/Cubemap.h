#pragma once
#include "Material.h"
#include "Resource.h"
#include "Vertex.h"
#include <vector>


class Cubemap : Resource
{
public:
    Cubemap() = default;



    void ResetResource() override
    {
        m_BackTexture.ResetMaterial();
        m_BottomTexture.ResetMaterial();
        m_FrontTexture.ResetMaterial();
        m_LeftTexture.ResetMaterial();
        m_RightTexture.ResetMaterial();
        m_TopTexture.ResetMaterial();
    }


    void GetCubeMapVertices
    (
        std::vector<Vertex>& vertexBuffer,
        std::vector<uint32_t>& indexBuffer
    )
    {
        
    }

    void LoadCubeMapFaces(CubeMapFaces faces, const char* cubeMapTexture)
    {
        switch (faces)
        {
        case Back:
            m_BackTexture.LoadMaterial(cubeMapTexture);
            break;
        case Bottom:
            m_BottomTexture.LoadMaterial(cubeMapTexture);
            break;
        case Front:
            m_FrontTexture.LoadMaterial(cubeMapTexture);
            break;
        case Left:
            m_LeftTexture.LoadMaterial(cubeMapTexture);
            break;
        case Right:
            m_RightTexture.LoadMaterial(cubeMapTexture);
            break;
        case Top:
            m_TopTexture.LoadMaterial(cubeMapTexture);
            break;
        }
    }

private:
    Material m_BackTexture;
    Material m_BottomTexture;
    Material m_FrontTexture;
    Material m_LeftTexture;
    Material m_RightTexture;
    Material m_TopTexture;

};
