#include "stdafx.h"
#include "FragmentShader.h"

#include "ECSManager.h"
#include "AssetServer.h"
#include "RenderDirect.h"
#include "../App/app.h"

extern ECSManager ECS;

// By default opengl is not used in the rendering engine
// however we can speed things up by rendering directly to
// an opengl texture
// the below preprocessor is meant to serve as a demonstrator
// on how we could potentially speed up our renderer
// Comment in USE_DIRECT_RENDER to use modified optionally modified opengl code

// #define USE_DIRECT_RENDER


FragmentShader::FragmentShader()
{
    m_PixelBuffer = ECS.GetResource<PixelBuffer>();
    m_ClippedTriangle = ECS.GetResource<ClippedTriangleBuffer>();
    m_ColorBuffer = ECS.GetResource<ColorBuffer>();
    m_CubeMap = ECS.GetResource<CubeMap>();
    m_Cam = ECS.GetResource<Camera>();
    m_Light = ECS.GetResource<Lighting>();
#ifdef USE_DIRECT_RENDER
    DirectRender::SetupGL();
#endif
}

void FragmentShader::Shade()
{
    AssetServer& loader = AssetServer::GetInstance();
    std::vector<std::vector<Triangle>> ClippedTriangle = m_ClippedTriangle->Buffer;

    // No multiple light support for now 
    std::vector<PointLight> LightingList;
    LightingList.push_back(m_Light->GetLight());
    
    Concurrent::ForEach(m_PixelBuffer->begin(), m_PixelBuffer->end(), [&](SIMDPixel& pixel)
    {
        Triangle& triangle = ClippedTriangle[pixel.BinId][pixel.BinIndex];
        pixel.Interpolate(triangle);


        SIMDShader* shader = loader.GetShader(triangle.GetShaderID());
        if (triangle.GetCubmapID() != NotACubeMap)
        {
            Material& texture = m_CubeMap->GetMaterial(triangle.GetCubmapID());
            shader->Shade(pixel, LightingList, texture, *m_Cam);
        }
        else
        {
            Material& texture = loader.GetMaterial(triangle.GetTextureID());
            shader->Shade(pixel, LightingList, texture, *m_Cam);
        }

        SIMDVec3 color = pixel.Color * 255.0;

        color.X = color.X.floor();
        color.Y = color.Y.floor();
        color.Z = color.Z.floor();

        if (pixel.Mask.GetBit(0))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[0]), 
                static_cast<int>(pixel.ScreenSpacePosition.Y[0]), 
                static_cast<unsigned char>(color.X[0]), 
                static_cast<unsigned char>(color.Y[0]),
                static_cast<unsigned char>(color.Z[0])
            );
        }

        if (pixel.Mask.GetBit(1))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[1]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[1]),
                static_cast<unsigned char>(color.X[1]),
                static_cast<unsigned char>(color.Y[1]),
                static_cast<unsigned char>(color.Z[1])
            );
        }

        if (pixel.Mask.GetBit(2))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[2]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[2]),
                static_cast<unsigned char>(color.X[2]),
                static_cast<unsigned char>(color.Y[2]),
                static_cast<unsigned char>(color.Z[2])
            );
        }

        if (pixel.Mask.GetBit(3))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[3]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[3]),
                static_cast<unsigned char>(color.X[3]),
                static_cast<unsigned char>(color.Y[3]),
                static_cast<unsigned char>(color.Z[3])
            );
        }

        if (pixel.Mask.GetBit(4))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[4]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[4]),
                static_cast<unsigned char>(color.X[4]),
                static_cast<unsigned char>(color.Y[4]),
                static_cast<unsigned char>(color.Z[4])
            );
        }

        if (pixel.Mask.GetBit(5))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[5]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[5]),
                static_cast<unsigned char>(color.X[5]),
                static_cast<unsigned char>(color.Y[5]),
                static_cast<unsigned char>(color.Z[5])
            );
        }

        if (pixel.Mask.GetBit(6))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[6]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[6]),
                static_cast<unsigned char>(color.X[6]),
                static_cast<unsigned char>(color.Y[6]),
                static_cast<unsigned char>(color.Z[6])
            );
        }

        if (pixel.Mask.GetBit(7))
        {
            m_ColorBuffer->SetColor(
                static_cast<int>(pixel.ScreenSpacePosition.X[7]),
                static_cast<int>(pixel.ScreenSpacePosition.Y[7]),
                static_cast<unsigned char>(color.X[7]),
                static_cast<unsigned char>(color.Y[7]),
                static_cast<unsigned char>(color.Z[7])
            );
        }
    });

#ifdef USE_DIRECT_RENDER
    DirectRender::RenderTexture(m_ColorBuffer->GetBuffer());
#else
    // This does not work using multithreading
    // This is very hacky way to get around the fact that the API only supports
    // lines we draw tons of tiny lines
    // this enormously inefficient if we were able to render directly to an opengl
    // texture we be able to save 30ms - 20ms
    for (int y = 0; y < APP_VIRTUAL_HEIGHT; y++)
    {
        for (int x = 0; x < APP_VIRTUAL_WIDTH; x++)
        {
            unsigned char r, g, b;
            m_ColorBuffer->GetColor(x, y, r, g, b);
            App::DrawLine(
                static_cast<float>(x), 
                static_cast<float>(y), 
                static_cast<float>(x) + 1.0f, 
                static_cast<float>(y) + 1.0f, 
                static_cast<float>(r / 255.0), 
                static_cast<float>(g / 255.0), 
                static_cast<float>(b / 255.0)
            );
        }
    }
#endif
}