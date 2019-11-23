#pragma once
#include "HelpfulFuncs.h"

void* FindEsp(PBYTE adrOfGetWpnById = nullptr)
{
	PBYTE ptr = adrOfGetWpnById == nullptr ?
		__findPattern("\x55\x8B\xEC\x51\xA1\x00\x00\x00\x00\x53\x8B\x5D\x08\x56", 14)
		:
		(adrOfGetWpnById + 0x869840);

	return ptr;
}