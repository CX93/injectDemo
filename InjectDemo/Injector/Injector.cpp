#include <iostream>
#include <Windows.h>
#include <Tlhelp32.h>

DWORD findProcessByName(const WCHAR* processName)
{
    HANDLE hProcessSnp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (NULL == hProcessSnp) {
        return -1;
    }

    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);
    BOOL bResult = Process32First(hProcessSnp, &pe32);
    while (bResult)
    {
        if (lstrcmp(pe32.szExeFile, processName) == 0)
        {
            CloseHandle(hProcessSnp);
            return pe32.th32ProcessID;
        }
        bResult = Process32Next(hProcessSnp, &pe32);
    }
    CloseHandle(hProcessSnp);
    return -1;
}

bool doInject(DWORD pid, const WCHAR* dllname)
{
    HANDLE  hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
    if (hProcess == nullptr)
        return false;

    int sz = (1 + lstrlenW(dllname)) * sizeof(wchar_t);
    PWSTR pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, sz, MEM_COMMIT, PAGE_READWRITE);
    if (pszLibFileRemote == nullptr)
        return false;

    if (!WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)dllname, sz, nullptr))
        return false;

    PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");
    if (pfnThreadRtn == NULL)
        return false;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);
    if (hThread == NULL)
        return false;

    ::CloseHandle(hProcess);
    ::VirtualFree(pszLibFileRemote, sz, MEM_DECOMMIT);

    return true;
}

int main()
{
    DWORD pid = findProcessByName(L"TargetProcess.exe");
    if (pid > 0)
    {
        doInject(pid, L"InjectDll");
    }
    return 0;
}

