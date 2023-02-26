#ifndef __FUNCTION_DECLARATIONS__
#define __FUNCTION_DECLARATIONS__
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

void InjectDLL(const char* DLL_Path, const DWORD& Process_ID);
DWORD GetProcessID(const std::string_view& Process_Name);

namespace Index {
	enum
	{
		DLLPATH,
		PROCESSNAME,
		ARRAY_MAX_3,
	};
}

namespace Check {
	enum
	{
		VERSION_MATCH, //False
		VERSION_DOES_NOT_MATCH, //True
	};
}

#endif