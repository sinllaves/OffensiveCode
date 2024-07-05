#include "FileOpLock.h"
#include <threadpoolapiset.h>


/*
Summary:

1. Class Initialization:
   - FileOpLock constructor initializes critical members such as event handles and buffers.
   - Properly sets buffer versions and sizes according to current oplock standards.

2. Resource Management:
   - Destructor ensures all resources (file handles, events, thread pool waits) are properly released.

3. Lock Acquisition:
   - BeginLock methods handle the core functionality of requesting oplocks on files.
   - Overloads handle both filename-based and handle-based lock initiations.
   - Includes comprehensive error checking and resource management within lock acquisition.

4. Factory Methods:
   - CreateLock static methods facilitate the creation and initialization of FileOpLock objects.
   - These methods ensure that locks are properly started before returning a usable object.

5. Event Management:
   - WaitForLock provides a synchronization point, allowing operations to wait for the lock acquisition to complete.
   - Uses Windows event mechanisms to manage wait conditions and timeouts.

6. Callback Handling:
   - Callback functions (WaitCallback and WaitCallback2) are defined to respond to completion of the oplock wait.
   - Callbacks convert generic parameters back to class-specific context and invoke instance methods.

7. Action on Callbacks:
   - DoWaitCallback and DoWaitCallbackt methods handle post-wait operations.
   - These methods check operation results, invoke user callbacks, and clean up resources.

*/

// Constructor: Initializes an oplock object with a callback and sets default values.
FileOpLock::FileOpLock(UserCallback cb) :
    g_inputBuffer({ 0 }), g_outputBuffer({ 0 }), g_o({ 0 }), g_hFile(INVALID_HANDLE_VALUE), g_hLockCompleted(nullptr), g_wait(nullptr), _cb(cb)
{
    // Set the input and output buffers for the oplock request
    g_inputBuffer.StructureVersion = REQUEST_OPLOCK_CURRENT_VERSION;
    g_inputBuffer.StructureLength = sizeof(g_inputBuffer);
    g_inputBuffer.RequestedOplockLevel = OPLOCK_LEVEL_CACHE_READ | OPLOCK_LEVEL_CACHE_HANDLE;
    g_inputBuffer.Flags = REQUEST_OPLOCK_INPUT_FLAG_REQUEST;
    g_outputBuffer.StructureVersion = REQUEST_OPLOCK_CURRENT_VERSION;
    g_outputBuffer.StructureLength = sizeof(g_outputBuffer);
}

// Destructor: Cleans up resources, ensures no memory leaks or dangling handles
FileOpLock::~FileOpLock()
{
    // Clean up thread pool wait objects
    if (g_wait)
    {
        SetThreadpoolWait(g_wait, nullptr, nullptr);
        CloseThreadpoolWait(g_wait);
        g_wait = nullptr;
    }
    // Close event handles if they are open
    if (g_o.hEvent)
    {
        CloseHandle(g_o.hEvent);
        g_o.hEvent = nullptr;
    }
    // Close the file handle if it is valid
    if (g_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
}

// Start locking process using a file name
bool FileOpLock::BeginLock(const std::wstring& filename)
{
    // Create synchronization events
    g_hLockCompleted = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_o.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // Attempt to open the file with necessary permissions for oplock
    g_hFile = CreateFileW(filename.c_str(), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    // If file handle is invalid, return failure
    if (g_hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Create a thread pool wait object and set it up
    g_wait = CreateThreadpoolWait(WaitCallback, this, nullptr);
    if (g_wait == nullptr) {
        return false;
    }
    SetThreadpoolWait(g_wait, g_o.hEvent, nullptr);

    // Request the oplock
    DeviceIoControl(g_hFile, FSCTL_REQUEST_OPLOCK,
        &g_inputBuffer, sizeof(g_inputBuffer),
        &g_outputBuffer, sizeof(g_outputBuffer),
        nullptr, &g_o);
    // Check for pending I/O, indicating success
    if (GetLastError() != ERROR_IO_PENDING) {
        return false;
    }

    return true;
}

// Start locking process using an existing file handle
bool FileOpLock::BeginLock(HANDLE hfile)
{
    // Create synchronization events
    g_hLockCompleted = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_o.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // Use the provided file handle
    g_hFile = hfile;
    // If file handle is invalid, return failure
    if (g_hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    // Create and set up a thread pool wait object
    g_wait = CreateThreadpoolWait(WaitCallback, this, nullptr);
    if (g_wait == nullptr) {
        return false;
    }
    SetThreadpoolWait(g_wait, g_o.hEvent, nullptr);

    // Request the oplock
    DeviceIoControl(g_hFile, FSCTL_REQUEST_OPLOCK,
        &g_inputBuffer, sizeof(g_inputBuffer),
        &g_outputBuffer, sizeof(g_outputBuffer),
        nullptr, &g_o);
    // Check for pending I/O, indicating success
    if (GetLastError() != ERROR_IO_PENDING) {
        return false;
    }

    return true;
}

// Factory method to create and initiate an oplock with a file name
FileOpLock* FileOpLock::CreateLock(const std::wstring& name, FileOpLock::UserCallback cb)
{
    FileOpLock* ret = new FileOpLock(cb);
    if (ret->BeginLock(name)) {
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}

// Factory method to create and initiate an oplock with a file handle
FileOpLock* FileOpLock::CreateLock(HANDLE hfile, FileOpLock::UserCallback cb)
{
    FileOpLock* ret = new FileOpLock(cb);
    if (ret->BeginLock(hfile)) {
        return ret;
    }
    else {
        delete ret;
        return nullptr;
    }
}

// Wait for the oplock to complete or timeout
void FileOpLock::WaitForLock(UINT Timeout)
{
    WaitForSingleObject(g_hLockCompleted, Timeout);
}

// Callback function triggered when oplock event is ready
void FileOpLock::WaitCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
{
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(WaitResult);

    FileOpLock* lock = reinterpret_cast<FileOpLock*>(Parameter);
    lock->DoWaitCallback();
}

// Secondary callback for oplock event handling
void FileOpLock::WaitCallback2(PTP_CALLBACK_INSTANCE Instance, PVOID Parameter, PTP_WAIT Wait, TP_WAIT_RESULT WaitResult)
{
    UNREFERENCED_PARAMETER(Instance);
    UNREFERENCED_PARAMETER(Wait);
    UNREFERENCED_PARAMETER(WaitResult);

    FileOpLock* lock = reinterpret_cast<FileOpLock*>(Parameter);
    lock->DoWaitCallbackt();
}

// Callback function to handle oplock completion
void FileOpLock::DoWaitCallbackt()
{
    DWORD dwBytes;
    if (!GetOverlappedResult(g_hFile, &g_o, &dwBytes, TRUE)) {
        // Handle error
    }

    if (_cb) {
        _cb(); // Call user-defined callback
    }
    g_hFile = INVALID_HANDLE_VALUE;
    SetEvent(g_hLockCompleted); // Signal completion
}

// Primary callback function to finalize the oplock and perform cleanup
void FileOpLock::DoWaitCallback()
{
    DWORD dwBytes;
    if (!GetOverlappedResult(g_hFile, &g_o, &dwBytes, TRUE)) {
        // Handle error
    }

    if (_cb) {
        _cb(); // Call user-defined callback
    }

    CloseHandle(g_hFile); // Close the file handle
    g_hFile = INVALID_HANDLE_VALUE;
    SetEvent(g_hLockCompleted); // Signal completion
}
