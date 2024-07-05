// check with developer cmd line > dumpbin /exports path.dll
// 
// this method below is specified as an export function by declaration in the header file

#define MYDLL_EXPORTS
#include "Header.h"
#include <windows.h>

void DoSomething()
{
    MessageBox(NULL, L"It worked!", L"Info", MB_OK);
}
