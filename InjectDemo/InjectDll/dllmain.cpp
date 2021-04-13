#include <windows.h>
#include <iostream>

void hookPrint()
{
    std::cout << "bbbbbbbb" << std::endl;
}

DWORD WINAPI hookthread(LPVOID)
{
    HMODULE module = LoadLibrary(L"TargetDll");
    if (module)
    {
        typedef int (*func)();
        func fn = (func)GetProcAddress(module, "myPrint");

        int32_t offset = (INT_PTR)hookPrint - (INT_PTR)fn - 5;
        DWORD protect_val;
        VirtualProtect((void*)((uintptr_t)fn - 5), 5, PAGE_EXECUTE_READWRITE, &protect_val);
        uint8_t* ptr = (uint8_t*)fn;
        *(ptr++) = 0xE9;
        *((int32_t*)ptr) = (int32_t)offset;
        FreeLibrary(module);
    }
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::CloseHandle(::CreateThread(nullptr, 0, hookthread, nullptr, 0, 0));
        break;
    }
    return TRUE;
}

