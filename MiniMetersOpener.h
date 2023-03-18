#pragma once
#include "MacOsHelpers.h"
#include <JuceHeader.h>
#include <cstdlib>
#if defined(_WIN32)
#include <processthreadsapi.h>
#include <string>
#include <tlhelp32.h>
#include <windows.h>
#endif

class MiniMetersOpener {

public:
    enum Error {
        Success = 0,
        Failure
    };

    static bool is_minimeters_running() {
#if defined(__APPLE__)
        // Naive way of checking on macOS.
        // bool is_running = system("ps -Ac | grep 'MiniMeters' > /dev/null") == 0;
        return MacOsHelpers::is_minimeters_running();
#elif defined(_WIN32)
        // Pulled from here: https://stackoverflow.com/questions/3477097/get-full-running-process-list-visual-c
        std::string compare;
        bool minimeters_running = false;

        HANDLE process_snapshot_handle;
        PROCESSENTRY32 pe32;
        process_snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (process_snapshot_handle == INVALID_HANDLE_VALUE) {
            minimeters_running = false;
        } else {
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(process_snapshot_handle, &pe32)) { // Gets first running process
                if (std::string(pe32.szExeFile) == std::string("MiniMeters.exe")) {
                    minimeters_running = true;
                } else {
                    // loop through all running processes looking for process
                    while (Process32Next(process_snapshot_handle, &pe32)) {
                        if (std::string(pe32.szExeFile) == "MiniMeters.exe") {
                            // if found process is running, set to true and break from loop
                            minimeters_running = true;
                            break;
                        }
                    }
                }
                // clean the snapshot object
                CloseHandle(process_snapshot_handle);
            }
        }

        return minimeters_running;
#endif
        // Assume it is running otherwise.
        return true;
    }

    static bool launch_minimeters() {
#if defined(__APPLE__)
//        juce::Process::openDocument("com.josephlyncheski.minimeters", "-d \"MiniMeters Plugin\"");
        MacOsHelpers::open_minimeters();
#elif defined(_WIN32)
        // Pulled from here: https://stackoverflow.com/questions/15435994/how-do-i-open-an-exe-from-another-c-exe
        STARTUPINFOA startup_info;
        PROCESS_INFORMATION process_info;

        ZeroMemory(&startup_info, sizeof(startup_info));
        startup_info.cb = sizeof(startup_info);
        ZeroMemory(&process_info, sizeof(process_info));

        CreateProcessA(
            "C:/Program Files/MiniMeters/MiniMeters.exe",
            "-d \"MiniMeters Plugin\"",
            nullptr,
            nullptr,
            FALSE,
            CREATE_NEW_CONSOLE,
            nullptr,
            nullptr,
            &startup_info,
            &process_info);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#endif
        return false;
    }
};