/*
Now, the DLL and the executable can use the same header file to declare 
the DoSomething function, and the correct symbol (__declspec(dllexport)
or __declspec(dllimport)) will be used based on whether the function
is being defined in the DLL or imported into the executable.

*/

#include <Windows.h>
#include <iostream>



typedef void (*DoSomethingFunc)(void);


int main()
{
    HINSTANCE hDLL;
    DoSomethingFunc DoSomething;

// loading the DLL using LoadLibrary
 
    hDLL = LoadLibrary(L"C:\\Users\\admin\\source\\repos\\GLLLIBRARY\\x64\\Release\\GLLLIBRARY.dll");
    if (hDLL == NULL)
    {
        std::cout << "Could not load the DLL." << std::endl;
        return 1;
    }
/* retrieving the address of the function using GetProcAddress The DoSomething function is declared as a function pointer DoSomethingFunc with the
typedef keyword, so that it can be assigned the address returned by GetProcAddress*/

    DoSomething = (DoSomethingFunc)GetProcAddress(hDLL, "DoSomething");
    if (DoSomething == NULL)
    {
        std::cout << "Could not find the function." << std::endl;
        return 1;
    }

    // calling the function. 

    DoSomething();

    //free the memory

    FreeLibrary(hDLL);

    return 0;
}
