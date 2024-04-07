#include "stdafx.h"
#include "FragmentShader.h"

#include "ECSManager.h"
#include "AssetServer.h"
#include "RenderDirect.h"
#include "app.h"

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
    mTexture = SDL_CreateTexture(rendererSDL, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);
    if (mTexture == 0) 
    {
        std::cerr << "Failed to create texture\n";
    }
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
    if (mTexture == 0)
    {
        std::cerr << "Failed to create texture\n";
    }
    unsigned char* pixels; // Your pixel data, should be of size APP_VIRTUAL_WIDTH * APP_VIRTUAL_HEIGHT * 4
    int pitch; // Width of the texture in bytes.
    int success = SDL_LockTexture(mTexture, NULL, (void**)&pixels, &pitch);
    if (success < 0)
    {
        std::cout << "failed to lock texture again: (" << SDL_GetError() << ")\n";
    }
    for (int y = 0; y < APP_VIRTUAL_HEIGHT; ++y) {
        for (int x = 0; x < APP_VIRTUAL_WIDTH; ++x) {
            unsigned char r, g, b;
            // Retrieve color from your color buffer
            m_ColorBuffer->GetColor(x, y, r, g, b);
            // Calculate the offset into the pixels array.
            int offset = ((y * APP_VIRTUAL_WIDTH) + x) * 4; // 3 bytes per pixel in RGB888
            // Set the pixel color. Note: The order of RGB might need to be adjusted based on your buffer format.
            pixels[offset] = b;     // Blue
            pixels[offset + 1] = g;   // Green
            pixels[offset + 2] = r;   // Red
            pixels[offset + 3] = 255;   // ALPHA
        }
    }
    SDL_UnlockTexture(mTexture);
    SDL_RenderCopy(rendererSDL, mTexture, NULL, NULL);
#endif
}