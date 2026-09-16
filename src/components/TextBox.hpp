#pragma once

#include "clay.h"
#include "../utils/ColorUtils.hpp"
#include "raylib.h"
#include <cmath>
#include <cstdint>
#include <string_view>
#include <span>

enum class ETextBoxState : uint32_t {
	None = 0,
	IsPlaceholder = 1,
	IsFocused = 2,
	WasClicked = 4
};

struct TextBoxInfo {
	std::string_view placeholderText{};
	Clay_Color placeholderColor = ColorUtils::White(100);
	Clay_Color cursorColor = ColorUtils::White();
	Clay_Sizing sizing;
	std::span<char> textBuffer;
	size_t textSize = 0;
	ETextBoxState outStateFlags;
	bool indentOnNewLine;

	TextBoxInfo(std::span<char> buffer) {
		this->textBuffer = buffer;
		this->textSize = strlen(this->textBuffer.data());
	}
};



void DrawCursorTextBox(Font* font, std::string_view text, Clay_ElementData elementData, float fontSize, Clay_Color color) {
	if (!elementData.found || std::fmod(GetTime(), 1.0) < 0.5) {
		return;
	}
	auto textScale = MeasureTextEx(*font, text.data(), fontSize, 0);
	float factor = text.size() > 0 ? 1.0 : 0.0;
	DrawRectangle(
		(int32_t)(elementData.boundingBox.x + textScale.x),
		(int32_t)(elementData.boundingBox.y + textScale.y - ((float)fontSize * factor)),
		20,
		(int32_t)fontSize,
		Color {
			uint8_t(color.r),
			uint8_t(color.g),
			uint8_t(color.b),
			uint8_t(color.a),
		}
	);
}
