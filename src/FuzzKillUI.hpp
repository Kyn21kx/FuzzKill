#pragma once

#include <vector>
#include "types/WinProcess.hpp"
#include <string>

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
	
	void ResetFilterIfNeeded();
	
	std::vector<WinProcess> m_activeProcesses;
	std::vector<std::string_view> m_activeProcessesNames;
	std::vector<size_t> m_filteredProcesses;
	std::string m_query;
	
	int32_t selectedProcess = 0;
	
};

