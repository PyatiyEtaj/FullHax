#include "HelpfulFuncs.h"


PBYTE __findPattern(const char* pattern, int length, const char* module)
{
	PBYTE adrOfEsp = FindPatternInModule(
		CrtVec(pattern, length),
		module
		//"CShell.dll"
	);

	if (adrOfEsp == nullptr) { printf_s("Cannot find pattern\n"); return nullptr; }

	return adrOfEsp;
}

void DetourFunc(PBYTE adrOld, DWORD adrNew, bool needE9)
{
	if (!adrOld) { printf_s("detourFunc failed\n"); return; }
	DWORD off = adrNew - (DWORD)adrOld - 5;
	PBYTE bOff = (PBYTE)&off;
	if (needE9) memcpy_s((void*)(adrOld), 1, "\xE9", 1);
	memcpy_s((void*)(adrOld + 1), 4, bOff, 4);
	//printf_s("detourFunc at address 0x%X success\n", adrOld);
}

std::vector<BYTE> ReadFileHex(LPCSTR path)
{
	std::ifstream file(path);
	std::vector<BYTE> bytes;
	if (file.is_open())
	{
		int buf;
		while (file >> std::hex >> buf)
		{
			bytes.push_back(BYTE(buf));
		}
		file.close();
	}
	return bytes;
}

std::vector<BYTE> CrtVec(const char* str, int length)
{
	std::vector<BYTE> vec;
	for (int i = 0; i < length; i++)
	{
		vec.emplace_back(str[i]);
	}
	return vec;
}

void __printer(const BYTE* b, int length)
{
	for (int i = 0; i < length; i++)
	{
		printf_s("%X ", b[i]);
	}
	printf_s("\n");
}


void __printerFile(const BYTE* b, int length, LPCSTR name)
{
	std::ofstream f(name);
	for (int i = 0; i < length; i++)
	{
		f << std::hex << b[i];
	}
	f << "\n";
	f.close();
}

bool __compare(const std::vector<BYTE>& b, PBYTE pos)
{
	for (size_t i = 0; i < b.size(); i++)
	{
		if (b[i] != 0x0 && b[i] != pos[i])
		{
			return false;
		}
	}
	return true;
}

PBYTE FindPattern(
	const std::vector<BYTE>& pattern, PBYTE start, PBYTE end,
	std::function<PBYTE(PBYTE)> f
)
{
	end -= pattern.size();
	int endI = pattern.size() - 1;
	int middle = endI / 2;
	for (; start < end; start++)
	{
		//printf_s("%X == %X | %X == %X | %X == %X |\n", start[0], pattern[0], start[middle], pattern[middle], start[endI], pattern[endI]);
		if (!(start[0] == pattern[0] || (pattern[0] == 0x0))) continue;
		if (!(start[middle] == pattern[middle] || (pattern[middle] == 0x0))) { start += middle; continue; }
		if (!(start[endI] == pattern[endI] || (pattern[endI] == 0x0))) { start += endI; continue; }

		if (__compare(pattern, start))
		{
			auto res = f(start);
			if (res != nullptr) return res;
		}
		else
		{
			//start += endI;
			start++;
		}
	}
	return nullptr;
}

MODULEINFO GetModuleInfo(const char* szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandleA(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}

PBYTE FindPatternInModule(
	std::vector<BYTE> pattern,
	LPCSTR moduleName,
	DWORD startAdr, DWORD endAdr,
	std::function<PBYTE(PBYTE)> f
)
{
	HANDLE hProc = GetCurrentProcess();
	MEMORY_BASIC_INFORMATION mbi;
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	MODULEINFO mInfo = GetModuleInfo(moduleName);
	//printf_s("0x%X %d\n", mInfo.lpBaseOfDll, mInfo.SizeOfImage);
	LPVOID lpMem = (startAdr == 0) ? mInfo.lpBaseOfDll : LPVOID(startAdr);
	LPVOID   end = (endAdr == 0) ? (LPVOID)((DWORD)lpMem + mInfo.SizeOfImage) : LPVOID(endAdr);
	if (startAdr != 0 && endAdr == 0)
	{
		end = si.lpMaximumApplicationAddress;
	}
	//printf_s("adr [ %X , %X ]\n", startAdr, endAdr);
	while (lpMem < end)
	{
		VirtualQueryEx(hProc, lpMem, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		DWORD oldprotect;
		//printf_s("adr [ %X , %X ]", lpMem, (DWORD)lpMem + mbi.RegionSize);
		if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect))
		{
			//printf_s(" -- oldprotect %X   ", oldprotect);
			PBYTE ptr = FindPattern(pattern, (PBYTE)mbi.BaseAddress, (PBYTE)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize), f);
			if (ptr != nullptr) return ptr;
			DWORD __temp;
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldprotect, &__temp);
		}
		//printf_s("\n");
		lpMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
	}
	return nullptr;
}

std::vector<PBYTE> FindAllPatternsInModule(
	std::vector<BYTE> pattern, 
	DWORD offset,
	LPCSTR moduleName,
	DWORD startAdr, DWORD endAdr,
	std::function<PBYTE(PBYTE)> f
)
{
	std::vector<PBYTE> res;
	HANDLE hProc = GetCurrentProcess();
	MEMORY_BASIC_INFORMATION mbi;
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	MODULEINFO mInfo = GetModuleInfo(moduleName);
	LPVOID lpMem = (startAdr == 0) ? mInfo.lpBaseOfDll : LPVOID(startAdr);
	LPVOID   end = (endAdr == 0) ? (LPVOID)((DWORD)lpMem + mInfo.SizeOfImage) : LPVOID(endAdr);
	if (startAdr != 0 && endAdr == 0)
	{
		end = si.lpMaximumApplicationAddress;
	}
	//printf_s("adr [ %X , %X ]\n", lpMem, end);
	while (lpMem < end)
	{
		VirtualQueryEx(hProc, lpMem, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		DWORD oldprotect;
		if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect))
		{
			PBYTE ptr = (PBYTE)mbi.BaseAddress;
			PBYTE endRegion = (PBYTE)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
			//printf_s("ptr - 0x%X\n", ptr);
			do
			{
				//printf_s("ptr - 0x%X", ptr);
				ptr = FindPattern(pattern, ptr, endRegion, f);
				if (ptr != nullptr)
				{
					res.push_back(ptr);
					ptr += offset;
					//printf_s(" - find 0x%X", ptr);
					//if (res.size() > 10) return res;
				}
				//printf_s("\n");
			} while (ptr < endRegion && ptr != nullptr);
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldprotect, NULL);
		}
		lpMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
	}
	return res;
}

void MakeBin(PBYTE buff, SIZE_T sz, LPCSTR name)
{
	std::ofstream f(name, std::ios::binary);

	for (SIZE_T i = 0; i < sz; i++)
	{
		f.write((char*)&buff[i], sizeof(unsigned char));
	}
	f.close();
}

void CopePtrToAdr(void* ptr, PBYTE adr, DWORD off)
{
	DWORD adresOfP = (DWORD)ptr;
	memcpy_s(adr - off, 4, &adresOfP, 4);
}

void CopePtrToAdr(void* ptr, PBYTE adr, DWORD off, DWORD offFunc)
{
	DWORD adresOfP;
	adresOfP = (DWORD)((adr - off));
	memcpy_s((PBYTE)ptr + offFunc, 4, &adresOfP, 4);
}

void* CopyInMem(void* dest, void* source, size_t sz)
{
	DWORD temp;
	if (VirtualProtect(dest, sz, PAGE_EXECUTE_READWRITE, &temp))
	{
		memcpy_s(dest, sz, source, sz);
		return dest;
	}

	return NULL;
}