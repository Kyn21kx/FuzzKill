#pragma once

#include <cstdint>

enum class EError : uint32_t {
	Ok = 0,
	FailedToFetchProcesses,
	FailedToOpenProcess,
	NoWindowFound,
	FailedToEnumModuleFromProcess
};
