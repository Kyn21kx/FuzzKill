#pragma once

#include <vector>
#include "types/WinProcess.hpp"
struct Font;

class FuzzKillUI {
public:
	FuzzKillUI() = default;

	void Init();
	
	void OnUpdate(float delta, Font* fonts);
private:
	void DrawUI();

	void RefreshProcesses();

	void DrawProcessListItem(const WinProcess& processInfo, int32_t index);
	
	void HandleKeyboardInput(float delta);
	
	std::vector<WinProcess> m_activeProcesses;
	
	int32_t selectedProcess = 0;
	
};

