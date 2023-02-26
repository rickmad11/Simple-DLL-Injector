#include "stdafx.h"
#include "Declarations.h"
#undef max

//Check whenever Injection is possible
bool x64_x86_Check(const DWORD& Process_ID) {

	HANDLE Target = { ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, Process_ID) };

	BOOL Version_self_check{ FALSE };
	BOOL Version_Target_check{ FALSE };

	if (!::IsWow64Process(::GetCurrentProcess(), &Version_self_check))
		printf_s("IsWow64Process failed Error Code: %lu\n", ::GetLastError());

	if (!::IsWow64Process(Target, &Version_Target_check))
		printf_s("IsWow64Process failed Error Code: %lu\n", ::GetLastError());

	if (Version_self_check == Version_Target_check) { if (Target)::CloseHandle(Target); return Check::VERSION_MATCH; }
	else { 
		if (Target)::CloseHandle(Target);
		if(Version_Target_check)
		printf_s("Target a 32-bit application use the 32-bit Injector\n");
		else
			printf_s("Target is a 64-bit application use the 64-bit Injector\n");
		return Check::VERSION_DOES_NOT_MATCH; 
	}
}

//Reads the DLLPath and ProcessName from DLL.txt
const std::string* DLLPath_AND_Process_Name() {
	std::string FileString;
	std::string* DLLPath = new std::string[Index::ARRAY_MAX_3];

	std::fstream TextFile{ "DLL.txt", std::ios::out | std::ios::in };
	if (!TextFile) {
		printf_s("Failed to open file Creating new one\n");

		FILE* file = nullptr;
		fopen_s(&file, "DLL.txt", "a+");

		if(file)
			fclose(file);

		std::cout << "\nPress Enter to exit...\n";
		std::cin.sync();
		std::cin.get();
		delete[] DLLPath;
		std::exit(EXIT_FAILURE);
	}
	
	std::getline(TextFile, FileString);
	FileString.erase(std::remove(FileString.begin(), FileString.end(), '\"'), FileString.end());
	size_t pos = 0;
	while ((pos = FileString.find('\\', pos)) != std::string::npos) {
		FileString.replace(pos, 1, "\\\\");
		pos += 2;
	}

	DLLPath[Index::DLLPATH] = FileString;
	std::getline(TextFile, FileString);
	DLLPath[Index::PROCESSNAME] = FileString;

	return DLLPath;
}

void Injection_Loop() {
	DWORD ID = ::MessageBoxA(NULL, "Inject?", "Simple DLL Injector", MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON1| MB_TOPMOST);

	switch (ID)
	{
	case(IDYES):
	{
		break;
	}
	case(IDNO):
	{
		return;
	}
	default:
		break;
	}

	DWORD ProcessID{};
	const std::string* DllPath_ProcessName = DLLPath_AND_Process_Name();
	const std::string_view DllPath_ProcessName_String_View[Index::ARRAY_MAX_3]{ DllPath_ProcessName[Index::DLLPATH], DllPath_ProcessName[Index::PROCESSNAME] };

	printf_s("Waiting for Process...\n");
	while (!ProcessID) {
		ProcessID = GetProcessID(DllPath_ProcessName_String_View[Index::PROCESSNAME]);
		Sleep(50);
	}

	printf_s("Injecting...\n");

	if (ProcessID && x64_x86_Check(ProcessID) == Check::VERSION_MATCH) {
		InjectDLL(DllPath_ProcessName[Index::DLLPATH].c_str(), ProcessID);
		printf_s("successfully Injected!\n");
	}
	if (DllPath_ProcessName)
		delete[] DllPath_ProcessName;
}

int main(){
#ifdef _DEBUG
	printf_s("Debug Mode Enabled\n");
#endif

	Injection_Loop();
	printf_s("Closing Injector in 5 Seconds...");
	Sleep(5000);
	return 0;
}