#pragma once
#include <cstddef>
#include <cstdint>

namespace ImageUtils {
	uint64_t LoadImageToGPU(std::byte* data, size_t size, int32_t width, int32_t height);
}
