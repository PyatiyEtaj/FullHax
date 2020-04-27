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
void  AddNewWpnTest(Patcher_c* p, std::string path, bool withoutPkm = false);
void  AddNewWpnRaw(Patcher_c* p, std::string path);
void  GM(Patcher_c* p, bool lifewithoutgrenade);
void  AddABanchOfWpns(Patcher_c* p);
void  MakeDumpAllWpns(Patcher_c* p, std::string path, bool full = false); // "E://GamesMailRu//NewDumps//Wpns//"
