#pragma once
#include "../types/WinProcess.hpp"
#include "../types/Error.hpp"
#include <vector>

namespace ProcessLayer {
	EError FetchProcessesInto(std::vector<WinProcess>* target);

	void SwitchWindow(uint64_t hwnd);
}

