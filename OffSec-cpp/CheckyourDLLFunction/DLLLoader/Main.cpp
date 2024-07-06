#include <iostream>
#include <windows.h>

int main()
{
    HMODULE hDll = LoadLibraryA("C:\\Users\\b\\source\\repos\\repos\\OffsecDev-main\\cpp\\DllHijackTest\\x64\\Release\\dllfunctest.dll");
    if (hDll == NULL){
        std::cerr << "Unable to load dll";
        return 0;
    }
    
    std::cout << "Dll loaded @ " << hDll;
    //while (1)
    //    Sleep(1000);

    Sleep(1000);
    FreeLibrary(hDll);
}
