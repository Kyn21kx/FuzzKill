#include "FuzzKillUI.hpp"
#include "clay.h"
#include "raylib.h"
#include "renderer/clay_renderer_raylib.h"
#include "types/Error.hpp"
#include "types/WinProcess.hpp"
#include "utils/ArrayUtils.h"
#include "utils/ColorUtils.hpp"
#include "utils/ProcessLayer.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <vector>

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


void FuzzKillUI::PreAllocate() {
	this->m_query.reserve(WinProcess::MAX_PROCESS_NAME);
	this->m_activeProcessesNames.reserve(this->m_activeProcesses.size());
}

void FuzzKillUI::Init() {
	this->m_query = "";
	this->selectedProcess = 0;
	this->m_filteredProcesses.clear();
	this->m_activeProcesses.clear();
	this->m_activeProcessesNames.clear();
	// Gather all the processes here from the windows API
	EError err = ProcessLayer::FetchProcessesInto(&this->m_activeProcesses);
	if (err != EError::Ok) {
		fprintf(stderr, "Failed to query process data, error no: %d\n", static_cast<uint32_t>(err));
		return;
	}

	for (int32_t i = 0; i < this->m_activeProcesses.size(); i++) {
		this->m_filteredProcesses.emplace_back(i);
		this->m_activeProcessesNames.emplace_back(this->m_activeProcesses[i].name);
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
    auto bgColor = BLANK;
    bgColor.a = BG_COLOR_ALPHA;
    ClearBackground(bgColor);

    this->HandleKeyboardInput(delta);
    this->DrawUI();
	    
    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    Clay_Raylib_Render(renderCommands, fonts);
    
    EndDrawing();
}


void FuzzKillUI::DrawUI() {	
    CLAY({.id = CLAY_ID("MainContainer"), .layout = { .sizing = SIZE_AUTO_GROW_XY, .layoutDirection = CLAY_TOP_TO_BOTTOM }, .backgroundColor = ColorUtils::White(BG_COLOR_ALPHA * 2)}) {
    	bool isPlaceholder = this->m_query.empty();
    	Clay_String headerText = isPlaceholder ? CLAY_STRING("Search for any running application...") :StrToClayString(this->m_query.c_str(), this->m_query.size());
        CLAY_TEXT(headerText, CLAY_TEXT_CONFIG(DefaultText(72)));
        for (size_t i = 0; i < this->m_filteredProcesses.size(); i++) {
        	const WinProcess& process = this->m_activeProcesses[this->m_filteredProcesses[i]];
        	this->DrawProcessListItem(process, i);
        }
    }
}


void FuzzKillUI::ResetFilterIfNeeded() {
	if (this->m_filteredProcesses.empty()) {
		for (int32_t i = 0; i < this->m_activeProcesses.size(); i++) {
			this->m_filteredProcesses.emplace_back(i);
		}
	}
}

void FuzzKillUI::HandleKeyboardInput(float delta) {
	int32_t c = GetCharPressed();
	if (c > 0) {
		this->m_query += c;
		this->m_filteredProcesses = FuzzyFindIndices<std::vector<std::string_view>, std::string_view>(this->m_activeProcessesNames, this->m_query);
		this->selectedProcess = 0;
	}
	if (IsKeyPressed(KEY_ESCAPE)) {
		SetWindowState(FLAG_WINDOW_HIDDEN);
	}
	if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && !this->m_query.empty()) {
		this->m_query.erase(this->m_query.size() - 1);
		this->m_filteredProcesses = FuzzyFindIndices<std::vector<std::string_view>, std::string_view>(this->m_activeProcessesNames, this->m_query);
		this->selectedProcess = 0;
		this->ResetFilterIfNeeded();
	}
	if (IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)) {
		this->selectedProcess = (this->selectedProcess + 1) % this->m_filteredProcesses.size();
	}
	else if (IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)) {
		this->selectedProcess = (this->selectedProcess - 1) % this->m_filteredProcesses.size();
	}
	if (IsKeyPressed(KEY_ENTER)) {
		int32_t processIndex = this->m_filteredProcesses[this->selectedProcess];
		const WinProcess& process = this->m_activeProcesses[processIndex];
		ProcessLayer::SwitchWindow(process.windowHandle);
		// Kill raylib here
		SetWindowState(FLAG_WINDOW_HIDDEN);
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

