#pragma warning (disable : 4005) //macro defs (dx)

#include "Core.h"

#pragma comment(lib, "mscoree.lib")

void restoreMemValues(bool flag) {
	if (flag) {
		TurnOffHitBoxes(flag);
		TurnOffBPI(flag);
	}
	else {
		restoreHitBoxValues();
		restoreBPIValues();
	}
}

void loadMemHack(const std::vector<int>& offs) {
	TurnOnHitBoxes(offs);
	TurnOnBPI(offs);
}

void initD3D(const std::vector<int>& offs, bool espOnly) {
	if (!espOnly) {
		wh = TurnOnWH(offs);
		wh->IsOn = false;
		TurnOnCh();
	}
	Wh::esp = (oGlowESP)FindEsp(offs);
}

bool InitRoutine(const char* path, bool espOnly) {
	PBYTE adr = NULL;
	auto offs = ReaderWriterBin().Read(path);
	if (offs.size() == 0) return false;
	initD3D(offs, espOnly);
	loadMemHack(offs);
	return true;
}

bool isInMatch(bool flag) {
	return GetModuleHandleA(XorStr("Object.dll")) != NULL;
}

std::wstring GetAddresses() {
	auto delim = std::wstring(L"|");
	return std::to_wstring((DWORD)&turnCrossHair) + delim +
		std::to_wstring((DWORD)&turnWallHack) + delim +
		std::to_wstring((DWORD)&setDefuseTime) + delim +
		std::to_wstring((DWORD)&setSideRate) + delim +
		std::to_wstring((DWORD)&setSpeedHackOnShift) + delim +
		std::to_wstring((DWORD)&setHitBoxAreaDims) + delim +
		std::to_wstring((DWORD)&setHitBoxAreaMul) + delim +
		std::to_wstring((DWORD)&InitRoutine) + delim +
		std::to_wstring((DWORD)&isInMatch) + delim +
		std::to_wstring((DWORD)&restoreMemValues);
}

void hostCLR() {
	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));
	hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pClrRuntimeHost));

	hr = pClrRuntimeHost->Start();
}

void executeCLRModule() {
	DWORD pReturnValue;
	hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(L"dummy.dll", L"pj_ld13.Engine", L"XEntry", GetAddresses().c_str(), &pReturnValue);
}

void freeCLR() {
	if (pMetaHost) pMetaHost->Release();
	if (pRuntimeInfo) pRuntimeInfo->Release();
	if (pClrRuntimeHost) pClrRuntimeHost->Release();
}

void sharpWay() {
	hostCLR();
	Beep(150, 200);
	executeCLRModule();
}

void nativeWay(const char* path, bool esp) {
	InitRoutine(path, esp);
	setHitBoxAreaDims(0, 11.f, 11.f, 11.f);
	setHitBoxAreaDims(1, 23.f, 23.f, 23.f);
	setHitBoxAreaDims(5, 26.f, 12.f, 12.f);
	setHitBoxAreaDims(6, 26.f, 12.f, 12.f);
	setHitBoxAreaDims(7, 26.f, 12.f, 12.f);
	setHitBoxAreaDims(8, 26.f, 12.f, 12.f);
	setHitBoxAreaDims(17, 25.f, 25.f, 25.f);
	turnDrawOutline(true);
}

void MainThread(void* args)
{
	sharpWay();
	//nativeWay("C:\\aob.data", true);
}

bool isActivated = false;

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (isActivated)
	{
		if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(VK_INSERT) & 1) Wh::espFlag = !Wh::espFlag;
		if (Wh::esp != NULL) Wh::esp(Wh::espFlag && isInMatch(false));
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
	isActivated = true;
	if (GetModuleHandleA(XorStr("CShell.dll"))) CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, 0, 0, 0);
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	DisableThreadLibraryCalls(hModule);
	if (!GetModuleHandleA(XorStr("CShell.dll"))) return TRUE;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		freeCLR();
		restoreMemValues(true);
		turnWallHack(false);
		break;
	}
	return TRUE;
}