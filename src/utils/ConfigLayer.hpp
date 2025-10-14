#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string_view>
#include "../types/Error.hpp"


enum class EConfigFlags : uint8_t{
	None = 0
};

enum class EConfigStages : uint8_t {
	Flags = 0,
	BGColor,
	FGColor,
	BorderColor,
	HighlightColor,
	ItemColor
};

struct ConfigData {
	EConfigFlags flags;
	uint32_t backgroundColor;
	uint32_t foregroundColor;
	uint32_t borderColor;
	uint32_t highlightColor;
	uint32_t itemColor;
};

constexpr size_t MAX_LINE_LENGTH = 13;

ConfigData ReadConfigFile(const std::string_view& file, EError* outErr);
