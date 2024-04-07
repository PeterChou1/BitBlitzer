#include "stdafx.h"

#include "RenderDirect.h"
#include "app.h"


void DirectRender::SetupGL()
{
    // glGenTextures(1, &textureID); // Generate texture ID
    // glBindTexture(GL_TEXTURE_2D, textureID);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectRender::RenderTexture(unsigned char* imageData)
{
    // SDL_Texture* texture = SDL_CreateTexture(rendererSDL, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT);
    // // Update the texture with the pixel data
    // SDL_UpdateTexture(texture, NULL, imageData, APP_VIRTUAL_WIDTH * 3); // 3 bytes per pixel in RGB format
    // 
    // // Copy the texture to the renderer
    // SDL_RenderCopy(rendererSDL, texture, NULL, NULL); // Render the entire texture to the entire rendering target
    // 
    // // Cleanup
    // SDL_DestroyTexture(texture);
}