#pragma once

#ifdef TARGETDLL_EXPORTS
#define TARGETDLL_API __declspec(dllexport)
#else
#define TARGETDLL_API __declspec(dllimport)
#endif

extern "C"
{
    TARGETDLL_API void myPrint(void);
}