//---------------------------------------------------------------------------------
// Main entry point adjusted for SDL.
//---------------------------------------------------------------------------------
#include <iostream>
#include <emscripten.h>
#include <SDL2/SDL_mixer.h>
#include "Main.h"
#include "AppSettings.h"

int WINDOW_WIDTH = APP_INIT_WINDOW_WIDTH;
int WINDOW_HEIGHT = APP_INIT_WINDOW_HEIGHT;

SDL_Window* window = nullptr;
SDL_Renderer* rendererSDL = nullptr;

//---------------------------------------------------------------------------------
// User implemented methods.
//---------------------------------------------------------------------------------

extern void Init();

extern void Update(float deltaTime);

extern void Render();

extern void Shutdown();



Uint64 NOW;
Uint64 LAST = 0;
double deltaTime = 0;

bool initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		std::cerr <<  "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
		return false;
	}

	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) {
		std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
		return false;
	}

	// Create renderer for window
	rendererSDL = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!rendererSDL) {
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
		return false;
	}
	// Initialize SDL_mixer for audio if needed here

	return true;
}

void closeSDL() {
	// Destroy window    
	SDL_DestroyRenderer(rendererSDL);
	SDL_DestroyWindow(window);
	window = nullptr;
	rendererSDL = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

void mainLoopUpdate() {
	bool quit = false;
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			quit = true;
		}
		// Handle other events such as input here
	}

	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();
	deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

	Update(deltaTime);
	// Clear screen
	SDL_SetRenderDrawColor(rendererSDL, 0, 0, 0, 0);
	SDL_RenderClear(rendererSDL);

	Render();

	// Update screen
	SDL_RenderPresent(rendererSDL);
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!initSDL())
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}

	Init();
	Uint64 NOW = SDL_GetPerformanceCounter();
	emscripten_set_main_loop(mainLoopUpdate, 0, true);
	//Free resources and close SDL
	closeSDL();
	return 0;
}
