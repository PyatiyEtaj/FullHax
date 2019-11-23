#pragma once
#include "JustOneHeader.h"

PBYTE __findPattern(const char* pattern, int length, const char* module = "CShell.dll");

std::vector<BYTE> ReadFileHex(LPCSTR path = "NULL");

void DetourFunc(PBYTE adrOld, DWORD adrNew, bool needE9 = false);

std::vector<BYTE> CrtVec(const char* str, int length);

void __printer(const BYTE* b, int length);

void __printerFile(const BYTE* b, int length, LPCSTR name = "Log.txt");

PBYTE FindPattern(
	const std::vector<BYTE>& pattern, PBYTE start, PBYTE end,
	std::function<PBYTE(PBYTE)> f = [](PBYTE ptr) -> PBYTE {return ptr; }
);

MODULEINFO GetModuleInfo(const char* szModule);

PBYTE FindPatternInModule(
	std::vector<BYTE> pattern,
	LPCSTR moduleName = NULL,
	DWORD startAdr = 0, DWORD endAdr = 0,
	std::function<PBYTE(PBYTE)> f = [](PBYTE ptr) -> PBYTE {return ptr; }
);

std::vector<PBYTE> FindAllPatternsInModule(
	std::vector<BYTE> pattern, 
	DWORD offset,
	LPCSTR moduleName = NULL,
	DWORD startAdr = 0, DWORD endAdr = 0,
	std::function<PBYTE(PBYTE)> f = [](PBYTE ptr) -> PBYTE {return ptr; }
);

void MakeBin(PBYTE buff, SIZE_T sz, LPCSTR name);

void CopePtrToAdr(void* ptr, PBYTE adr, DWORD off);

void CopePtrToAdr(void* ptr, PBYTE adr, DWORD off1, DWORD offFunc);

void* CopyInMem(void* dest, void* source, size_t sz);