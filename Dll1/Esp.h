#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

void* FindEsp(const std::vector<int>& offs)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	PBYTE ptr = (PBYTE)(h + offs[OffsEnum::Esp]);
	return ptr;
}