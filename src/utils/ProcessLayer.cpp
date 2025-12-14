#include "ProcessLayer.hpp"
#include "ImageUtils.hpp"
#include <Windows.h>
#include <Psapi.h>
#include <array>
#include <basetsd.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <handleapi.h>
#include <libloaderapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <string.h>
#include <vector>
#include <winbase.h>
#include <windef.h>
#include <wingdi.h>
#include <winnt.h>
#include <winuser.h>

#pragma comment(lib, "coredll.lib")

std::array<DWORD, 1024> processIDs;


void GetFileStem(const char *path, char *output_buffer, size_t buffer_size, size_t* outLength) {
	assert(outLength != nullptr);
    const char *filename_start = path;
    const char *last_slash = strrchr(path, '/');
    const char *last_backslash = strrchr(path, '\\');
    const char *last_separator;
    const char *last_dot;
    size_t length;

    if (last_slash == nullptr) {
        last_separator = last_backslash;
    }
    else if (last_backslash == nullptr) {
        last_separator = last_slash;
    }
    else {
        /* Use the separator that appears later in the string */
        last_separator = (last_slash > last_backslash) ? last_slash : last_backslash;
    }

    if (last_separator != nullptr) {
        filename_start = last_separator + 1;
    }

    last_dot = strrchr(filename_start, '.');

    if (last_dot != nullptr) {
        length = (size_t)(last_dot - filename_start);
    }
    else {
        length = strlen(filename_start);
    }

    /* Copy the stem into the output buffer, ensuring no overflow */
    if (length >= buffer_size) {
        length = buffer_size - 1; /* Truncate if too long, leaving space for null terminator */
    }

    strncpy(output_buffer, filename_start, length);
    output_buffer[length] = '\0'; /* Null-terminate the string */
    *outLength = length;
}

uint64_t GetFileIconGPUHandle(const char* file) {
	uint64_t gpuHandle;
	SHFILEINFO fileInfo{};
	DWORD_PTR result = SHGetFileInfo(
        file,
        0,
        &fileInfo,
        sizeof(SHFILEINFO),
        SHGFI_ICON | SHGFI_SMALLICON
    );
	HICON icon = fileInfo.hIcon;
	ICONINFO iconInfo{};
	BITMAP bmp{};
	BITMAPINFO bmpInfo = {0};
	constexpr size_t MAX_ICON_SIZE = 16 * 16 * 4;
	std::array<std::byte, MAX_ICON_SIZE> imageBuffer;
	HDC hdc = CreateCompatibleDC(nullptr);
    BITMAPINFOHEADER biInfoHeader;

	if (!GetIconInfo(icon, &iconInfo)) {
		//GetLastError() if this becomes too unmanageable
		goto cleanup;
	}
	
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);

    biInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    biInfoHeader.biWidth = bmp.bmWidth;
    biInfoHeader.biHeight = -bmp.bmHeight; // Negative height for top-down DIB
    biInfoHeader.biPlanes = 1;
    biInfoHeader.biBitCount = 32; // Request 32-bit (RGBA) format
    biInfoHeader.biCompression = BI_RGB;	
	bmpInfo.bmiHeader = biInfoHeader;
	GetDIBits(hdc, (HBITMAP)&bmp, 0, bmp.bmHeight, imageBuffer.data(), &bmpInfo, DIB_RGB_COLORS);
	
	gpuHandle = ImageUtils::LoadImageToGPU(imageBuffer.data(), imageBuffer.size(), bmp.bmWidth, bmp.bmHeight);
	
	cleanup: {
		DestroyIcon(fileInfo.hIcon);
		ReleaseDC(nullptr, hdc);
	}
	return gpuHandle;
}

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

        DWORD pid;
        DWORD getFileNameRes;
        int32_t windowTextLength;
        size_t outLength;
        char processTextBuffer[WinProcess::MAX_PROCESS_NAME];
        constexpr size_t separatorLength = 4;
        
        GetWindowThreadProcessId(hwnd, &pid);
        processInfo.pid = static_cast<uint64_t>(pid);
        HANDLE pHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        
        if (pHandle == nullptr) {
        	goto cleanup;
        }
        
        getFileNameRes = GetModuleFileNameEx(pHandle, 0, processTextBuffer, WinProcess::MAX_PROCESS_NAME);
        GetFileStem(processTextBuffer, processInfo.name, WinProcess::MAX_PROCESS_NAME, &outLength);
        
        strcat_s(processInfo.name, " - ");
        
		windowTextLength = GetWindowText(hwnd, processTextBuffer, WinProcess::MAX_PROCESS_NAME - outLength - separatorLength);
        strcat_s(processInfo.name, processTextBuffer);
		
	    if (!IsWindowVisible(hwnd) || getFileNameRes == 0 || windowTextLength < 1) {
	    	goto cleanup;
	    }

		pTarget->emplace_back(processInfo);
		cleanup: {
			CloseHandle(pHandle);
			return TRUE;
		}
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

