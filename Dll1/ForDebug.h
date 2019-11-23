#pragma once
#include "HelpfulFuncs.h"
#include <sstream>
#include <iomanip>

namespace ForDebug
{
	HMODULE GetModulesNames(std::string moduleName = "NULL");

	bool CrtConsole();

	void FreeCons();

	bool TryToGetDump(HMODULE pDll, LPCSTR name = "dll_dump.dll");

	bool Dumping(std::vector<BYTE> bytes, std::string subPath = "", DWORD start = 0, DWORD end = 0);
}
