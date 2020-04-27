#include "D3D.h"
#include "Esp.h"
#include "Bh.h"
#include "Wh.h"
#include "HitBox.h"
#include "PatcherC.h"
#include "ForDebug.h"
#include "ReaderWriterBin.h"
#include "protector.h"

BOOL IsWhOn = TRUE;
BOOL IsBhOn = TRUE;
BOOL IsWpnDumpOn = TRUE;
BOOL IsSkinChangerOn = TRUE;
BOOL IsChOn = TRUE;
BOOL IsGMOn = FALSE;
BOOL LifeWithoutGrenade = FALSE;
//float HitBoxGolden = 8.0f;
//float HitBoxSilver = 17.0f;
int  EspKeyCode = VK_NUMPAD2;

float hbg_x = 8.0f;
float hbg_y = 8.0f;
float hbg_z = 8.0f;
float hbs_x = 17.0f;
float hbs_y = 17.0f;
float hbs_z = 17.0f;
float hboffset = 21.5f;

PBYTE ModelNodeBackup;
//HitBox::MyModelForHitBoxes* ModelNodes;
Patcher_c* p;

bool FileExists(LPCSTR fname)
{
	return GetFileAttributesA(fname) != DWORD(-1);
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void IsThereAConfig()
{
	LPCSTR path = "Bytes//config.ini";
	if (!FileExists(path))
	{
		std::string s = "[Settings]\n";
		s += "Wallhack = 0\nBunnyHop = 1\nSkinChanger = 0\nCrossHair = 0\nWpnDump = 0\nEspKeyCode = 0x62\nGM = 0\nLifeWithoutGrenade = 0\n";
		s += "\n//// size of golden head\nhbg_x = 8.0\nhbg_y = 8.0\nhbg_z = 8.0\n";
		s += "\n//// size of silver head\nhbs_x = 17.0\nhbs_y = 17.0\nhbs_z = 17.0\n";
		s += "\n//// head box offset\nhboffset = 21.5";
		std::ofstream f(path);
		f << s;
		f.close();
	}
}

void ReadSettings()
{
	auto readfloat = [](const char* name, const char* def) ->float 
	{
		char buf[20];
		GetPrivateProfileStringA("Settings", name, def, buf, 20, "Bytes//config.ini");
		return (float)std::stof(buf);
	};
	IsWhOn = GetPrivateProfileIntA("Settings", "Wallhack", TRUE, "Bytes//config.ini");
	IsChOn = GetPrivateProfileIntA("Settings", "CrossHair", TRUE, "Bytes//config.ini");
	IsBhOn = GetPrivateProfileIntA("Settings", "BunnyHop", TRUE, "Bytes//config.ini");
	IsWpnDumpOn = GetPrivateProfileIntA("Settings", "WpnDump", FALSE, "Bytes//config.ini");
	IsSkinChangerOn = GetPrivateProfileIntA("Settings", "SkinChanger", TRUE, "Bytes//config.ini");
	//HitBoxGolden = (float)GetPrivateProfileIntA("Settings", "HitBoxGolden", 8, "Bytes//config.ini");
	//HitBoxSilver = (float)GetPrivateProfileIntA("Settings", "HitBoxSilver", 17, "Bytes//config.ini");
	EspKeyCode = GetPrivateProfileIntA("Settings", "EspKeyCode", VK_NUMPAD2, "Bytes//config.ini");
	IsGMOn = GetPrivateProfileIntA("Settings", "GM", FALSE, "Bytes//config.ini");
	LifeWithoutGrenade = GetPrivateProfileIntA("Settings", "LifeWithoutGrenade", FALSE, "Bytes//config.ini");

	hbg_x = readfloat("hbg_x", "8.0");
	hbg_y = readfloat("hbg_y", "8.0");
	hbg_z = readfloat("hbg_z", "8.0");

	hbs_x = readfloat("hbs_x", "17.0");
	hbs_y = readfloat("hbs_y", "17.0");
	hbs_z = readfloat("hbs_z", "17.0");

	hboffset = readfloat("hboffset", "21.5");
}

LPVOID Hook_GetWpnById(int16_t idW)
{
	LPVOID res = fGetWpnById(p->AdrOfGetWpnById)(idW);
	PWeapon changedWpn = p->AllWpnsOriginals[idW];
	if (changedWpn != nullptr)
	{
		return changedWpn;
	}
	return res;
}
void TurnOnSkinChanger(PBYTE adrInCshell, DWORD offset)
{
	const int szOfHk = 0x2C;
	DWORD temp;
	if (p)
	{
		//PBYTE adr = p->AdrOfGetWpnById + 0xF1E542 + 0x13E97B;
		PBYTE adr = adrInCshell + offset;
		if (VirtualProtect(adr - 8, szOfHk + 8, PAGE_EXECUTE_READWRITE, &temp))
		{
			void* hkFunc = CopyInMem(adr, &Hook_GetWpnById, szOfHk);
			SetPatches(p, (DWORD)hkFunc);
			AddABanchOfWpns(p);
			CopePtrToAdr(p, adr, 8);
			CopePtrToAdr(hkFunc, adr, 8, 0x4);
			CopePtrToAdr(hkFunc, adr, 8, 0x14);
		}
	}
}

Wh::WH* globalWh = NULL;

void TurnOnWH(const std::vector<int>& offs, PBYTE adrInCshell, DWORD offset)
{
	const DWORD szOfWH = 0x80;
	const DWORD offWh   = 12;
	const DWORD offPdev = 8;
	DWORD temp;
	PBYTE adr = adrInCshell +offset;
	if (VirtualProtect(adr - 12, szOfWH + 12, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &Wh::__hkDIP, szOfWH); // hkFunc = adr
		Wh::WH* wh = Wh::MakeWhDetour(offs, hkFunc);
		CopePtrToAdr(wh, adr, offWh); // wh = adr - 12
		CopePtrToAdr(Wh::__pDev, adr, offPdev); // pdev = adr - 8

		//--------------| pDev  |---------------
		DWORD offsetsPdev[] = { 0x6, 0x12, 0x49, 0x5E };
		for (int i = 0; i < 4; i++) 
			CopePtrToAdr(hkFunc, adr, offPdev, offsetsPdev[i]);

		//--------------| __wh  |---------------
		//DWORD offsetsWh[] = { 0xC, 0x30, 0x3E, 0x6C};
		DWORD offsetsWh[] = { 0xC, 0x2D, 0x3E, 0x70 };
		for (int i = 0; i < 4; i++)
			CopePtrToAdr(hkFunc, adr, offWh, offsetsWh[i]);

		globalWh = wh;
	}
}

D3D::forch* ch = NULL;

void TurnOnCh(PBYTE adrInCshell, DWORD offset)
{
	D3D::Constructor("CROSSFIRE");
	DWORD temp;
	const DWORD szOfCh = 0x155;
	//PBYTE adr = p->AdrOfGetWpnById + 0xF1E542 + 0x13FB7B;
	PBYTE adr = adrInCshell + offset;
	if (VirtualProtect(adr - 8, szOfCh + 8, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &D3D::hkEndSceneTestVer, szOfCh);
		ch = (D3D::forch*)malloc(sizeof(D3D::forch));
		/*HookSetter* hsEndS*/ ch->hs = CrtHookSetter(D3D::_oEndScene, (DWORD)hkFunc, 7);
		ch->ison = true;
		CopePtrToAdr(ch, adr, 8); // hsEs
		CopePtrToAdr(hkFunc, adr, 8, 0xB);
		SetHookSetter(ch->hs);
	}
}

HitBox::MyModelForHitBoxes* mn;

void* hook_GetNodeById(int32_t arg)
{
	if (arg == 0 || arg == 1)
	{
		return mn->Backup[arg];
	}

	typedef PBYTE(*fGetModelById)(int32_t id);
	fGetModelById ret = (fGetModelById)(mn->origGetModelById);
	return ret(arg);
}

void TurnOnHitBoxes(const std::vector<int>& offs, PBYTE adrInCshell, DWORD offset)
{
	const int szOfHk = 0x2B;
	DWORD temp;
	PBYTE adr = adrInCshell + offset;
	if (VirtualProtect(adr - 8, szOfHk + 8, PAGE_EXECUTE_READWRITE, &temp))
	{
		void* hkFunc = CopyInMem(adr, &hook_GetNodeById, szOfHk);
		//mn = HitBox::HitBoxes(offs, (DWORD)hkFunc, HitBoxGolden, HitBoxSilver);
		mn = HitBox::HitBoxes(offs, (DWORD)hkFunc, hbg_x, hbg_y, hbg_z, hbs_x, hbs_y, hbs_z, hboffset);
		CopePtrToAdr(mn, adr, 8);
		CopePtrToAdr(hkFunc, adr, 8, 0x10);
		CopePtrToAdr(hkFunc, adr, 8, 0x1F);
	}
}


// в стеке сохраняются значения, поэтому после выгрузкл DLL ничего не падает
// поэтому такая хуета с параметрами
// TODO : вынесту в одну большую структуру
void ForeverCicle(FARPROC fKey, FARPROC fSleep, int espKeyCode, void* espPtr, Wh::WH* wh, D3D::forch* ch)
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
		if (key(espKeyCode) & 1)
		{
			espIsEnabled = !espIsEnabled;
			esp(espIsEnabled);
		}

		if (key(VK_NUMPAD0) & 1)
		{
			if (wh != NULL)
				wh->IsOn = !wh->IsOn;
		}

		if (key(VK_NUMPAD1) & 1)
		{
			if (ch != NULL)
				ch->ison = !ch->ison;
		}

		sleep(200);
	}
}

void PrepareAdresses(const std::vector<int>& offs, PBYTE adr, int keyCode = VK_NUMPAD2)
{
	HMODULE h = GetModuleHandleA("User32.dll");
	FARPROC fKey = GetProcAddress(h, "GetAsyncKeyState");
	h = GetModuleHandleA("Kernel32.dll");
	FARPROC fSleep = GetProcAddress(h, "Sleep");
	void* esp = FindEsp(offs);

	DWORD tempProtect;
	if (VirtualProtect(adr, 0x150, PAGE_EXECUTE_READWRITE, &tempProtect))
	{
		void* hkFunc = CopyInMem(adr, &ForeverCicle, 0x150);
		typedef void (WINAPI* fc)(FARPROC, FARPROC, int, void*, Wh::WH*, D3D::forch*);
		fc tmp = fc(hkFunc);
		tmp(fKey, fSleep, keyCode, esp, globalWh, ch);
	}
}

void SendMsgByPipe()
{
	HANDLE hPipe;
	DWORD dwWritten;
	hPipe = CreateFile(TEXT("\\\\.\\pipe\\pipe12332145"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		WriteFile(hPipe, "DONE", 5, &dwWritten, NULL);
		CloseHandle(hPipe);
	}
}

void Start_t(void* args)
{
	PBYTE adr = NULL;
	auto offs = ReaderWriterBin().Read("Bytes//aob.data");
	if (offs.size() == 0) {
		Beep(600, 200);
		ExitThread(0);
	}

	/*long key = 0x102030;
	long value = getkey(cpuname());
	if (key == value)
	{
		adr = (PBYTE)VirtualAlloc(NULL, 0x600, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}*/
	adr = (PBYTE)VirtualAlloc(NULL, 0x600, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	p = InitPatcher(offs);
	//if (IsWpnDumpOn) MakeDumpAllWpns(p, "Bytes//wpnsdatas//", true);
	if (IsWpnDumpOn) MakeDumpAllWpns(p, "Bytes//", false);
	if (IsSkinChangerOn) TurnOnSkinChanger(adr, 0x20);
	if (IsGMOn) GM(p, LifeWithoutGrenade);
	if (IsWhOn) TurnOnWH(offs, adr, 0x150);
	if (IsChOn) TurnOnCh(adr, 0x200);
	if (IsBhOn)	PatchBH(offs);
	TurnOnHitBoxes(offs, adr, 0x400);
	Beep(200, 200);
	SendMsgByPipe();
	PrepareAdresses(offs, adr + 0x450, EspKeyCode);		
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
			CreateDirectoryA("Bytes//wpnsdatas", NULL);
			IsThereAConfig();
			ReadSettings();
			//ForDebug::CrtConsole();
			DisableThreadLibraryCalls(hModule);
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start_t, 0, 0, 0);
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}