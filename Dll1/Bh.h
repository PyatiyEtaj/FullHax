#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

uint8_t* PatchBH(const std::vector<int>& offs)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	void* ptr = (void*)(h + offs[OffsEnum::Bh]);
	uint8_t* ptrchik = (uint8_t*)malloc(sizeof(BYTE));
	memcpy_s(ptr, 4, &ptrchik, 4);
	*ptrchik = 0;
	return ptrchik;
}

void PatchRecoil(const std::vector<int>& offs)
{
	PBYTE adr = FindPatternLight(
		CrtVec("\x55\x8B\xEC\x83\xEC\x0C\xF3\x0F\x10\x45\x10\x0F\x57\xD2\xF3\x0F\x10\x0D\x00\x00\x00\x00\x0F\x57\xDB",25),
		"CShell.dll"
	);
	if (adr)
	{
		/*PDWORD ptr = (PDWORD)(*((PDWORD)(adr + 0x12)));
		*ptr = 0;*/
		//printf_s("%x", *ptr);
		/*__printer(adr, 8);
		float* recoil = (float*)malloc(4);
		*recoil = 0.0f;
		memcpy_s(adr, 4, "\xF2\x0F\x10\x05", 4);
		memcpy_s(adr + 4, 4, &recoil, 4);
		__printer(adr, 8);*/
	}
	else
		Beep(600, 200);
	/*DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	void* ptr = (void*)(h + offs[OffsEnum::Bh]);
	uint8_t* ptrchik = (uint8_t*)malloc(sizeof(BYTE));
	memcpy_s(ptr, 4, &ptrchik, 4);
	*ptrchik = 0;
	return ptrchik;*/
}