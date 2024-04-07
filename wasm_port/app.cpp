//---------------------------------------------------------------------------------
// App.cpp
//---------------------------------------------------------------------------------
#include <string>
#include "app.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

//---------------------------------------------------------------------------------
// Utils and externals for system info.
extern SDL_Renderer* rendererSDL;


namespace App
{

    void DrawLine(float sx, float sy, float ex, float ey, float r, float g, float b)
    {
        SDL_SetRenderDrawColor(rendererSDL, r * 255, g * 255, b * 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(rendererSDL, static_cast<int>(sx), static_cast<int>(sy), static_cast<int>(ex), static_cast<int>(ey));
    }

    // This prints a string to the screen
    void Print(float x, float y, const char* st, float r, float g, float b)
    {
        // Initialize SDL_ttf if it hasn't been initialized already
        if (TTF_Init() == -1)
        {
            std::cout << "Failed to Initialize TTF\n";
            return;
        }

        // Load a font (specify your font path and size)
        TTF_Font* font = TTF_OpenFont("./Assets/arial.ttf", 16); // Adjust font path and size
        if (!font)
        {
            std::cout << "Failed to load font\n";
            return;
        }

        // Set text color
        SDL_Color color = { static_cast<Uint8>(r * 255), static_cast<Uint8>(g * 255), static_cast<Uint8>(b * 255), 255 };

        // Render the text to a surface
        SDL_Surface* surface = TTF_RenderText_Blended(font, st, color);
        if (!surface)
        {
            std::cout << "Failed to create surface from text\n";
            TTF_CloseFont(font);
            return;
        }

        // Convert the surface to a texture
        SDL_Texture* texture = SDL_CreateTextureFromSurface(rendererSDL, surface);
        if (!texture)
        {
            std::cout <<  "Failed to create texture from surface\n";
            SDL_FreeSurface(surface);
            TTF_CloseFont(font);
            return;
        }

        // Prepare the destination rectangle
        SDL_Rect dstrect = { static_cast<int>(x), static_cast<int>(y), surface->w, surface->h };
        // Copy the texture to the renderer
        SDL_RenderCopy(rendererSDL, texture, NULL, &dstrect);
        // Clean up
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        TTF_CloseFont(font);
    }

    void PlaySound(const char* fileName, bool looping)
    {

        std::cout << "play sound " << fileName << "\n";
        
        Mix_Chunk* sound = Mix_LoadWAV(fileName);

        if (sound == NULL) {
            std::cerr << "Failed to load sound. SDL_mixer Error: " << Mix_GetError() << "\n";
            return;
        }

        if (Mix_PlayChannel(-1, sound, looping ? -1 : 0) == -1) {
            std::cerr << "Failed to play sound. SDL_mixer Error: " << Mix_GetError() << "\n";
        }

        // Mix_FreeChunk(sound);
    }

    bool IsSoundPlaying(const char* filename)
    {
        return false;
    }

    bool IsKeyPressed(int key)
    {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        SDL_Scancode sdl_key = SDL_SCANCODE_UNKNOWN;

        if (key == ' ') {
            sdl_key = SDL_SCANCODE_SPACE;
        }
        else if (key == '0') {
            sdl_key = SDL_SCANCODE_0;
        }
        else if (key >= '1' && key <= '9') {
            sdl_key = static_cast<SDL_Scancode>(SDL_SCANCODE_1 + (key - '1'));
        }
        else if (key >= 'a' && key <= 'z') {
            sdl_key = static_cast<SDL_Scancode>(SDL_SCANCODE_A + (key - 'a'));
        }
        else if (key >= 'A' && key <= 'Z') {
            sdl_key = static_cast<SDL_Scancode>(SDL_SCANCODE_A + (key - 'A'));
        }

        // Optionally, handle more keys or provide a default case
        if (sdl_key == SDL_SCANCODE_UNKNOWN) {
            return false; // Key not handled
        }

        return state[sdl_key] != 0;
    }

    void GetMousePos(float& x, float& y)
    {
        int x_i;
        int y_i;
        SDL_GetMouseState(&x_i, &y_i);
        x = static_cast<float>(x_i);
        y = static_cast<float>(y_i);
    }

}
