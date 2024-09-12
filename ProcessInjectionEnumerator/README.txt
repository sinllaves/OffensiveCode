### README: **Process Injection Checker**

This is a **Process Injection Feasibility Checker** tool for Windows systems. The tool performs a series of checks on a running process to determine if it's feasible to inject code via different methods such as **DLL Injection**, **Reflective DLL Injection**, **APC Injection**, **Thread Injection**, and **Reflective PE Injection**.
Compile in x86 for 32 bit and x64 for 64 bit. 

Run the process

run tasklist 
tasklist -v
tasklist /m /fi "imagename eq notepad.exe"
Look for the presence of SysWOW64 modules in the output. 
If you see modules from C:\Windows\SysWOW64\, the process is running as a 32-bit process.

---

### Features:
- **DLL Injection Check**: Checks if it's feasible to inject a DLL using the `LoadLibrary()` method.
- **Reflective DLL Injection Check**: Verifies if there are writable and executable memory regions for Reflective DLL injection.
- **APC Injection Check**: Determines if any threads are available to accept Asynchronous Procedure Call (APC) for APC-based injection.
- **Thread Injection Check**: Checks if the process allows the creation of a remote thread for injecting code.
- **Reflective PE Injection Check**: Finds large executable and writable memory regions suitable for injecting a full Portable Executable (PE) file.

---

### Prerequisites:
1. **Windows** operating system.
2. A **C++ compiler** capable of building Windows API applications (such as `g++` with MinGW or Visual Studio).
3. **Administrator privileges** might be required to check permissions on certain processes or perform certain memory and thread operations.

---

### Installation:

1. **Clone or download the repository**.
2. **Compile the source code**:
    ```bash
    g++ -o process_injection_checker process_injection_checker.cpp -lpsapi
    ```
   Or, for Visual Studio:
   - Open the solution, then build and compile.

---
Note:  you can insert two separate shellcodes into two different free memory regions of a target process. The steps to do this would involve:

Identifying two free memory regions: Use VirtualQueryEx to find multiple free memory regions.
Inserting shellcode into each region: Allocate memory or use existing free memory regions for each shellcode.
Executing each shellcode: After inserting, you'd likely create threads or use some mechanism to execute the shellcodes in each region



### Usage:

1. **Basic Help**:
    ```cmd
    process_injection_checker.exe -h  or  process_injection_checker.exe
    ```
    This command will display a help menu with a list of available checks.

2. **Perform Injection Feasibility Checks**:
    ```cmd
    process_injection_checker.exe [pid|process_name]
    ```
    Replace `[pid|process_name]` with either the **PID** or the **name** of the process you want to check.

    Example:
    ```cmd
    process_injection_checker.exe 1234
    ```
    or
    ```cmd
    process_injection_checker.exe credwiz.exe
    ```
Summary of What Is Being Checked:
DLL Injection:

Permissions: PROCESS_CREATE_THREAD, PROCESS_VM_WRITE, PROCESS_VM_OPERATION.
Feasibility: If permissions are OK, DLL injection is feasible using LoadLibrary.
Reflective DLL Injection:

Memory Regions: We look for executable and writable regions (PAGE_EXECUTE_READWRITE).
Feasibility: If writable and executable memory regions exist, reflective DLL injection is feasible.
APC Injection:

Threads: We check if threads are in an alertable state (suitable for APC queuing).
Feasibility: If threads in an alertable state are found, APC injection is feasible.
Thread Injection:

Permissions: PROCESS_CREATE_THREAD, PROCESS_VM_WRITE, PROCESS_VM_OPERATION.
Feasibility: If permissions are OK, thread injection is feasible using CreateRemoteThread.
Reflective PE Injection:

Memory Regions: We look for large writable regions (typically greater than 1 MB).
Feasibility: If large writable regions are found, reflective PE injection is feasible.
Memory Analysis:

Free Regions: We output free regions (MEM_FREE) available for memory allocation.
Executable and Writable Regions: We output existing executable and writable regions for direct injection.

### Author:
**Process Injection Checker** developed by sinllaves.

Feel free to contribute or raise issues via the repository's issue tracker.

---

Let me know if you'd like further customizations to the README or anything else!