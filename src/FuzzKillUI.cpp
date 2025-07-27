#include "FuzzKillUI.hpp"
#include "clay.h"
#include "raylib.h"
#include "renderer/clay_renderer_raylib.c"
#include "utils/ColorUtils.hpp"

// TODO: Move all of the Win32API stuff to a wrapper

constexpr Clay_Sizing SIZE_AUTO_GROW_XY = Clay_Sizing { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };

constexpr Clay_TextElementConfig DefaultText(uint16_t fontSize, Clay_Color color = ColorUtils::Black()) {
	return {
		.textColor = color,	
		.fontId = 0,
		.fontSize = fontSize,
	};
}


FuzzKillUI::FuzzKillUI() {
	// Gather all the processes here from the windows API
	EnumProcesses()
}

void FuzzKillUI::OnUpdate(float delta, Font* fonts) {
    Vector2 windowSize = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    
    
    Clay_Vector2 mousePosition = {GetMousePosition().x, GetMousePosition().y};
    Clay_Vector2 mouseScroll = {GetMouseWheelMoveV().x, GetMouseWheelMoveV().y};

    Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));
    Clay_UpdateScrollContainers(false, mouseScroll, GetFrameTime());
    Clay_SetLayoutDimensions({.width = windowSize.x, .height = windowSize.y});
    Clay_BeginLayout();

    
    BeginDrawing();
    // ClearBackground(ColorUtils::ToRaylibColor(ColorUtils::White(0.0f)));
    auto bgColor = DARKBLUE;
    bgColor.a = 50;
    ClearBackground(bgColor);

    this->DrawUI();
	    
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    Clay_Raylib_Render(renderCommands, fonts);
    
    EndDrawing();
}


void FuzzKillUI::DrawUI() {	
    CLAY({.id = CLAY_ID("MainContainer"), .layout = { .sizing = SIZE_AUTO_GROW_XY, .layoutDirection = CLAY_TOP_TO_BOTTOM }}) {
        CLAY_TEXT(CLAY_STRING("Welcome to FuzzKill!"), CLAY_TEXT_CONFIG(DefaultText(72)));
    }
}


