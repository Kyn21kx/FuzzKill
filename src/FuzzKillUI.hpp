#pragma once

#include <string_view>
#include <vector>
#include "types/WinProcess.hpp"
#include "utils/ConfigLayer.hpp"
#include <string>

struct Font;

enum class EState : uint8_t {
	Background = 0,
	ProcessMode,
	CommandMode
};

class FuzzKillUI {
public:
	FuzzKillUI() = default;

	void PreAllocate();

	void Init();
	
	void OnUpdate(float delta, Font* fonts);
private:
	static constexpr size_t MAX_OPERATION_RESULT_COUNT = 10;
	void DrawUI();

	void RefreshProcesses();

	void DrawProcessListItem(const WinProcess& processInfo, int32_t index);

	void DrawListContainer(const std::string_view& text);

	void DrawCommands();
	
	void HandleKeyboardInput(float delta);
	
	void ResetFilterIfNeeded();

	void OnTextSubmit();

	size_t ActiveListMaxSize();

	std::string_view GetContentUnderSelection();
	
	std::vector<WinProcess> m_activeProcesses;
	std::vector<std::string_view> m_activeProcessesNames;
	std::vector<size_t> m_filteredProcesses;
	std::string m_query;
	char m_operationResultStr[MAX_OPERATION_RESULT_COUNT] = "";
	int32_t selectedProcess = 0;
	ConfigData m_config;
	EState m_state = EState::Background;
	
};

