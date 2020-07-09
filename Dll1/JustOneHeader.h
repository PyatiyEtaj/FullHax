#pragma once
#include <Windows.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <Psapi.h>
#include "stdio.h"
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <unordered_map>

#define DEBUG TRUE

typedef struct _Weapon {
	//BYTE data[0x53D8];
	//BYTE data[0x6000]; // real size => 21512
	//BYTE data[21512]; // real size => 21512
	BYTE data[22104];
} Weapon, *PWeapon;

typedef LPVOID(*fGetWpnById)(int16_t);
typedef int   (*fEsp)(int flag1);
typedef void  (*fEspCallEax)(BYTE*);