#include "Memory.hpp"
#include <cwchar>
#include <vector>

MemoryManager::MemoryManager(const std::string& windowTitle)
    : handle(nullptr), processId(0), windowName(windowTitle) {
}

MemoryManager::~MemoryManager() {
    if (handle) {
        CloseHandle(handle);
    }
}

bool MemoryManager::initialize() {
    return openHandle();
}

DWORD MemoryManager::getProcessId() const {
    return processId;
}

bool MemoryManager::isProcessFound() const {
    return processId != 0;
}

bool MemoryManager::openHandle() {
    HWND hwnd = FindWindowA(nullptr, windowName.c_str());
    if (hwnd != nullptr) {
        GetWindowThreadProcessId(hwnd, &processId);
        if (processId != 0) {
            handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
            return handle != nullptr;
        }
    }
    return false;
}

uintptr_t MemoryManager::getModuleBaseAddress(const char* moduleName)
{
    if (!handle) return 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);
    MODULEENTRY32 currentModule{};
    currentModule.dwSize = sizeof(currentModule);
    if (Module32First(hSnap, &currentModule))
    {
        do
        {
            // Direct string comparison since both are narrow strings
            if (strcmp(currentModule.szModule, moduleName) == 0)
            {
                CloseHandle(hSnap);
                return (uintptr_t)currentModule.modBaseAddr;
            }
        } while (Module32Next(hSnap, &currentModule));
    }
    CloseHandle(hSnap);
    return 0;
}

uintptr_t MemoryManager::findDataModel() {
    if (!handle) return 0;

    uintptr_t dataModelPtr = this->read<uintptr_t>(0x68D7310);
    if (dataModelPtr == 0) return 0;

    return dataModelPtr;
}

uintptr_t MemoryManager::followPointerChain(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
    if (!handle) return 0;

    uintptr_t currentAddress = baseAddress;

    for (size_t i = 0; i < offsets.size(); i++) {
        if (currentAddress == 0) return 0;

        currentAddress = this->read<uintptr_t>(currentAddress + offsets[i]);
    }

    return currentAddress;
}
