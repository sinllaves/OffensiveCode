#pragma once
#pragma once

/*
This header file defines a macro MYDLL_API which is set
to __declspec(dllexport) if the MYDLL_EXPORTS symbol
is defined, and __declspec(dllimport) otherwise.
This macro is used to specify whether the function
should be exported from the DLL or imported into the
executable.


The header file also declares the DoSomething function
with the MYDLL_API keyword, which will either export
or import the function, depending on the value of
MYDLL_API
*/

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

extern "C" MYDLL_API void DoSomething();


