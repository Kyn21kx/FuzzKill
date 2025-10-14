

#pragma once
#include "clay.h"
#include "raylib.h"
#include <cstdint>

namespace ColorUtils {

	constexpr Clay_Color Transparent() {
		return { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	
	constexpr Clay_Color White(float opacity = 255.0f) {
		return { 255, 255, 255, opacity };
	}
	
	constexpr Clay_Color LightGray(float opacity = 255.0f) {
		return { 194, 194, 194, opacity };
	}
	
	constexpr Clay_Color Black(float opacity = 255.0f) {
		return { 0, 0, 0, opacity };
	}
	
	constexpr Clay_Color Red(float opacity = 255.0f) {
		return { 255, 0, 0, opacity };
	}
	
	// Professional tech color palette
	constexpr Clay_Color DeepBlue(float opacity = 255.0f) {
		return { 17, 45, 78, opacity };
	}
	
	constexpr Clay_Color Purple(float opacity = 255.0f) {
		return { 39, 27, 75, opacity };
	}
	
	constexpr Clay_Color MidBlue(float opacity = 255.0f) {
		return { 43, 84, 134, opacity };
	}
	
	constexpr Clay_Color LightBlue(float opacity = 255.0f) {
		return { 88, 160, 231, opacity };
	}
	
	constexpr Clay_Color AccentTeal(float opacity = 255.0f) {
		return { 0, 176, 182, opacity };
	}
	
	constexpr Clay_Color AccentGreen(float opacity = 255.0f) {
		return { 35, 203, 167, opacity };
	}
	
	constexpr Clay_Color Green(float opacity = 255.0f) {
		return { 54, 94, 58, opacity };
	}
	
	constexpr Clay_Color DarkCharcoal(float opacity = 255.0f) {
		return { 27, 38, 44, opacity };
	}
	
	constexpr Clay_Color CodeGray(float opacity = 255.0f) {
		return { 45, 55, 72, opacity };
	}
	
	constexpr Clay_Color SubtleGray(float opacity = 255.0f) {
		return { 226, 232, 240, opacity };
	}

	constexpr Color ToRaylibColor(const Clay_Color& color) {
		return {
			.a = static_cast<unsigned char>(color.a),
			.b = static_cast<unsigned char>(color.b),
			.g = static_cast<unsigned char>(color.g),
			.r = static_cast<unsigned char>(color.r)
		};
	}

	constexpr inline Color ToRaylibColor(const uint32_t& color) {
		constexpr uint32_t RED_BITS = 0xFF000000;
		constexpr uint32_t RED_OFFSET = 24;
		constexpr uint32_t GREEN_BITS = 0x00FF0000;
		constexpr uint32_t GREEN_OFFSET = 16;
		constexpr uint32_t BLUE_BITS = 0x0000FF00;
		constexpr uint32_t BLUE_OFFSET = 8;
		constexpr uint32_t ALPHA_BITS = 0x000000FF;
		constexpr uint32_t ALPHA_OFFSET = 0;
		return {
			.a = static_cast<uint8_t>((color & ALPHA_BITS) >> ALPHA_OFFSET),
			.b = static_cast<uint8_t>((color & BLUE_BITS) >> BLUE_OFFSET),
			.g = static_cast<uint8_t>((color & GREEN_BITS) >> GREEN_OFFSET),
			.r = static_cast<uint8_t>((color & RED_BITS) >> RED_OFFSET)
		};
	}

	constexpr inline Clay_Color ToClayColor(const uint32_t& color) {
		// A bit awful, but meh
		Color raylibColor = ToRaylibColor(color);
		return {
			.a = static_cast<float>(raylibColor.a),
			.b = static_cast<float>(raylibColor.b),
			.g = static_cast<float>(raylibColor.g),
			.r = static_cast<float>(raylibColor.r),
		};
	}
	
	// // Semantic UI colors
	// constexpr Clay_Color Primary() {
	// 	return DeepBlue();
	// }
	
	// constexpr Clay_Color Secondary() {
	// 	return MidBlue();
	// }
	
	// constexpr Clay_Color Accent() {
	// 	return AccentTeal();
	// }
	
	// constexpr Clay_Color Background() {
	// 	return DarkCharcoal();
	// }
	
	//  constexpr Clay_Color Surface() {
	// 	return CodeGray();
	// }
	
	// constexpr Clay_Color TextPrimary() {
	// 	return White();
	// }
	
	// constexpr Clay_Color TextSecondary() {
	// 	return SubtleGray();
	// }
}
