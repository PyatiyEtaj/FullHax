#pragma once
#include "HelpfulFuncs.h"

uint8_t* PatchBH(PBYTE adrOfGetWpnById = nullptr)
{
	void* ptr = adrOfGetWpnById == nullptr ?
		__findPattern("\x0F\xB6\x05\x00\x00\x00\x00\x8B\xCF\x50\x8D", 11) + 0x3
		:
		(adrOfGetWpnById + 0x88CBF3);
	uint8_t* ptrchik = (uint8_t*)malloc(sizeof(BYTE));
	memcpy_s(ptr, 4, &ptrchik, 4);
	*ptrchik = 0;

	return ptrchik;
}