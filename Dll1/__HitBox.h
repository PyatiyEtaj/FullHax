#pragma once
#include "HelpfulFuncs.h"

struct LTVector 
{
	float x, y, z;
};

struct MyModelForHitBoxes 
{
	PBYTE* Backup;
	PBYTE oGetModelByID;
};

typedef PBYTE(*fGetModelByID)(int32_t id);

MyModelForHitBoxes* localModels;

void* hkGetNodeById(int32_t arg)
{
	if (arg == 0 || arg == 1) return localModels->Backup[arg];
	fGetModelByID ret = (fGetModelByID)(localModels->oGetModelByID);
	return ret(arg);
}

class HitBoxManager 
{
	const int szHook = 0x2B;
	DWORD dwoGetModelByID = 0;
	DWORD dwoHBPatch = 0;
	DWORD dwHookAddr = 0;
	fGetModelByID f = NULL;
	void ChangeValue(PBYTE ptr, float x, float y, float z)
	{
		LTVector* v = (LTVector*)(ptr + 0x38);
		v->x = x;
		v->y = y;
		v->z = z;
	}
	void SetHook() 
	{
		DetourFunc((PBYTE)(dwoHBPatch), dwHookAddr);
	}
public:
	HitBoxManager(DWORD offset, DWORD patchOffset, PBYTE hbAddr): dwoGetModelByID(offset),
																  dwoHBPatch(patchOffset)
	{
		DWORD temp;
		if (!VirtualProtect(hbAddr - 8, szHook + 8, PAGE_EXECUTE_READWRITE, &temp)) return;
		void* hkFunc = CopyInMem(hbAddr, &hkGetNodeById, szHook);
		dwHookAddr = (DWORD)hkFunc;
		localModels = (MyModelForHitBoxes*)malloc(sizeof(MyModelForHitBoxes));
		localModels->oGetModelByID = (PBYTE)(dwoGetModelByID);
		localModels->Backup = (PBYTE*)malloc(2 * sizeof(PBYTE));
		f = (fGetModelByID)(localModels->oGetModelByID);
		for (int i = 0; i < 2; i++)
		{
			localModels->Backup[i] = (PBYTE)malloc(0x9C);
			memcpy(localModels->Backup[i], f(i), 0x9C);
		}
		Beep(200, 200);
		SetHook();
		SetValue(0, 25.0, 25.0, 25.0);
		SetValue(1, 25.0, 25.0, 25.0);
		CopePtrToAdr(localModels, hbAddr, 8);
		CopePtrToAdr(hkFunc, hbAddr, 8, 0x10);
		CopePtrToAdr(hkFunc, hbAddr, 8, 0x1F);
	}
	void SetValue(int nodeID, float valx, float valy, float valz)
	{
		if (nodeID == 0 || nodeID == 1) ChangeValue((PBYTE)f(nodeID), valx, valy, valz);
	}
};