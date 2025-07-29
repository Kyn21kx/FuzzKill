#include "ProcessLayer.hpp"
#include <Windows.h>
#include <Psapi.h>
#include <array>
#include <basetsd.h>
#include <cstdint>
#include <handleapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <vector>
#include <winbase.h>
#include <windef.h>
#include <winnt.h>
#include <winuser.h>

std::array<DWORD, 1024> processIDs;

EError ProcessLayer::FetchProcessesInto(std::vector<WinProcess>* target) {
	DWORD processCountBytes;
	BOOL fetchSuccess = EnumProcesses(processIDs.data(), processIDs.size() * sizeof(DWORD), &processCountBytes);
	if (!fetchSuccess) {
		return EError::FailedToFetchProcesses;
	}
	DWORD processCount = processCountBytes / sizeof(DWORD);
	
	EnumWindows([](HWND hwnd, LPARAM userInfo){
	    std::vector<WinProcess>* pTarget = reinterpret_cast<std::vector<WinProcess>*>(userInfo);
		WinProcess processInfo{};
	    processInfo.windowHandle = reinterpret_cast<uint64_t>(hwnd);
        int32_t length = GetWindowText(hwnd, processInfo.name, WinProcess::MAX_PROCESS_NAME);
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        processInfo.pid = static_cast<uint64_t>(pid);
	    if (!IsWindowVisible(hwnd) || length <= 0) {
	    	return TRUE;
	    }
		pTarget->emplace_back(processInfo);
		return TRUE;
    }, reinterpret_cast<LPARAM>(target));
		
	return EError::Ok;
}


void ProcessLayer::SwitchWindow(uint64_t hwnd) {
    const HWND windowHandle = reinterpret_cast<HWND>(hwnd);
    if (!IsWindow(windowHandle)) {
        return;
    }
    
    // If the window is minimized, restore it first
    if (IsIconic(windowHandle)) {
        ShowWindow(windowHandle, SW_RESTORE);
    }
    
    ShowWindow(windowHandle, SW_SHOW);
    
    // Bring the window to the top of the Z order
    BringWindowToTop(windowHandle);
    
    SetForegroundWindow(windowHandle);
    SetFocus(windowHandle);
}

