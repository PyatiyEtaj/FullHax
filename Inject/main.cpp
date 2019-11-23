#include "JustOneHeader.h"
#include <unordered_map>

using namespace std;

inline HHOOK SetWndHook(int hookType, HMODULE h, LPCSTR funcName, DWORD thread)
{
	return SetWindowsHookEx(hookType, (HOOKPROC)GetProcAddress(h, funcName), h, thread);
}

BOOL SetHook(HMODULE pDll, LPCSTR wndName)
{
	DWORD threadId = GetWindowThreadProcessId(FindWindow(NULL, wndName), NULL);
	if (threadId == 0)
	{
		cerr << "window " << wndName << " doesn't running" << endl;
		return FALSE;
	}

	HHOOK hhook = SetWndHook(WH_GETMESSAGE, pDll, "HookProc", threadId);

	if (GetLastError() != 0)
	{
		cerr << "something wrong --> " << GetLastError() << endl;
		return FALSE;
	}
	cout << "HOOK " << hhook << " have setted" << endl;

	//system("pause");
	Sleep(1000);
	if (hhook != NULL)
	{
		cout << "HOOK " << hhook << " released" << endl;
		UnhookWindowsHookEx(hhook);
	}
	return TRUE;
}

void InjectCF()
{
	//HMODULE pDll = LoadLibraryA("injectionHelpDll.dll");
	HMODULE pDll = LoadLibraryA("Dll1.dll");
	//HMODULE pDll = LoadLibraryA("FunDll.dll");
	SetLastError(0);

	//BOOL res = SetHook(pDll, "RivaTunerStatisticsServer");
	BOOL res = SetHook(pDll, "CROSSFIRE");
	//BOOL res = SetHook(pDll, "D3D9Test");

	cout << "RESULT : " << res << endl;
}

int main()
{
	InjectCF();
	//system("pause");
	//system("pause");
	return 0;
}
