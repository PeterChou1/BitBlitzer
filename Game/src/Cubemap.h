//---------------------------------------------------------------------------------
// Cubemap.h
//---------------------------------------------------------------------------------
//
// A giant cube texture that encompasses the camera
//
#pragma once
#include <cassert>
#include <vector>
#include <unordered_map>

#include "Material.h"
#include "Resource.h"
#include "Vertex.h"

#include "RenderConstants.h"


/**
 * \brief Types of cube maps you can load in engine
 */
enum CubeMapType
{
    SkyBox
};


class CubeMap : public Resource
{
public:
    CubeMap() = default;

    void ResetResource() override
    {
        m_BackTexture.ResetMaterial();
        m_BottomTexture.ResetMaterial();
        m_FrontTexture.ResetMaterial();
        m_LeftTexture.ResetMaterial();
        m_RightTexture.ResetMaterial();
        m_TopTexture.ResetMaterial();
        CubeMapEnabled = false;
    }

    void LoadCubeMap(CubeMapType typeID)
    {
        CubeMapEnabled = true;
        switch (typeID)
        {
        case SkyBox:
            LoadCubeMapFaces(BackFace, "./Assets/skybox/back.jpg");
            LoadCubeMapFaces(BottomFace, "./Assets/skybox/bottom.jpg");
            LoadCubeMapFaces(FrontFace, "./Assets/skybox/front.jpg");
            LoadCubeMapFaces(LeftFace, "./Assets/skybox/left.jpg");
            LoadCubeMapFaces(RightFace, "./Assets/skybox/right.jpg");
            LoadCubeMapFaces(TopFace, "./Assets/skybox/top.jpg");
            break;
        }
    }

    static void CreateCubeSides(
        Vertex v1,
        Vertex v2,
        Vertex v3,
        float length,
        CubeMapFaces face,
        std::vector<Vertex>& vertexBuffer,
        std::vector<uint32_t>& indexBuffer
    )
    {
        v1.LocalPosition = (v1.LocalPosition * 2 * length) - length;
        v2.LocalPosition = (v2.LocalPosition * 2 * length) - length;
        v3.LocalPosition = (v3.LocalPosition * 2 * length) - length;
        // you can choose to shade the cube map by changing the bottom shader
        // but its not a very common usecase
        v1.ShaderID = UnlitShader;
        v2.ShaderID = UnlitShader;
        v3.ShaderID = UnlitShader;
        v1.CubeMapID = face;
        v2.CubeMapID = face;
        v3.CubeMapID = face;
        vertexBuffer.push_back(v3);
        vertexBuffer.push_back(v2);
        vertexBuffer.push_back(v1);
        indexBuffer.push_back(indexBuffer.size());
        indexBuffer.push_back(indexBuffer.size());
        indexBuffer.push_back(indexBuffer.size());
    }

    void GetCubeMapVertices
    (
        float farplane,
        std::vector<Vertex>& vertexBuffer,
        std::vector<uint32_t>& indexBuffer,
        BufferRange& cubeMapRange
    )
    {
        assert(vertexBuffer.size() == 0 && "cube maps can only be loaded on scene startup");
        assert(indexBuffer.size() == 0 && "cube maps can only be loaded on scene startup");

        float sides = std::sinf(45.0f * 3.141f / 180.0f) * farplane;

        // SOUTH face
        CreateCubeSides(
            Vertex(Vec3(0, 0, 0), Vec2(0, 1)), 
            Vertex(Vec3(0, 1, 0), Vec2(0, 0)), 
            Vertex(Vec3(1, 1, 0), Vec2(1, 0)),
            sides,
            BackFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(0, 0, 0), Vec2(0, 1)), 
            Vertex(Vec3(1, 1, 0), Vec2(1, 0)), 
            Vertex(Vec3(1, 0, 0), Vec2(1, 1)),
            sides,
            BackFace,
            vertexBuffer,
            indexBuffer
        );
        // EAST face
        CreateCubeSides(
            Vertex(Vec3(1, 0, 0), Vec2(0, 1)), 
            Vertex(Vec3(1, 1, 0), Vec2(0, 0)), 
            Vertex(Vec3(1, 1, 1), Vec2(1, 0)),
            sides,
            LeftFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(1, 0, 0), Vec2(0, 1)), 
            Vertex(Vec3(1, 1, 1), Vec2(1, 0)), 
            Vertex(Vec3(1, 0, 1), Vec2(1, 1)),
            sides,
            LeftFace,
            vertexBuffer,
            indexBuffer
        ),
        // NORTH face
        CreateCubeSides(
            Vertex(Vec3(1, 0, 1), Vec2(0, 1)), 
            Vertex(Vec3(1, 1, 1), Vec2(0, 0)), 
            Vertex(Vec3(0, 1, 1), Vec2(1, 0)),
            sides,
            FrontFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(1, 0, 1), Vec2(0, 1)), 
            Vertex(Vec3(0, 1, 1), Vec2(1, 0)), 
            Vertex(Vec3(0, 0, 1), Vec2(1, 1)),
            sides,
            FrontFace,
            vertexBuffer,
            indexBuffer
        );
        // WEST face
        CreateCubeSides(
            Vertex(Vec3(0, 0, 1), Vec2(0, 1)), 
            Vertex(Vec3(0, 1, 1), Vec2(0, 0)), 
            Vertex(Vec3(0, 1, 0), Vec2(1, 0)),
            sides,
            RightFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(0, 0, 1), Vec2(0, 1)), 
            Vertex(Vec3(0, 1, 0), Vec2(1, 0)), 
            Vertex(Vec3(0, 0, 0), Vec2(1, 1)),
            sides,
            RightFace,
            vertexBuffer,
            indexBuffer
        );
     
         // TOP face
        CreateCubeSides(
            Vertex(Vec3(0, 1, 0), Vec2(1, 0)),
            Vertex(Vec3(0, 1, 1), Vec2(1, 1)),
            Vertex(Vec3(1, 1, 1), Vec2(0, 1)),
            sides,
            TopFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(0, 1, 0), Vec2(1, 0)),
            Vertex(Vec3(1, 1, 1), Vec2(0, 1)),
            Vertex(Vec3(1, 1, 0), Vec2(0, 0)),
            sides,
            TopFace,
            vertexBuffer,
            indexBuffer
        );
     
        // BOTTOM face
        CreateCubeSides(
            Vertex(Vec3(1, 0, 1), Vec2(0, 0)),
            Vertex(Vec3(0, 0, 1), Vec2(1, 0)),
            Vertex(Vec3(0, 0, 0), Vec2(1, 1)),
            sides,
            BottomFace,
            vertexBuffer,
            indexBuffer
        );
        CreateCubeSides(
            Vertex(Vec3(1, 0, 1), Vec2(0, 0)),
            Vertex(Vec3(0, 0, 0), Vec2(1, 1)),
            Vertex(Vec3(1, 0, 0), Vec2(0, 1)),
            sides,
            BottomFace,
            vertexBuffer,
            indexBuffer
        );

        cubeMapRange.first = 0;
        cubeMapRange.second = vertexBuffer.size();
    }

    void LoadCubeMapFaces(CubeMapFaces faces, const char* cubeMapTexture)
    {
        switch (faces)
        {
        case BackFace:
            m_BackTexture.LoadMaterial(cubeMapTexture);
            break;
        case BottomFace:
            m_BottomTexture.LoadMaterial(cubeMapTexture);
            break;
        case FrontFace:
            m_FrontTexture.LoadMaterial(cubeMapTexture);
            break;
        case LeftFace:
            m_LeftTexture.LoadMaterial(cubeMapTexture);
            break;
        case RightFace:
            m_RightTexture.LoadMaterial(cubeMapTexture);
            break;
        case TopFace:
            m_TopTexture.LoadMaterial(cubeMapTexture);
            break;
        }
    }


    Material& GetMaterial(CubeMapFaces faces)
    {
        switch (faces)
        {
        case BackFace:
            return m_BackTexture;
        case BottomFace:
            return m_BottomTexture;
        case FrontFace:
            return m_FrontTexture;
        case LeftFace:
            return m_LeftTexture;
        case RightFace:
            return m_RightTexture;
        case TopFace:
            return m_TopTexture;
        default:
            assert(false && "Should not happen");
            // This is to ensure the release compilation does not fail
            return m_BackTexture;
        }
    }

    // set this boolean to true to disable cube map
    bool CubeMapEnabled;

private:
    Material m_BackTexture;
    Material m_BottomTexture;
    Material m_FrontTexture;
    Material m_LeftTexture;
    Material m_RightTexture;
    Material m_TopTexture;

};
