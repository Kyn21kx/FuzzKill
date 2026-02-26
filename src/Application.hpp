#pragma once

#include <cstdint>

class Application {
public:
	void PreAllocate(int32_t targetMonitor = 1);

	void Init();
	
	void Run();
	
	void Dispose();

private:
	void LoadResources();
};
