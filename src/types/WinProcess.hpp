#pragma once
#include <cstdint>

struct WinProcess {
	constexpr static inline uint32_t MAX_PROCESS_NAME = 64;
	uint64_t pid; // Non standard, this is used bc of the typedef on the Windows API
	uint64_t windowHandle;
	char name[MAX_PROCESS_NAME];
};
