//Explanation

//Header Inclusion : 
//The #pragma once directive prevents multiple inclusions of the same header file, which can lead to errors and increased compile times. #include directives bring in the Windows API for system - level functions and the standard string library for handling strings.

//Class Definition : 
//FileOpLock manages file operation locks, useful for synchronizing file access in Windows applications.

//Public Interface : 
//Includes methods to create locks using either file handles or file names and a method to wait for a lock to complete with an optional timeout.

//Private Members : 
//These are used internally by the class to manage the file handle, asynchronous operations, oplock buffers, and thread pool waits.

//Constructor& Destructor : 
//Control creation and cleanup of lock objects.

//Callback Methods : 
//Defined to handle asynchronous events related to file operation locks

#pragma once  // Ensures the file is included only once during compilation

#include <Windows.h>  // Include Windows API header
#include <string>     // Include the standard string library for string manipulation

// Class to handle file operation locks
class FileOpLock
{
public:
    typedef void(*UserCallback)();  // Define a type for user callback functions
    // Static method to create a file operation lock directly with a file handle
    static FileOpLock* CreateLock(HANDLE hfile, FileOpLock::UserCallback cb);
    // Static method to create a file operation lock using a file name
    static FileOpLock* CreateLock(const std::wstring& name, FileOpLock::UserCallback cb);
    // Wait for the lock to complete with a timeout
    void WaitForLock(UINT Timeout);

    ~FileOpLock();  // Destructor to clean up resources

private:
    HANDLE g_hFile;  // Handle to the file
    OVERLAPPED g_o;  // Overlapped structure for asynchronous operations
    REQUEST_OPLOCK_INPUT_BUFFER g_inputBuffer;  // Input buffer for request oplock
    REQUEST_OPLOCK_OUTPUT_BUFFER g_outputBuffer;  // Output buffer for request oplock
    HANDLE g_hLockCompleted;  // Handle to the event signaling completion of the lock
    PTP_WAIT g_wait;  // Pointer to the thread pool wait object
    UserCallback _cb;  // Callback function to be called when the oplock is triggered

    FileOpLock(UserCallback cb);  // Private constructor

    // Callback function called when the oplock wait is completed
    static void CALLBACK WaitCallback(PTP_CALLBACK_INSTANCE Instance,
        PVOID Parameter, PTP_WAIT Wait,
        TP_WAIT_RESULT WaitResult);
    static void CALLBACK WaitCallback2(PTP_CALLBACK_INSTANCE Instance,
        PVOID Parameter, PTP_WAIT Wait,
        TP_WAIT_RESULT WaitResult);
    void DoWaitCallback();  // Perform actions after wait callback is triggered
    void DoWaitCallbackt();  // Perform additional actions after wait callback is triggered
    bool BeginLock(HANDLE hfile);  // Start the lock operation using a file handle
    bool BeginLock(const std::wstring& name);  // Start the lock operation using a file name

};
