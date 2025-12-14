#include "ImageUtils.hpp"
#include "raylib.h"

uint64_t ImageUtils::LoadImageToGPU(std::byte* data, size_t size, int32_t width, int32_t height) {
	if (data == nullptr || size < 1) {
		return 0;
	}
	Image image = {
		.data = reinterpret_cast<void*>(data),
		.width = width,
		.height = height,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1
	};
	Texture2D tex = LoadTextureFromImage(image);
	return tex.id;
}

