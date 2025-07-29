#include "FuzzKillUI.hpp"
#include "clay.h"
#include "raylib.h"
#include "renderer/clay_renderer_raylib.h"
#include "types/Error.hpp"
#include "types/WinProcess.hpp"
#include "utils/ColorUtils.hpp"
#include "utils/ProcessLayer.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>

// TODO: Move all of the Win32API stuff to a wrapper

constexpr Clay_Sizing SIZE_AUTO_GROW_XY = Clay_Sizing { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() };
constexpr float BG_COLOR_ALPHA = 50;
constexpr float INPUT_DELAY_TIME = 0.16f;
float inputDelayTimer = 0.f;

constexpr Clay_TextElementConfig DefaultText(uint16_t fontSize, Clay_Color color = ColorUtils::Black()) {
	return {
		.textColor = color,	
		.fontId = 0,
		.fontSize = fontSize,
	};
}

Clay_String StrToClayString(const char* data, size_t size) noexcept {
    return Clay_String{ false, static_cast<int32_t>(size), data};
}


void FuzzKillUI::Init() {
	// Gather all the processes here from the windows API
	printf("Calling init!\n");
	EError err = ProcessLayer::FetchProcessesInto(&this->m_activeProcesses);
	if (err != EError::Ok) {
		fprintf(stderr, "Failed to query process data, error no: %d\n", static_cast<uint32_t>(err));
	}
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
    bgColor.a = BG_COLOR_ALPHA;
    ClearBackground(bgColor);

    this->HandleKeyboardInput(delta);
    this->DrawUI();
	    
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    Clay_Raylib_Render(renderCommands, fonts);
    
    EndDrawing();
}


void FuzzKillUI::DrawUI() {	
    CLAY({.id = CLAY_ID("MainContainer"), .layout = { .sizing = SIZE_AUTO_GROW_XY, .layoutDirection = CLAY_TOP_TO_BOTTOM }}) {
        CLAY_TEXT(CLAY_STRING("Welcome to FuzzKill!"), CLAY_TEXT_CONFIG(DefaultText(72)));
        int32_t cntr = 0;
        for (const WinProcess& process : this->m_activeProcesses) {
        	this->DrawProcessListItem(process, cntr);
        	cntr++;
        }
    }
}

void FuzzKillUI::HandleKeyboardInput(float delta) {
	if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
		this->selectedProcess = (this->selectedProcess + 1) % this->m_activeProcesses.size();
	}
	else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
		this->selectedProcess = (this->selectedProcess - 1) % this->m_activeProcesses.size();
	}
	if (IsKeyPressed(KEY_ENTER)) {
		ProcessLayer::SwitchWindow(this->m_activeProcesses[this->selectedProcess].windowHandle);
	}
}

void FuzzKillUI::DrawProcessListItem(const WinProcess& processInfo, int32_t index) {
	const Clay_LayoutConfig layoutConfig = {
		.sizing = SIZE_AUTO_GROW_XY
	};
	const Clay_BorderElementConfig borderConfig = {
		.color = ColorUtils::Red()
	};
	constexpr Clay_Color inactiveColor = ColorUtils::White(BG_COLOR_ALPHA * 2);
	constexpr Clay_Color activeColor = ColorUtils::Red(BG_COLOR_ALPHA * 2);

	Clay_String nameStr = StrToClayString(processInfo.name, strlen(processInfo.name));
	CLAY({ .id = CLAY_IDI("ProcessContainer", index), .layout = layoutConfig, .backgroundColor = index == selectedProcess ? activeColor : inactiveColor, .border = borderConfig}) {
	    CLAY_TEXT(nameStr, CLAY_TEXT_CONFIG(DefaultText(32)));
	}
	
}

