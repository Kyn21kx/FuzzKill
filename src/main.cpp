#include "Application.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

Application g_app;

int FuzzKillEntryPoint(void) {
	g_app.Init();
	g_app.Run();
	return 0;
}

#include <Windows.h>

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnHotkeyPressed();

// Constants
const char g_szClassName[] = "HotkeyWindowClass";
const UINT WM_HOTKEY_MSG = WM_USER + 1;
const int HOTKEY_ID = 1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Find the monitor argument if any
    std::string cmdArgs = lpCmdLine;
    constexpr std::string_view MONITOR_ARG_STR = "--monitor";
    size_t idx = cmdArgs.find(MONITOR_ARG_STR.data());

    int32_t targetMonitor = 1;
    
    if (idx != std::string::npos) {
        size_t monitorIdx = idx + MONITOR_ARG_STR.size(); // Account for space by not doing -1
        std::string monitorStr = cmdArgs.substr(monitorIdx);
        targetMonitor = std::atoi(monitorStr.c_str());
    }

	g_app.PreAllocate(targetMonitor);
    // Register window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 
        0, 0, hInstance, NULL, NULL, NULL, NULL, g_szClassName, NULL };
    RegisterClassEx(&wc);

    // Create hidden window
    HWND hWnd = CreateWindowEx(0, g_szClassName, "Hotkey Listener", 0, 
        0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    
    RegisterHotKey(hWnd, HOTKEY_ID, MOD_ALT | MOD_SHIFT, VK_DELETE);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(hWnd, HOTKEY_ID);
	g_app.Dispose();
    return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_HOTKEY:
        if (wParam == HOTKEY_ID)
        {
            OnHotkeyPressed();
        }
        return 0;
    
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void OnHotkeyPressed()
{    
    FuzzKillEntryPoint();
}
