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