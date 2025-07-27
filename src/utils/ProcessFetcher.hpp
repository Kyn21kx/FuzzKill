#pragma once
#include "../types/WinProcess.hpp"
#include "../types/Error.hpp"
#include <vector>

namespace ProcessFetcher {
	EError FetchProcessesInto(std::vector<WinProcess>* target);
}

