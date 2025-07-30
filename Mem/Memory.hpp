#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <TlHelp32.h>
#include <vector>

class MemoryManager {
private:
    HANDLE handle;
    DWORD processId;
    std::string windowName;

public:
    MemoryManager(const std::string& windowTitle = "Roblox");
    ~MemoryManager();

    bool initialize();
    DWORD getProcessId() const;
    bool isProcessFound() const;
    bool openHandle();
    uintptr_t getModuleBaseAddress(const char* moduleName);
    uintptr_t findDataModel();
    uintptr_t followPointerChain(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);

    template<typename T>
    inline T read(uintptr_t address)
    {
        if (!handle)
        {
            return T{};
        }
        T buffer;
        ReadProcessMemory(handle, (LPCVOID)address, &buffer, sizeof(buffer), nullptr);
        return buffer;
    }

    template<typename T>
    inline void write(uintptr_t address, T value)
    {
        if (!handle)
        {
            return;
        }
        WriteProcessMemory(handle, (LPVOID)address, &value, sizeof(value), nullptr);
    }

    HANDLE getHandle() const { return handle; }
};