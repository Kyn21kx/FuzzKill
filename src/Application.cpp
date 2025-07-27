#include "Application.hpp"
#include "FuzzKillUI.hpp"
#include <cstdint>
#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib.h"
#include "renderer/clay_renderer_raylib.c"
#include <cstdlib>

constexpr int32_t INIT_WIDTH = 1080;
constexpr int32_t INIT_HEIGHT = 480;

Font fonts[10];
Clay_Arena clayArena;
FuzzKillUI fuzzKillUI;

void HandleClayErrors(Clay_ErrorData error) {
    printf("%s", error.errorText.chars);
}

void Application::Init() {
	printf("Init!\n");
	uint32_t memorySize = Clay_MinMemorySize();
	clayArena = Clay_CreateArenaWithCapacityAndMemory(memorySize, malloc(memorySize));

	Clay_Raylib_Initialize(INIT_WIDTH, INIT_HEIGHT, "FuzzKill", FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);
	Clay_Initialize(clayArena, (Clay_Dimensions){INIT_WIDTH, INIT_HEIGHT}, (Clay_ErrorHandler){HandleClayErrors});
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    this->LoadResources();
}

void Application::Run() {
    while (!WindowShouldClose()) {
    	fuzzKillUI.OnUpdate(GetFrameTime(), fonts);
    }
}

void Application::Dispose() {
	CloseWindow();
	free(clayArena.memory);
}

void Application::LoadResources() {	
  fonts[0] = LoadFontEx("assets/fonts/Nova_Square/NovaSquare-Regular.ttf", 72, 0, 0);        	  
  SetTextureFilter(fonts[0].texture, TEXTURE_FILTER_POINT);	
}
