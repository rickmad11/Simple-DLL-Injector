#include "stdafx.h"

//simple DLL injection with LoadLibraryA
void InjectDLL(const char* DLL_Path, const DWORD& Process_ID) {
	if (!Process_ID) { printf_s("Process not found\n"); return; }
	DWORD old_protection{};

	HANDLE Process_Handle{ ::OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION ,NULL,Process_ID) };
	if (!Process_Handle) { printf_s("Error code: %lu, could not open handle\n", ::GetLastError()); return; }

	DWORD* Base_Address = reinterpret_cast<DWORD*>(::VirtualAllocEx(Process_Handle, NULL, (std::strlen(DLL_Path) + 1), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
	if (!Base_Address) {
		printf_s("VirtualAllocEx Returned 0 Error Code: %lu\n", ::GetLastError());
		::CloseHandle(Process_Handle);
		return;
	}

	if (!::WriteProcessMemory(Process_Handle, Base_Address, DLL_Path, (std::strlen(DLL_Path) + 1), NULL)) {
		printf_s("WriteProcessMemory Failed Error Code: %lu\n", ::GetLastError());
		::VirtualFreeEx(Process_Handle, Base_Address, 0, MEM_RELEASE); 
		::CloseHandle(Process_Handle);
		return;
	}
																															
	HANDLE Thread_Handle = ::CreateRemoteThread(Process_Handle, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(::LoadLibraryA), Base_Address, 0, NULL);
	if (!Thread_Handle) {
		printf_s("CreateRemoteThread Failed Error Code: %lu\n", ::GetLastError());
		::VirtualFreeEx(Process_Handle, Base_Address, 0, MEM_RELEASE);
		::CloseHandle(Process_Handle);
		return;
	}
	WaitForSingleObject(Thread_Handle,INFINITE);

	::VirtualFreeEx(Process_Handle, Base_Address, 0, MEM_RELEASE);
	if (Process_Handle)
		::CloseHandle(Process_Handle);
	if (Thread_Handle)
		::CloseHandle(Thread_Handle);

}


//Returns Process ID when Process found
DWORD GetProcessID(const std::string_view& Process_Name) {
	DWORD ProcessID{};
	DWORD Last_Error_Code{};
	HANDLE Snapshot{ ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL) };
	if (Snapshot == INVALID_HANDLE_VALUE) { printf_s("Snapshot Failed Error Code: %lu\n", ::GetLastError()); return 0; }
	PROCESSENTRY32 process_entry;
	process_entry.dwSize = sizeof(process_entry);

	if (::Process32First(Snapshot, &process_entry)) {
		do
		{
			if (!Process_Name.compare(process_entry.szExeFile)) { ProcessID = process_entry.th32ProcessID; break; }

		} while (::Process32Next(Snapshot, &process_entry));
	}
	else
		printf_s("snapshot does not contain process information\nProcess32First Failed\n\n");

	if (Snapshot)
		::CloseHandle(Snapshot);

	return ProcessID;
}