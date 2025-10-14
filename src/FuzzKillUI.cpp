#include "FuzzKillUI.hpp"
#include "clay.h"
#include "raylib.h"
#include "renderer/clay_renderer_raylib.h"
#include "types/Error.hpp"
#include "types/WinProcess.hpp"
#include "utils/ArrayUtils.h"
#include "utils/ColorUtils.hpp"
#include "utils/ConfigLayer.hpp"
#include "utils/MathUtils.hpp"
#include "utils/ProcessLayer.hpp"
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stack>
#include <string>
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
	EError err;
	this->m_config = ReadConfigFile("fuzzkill.config", &err);
}

void FuzzKillUI::Init() {
	this->m_query = "";
	this->selectedProcess = 0;
	this->m_filteredProcesses.clear();
	this->m_activeProcesses.clear();
	this->m_activeProcessesNames.clear();
	std::memset(this->m_operationResultStr, 0, MAX_OPERATION_RESULT_COUNT);
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
    // This is absolutely monstrous, but if it's here it is bc it worked
    auto bgColor = ColorUtils::ToRaylibColor(this->m_config.backgroundColor);
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
        if (this->m_query.empty() && strlen(this->m_operationResultStr) > 0) {
        	headerText = StrToClayString(this->m_operationResultStr, strlen(this->m_operationResultStr));
        }
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
		// Reset the operation result string here
		std::memset(this->m_operationResultStr, 0, MAX_OPERATION_RESULT_COUNT);
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
		this->OnTextSubmit();
	}
}

EError ParseOperation(const std::string& expression, float* outResult) {
	// Let's check what type of operation this is
	std::stack<MathUtils::OperationType> operators;
	std::stack<float> numbers;

	std::stringstream stream(expression);
	constexpr size_t tokenMaxSize = 8;
	constexpr char tokenSeparator = ' ';

	std::string token;
	token.reserve(tokenMaxSize);

	// We parse the string token by token (space delimitted)
	while (std::getline(stream, token, tokenSeparator)) {
		if (token.empty()) continue;
		// For most ops we can check only the token's first character
		if (std::isdigit(token[0])) {
			float val = std::strtof(token.c_str(), nullptr);
			numbers.push(val);
			continue;
		}
		if (MathUtils::IsOperator(token[0])) {
			while (!operators.empty()) {
				// Pop two numbers and one operator
                float b = numbers.top();
                numbers.pop();
                float a = numbers.top();
                numbers.pop();
                MathUtils::OperationType op = operators.top();
                operators.pop();
                // Apply the operator to the numbers and
                // push the result onto the operand stack
                float opResult = MathUtils::ApplyOperation(a, b, op);
                numbers.push(opResult);
			}
			operators.push(static_cast<MathUtils::OperationType>(token[0]));
			continue;
		}
		if (token[0] == '(') {
			// This enum entry does not really exist, but we'll just trust it
			operators.push(static_cast<MathUtils::OperationType>('('));
		}
		else if (token[0] == ')') {
			while (!operators.empty() && (char)operators.top() != '(') {
				// Pop two numbers and one operator
                float b = numbers.top();
                numbers.pop();
                float a = numbers.top();
                numbers.pop();
                MathUtils::OperationType op = operators.top();
                operators.pop();
                // Apply the operator to the numbers and
                // push the result onto the operand stack
                float result = MathUtils::ApplyOperation(a, b, op);
                numbers.push(result);
				
			}
			// Pop the parenthesis
			operators.pop();
		}
	}

	if (operators.empty() || numbers.size() < 2) {
		// Not enough operators or operands to execute an action
		return EError::ParseFailed;
	}
	
    // While the operator stack is not empty
    while (!operators.empty()) {
        // Pop two numbers and one operator
        float b = numbers.top();
        numbers.pop();
        float a = numbers.top();
        numbers.pop();
        MathUtils::OperationType op = operators.top();
        operators.pop();
        // Apply the operator to the numbers and push the
        // result onto the operand stack
        float result = MathUtils::ApplyOperation(a, b, op);
        numbers.push(result);
    }

    *outResult = numbers.top();

	return EError::Ok;
}

void FuzzKillUI::OnTextSubmit() {
	// Check first if the string is a math expression
	float operationResult;
	EError operationErr = ParseOperation(this->m_query, &operationResult);
	if (operationErr == EError::Ok) { // It WAS a math expression, let's update the UI accordingly
		this->m_query = "";
		std::snprintf(this->m_operationResultStr, MAX_OPERATION_RESULT_COUNT, "%f", operationResult);
		return;
	}

	// TODO: Maybe do a shake or something
	if(this->selectedProcess >= this->m_filteredProcesses.size()) return;
	int32_t processIndex = this->m_filteredProcesses[this->selectedProcess];
	const WinProcess& process = this->m_activeProcesses[processIndex];
	ProcessLayer::SwitchWindow(process.windowHandle);
	// Kill raylib here
	SetWindowState(FLAG_WINDOW_HIDDEN);
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


void FuzzKillUI::DrawListContainer(const std::string_view& text) {
	const Clay_LayoutConfig layoutConfig = {
		.sizing = SIZE_AUTO_GROW_XY
	};
	const Clay_BorderElementConfig borderConfig = {
		.color = ColorUtils::Red()
	};
	constexpr Clay_Color inactiveColor = ColorUtils::White(BG_COLOR_ALPHA * 2);
	constexpr Clay_Color activeColor = ColorUtils::Red(BG_COLOR_ALPHA * 2);

	Clay_String nameStr = StrToClayString(text.data(), text.size());
	// TODO: Fix the id here
	CLAY({ .id = CLAY_IDI("ListContainer", 0), .layout = layoutConfig, .backgroundColor = inactiveColor, .border = borderConfig}) {
	    CLAY_TEXT(nameStr, CLAY_TEXT_CONFIG(DefaultText(32)));
	}
}
