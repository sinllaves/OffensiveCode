#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

void SuspendThreadsInTarget(DWORD targetPID) {
    HANDLE hSnapshot;
    THREADENTRY32 threadEntry32;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    threadEntry32.dwSize = sizeof(THREADENTRY32);
    if (Thread32First(hSnapshot, &threadEntry32)) {
        do {
            if (threadEntry32.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(threadEntry32.th32OwnerProcessID)) {
                // Suspend all threads
                if (threadEntry32.th32OwnerProcessID == targetPID) {
                    // Get the handle to the current thread and then suspend it
                    HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry32.th32ThreadID);
                    if (hThread != nullptr) {
                        SuspendThread(hThread);
                        CloseHandle(hThread);
                    }
                }
            }
            threadEntry32.dwSize = sizeof(THREADENTRY32);
        } while (Thread32Next(hSnapshot, &threadEntry32));
    }
    CloseHandle(hSnapshot);
}

DWORD FindPidByName(const wchar_t* processName) {
    PROCESSENTRY32 processEntry32;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    processEntry32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &processEntry32)) {
        do {
            if (_wcsicmp(processEntry32.szExeFile, processName) == 0) {
                CloseHandle(hSnapshot);
                return processEntry32.th32ProcessID;
            }
            processEntry32.dwSize = sizeof(PROCESSENTRY32);
        } while (Process32Next(hSnapshot, &processEntry32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    std::wstring targetProcess{ L"firefox.exe" };

    std::wcout << "[*] Finding the process ID of " << targetProcess << std::endl;
    DWORD pid = FindPidByName(targetProcess.c_str());
    if (pid == 0) {
        std::wcout << "[!] Process not found" << std::endl;
        return 1;
    }
    std::wcout << "[+] Process ID: " << pid << std::endl;

    std::wcout << "[*] Requesting a handle with all access" << std::endl;
    HANDLE hfirefox = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
    if (!hfirefox) {
        std::wcout << "[!] Could not obtain a PROCESS_ALL_ACCESS handle to " << targetProcess << std::endl;
        return 1;
    }
    std::wcout << "[+] Obtained a handle" << std::endl;

    std::wcout << "[+] Attempting to suspend all threads" << std::endl;
    SuspendThreadsInTarget(pid);

    std::
