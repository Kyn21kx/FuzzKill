#pragma once

#include "Result.hpp"
#include <string_view>


enum class EFileFlags : uint8_t {
    None = 0,
    MemoryMapped = 1
};

constexpr size_t MAX_FILE_NAME_LENGTH = 128;

struct File {
    uintptr_t handle = 0;
    uintptr_t mapHandle = 0; // This is only set on Windows
    size_t size = 0;
    char name[MAX_FILE_NAME_LENGTH] = {0};
    size_t nameLength;
    EFileFlags flags = EFileFlags::None;
};

namespace FileLayer {
    enum class EError {
        Ok = 0,
        FileNotFound,
        CantOpen,
        FailedToMapFile,
        NotYetImplemented
    };

    Result<File, EError> OpenFile(std::string_view path, EFileFlags flags = EFileFlags::None);

    Result<uint8_t*, EError> ReadIntoBuffer(uint8_t* buffer, size_t size, File& file);

    void CloseFile(File* file);
    
};

