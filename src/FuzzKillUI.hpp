#pragma once

#include <vector>
#include "types/WinProcess.hpp"
struct Font;

class FuzzKillUI {
public:
	FuzzKillUI();
	void OnUpdate(float delta, Font* fonts);
private:
	void DrawUI();

	void RefreshProcesses();

	std::vector<WinProcess> m_activeProcesses;
	
};

