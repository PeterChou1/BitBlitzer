#include "stdafx.h"
#include "FragmentShader.h"

#include "ECSManager.h"
#include "AssetServer.h"
#include "../App/app.h"

extern ECSManager ECS;

FragmentShader::FragmentShader()
{
    m_PixelBuffer = ECS.GetResource<PixelBuffer>();
    m_ClippedTriangle = ECS.GetResource<ClippedTriangleBuffer>();
    m_ColorBuffer = ECS.GetResource<ColorBuffer>();
    m_CubeMap = ECS.GetResource<CubeMap>();
    m_Cam = ECS.GetResource<Camera>();
}

void FragmentShader::Shade()
{
    AssetServer& loader = AssetServer::GetInstance();
    std::vector<std::vector<Triangle>> ClippedTriangle = m_ClippedTriangle->Buffer;
    std::vector<PointLight> m_Lights;
    m_Lights.push_back({ Vec3(0, 5, 5), Vec3(1, 1, 1), 40.0 });
    
    Concurrent::ForEach(m_PixelBuffer->begin(), m_PixelBuffer->end(), [&](SIMDPixel& pixel)
    {
        Triangle& triangle = ClippedTriangle[pixel.BinId][pixel.BinIndex];
        pixel.Interpolate(triangle);


        SIMDShader* shader = loader.GetShader(triangle.GetShaderID());
        if (triangle.GetCubmapID() != NotACubeMap)
        {
            Material& texture = m_CubeMap->GetMaterial(triangle.GetCubmapID());
            shader->Shade(pixel, m_Lights, texture, *m_Cam);
        }
        else
        {
            Material& texture = loader.GetMaterial(triangle.GetTextureID());
            shader->Shade(pixel, m_Lights, texture, *m_Cam);
        }

        SIMDVec3 color = pixel.Color * 255.0;

        color.X = color.X.floor();
        color.Y = color.Y.floor();
        color.Z = color.Z.floor();

        if (pixel.Mask.GetBit(0))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[0], pixel.ScreenSpacePosition.Y[0], color.X[0], color.Y[0], color.Z[0]);
        }

        if (pixel.Mask.GetBit(1))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[1], pixel.ScreenSpacePosition.Y[1], color.X[1], color.Y[1], color.Z[1]);
        }

        if (pixel.Mask.GetBit(2))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[2], pixel.ScreenSpacePosition.Y[2], color.X[2], color.Y[2], color.Z[2]);
        }

        if (pixel.Mask.GetBit(3))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[3], pixel.ScreenSpacePosition.Y[3], color.X[3], color.Y[3], color.Z[3]);
        }

        if (pixel.Mask.GetBit(4))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[4], pixel.ScreenSpacePosition.Y[4], color.X[4], color.Y[4], color.Z[4]);
        }

        if (pixel.Mask.GetBit(5))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[5], pixel.ScreenSpacePosition.Y[5], color.X[5], color.Y[5], color.Z[5]);
        }

        if (pixel.Mask.GetBit(6))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[6], pixel.ScreenSpacePosition.Y[6], color.X[6], color.Y[6], color.Z[6]);
        }

        if (pixel.Mask.GetBit(7))
        {
            m_ColorBuffer->SetColor(pixel.ScreenSpacePosition.X[7], pixel.ScreenSpacePosition.Y[7], color.X[7], color.Y[7], color.Z[7]);
        }
    });

    // Render to OpenGL Texture
    App::RenderTexture(m_ColorBuffer->GetBuffer());
}