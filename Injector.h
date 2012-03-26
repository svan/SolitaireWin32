#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <shlwapi.h>

//injector define
BOOL WriteProcessBYTES(HANDLE hProcess,LPVOID lpBaseAddress,LPCVOID lpBuffer,SIZE_T nSize);
BOOL LoadDll(char *procName, char *dllName);
BOOL InjectDLL(DWORD ProcessID, char *dllName);
unsigned long GetTargetProcessIdFromProcname(char *procName);
//
void enableDebugPrivileges();