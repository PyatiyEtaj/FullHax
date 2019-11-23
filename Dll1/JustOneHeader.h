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

// OFFSETS
// 25A4 - texture
// 2750 - model
// 3644-3657 - FOV
typedef struct _Weapon {
	//BYTE data[0x4E18];
	BYTE data[0x47A8];
	//BYTE data[0x5000];
} Weapon, *PWeapon;

typedef LPVOID(*fGetWpnById)(int16_t);
typedef void  (WINAPI *fEsp)(int32_t __, int32_t flag);
typedef void  (*fEspCallEax)(BYTE*);