#include "protector.h"
using namespace std;

#define WND_NAME "CROSSFIRE"

inline HHOOK SetWndHook(int hookType, HMODULE h, LPCSTR funcName, DWORD thread)
{
	return SetWindowsHookEx(hookType, (HOOKPROC)GetProcAddress(h, funcName), h, thread);
}
// With unload DLL
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

HHOOK SetHook(HMODULE pDll, LPCSTR wndName)
{
	HWND cfwnd = FindWindowA(NULL, wndName);

	DWORD threadId = GetWindowThreadProcessId(cfwnd, NULL);
	if (threadId == 0)
	{
		MessageBox(GetForegroundWindow(), "crossfire doesn't running", "Error", MB_OK);
		return NULL;
	}

	HHOOK hhook = SetWndHook(WH_CALLWNDPROC, pDll, "HookProc", threadId);

	if (GetLastError() != 0)
	{
		std::string s = "something wrong --> " + GetLastError();
		cout << s << endl;
		return NULL;
	}
	cout << "HOOK " << hhook << " have setted" << endl;
	SendMessage(cfwnd, WM_MOUSEMOVE, MK_LBUTTON, NULL);

	return hhook;
}

bool InjectCF()
{
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

	return true;
}


// By Global HOOK
HHOOK InstallHook(HMODULE pDll, LPCSTR wndName)
{
	HWND cfwnd = FindWindowA(NULL, wndName);
	DWORD threadId = GetWindowThreadProcessId(cfwnd, NULL);
	
	HHOOK hhook = SetWndHook(WH_CALLWNDPROC, pDll, "HookProc", threadId);

	if (GetLastError() != 0)
	{
		cout << "код ошибки: " + GetLastError() << endl;
		return NULL;
	}
	cout << "загружено!" << endl;
	SendMessage(cfwnd, WM_MOUSEMOVE, MK_LBUTTON, NULL);

	return hhook;
}

void InjectionGlobalHook()
{
	Sleep(3500);

	HMODULE pDll = LoadLibraryA("Dll1.dll");
	HHOOK hhk = InstallHook(pDll, WND_NAME);
	if (hhk)
	{
		cout << "нажмите ENTER для выгрузки (это приведёт к крашу игры)" << endl;
		int k = getchar();
		cout << "выгружено!" << endl;
		UnhookWindowsHookEx(hhk);
	}
	else cout << "свяжитесь с кодером!" << endl;
	system("pause");
}

int main()
{
	setlocale(LC_ALL, "ru-RU");
	while (true)
	{
		if (FindWindowA(NULL, WND_NAME)) {
			InjectionGlobalHook();
			break;
		}

		if (GetAsyncKeyState(VK_SPACE) & 1)
			break;

		Sleep(200);
	}

	return 0;
}
