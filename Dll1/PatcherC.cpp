#include "PatcherC.h"

/*Patcher_c* InitPatcher(const std::vector<int> &offs)
{
	HMODULE h = GetModuleHandleA("CShell.dll");
	Patcher_c* p = (Patcher_c*)VirtualAlloc(NULL, sizeof(Patcher_c), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!p) return NULL;

	p->AllWpnsOriginals = (PWeapon*)VirtualAlloc(NULL, 4000 * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	p->AdrOfGetWpnById = FindPatternInModule(
		CrtVec("\x55\x8B\xEC\x66\x8B\x4D\x08\x66\x83\xF9\xFF", 11),
		"CShell.dll"
	);
	if (!p->AdrOfGetWpnById) { free(p); return NULL; }

	p->NeedToDetour = (PBYTE*)VirtualAlloc(NULL, sizeof(PBYTE) * 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!p->NeedToDetour) { free(p); return NULL; }

	
	// ������ �������
	//PBYTE adr = p->AdrOfGetWpnById + 0x15DB1A;
	//p->NeedToDetour[0] = adr - 0x12;
	//p->NeedToDetour[1] = adr;
	//p->NeedToDetour[2] = adr + 0x5A8;
	//p->NeedToDetour[3] = adr + 0x20AB;
	//p->NeedToDetour[4] = adr - 0x10946;

	PBYTE adr = p->AdrOfGetWpnById + 0x17F13A;
	p->NeedToDetour[0] = adr - 0x12;
	p->NeedToDetour[1] = adr;
	p->NeedToDetour[2] = adr + 0x5A8;
	p->NeedToDetour[3] = adr + 0x209B;
	p->NeedToDetour[4] = adr - 0x1099A;
	//						       B19
	//						       c5c
	//						       B55
	return p;
}*/

size_t WEAPON_SIZE;

size_t GetWeaponSize(PBYTE proc)
{
	PBYTE ptr = (PBYTE)fGetWpnById(proc)(1897);
	PBYTE ptr2 = (PBYTE)fGetWpnById(proc)(1898);
	//printf_s("%X %x %d\n", ptr2, ptr, size);
	return static_cast<size_t>(ptr2 - ptr);
}

Patcher_c* InitPatcher(const std::vector<int>& offs)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	Patcher_c* p = (Patcher_c*)VirtualAlloc(NULL, sizeof(Patcher_c), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!p) return NULL;

	p->AllWpnsOriginals = (PWeapon*)VirtualAlloc(NULL, 4000 * sizeof(void*), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	p->AdrOfGetWpnById = (PBYTE)(h + offs[OffsEnum::SKGetWpn]);
	if (!p->AdrOfGetWpnById) { free(p); return NULL; }

	p->NeedToDetour = (PBYTE*)VirtualAlloc(NULL, sizeof(PBYTE) * 5, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!p->NeedToDetour) { free(p); return NULL; }


	//PBYTE adr = p->AdrOfGetWpnById + 0x17F13A;
	p->NeedToDetour[0] = (PBYTE)(h + offs[OffsEnum::SKPatch2]);//adr - 0x12;
	p->NeedToDetour[1] = (PBYTE)(h + offs[OffsEnum::SKPatch1]);
	p->NeedToDetour[2] = (PBYTE)(h + offs[OffsEnum::SKPatch3]);
	p->NeedToDetour[3] = (PBYTE)(h + offs[OffsEnum::SKPatch4]);
	p->NeedToDetour[4] = (PBYTE)(h + offs[OffsEnum::SKPatch5]);


	WEAPON_SIZE = GetWeaponSize(p->AdrOfGetWpnById);
	std::ofstream f("Bytes/WEAPON_SIZE.txt");
	f << WEAPON_SIZE << "--" << sizeof(Weapon) << std::endl;
	f.close();
	return p;
}

void SetPatches(Patcher_c* p, DWORD adrNew)
{
	for (int i = 0; i < 5; i++)
	{
		DetourFunc(p->NeedToDetour[i], adrNew);
	}

	for (int i = 0; i < 4000; i++)
	{
		PBYTE ptr = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(i);
		if (!ptr) continue;

		int16_t id = *((int16_t*)ptr); //((int16_t)(ptr)[1] << 8) | (int16_t)((ptr)[0]);
		PWeapon weapon = (PWeapon)VirtualAlloc(NULL, sizeof(Weapon), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		memcpy_s(weapon, sizeof(Weapon), ptr, sizeof(Weapon));
		p->AllWpnsOriginals[id] = weapon;
	}
}


void* MakeAdrOfFunc(void* ptr, size_t sz)
{
	void* newAdr = VirtualAlloc(NULL, sz, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);;
	DWORD temp;
	if (newAdr && VirtualProtect(newAdr, sz, PAGE_EXECUTE_READWRITE, &temp))
	{
		memcpy_s(newAdr, sz, ptr, sz);
		return newAdr;
	}

	return NULL;
}

void AddNewWpnByIds(Patcher_c* p, std::string path, bool izyMode)
{
	// parts and them lengths
	std::vector<DWORD>
		  parts = { 0xE  , 0x1FA0 },//parts = { 0xE  , 0x1E58 };
		lengths = { 0xB42, 0x3468 };//lengths = { 0xB46, 0x2874 };
	//----------
	int16_t id_wpn, id_zamena;
	PBYTE src, newOne;
	std::ifstream file(path);

	file >> id_wpn >> id_zamena;

	src = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(id_wpn);
	newOne = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(id_zamena);

	if (src == nullptr || newOne == nullptr) { printf_s("SOOOKA BLYAT!\n"); return; }

	for (int i = 0; i < lengths.size(); i++)
	{
		memcpy_s(src + parts[i], lengths[i], newOne + parts[i], lengths[i]);
	}
}


void AddNewWpnTest(Patcher_c* p, std::string path, bool withoutPkm)
{
	int16_t id_wpn, id_zamena;
	PBYTE src, newOne;
	std::ifstream file(path);

	file >> id_wpn >> id_zamena;

	   src = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(id_wpn);
	newOne = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(id_zamena);

	if (src == nullptr || newOne == nullptr) { /*printf_s("wpn with id %d or %d doesnt exist. continue...\n", id_wpn, id_zamena);*/ return; }

	memcpy_s(   src + 0x2, sizeof(Weapon) - 2, p->AllWpnsOriginals[id_zamena]->data + 0x2, sizeof(Weapon) - 2);
	memcpy_s(newOne + 0x2, sizeof(Weapon) - 2, p->AllWpnsOriginals[id_wpn]->data    + 0x2, sizeof(Weapon) - 2);
	memcpy_s(src + 0x990,  0x40, p->AllWpnsOriginals[id_wpn]->data + 0x990,  0x40);
	memcpy_s(src + 0xEBD, 0x1C, p->AllWpnsOriginals[id_wpn]->data + 0xEBD, 0x1C);
	if (withoutPkm)
		memcpy_s(src + 0xDF0, 0x6D0, p->AllWpnsOriginals[id_wpn]->data + 0xDF0, 0x6D0);
	//*(float*)(src + 3088) = 0.0001f;//damageperdistance
	//*(float*)(src + 4348) = 0.0f;//speedpenalty
}

void AddNewWpnRaw(Patcher_c* p, std::string path)
{
	auto zamena = ReadFileHex(path.c_str());
	int16_t id = *((int16_t*)zamena.data());
	if (id < 0 || id > 4000) return;
	PBYTE src = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(id);
	memcpy_s(src, sizeof(Weapon), zamena.data(), sizeof(Weapon));
}

void GM(Patcher_c* p, bool lifewithoutgrenade)
{
	auto setzero = [](PBYTE ptr, size_t sz)->void {
		for (size_t i = 0; i < sz; i++)
		{
			*(ptr+ i) = 0;
		}
	};
	DWORD offwpn = 0x2445;
	for (int i = 0; i < 4000; i++)
	{
		auto ptr = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(i);
		if (ptr == nullptr) continue;
		if (strcmp((char*)(ptr + 0x99F), "grenade") == 0)
		{
			if (lifewithoutgrenade) setzero(ptr + 0x3C0, 0x1D0);
		}
		else
		{
			setzero(ptr + offwpn, 0x20);
		}
	}
}

std::vector<std::string> Dir(std::string root, std::string pattern)
{
	WIN32_FIND_DATAA fd;
	std::vector<std::string> res;
	HANDLE hFind = FindFirstFileA((root + pattern).c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			//printf("%s: %s\n", (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "Folder" : "File", fd.cFileName);
			std::string s = root;
			s += fd.cFileName;
			res.push_back(s);
		} while (FindNextFileA(hFind, &fd));

		FindClose(hFind);
	}
	return res;
}

void AddABanchOfWpns(Patcher_c* p)
{
	auto	 dotT = Dir("Bytes//", "*.semi.txt");
	auto  dotTest = Dir("Bytes//", "*.full.txt");
	auto dotTest2 = Dir("Bytes//", "*.full2.txt");
	auto      raw = Dir("Bytes//", "*.raw.txt");
	for (auto el : dotT)
	{
		AddNewWpnByIds(p, el, false);
	}
	for (auto el : dotTest)
	{
		AddNewWpnTest(p, el);
	}
	for (auto el : dotTest2)
	{
		AddNewWpnTest(p, el, true);
	}
	for (auto el : raw)
	{
		AddNewWpnRaw(p, el);
	}
}

void MakeDumpAllWpns(Patcher_c* p, std::string path, bool full)
{
	std::ofstream f(path + "weapons.txt");
	for (int i = 0; i < 4000; i++)
	{
		auto ptr = (PBYTE)fGetWpnById(p->AdrOfGetWpnById)(i);
		if (ptr == nullptr) continue;
		f << std::string((char*)(ptr + 0xE)) + "  [" + std::to_string(i) + "]\n";
		if (full)
		{
			auto s = path + "//wpnsdatas//" + std::string((char*)(ptr + 0xE)) + "  [" + std::to_string(i) + "].data";
			MakeBin(ptr, sizeof(Weapon), s.c_str());
		}		
	}
	f.close();
}
