#pragma once
#include <cstdint>

struct WinProcess {
	constexpr static inline uint32_t MAX_PROCESS_NAME = 64;
	uint64_t pid;
	char name[MAX_PROCESS_NAME];
};
