#include "D3D.h"
#include "Esp.h"
#include "Bh.h"
#include "Wh.h"
#include "HitBox.h"
#include "PatcherC.h"
#include "ForDebug.h"

BOOL IsWhOn = TRUE;
BOOL IsBhOn = TRUE;
BOOL IsWpnDumpOn = TRUE;
BOOL IsSkinChangerOn = TRUE;
BOOL IsChOn = TRUE;
float HitBoxGolden = 8.0f;
float HitBoxSilver = 17.0f;


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void ReadSettings()
{
	IsWhOn = GetPrivateProfileIntA("Settings", "Wallhack", TRUE, "Bytes//config.ini");
	IsChOn = GetPrivateProfileIntA("Settings", "CrossHair", TRUE, "Bytes//config.ini");
	IsBhOn = GetPrivateProfileIntA("Settings", "BunnyHop", TRUE, "Bytes//config.ini");
	IsWpnDumpOn = GetPrivateProfileIntA("Settings", "WpnDump", FALSE, "Bytes//config.ini");
	IsSkinChangerOn = GetPrivateProfileIntA("Settings", "SkinChanger", TRUE, "Bytes//config.ini");
	HitBoxGolden = (float)GetPrivateProfileIntA("Settings", "HitBoxGolden", 8, "Bytes//config.ini");
	HitBoxSilver = (float)GetPrivateProfileIntA("Settings", "HitBoxSilver", 17, "Bytes//config.ini");
}

Patcher_c* p;
LPVOID Hook_GetWpnById(int16_t idW)
{
	LPVOID res = fGetWpnById(p->AdrOfGetWpnById)(idW);
	PWeapon changedWpn = p->AllWpnsOriginals[idW];
	//printf_s("\n--------------------------------------hook\n");
	//printf("idW = %d ", idW);
	if (changedWpn != nullptr)
	{
		//printf("| wpn %s\n", idW, changedWpn->data + 0xE);
		return changedWpn;
	}
	//printf("\n");
	return res;
}
void TurnOnSkinChanger(PBYTE adrInCshell, DWORD offset)
{
	const int szOfHk = 0x2C;
	DWORD temp;
	if (p)
	{
		//PBYTE adr = p->AdrOfGetWpnById + 0xF1E542 + 0x13E97B;
		PBYTE adr = adrInCshell +offset;
		if (VirtualProtect(adr - 8, szOfHk + 8, PAGE_EXECUTE_READWRITE, &temp))
		{
			void* hkFunc = CopyInMem(adr, &Hook_GetWpnById, szOfHk);
			SetPatches(p, (DWORD)hkFunc);
			AddABanchOfWpns(p);
			CopePtrToAdr(p, adr, 8);
			CopePtrToAdr(hkFunc, adr, 8, 0x4);
			CopePtrToAdr(hkFunc, adr, 8, 0x14);
			//DWORD adresOfP = (DWORD)p;
			//memcpy_s(adr - 8, 4, &adresOfP, 4);
			//adresOfP = (DWORD)((adr-8));
			//memcpy_s((PBYTE)hkFunc + 4, 4, &adresOfP, 4);
			//memcpy_s((PBYTE)hkFunc + 0x14, 4, &adresOfP, 4);
		}
	}
}

void TurnOnWH(PBYTE adrInCshell, DWORD offset)
{
	const DWORD szOfWH = 0x78;
	const DWORD offWh   = 12;
	const DWORD offPdev = 8;
	DWORD temp;
	PBYTE adr = adrInCshell +offset;
	if (VirtualProtect(adr - 12, szOfWH + 12, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &Wh::__hkDIP, szOfWH); // hkFunc = adr
		Wh::WH* wh = Wh::MakeWhDetour(hkFunc);
		CopePtrToAdr(wh, adr, offWh); // wh = adr - 12
		CopePtrToAdr(Wh::__pDev, adr, offPdev); // pdev = adr - 8

		//--------------| pDev  |---------------
		//DWORD offsetsPdev[] = { 0x6, 0x12, 0x4F };
		DWORD offsetsPdev[] = { 0x6, 0x12, 0x48, 0x5A };
		for (int i = 0; i < 4; i++) 
			CopePtrToAdr(hkFunc, adr, offPdev, offsetsPdev[i]);

		//--------------| __wh  |---------------
		//DWORD offsetsWh[] = { 0xC, 0x30, 0x3E, 0x61 };
		DWORD offsetsWh[] = { 0xC, 0x30, 0x3E, 0x6C};//0x5A };
		for (int i = 0; i < 4; i++)
			CopePtrToAdr(hkFunc, adr, offWh, offsetsWh[i]);
	}
}

void TurnOnCh(PBYTE adrInCshell, DWORD offset)
{
	D3D::Constructor("CROSSFIRE");
	DWORD temp;
	const DWORD szOfCh = 0x137;
	//PBYTE adr = p->AdrOfGetWpnById + 0xF1E542 + 0x13FB7B;
	PBYTE adr = adrInCshell + offset;
	if (VirtualProtect(adr - 8, szOfCh + 8, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &D3D::hkEndSceneTestVer, szOfCh);
		HookSetter* hsEndS = CrtHookSetter(D3D::_oEndScene, (DWORD)hkFunc, 7);
		CopePtrToAdr(hsEndS, adr, 8); // hsEs
		CopePtrToAdr(hkFunc, adr, 8, 0xA);
		SetHookSetter(hsEndS);
	}
}

void ForeverCicle(FARPROC fKey, FARPROC fSleep, void* espPtr)
{
	typedef SHORT(WINAPI* myGetAsyncKeyState)(int vKey);
	typedef void (WINAPI* mySleep)(DWORD dwMilliseconds);
	myGetAsyncKeyState key;
	mySleep sleep;
	bool espIsEnabled = false;
	fEsp esp = fEsp(espPtr);

	while (true)
	{
		key = myGetAsyncKeyState(fKey);
		sleep = mySleep(fSleep);
		if (key(VK_NUMPAD2) & 1)
		{
			espIsEnabled = !espIsEnabled;
			esp(espIsEnabled, espIsEnabled);
		}

		sleep(200);
	}
}

PBYTE ModelNodeBackup;

void PrepareAdresses(PBYTE adr)
{
	HMODULE h = GetModuleHandleA("User32.dll");
	FARPROC fKey = GetProcAddress(h, "GetAsyncKeyState");
	h = GetModuleHandleA("Kernel32.dll");
	FARPROC fSleep = GetProcAddress(h, "Sleep");
	void* esp = FindEsp();

	DWORD tempProtect;
	if (VirtualProtect(adr, 0xC4, PAGE_EXECUTE_READWRITE, &tempProtect))
	{
		void* hkFunc = CopyInMem(adr, &ForeverCicle, 0xC4);
		typedef void (WINAPI* fc)(FARPROC, FARPROC, void*);
		fc tmp = fc(hkFunc);
		tmp(fKey, fSleep, esp);
	}
}

void* hook_GetNodeById(int32_t arg)
{
	void* result = NULL;
	if (arg >= 0 && arg <= 0x7CF)
	{
		result = (void*)(ModelNodeBackup + 0x9C * arg);
	}
	return result;
}

void TurnOnHitBoxes(PBYTE adrInCshell, DWORD offset)
{
	
	const int szOfHk = 0x1F;
	DWORD temp;
	PBYTE adr = adrInCshell + offset;
	if (VirtualProtect(adr - 8, szOfHk + 8, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &hook_GetNodeById, szOfHk);
		ModelNodeBackup = HitBox::HitBoxes((DWORD)hkFunc, HitBoxGolden, HitBoxSilver);
		CopePtrToAdr(ModelNodeBackup, adr, 8);
		CopePtrToAdr(hkFunc, adr, 8, 0x15 );
	}
}

void Start_t(void* args)
{
	// копируется в CShell.dll
	PBYTE adr = p->AdrOfGetWpnById + 0xF1E542 + 0x1E09A0;
	// тоже работает | адрес просто в памяти игры
	//PBYTE adr = (PBYTE)VirtualAlloc(NULL, 0x600, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (IsSkinChangerOn) TurnOnSkinChanger(adr, 0x20);
	if (IsWhOn) TurnOnWH(adr, 0x150);
	if (IsChOn) TurnOnCh(adr, 0x300);
	if (IsBhOn)	PatchBH();
	TurnOnHitBoxes(adr, 0x400);
	Beep(200, 200);
	PrepareAdresses(adr + 0x450);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (GetModuleHandleA("CShell.dll"))
		{
			CreateDirectoryA("Bytes", NULL);
			CreateDirectoryA("Bytes//WpnsDump", NULL);
			ReadSettings();
			p = InitPatcher();
			if (IsWpnDumpOn) MakeDumpAllWpns(p, "Bytes//WpnsDump//");
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start_t, 0, 0, 0);
		}
		/*
		// отладка
#define WND_NAME L"crossfire.exe"
//#define WND_NAME L"D3D9Test.exe"
		if (GetProcIdsByName(WND_NAME) == GetCurrentProcessId())
		{
			//CrtConsole();	
			CreateDirectoryA("Bytes", NULL);
			CreateDirectoryA("Bytes//WpnsDump", NULL);
			ReadSettings();
			p = InitPatcher();
			if (IsWpnDumpOn) MakeDumpAllWpns(p, "Bytes//WpnsDump//");
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start_t, 0, 0, 0);
			//Start_method();
		}*/
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//FreeCons();
		/*if (D3DObject::IsCreated)
		{
			D3DObject::Destructor();
		}*/
		break;
	}
	return TRUE;
}



/*

#include "Patcher.h"
BOOL NotTwice = TRUE;
BOOL IsDumped = FALSE;
BOOL lever = false;
BOOL IsSpaceDown = FALSE;
BOOL IsOnBH = false; // F5
BOOL IsSoundOff = false;
INPUT input;
UINT sec = 0;



Patcher patcher;
bool first = true;
LPVOID Hook_GetWpnById(int16_t idW)
{
	LPVOID res = fGetWpnById(patcher.AdrOfGetWpnById)(idW);
	PWeapon changedWpn = patcher.AllWpnsOriginals[idW];
	//printf_s("\n--------------------------------------hook\n");
	//printf("idW = %d ", idW);
	if (changedWpn != nullptr)
	{
		//printf("| wpn %s\n", idW, changedWpn->data + 0xE);
		return changedWpn;
	}
	//printf("\n");
	return res;
}

std::vector<std::string> Dir(std::string root, std::string pattern)
{
	WIN32_FIND_DATAA fd;
	std::vector<std::string> res;
	HANDLE hFind = FindFirstFileA((root + pattern).c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			//printf("%s: %s\n", (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "Folder" : "File", fd.cFileName);
			std::string s = root;
			s += fd.cFileName;
			res.push_back(s);
		} while (FindNextFileA(hFind, &fd));

		FindClose(hFind);
	}
	return res;
}

void AddABanchOfWpns()
{
	patcher.Recover();
	auto   dotTx = Dir("bytes//", "*.bytes");
	auto	dotT = Dir("bytes//", "*.byids");
	auto dotIzyT = Dir("bytes//", "*.izy_byids");
	auto dotTest = Dir("bytes//", "*.full_bytes");
	for (auto el : dotTx)
	{
		patcher.AddNewWpn(el);
	}
	for (auto el : dotT)
	{
		patcher.AddNewWpnByIds(el);
	}
	for (auto el : dotIzyT)
	{
		patcher.AddNewWpnByIds(el, true);
	}
	for (auto el : dotTest)
	{
		patcher.AddNewWpnTest(el);
	}
}

// THREADS
void tBunnyHop(void* pArg)
{
	int awaiting = 25;

	WORD vkey = 0x56;
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0;

	while (true)
	{
		D3DObject::BH = IsOnBH;
		if (IsOnBH && ((GetAsyncKeyState(VK_SPACE) & 0x8000)))
		{
			UINT eventCount = 0;
			input.ki.dwFlags = 0;
			eventCount = SendInput(1, &input, sizeof(INPUT));

			input.ki.dwFlags = KEYEVENTF_KEYUP;
			eventCount = SendInput(1, &input, sizeof(INPUT));

			input.ki.dwFlags = 0;
			eventCount = SendInput(1, &input, sizeof(INPUT));

			input.ki.dwFlags = KEYEVENTF_KEYUP;
			eventCount = SendInput(1, &input, sizeof(INPUT));

			Sleep(awaiting);
		}

		if ((GetAsyncKeyState(VK_DELETE) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			break;
		}

	}
	ExitThread(0);
}

void RecreatingTextures(void* pArg)
{
	while (true)
	{
		Sleep(30000);
		D3DObject::AreTexturesCreated = !D3DObject::AreTexturesCreated;
	}
	ExitThread(0);
}

void MainThread(void* pArg)
{
	//D3DObject::Constructor("CROSSFIRE");
	uint8_t* ptrchik = PatchBH();
	patcher.Init();
	patcher.SetPatches((DWORD)&Hook_GetWpnById);
	//bool espIsEnabled = false;
	//fEsp esp = FindEsp();
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RecreatingTextures, 0, 0, 0);
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)tBunnyHop, 0, 0, 0);

	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD7) & 1)
		{
			printf_s("-------------------------------------------------\n");
			AddABanchOfWpns();
			Beep(600, 200);
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			espIsEnabled = !espIsEnabled;
			D3DObject::Esp = espIsEnabled;
			esp(espIsEnabled, espIsEnabled);
		}
		if (GetAsyncKeyState(VK_F6) & 1)
		{
			D3DObject::IsDrawCH = !D3DObject::IsDrawCH;
		}

		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			D3DObject::WH = !D3DObject::WH;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			D3DObject::GM = !D3DObject::GM;
		}

		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
		{
			D3DObject::FullBright = !D3DObject::FullBright;
			D3DObject::Asus = !D3DObject::Asus;
		}

		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			D3DObject::ShowMenu = !D3DObject::ShowMenu;
		}

		if (D3DObject::IsTdCreated && (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			D3DObject::IsTdCreated = false;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			D3DObject::IsTexturing = ++D3DObject::IsTexturing % 2;
		}

		if (GetAsyncKeyState(VK_F5)      & 1) IsOnBH = !IsOnBH;
		if (GetAsyncKeyState(VK_NUMPAD9) & 1) patcher.MakeDumpAllWpns("bytes//wpns//");


		if (GetAsyncKeyState(VK_NUMPAD8) & 1)
		{
			patcher.OffAllSounds(IsSoundOff);
			IsSoundOff = !IsSoundOff;
			D3DObject::IsSoundOff = IsSoundOff;
		}

		if ((GetAsyncKeyState(VK_DELETE) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
			printf_s("--< Deattach >--\n");
			patcher.RemovePatches();
			FreeCons();
			Beep(200, 200);
			break;
		}
		Sleep(200);
	}
	ExitThread(0);
}
*/
