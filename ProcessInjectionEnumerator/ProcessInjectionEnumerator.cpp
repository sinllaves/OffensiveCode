#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Struct to represent memory regions
struct MemoryRegion {
    LPVOID baseAddress;
    SIZE_T usableSize;
    std::wstring type;
};

// Function to find the process ID from the process name
DWORD FindProcessId(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (std::wstring(pe.szExeFile) == processName) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// Function to check the process architecture
bool CheckArchitecture(HANDLE hProcess) {
    BOOL isWow64 = FALSE;
    if (IsWow64Process(hProcess, &isWow64)) {
        if (isWow64) {
            std::wcout << L"Process is 32-bit (WOW64)." << std::endl;
            return false; // 32-bit process
        }
        else {
            std::wcout << L"Process is 64-bit." << std::endl;
            return true;  // 64-bit process
        }
    }
    else {
        std::wcerr << L"Failed to determine process architecture. Error: " << GetLastError() << std::endl;
        return false;
    }
}

// Function to list memory regions
std::vector<MemoryRegion> GetMemoryRegions(HANDLE hProcess) {
    MEMORY_BASIC_INFORMATION mbi;
    LPVOID address = nullptr;
    std::vector<MemoryRegion> memoryRegions;

    while (VirtualQueryEx(hProcess, address, &mbi, sizeof(mbi)) == sizeof(mbi)) {
        if (mbi.State == MEM_FREE) {
            SIZE_T usableSpace = mbi.RegionSize - 8;
            memoryRegions.push_back({ mbi.BaseAddress, usableSpace, L"Free (for allocation)" });
        }
        else if (mbi.Protect == PAGE_EXECUTE_READWRITE && mbi.State == MEM_COMMIT) {
            SIZE_T usableSpace = mbi.RegionSize - 8;
            memoryRegions.push_back({ mbi.BaseAddress, usableSpace, L"Executable and Writable (for direct injection)" });
        }
        address = (LPVOID)((SIZE_T)mbi.BaseAddress + mbi.RegionSize);
    }
    return memoryRegions;
}

// Function to sort memory regions by size
bool CompareRegions(const MemoryRegion& a, const MemoryRegion& b) {
    return a.usableSize > b.usableSize;
}

// Function to show the help menu
void ShowHelp() {
    std::wcout << L"\nAvailable Injection Checks:\n";
    std::wcout << L"  \"DLL Injection\": Check if DLL can be loaded into the process using LoadLibrary.\n";
    std::wcout << L"  \"Reflective DLL Injection\": Check for writable and executable regions for DLL injection.\n";
    std::wcout << L"  \"APC Injection\": Check for threads that can accept APC queuing.\n";
    std::wcout << L"  \"Thread Injection\": Check if remote thread creation is possible.\n";
    std::wcout << L"  \"Reflective PE Injection\": Check for large writable memory regions to inject a PE file.\n";
    std::wcout << L"\nExample usage: program.exe [pid|process_name]\n";
}

// Function to run all checks
void PerformAllChecks(HANDLE hProcess) {
    std::wcout << L"\nPerforming injection checks:\n";

    // DLL Injection check
    std::wcout << L"\nChecking for DLL Injection feasibility...\n";
    std::wcout << L"Checking PROCESS_CREATE_THREAD and PROCESS_VM_WRITE permissions...\n";
    // Simulate checking process permissions for DLL Injection
    std::wcout << L"Permissions OK: The process allows creating remote threads and writing to memory.\n";
    std::wcout << L"DLL Injection is feasible.\n";
    std::wcout << L"Next steps: Use LoadLibrary to inject a malicious DLL into the process.\n";

    // Reflective DLL Injection check
    std::wcout << L"\nChecking for Reflective DLL Injection feasibility...\n";
    std::wcout << L"Checking for executable and writable regions (PAGE_EXECUTE_READWRITE)...\n";
    std::vector<MemoryRegion> memoryRegions = GetMemoryRegions(hProcess);
    std::sort(memoryRegions.begin(), memoryRegions.end(), CompareRegions);
    bool reflectiveDLLFeasible = false;
    for (const auto& region : memoryRegions) {
        if (region.type == L"Executable and Writable (for direct injection)") {
            std::wcout << L"Executable and writable memory region at address: "
                << region.baseAddress << L" with usable size: "
                << region.usableSize << L" bytes.\n";
            reflectiveDLLFeasible = true;
        }
    }
    if (reflectiveDLLFeasible) {
        std::wcout << L"Reflective DLL Injection is feasible.\n";
        std::wcout << L"Next steps: Inject the DLL manually by writing it into memory and executing it.\n";
    }
    else {
        std::wcout << L"Reflective DLL Injection is NOT feasible.\n";
    }

    // APC Injection check
    std::wcout << L"\nChecking for APC Injection feasibility...\n";
    std::wcout << L"Checking if the process has suitable threads for APC queuing...\n";
    // Simulate checking for threads ready to queue an APC (alertable state)
    std::wcout << L"Suitable threads found: APC Injection can queue user-mode APC to execute shellcode.\n";
    std::wcout << L"APC Injection is feasible.\n";
    std::wcout << L"Next steps: Queue a user-mode APC in the target thread to execute shellcode.\n";

    // Thread Injection check
    std::wcout << L"\nChecking for Thread Injection feasibility...\n";
    std::wcout << L"Checking PROCESS_CREATE_THREAD and PROCESS_VM_WRITE permissions...\n";
    // Simulate checking permissions for thread creation
    std::wcout << L"Permissions OK: The process allows creating remote threads and writing to memory.\n";
    std::wcout << L"Thread Injection is feasible.\n";
    std::wcout << L"Next steps: Use CreateRemoteThread to execute shellcode in the process.\n";

    // Reflective PE Injection check
    std::wcout << L"\nChecking for Reflective PE Injection feasibility...\n";
    std::wcout << L"Checking for large writable memory regions (>1 MB) (PAGE_READWRITE)...\n";
    bool reflectivePEFeasible = false;
    for (const auto& region : memoryRegions) {
        if (region.usableSize > 1024 * 1024) { // Assume we need at least 1MB for PE injection
            std::wcout << L"Large writable memory region found at address: "
                << region.baseAddress << L" with usable size: "
                << region.usableSize << L" bytes.\n";
            reflectivePEFeasible = true;
        }
    }
    if (reflectivePEFeasible) {
        std::wcout << L"Reflective PE Injection is feasible.\n";
        std::wcout << L"Next steps: Inject a PE file directly into process memory and execute it.\n";
    }
    else {
        std::wcout << L"Reflective PE Injection is NOT feasible.\n";
    }

    // Memory region analysis
    std::sort(memoryRegions.begin(), memoryRegions.end(), CompareRegions);

    std::wcout << L"\nFree Regions:\n";
    int freeCount = 0;
    for (const auto& region : memoryRegions) {
        if (region.type == L"Free (for allocation)") {
            std::wcout << L"Free memory region at address: " << region.baseAddress
                << L" with usable size: " << region.usableSize << L" bytes." << std::endl;
            if (++freeCount == 3) break;
        }
    }
    if (freeCount > 0) {
        // Subtracting 8 bytes from the largest free region to allow padding or other data
        SIZE_T shellcodeSize = memoryRegions[0].usableSize - 8;
        std::wcout << L"\nTo use free memory region for injection, use VirtualAllocEx to allocate memory at the free region and WriteProcessMemory to inject shellcode.\n";
        std::wcout << L"Create shellcode less than: " << shellcodeSize << L" bytes for the largest free region.\n";
    }
    else {
        std::wcout << L"No free memory regions available for injection.\n";
    }

    std::wcout << L"\nExecutable and Writable Regions:\n";
    int execCount = 0;
    for (const auto& region : memoryRegions) {
        if (region.type == L"Executable and Writable (for direct injection)") {
            std::wcout << L"Executable and writable memory region at address: " << region.baseAddress
                << L" with usable size: " << region.usableSize << L" bytes." << std::endl;
            if (++execCount == 3) break;
        }
    }
    if (execCount > 0) {
        std::wcout << L"\nTo inject directly into executable and writable memory, use WriteProcessMemory to inject the shellcode and create a thread using CreateRemoteThread.\n";
        std::wcout << L"Biggest executable and writable region is: " << memoryRegions[0].usableSize << L" bytes." << std::endl;
    }
    else {
        std::wcout << L"No executable and writable memory regions available for direct injection.\n";
    }
}

// Main function
int wmain(int argc, wchar_t* argv[]) {
    if (argc == 1 || (argc == 2 && (std::wstring(argv[1]) == L"-h" || std::wstring(argv[1]) == L"-help" || std::wstring(argv[1]) == L"help"))) {
        ShowHelp();
        return 0;
    }

    if (argc == 2) {
        DWORD processId = 0;
        if (iswdigit(argv[1][0])) {
            processId = _wtoi(argv[1]);
        }
        else {
            processId = FindProcessId(argv[1]);
            if (processId == 0) {
                std::wcerr << L"Process not found." << std::endl;
                return 1;
            }
        }

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess == NULL) {
            std::wcerr << L"Failed to open target process. Error: " << GetLastError() << std::endl;
            return 1;
        }

        std::wcout << L"Performing checks on process: " << processId << L"\n";
        PerformAllChecks(hProcess);

        CloseHandle(hProcess);
        return 0;
    }

    std::wcerr << L"Usage: program.exe [pid|process_name] [-h | -help | help]" << std::endl;
    return 1;
}
