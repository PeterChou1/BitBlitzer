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
    m_Cam = ECS.GetResource<Camera>();
}

void FragmentShader::Shade()
{
    AssetServer& loader = AssetServer::GetInstance();
    std::vector<std::vector<Triangle>> ClippedTriangle = m_ClippedTriangle->buffer;
    std::vector<PointLight> m_Lights;
    m_Lights.push_back({ Vec3(0, 5, 5), Vec3(1, 1, 1), 40.0 });
    
    Concurrent::ForEach(m_PixelBuffer->begin(), m_PixelBuffer->end(), [&](SIMDPixel& pixel)
    {
        Triangle& triangle = ClippedTriangle[pixel.binId][pixel.binIndex];
        pixel.Interpolate(triangle);
        Material& texture = loader.GetMaterial(triangle.GetTextureID());
        SIMDShader* shader = loader.GetShader(triangle.GetShaderID());
        shader->Shade(pixel, m_Lights, texture, *m_Cam);
        SIMDVec3 color = pixel.color * 255.0;

        color.x = color.x.floor();
        color.y = color.y.floor();
        color.z = color.z.floor();

        if (pixel.mask.GetBit(0))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[0], pixel.screenSpacePosition.y[0], color.x[0], color.y[0], color.z[0]);
        }

        if (pixel.mask.GetBit(1))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[1], pixel.screenSpacePosition.y[1], color.x[1], color.y[1], color.z[1]);
        }

        if (pixel.mask.GetBit(2))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[2], pixel.screenSpacePosition.y[2], color.x[2], color.y[2], color.z[2]);
        }

        if (pixel.mask.GetBit(3))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[3], pixel.screenSpacePosition.y[3], color.x[3], color.y[3], color.z[3]);
        }

        if (pixel.mask.GetBit(4))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[4], pixel.screenSpacePosition.y[4], color.x[4], color.y[4], color.z[4]);
        }

        if (pixel.mask.GetBit(5))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[5], pixel.screenSpacePosition.y[5], color.x[5], color.y[5], color.z[5]);
        }

        if (pixel.mask.GetBit(6))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[6], pixel.screenSpacePosition.y[6], color.x[6], color.y[6], color.z[6]);
        }

        if (pixel.mask.GetBit(7))
        {
            m_ColorBuffer->SetColor(pixel.screenSpacePosition.x[7], pixel.screenSpacePosition.y[7], color.x[7], color.y[7], color.z[7]);
        }
    });

    // Blit to OpenGL Texture
    App::RenderTexture(m_ColorBuffer->GetBuffer());
}