#include <iostream>
#include "Mem/Memory.hpp"
#include "Offsets/offsets.hpp"

int main() {
    MemoryManager memory("Roblox");

    if (memory.initialize()) {
        std::cout << "Roblox Pid Found: " << memory.getProcessId() << std::endl;
        std::cout << "Handle opened successfully!" << std::endl;

        uintptr_t moduleBase = memory.getModuleBaseAddress("RobloxPlayerBeta.exe");
        if (moduleBase != 0) {
            std::cout << "Module base address: 0x" << std::hex << moduleBase << std::endl;

            uintptr_t dataModel = memory.findDataModel();
            if (dataModel != 0) {
                std::cout << "DataModel found at: 0x" << std::hex << dataModel << std::endl;

                try {
                    uintptr_t localPlayer = memory.read<uintptr_t>(dataModel + offsets::LocalPlayer);
                    if (localPlayer != 0) {
                        std::cout << "LocalPlayer found at: 0x" << std::hex << localPlayer << std::endl;

                        uintptr_t namePtr = memory.read<uintptr_t>(localPlayer + offsets::Name);
                        if (namePtr != 0) {
                            uint32_t nameLength = memory.read<uint32_t>(namePtr + offsets::StringLength);
                            if (nameLength > 0 && nameLength < 1000) {
                                std::string playerName(nameLength, '\0');
                                // Read the string data
                                for (uint32_t i = 0; i < nameLength; i++) {
                                    playerName[i] = memory.read<char>(namePtr + offsets::StringLength + 4 + i);
                                }
                                std::cout << "Player name: " << playerName << std::endl;
                            }
                        }
                    }

                    uintptr_t workspace = memory.read<uintptr_t>(dataModel + offsets::Workspace);
                    if (workspace != 0) {
                        std::cout << "Workspace found at: 0x" << std::hex << workspace << std::endl;
                    }

                }
                catch (...) {
                    std::cout << "Error reading memory values" << std::endl;
                }
            }
            else {
                std::cout << "DataModel not found!" << std::endl;
            }
        }
        else {
            std::cout << "Module not found!" << std::endl;
        }
    }
    else {
        std::cout << "Roblox not found!" << std::endl;
    }

    return 0;
}