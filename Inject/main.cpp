#include "protector.h"
#pragma warning(disable:4996)
using namespace std;

#define WND_NAME "CROSSFIRE"

void GetMsgFromDllByPipe()
{
	HANDLE hPipe;
	char buffer[1024];
	DWORD dwRead;

	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\pipe12332145"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024,
		1024,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	bool done = false;
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				done = !strcmp(buffer, "DONE");
				if (done) break;
			}
		}

		DisconnectNamedPipe(hPipe);
		if (done) break;
	}
	CloseHandle(hPipe);
}

inline HHOOK SetWndHook(int hookType, HMODULE h, LPCSTR funcName, DWORD thread)
{
	return SetWindowsHookEx(hookType, (HOOKPROC)GetProcAddress(h, funcName), h, thread);
}

HHOOK SetHook(HMODULE pDll, LPCSTR wndName)
{
	HWND cfwnd = FindWindowA(NULL, wndName);

	DWORD threadId = GetWindowThreadProcessId(cfwnd, NULL);
	if (threadId == 0)
	{
		MessageBox(GetForegroundWindow(), "crossfire doesn't running", "Error", MB_OK);
		return false;
	}

	HHOOK hhook = SetWndHook(WH_CALLWNDPROC, pDll, "HookProc", threadId);

	if (GetLastError() != 0)
	{
		std::string s = "something wrong --> " + GetLastError();
		cout << s << endl;
		//MessageBox(GetForegroundWindow(), s.c_str(), "Error", MB_OK);
		return false;
	}
	cout << "HOOK " << hhook << " have setted" << endl;
	SendMessage(cfwnd, WM_MOUSEMOVE, MK_LBUTTON, NULL);

	return hhook;
}

bool InjectCF()
{
	Sleep(10000);
	HMODULE pDll = LoadLibraryA("Dll1.dll");
	HHOOK hhook = SetHook(pDll, WND_NAME);
	GetMsgFromDllByPipe();
	Sleep(200);
	if (hhook != NULL)
	{
		cout << "HOOK " << hhook << " released" << endl;
		UnhookWindowsHookEx(hhook);
		return true;
	}

	return false;
}

inline bool compare(size_t i, const char* p, size_t szpattern, const char* pattern)
{
	for (size_t j = 0; j < szpattern && i < i+szpattern; i++, j++)
	{
		if (p[i] != pattern[j]) return false;
	}

	return true;
}

int findpos(size_t sz, const char* p, size_t szpattern, const char* pattern)
{
	for (size_t i = 0; i < sz-szpattern-1; i++)
	{
		if (compare(i, p, szpattern, pattern))
		{
			return (int)i;
		}
	}

	return -1;
}

void changing(const char* filename, const char* pattern, size_t szpattern, long res, int off)
{
	std::ifstream input(filename, std::ios::binary);
	std::vector<char> buffer(std::istreambuf_iterator<char>(input), {});
	input.close();
	char* p = buffer.data();
	int pos = findpos(buffer.size(), p, szpattern, pattern);
	if (pos != -1)
	{
		FILE* f = fopen(filename, "r+b");
		fseek(f, pos+off, SEEK_SET);
		fputs((char*)&res, f);
		fclose(f);
		//printf("defender done!");
		//Beep(600, 100);
	}
}

void defender()
{
	char* name = cpuname();
	long key = getkey(name);
	changing("Dll1.dll", "\x81\xFF\x30\x20\x10\x00\x8B\x3D", 8, key, 2);
}

int main()
{
	HWND hWnd = GetForegroundWindow();
	//ShowWindow(hWnd, SW_HIDE);
	//defender();
	//SetWindowTextA(GetForegroundWindow(), "INJECTOR");
	while (true)
	{
		if (FindWindowA(NULL, WND_NAME) && InjectCF()) 
			break;

		if (GetAsyncKeyState(VK_SPACE) & 1)
			break;

		Sleep(200);
	}

	return 0;
}
