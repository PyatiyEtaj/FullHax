#include "D3D.h"
#include "Esp.h"
#include "Bh.h"
#include "Wh.h"
#include "HitBox.h"
#include "PatcherC.h"
#include "ForDebug.h"
#include "ReaderWriterBin.h"
#include "protector.h"
#include "BasicPlayerInfo.h"
#include "Skin.h"
#include "Textures.h"
#include "CLTClient.h"

BOOL IsWhOn = TRUE;
BOOL IsBhOn = TRUE;
BOOL IsWpnDumpOn = TRUE;
BOOL IsSkinChangerOn = TRUE;
BOOL IsChOn = TRUE;
BOOL IsGMOn = FALSE;
BOOL LifeWithoutGrenade = FALSE;
int  EspKeyCode = VK_NUMPAD2;

float hbg_x = 8.0f;
float hbg_y = 8.0f;
float hbg_z = 8.0f;
float hbs_x = 17.0f;
float hbs_y = 17.0f;
float hbs_z = 17.0f;
float hboffset = 21.5f;

Patcher_c* p;

bool FileExists(LPCSTR fname)
{
	return GetFileAttributesA(fname) != DWORD(-1);
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
void TurnOnSkinChanger()
{
	SetPatches(p, (DWORD)(&Hook_GetWpnById));
	AddABanchOfWpns(p);
}


Wh::WH* TurnOnWH(const std::vector<int>& offs)
{
	return Wh::MakeWhDetour(offs, &Wh::__hkDIP);
}

void TurnOnCh()
{
	D3D::Constructor("CROSSFIRE");
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
void TurnOnHitBoxes(const std::vector<int>& offs)
{
	mn = HitBox::HitBoxes(offs, (DWORD)(&hook_GetNodeById), hbg_x, hbg_y, hbg_z, hbs_x, hbs_y, hbs_z, hboffset);
}

BPI::BPIForHook* bpi;
void* hook_GetBPIById(int32_t arg)
{
	if (!arg)
		return bpi->backup[arg];

	return 0;
}
void TurnOnBPI(const std::vector<int>& offs)
{
	bpi = BPI::Init(offs);
	BPI::SetDetour(offs, (DWORD)(&hook_GetBPIById));
}

void MainThread(void* args)
{
	PBYTE adr = NULL;
	auto offs = ReaderWriterBin().Read("Bytes//aob.data");
	if (offs.size() == 0) {
		Beep(600, 200);
		ExitThread(0);
	}

	p = InitPatcher(offs);
	Wh::WH* wh = NULL;
	//Skins s(offs);
	//Texture t(offs);
	bool fullWeaponDump = false;
	if (IsWpnDumpOn) MakeDumpAllWpns(p, "Bytes//", fullWeaponDump);
	if (IsSkinChangerOn) TurnOnSkinChanger();
	if (IsGMOn) {
		GM(p, LifeWithoutGrenade);
		//t.SoundOff();
	}
	if (IsWhOn) wh = TurnOnWH(offs);
	if (IsChOn) TurnOnCh();
	if (IsBhOn)	PatchBH(offs);
	//PatchRecoil(offs);
	TurnOnHitBoxes(offs);
	TurnOnBPI(offs);
	//s.DumpAllSkins();
	//s.SkinChange(35, 99);
	//t.WallShot();
	Beep(200, 200);
	fEsp esp = fEsp(FindEsp(offs));
	bool espIsEnabled = false;
	while (true)
	{
		/*if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			espIsEnabled = !espIsEnabled;
			esp(espIsEnabled);
		}*/

		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			if (wh != NULL)
				wh->IsOn = !wh->IsOn;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			D3D::IsDrawCH = !D3D::IsDrawCH;
		}

		Sleep(200);
	}
}

bool isActivated = false;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || isActivated)
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	isActivated = true;
	if (GetModuleHandleA("CShell.dll"))
	{
		CreateDirectoryA("Bytes", NULL);
		CreateDirectoryA("Bytes//wpnsdatas", NULL);
		IsThereAConfig();
		ReadSettings();
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	DisableThreadLibraryCalls(hModule);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		/*if (GetModuleHandleA("CShell.dll"))
		{
			CreateDirectoryA("Bytes", NULL);
			CreateDirectoryA("Bytes//wpnsdatas", NULL);
			IsThereAConfig();
			ReadSettings();
			//ForDebug::CrtConsole();
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start_t, 0, 0, 0);
		}*/
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