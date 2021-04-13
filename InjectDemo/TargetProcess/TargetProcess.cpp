#include <iostream>
#include <windows.h>

static bool gst_running;

DWORD WINAPI workthread(LPVOID)
{
    HMODULE module = LoadLibrary(L"TargetDll");
    if (module)
    {
        typedef void (*func)();
        func fn = (func)GetProcAddress(module, "myPrint");
        gst_running = true;
        while (gst_running)
        {
            fn();
            Sleep(1000);
        }
        FreeLibrary(module);
    }
    return 0;
}

int main()
{
    HANDLE handle = ::CreateThread(nullptr, 0, workthread, nullptr, 0, 0);

    if (getchar() == 'q')
        gst_running= false;
    ::WaitForSingleObject(handle, INFINITE);
    ::CloseHandle(handle);
    return 0;
}

