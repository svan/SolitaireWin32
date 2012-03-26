#include "Injector.h"


#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

using namespace std;


// INJECTOR CODE //
void enableDebugPrivileges() {
	HANDLE hcurrent=GetCurrentProcess();
	HANDLE hToken;
	BOOL bret=OpenProcessToken(hcurrent,40,&hToken);
	LUID luid;
	bret=LookupPrivilegeValue(NULL,"SeDebugPrivilege",&luid);
	TOKEN_PRIVILEGES NewState,PreviousState;
	DWORD ReturnLength;
	NewState.PrivilegeCount =1;
	NewState.Privileges[0].Luid =luid;
	NewState.Privileges[0].Attributes=2;
	AdjustTokenPrivileges(hToken,FALSE,&NewState,28,&PreviousState,&ReturnLength);
}

BOOL LoadDll(char *procName, char *dllName)
{
   DWORD ProcID = 0;

   ProcID = GetTargetProcessIdFromProcname(procName);

   if(!(InjectDLL(ProcID, dllName)))
      MessageBox(NULL, "Process located, but injection failed", "Loader", NULL);
   
   return true;
}

BOOL InjectDLL(DWORD ProcessID, char *dllName)
{
   HANDLE Proc;
   char buf[50]={0};

   LPVOID RemoteString, LoadLibAddy;
   DWORD  hLibModule = 0;

   if(!ProcessID)
      return false;


   Proc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, ProcessID);
   //Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

   //cout << Proc << "\n" ;
   //cout << ProcessID << "\n" ;
   //cout << dllName << "\n" ;

   if(!Proc)
   {
      sprintf(buf, "OpenProcess() failed: %d", GetLastError());
      MessageBox(NULL, buf, "Loader", NULL);
      return false;
   }

   LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

   RemoteString = (LPVOID)VirtualAllocEx(Proc, NULL, strlen(dllName), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
   WriteProcessMemory(Proc, (LPVOID)RemoteString, dllName, strlen(dllName), NULL);
           CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);  

   CloseHandle(Proc);
   //WaitForSingleObject( Proc, INFINITE );
	
   return true;
}

unsigned long GetTargetProcessIdFromProcname(char *procName)
{
   PROCESSENTRY32 pe;
   HANDLE thSnapshot;
   BOOL retval, ProcFound = false;

   thSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   if(thSnapshot == INVALID_HANDLE_VALUE)
   {
      MessageBox(NULL, "Error: unable to create toolhelp snapshot", "Loader", NULL);
      return false;
   }

   pe.dwSize = sizeof(PROCESSENTRY32);

   retval = Process32First(thSnapshot, &pe);

   while(retval)
   {
      if(StrStrI(pe.szExeFile, procName) )
      {
         ProcFound = true;
         break;
      }

      retval    = Process32Next(thSnapshot,&pe);
      pe.dwSize = sizeof(PROCESSENTRY32);
   }

   return pe.th32ProcessID;
}

// RWMEM CODE //
DWORD GetProcId(char* ProcName)
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = NULL;

	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if( Process32First( hSnapshot, &pe32 ) )
	{
		do 
		{
			if( strcmp( pe32.szExeFile, ProcName ) == 0 )
				break;
		}
		while( Process32Next( hSnapshot, &pe32 ) );
	}

	if( hSnapshot != INVALID_HANDLE_VALUE )
		CloseHandle( hSnapshot );

	return pe32.th32ProcessID;
}

DWORD GetModuleBase(char* DLLName, DWORD iProcId)
{
  HANDLE hSnap;
  MODULEENTRY32 xModule;
  hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, iProcId);
  xModule.dwSize = sizeof(MODULEENTRY32);
  if (Module32First(hSnap, &xModule))
  {
    while (Module32Next(hSnap, &xModule))
    {
        if (strcmp(xModule.szModule, DLLName) == 0)
        {
        CloseHandle(hSnap);
        return (DWORD)xModule.modBaseAddr;
        }
    }
  }
  cout << "Failed to get module base";
  CloseHandle(hSnap);
  return 0;
}