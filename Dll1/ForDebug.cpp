#include "ForDebug.h"

bool ConsoleIsAllocated = false;

HMODULE ForDebug::GetModulesNames(std::string moduleName)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
	if (EnumProcessModules(handle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			CHAR szModName[MAX_PATH];
			if (GetModuleFileNameExA(handle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				printf_s("%s | %s\n", szModName, moduleName.c_str());
				if (_stricmp(szModName, moduleName.c_str()) == 0)
				{
					CloseHandle(handle);
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}
bool ForDebug::CrtConsole()
{
	bool check = AllocConsole();
	if (check) // новая консоль
	{
		char wnd_title[256];
		HWND hwnd = GetForegroundWindow(); // get handle of currently active window
		GetWindowTextA(hwnd, wnd_title, sizeof(wnd_title));

		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		//freopen("CONOUT$", "w", stdout); // перенаправление вывода
		system("chcp 1251");
		printf_s("Console of %s has opened\n=======================================================\n", wnd_title);
	}
	ConsoleIsAllocated = check;
	return check;
}
void ForDebug::FreeCons()
{
	//TerminateProcess(GetConsoleWindow(),0);
	if (ConsoleIsAllocated) FreeConsole();
	ConsoleIsAllocated = FALSE;
}
bool ForDebug::TryToGetDump(HMODULE pDll, LPCSTR name)
{
	PIMAGE_NT_HEADERS ImageNtHeaders = PIMAGE_NT_HEADERS(DWORD(pDll) + DWORD(PIMAGE_DOS_HEADER(pDll)->e_lfanew));
	SIZE_T sz = ImageNtHeaders->OptionalHeader.SizeOfImage;

	PBYTE buff = new BYTE[sz];

	if (ReadProcessMemory(GetCurrentProcess(), pDll, buff, sz, NULL))
	{
		MakeBin(buff, sz, name);
	}
	else
	{
		printf_s("AM GET RECT!\n");
		return false;
	}
	//Beep(300, 300);
	return true;
}

template< class T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}
bool ForDebug::Dumping(std::vector<BYTE> bytes, std::string subPath, DWORD start, DWORD end)
{
	HANDLE hProc = GetCurrentProcess();
	MEMORY_BASIC_INFORMATION    mbi;
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	LPVOID lpMem = (LPVOID)start;
	LPVOID lpEnd = end == 0 ? si.lpMaximumApplicationAddress : (LPVOID)end;
	LPCVOID hMod = 0;
	DWORD lpList = 0;
	while (lpMem < lpEnd)
	{
		VirtualQueryEx(hProc, lpMem, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		if (mbi.Protect == PAGE_READWRITE && mbi.AllocationProtect == PAGE_READWRITE)
		{
			PBYTE ptr = FindPattern(bytes, (PBYTE)mbi.BaseAddress, (PBYTE)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize));
			if (ptr != nullptr)
			{
				printf_s("Find pattern in %X\n", ptr);
				PBYTE pBuffer = new BYTE[mbi.RegionSize];
				ReadProcessMemory(hProc, mbi.BaseAddress, pBuffer, mbi.RegionSize, 0);
				std::string name = "E://GamesMailRu//NewDumps//" + subPath + int_to_hex<DWORD>((DWORD)mbi.BaseAddress) + ".dll";
				MakeBin(pBuffer, mbi.RegionSize, name.c_str());
				delete[] pBuffer;
			}
		}
		lpMem = (LPVOID)((DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize);
	}
	printf_s("DONE!\n");
	return true;
}