#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

typedef struct
{
	PBYTE* NeedToDetour;
	PWeapon* AllWpnsOriginals;
	PBYTE AdrOfGetWpnById;
} Patcher_c;

Patcher_c* InitPatcher(const std::vector<int> &offs);
void  SetPatches(Patcher_c* p, DWORD adrNew);
void* MakeAdrOfFunc(void* ptr, size_t sz);
void  AddNewWpnByIds(Patcher_c* p, std::string path, bool izyMode);
void  AddNewWpnTest(Patcher_c* p, std::string path);
void  AddNewWpnRaw(Patcher_c* p, std::string path);
void  AddABanchOfWpns(Patcher_c* p);
void  MakeDumpAllWpns(Patcher_c* p, std::string path, bool full = false); // "E://GamesMailRu//NewDumps//Wpns//"
