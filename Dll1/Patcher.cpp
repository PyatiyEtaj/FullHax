#include "Patcher.h"
#include "HelpfulFuncs.h"

void Patcher::Init()
{
	printf_s("--< Game is patching, pls wait... >--\n");

	AllWpnsOriginals = new PWeapon[4000];

	AdrOfGetWpnById = FindPatternInModule(
		CrtVec("\x55\x8B\xEC\x66\x8B\x4D\x08\x66\x83\xF9\xFF", 11),
		"CShell.dll"
	);

	if (AdrOfGetWpnById == nullptr) { printf("Sho za naxoy!\n"); return; }

	PBYTE adr = AdrOfGetWpnById + 0x15DB1A;

	// новые зигзаги
	// патч 31_0
	_adrOfDetours.push_back(adr - 0x12);
	_adrOfDetours.push_back(adr);
	_adrOfDetours.push_back(adr + 0x5A8);
	// патч 28_3
	_adrOfDetours.push_back(adr + 0x20AB);
	// патч вылеты после 5 минут игры
	_adrOfDetours.push_back(adr - 0x10946);
	
	_isPatched = true;
}
// олдовые зизаги
//adr -= 0x1CD;
// 31_0
/*_adrOfDetours.push_back(adr);
_adrOfDetours.push_back(adr + 0x13);
_adrOfDetours.push_back(adr + 0x5D0);
//_adrOfDetours.push_back(adr - 0x1B4B7D);
//_adrOfDetours.push_back(adr + 0x2DD933);
//_adrOfDetours.push_back(adr + 0x2FC6A4);
//_adrOfDetours.push_back(adr + 0x2FC8E4);
//_adrOfDetours.push_back(adr - 0x1B4B7D);
_adrOfDetours.push_back(adr - 0x108B8);

// 28_3
_adrOfDetours.push_back(adr - 0x182E); */


void Patcher::OffAllSounds(bool isSoundOff)
{
	DWORD offset = 0x2A45;
	auto b = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	for (int i = 0; i < 4000; i++)
	{
		auto ptr = (PBYTE)fGetWpnById(AdrOfGetWpnById)(i);
		if (ptr == nullptr) continue;
		if (!isSoundOff)
		{
			memcpy_s(ptr + offset, 0x21, b, 0x21);
		}
		else
		{
			memcpy_s(ptr + offset, 0x21, AllWpnsOriginals[i]->data+offset, 0x21);
		}
	}
}

void Patcher::AddNewWpn(std::string path)
{
	DWORD offset = 0x3C2;
	int16_t id;
	auto zamena = ReadFileHex(path.c_str());
	PBYTE ptr = zamena.data(), src;

	std::vector<BYTE> helper(ptr, ptr + 0x25);
	id = ((int16_t)(helper)[1] << 8) | (int16_t)((helper)[0]);

	src = (PBYTE)fGetWpnById(AdrOfGetWpnById)(id);
	if (src == nullptr) { printf_s("SOOOKA BLYAT!\n"); return; }

	//memcpy_s(src, sizeof(Weapon), AllWpnsOriginals[id]->data, sizeof(Weapon));
	memcpy_s(src, sizeof(Weapon), zamena.data(), sizeof(Weapon));

	_pairWpn w = { src, AllWpnsOriginals[id] };
	Changed[id] = w;

	//printf_s("changing: source = %s\n             new = %s\n", AllWpnsOriginals[id]->data + 0xE, helper.data() + 0xE);
	//printf_s("-------------------------------------------------\n");
}


void Patcher::AddNewWpnByIds(std::string path, bool izyMode)
{
	// parts and them lengths
	std::vector<DWORD> lengths, parts;
	if (izyMode)
	{
		lengths = { 0x950, 0x40 , 0x240 , 0x480 , 0x20 };
		parts = { 0xE  , 0xA30, 0x25A4, 0x29A0, 0x3640 };
	}
	else
	{
		lengths = { 0xB46, 0x2874 };
		parts = { 0xE  , 0x25A4 };
	}
	//----------
	int16_t id_wpn, id_zamena;
	PBYTE src, newOne;
	std::ifstream file(path);

	file >> id_wpn >> id_zamena;

	src = (PBYTE)fGetWpnById(AdrOfGetWpnById)(id_wpn);
	newOne = (PBYTE)fGetWpnById(AdrOfGetWpnById)(id_zamena);

	if (src == nullptr || newOne == nullptr) { printf_s("SOOOKA BLYAT!\n"); return; }	

	//memcpy_s(src, sizeof(Weapon), AllWpnsOriginals[id_wpn]->data, sizeof(Weapon));
	for (int i = 0; i < lengths.size(); i++)
	{
		memcpy_s(src + parts[i], lengths[i], newOne + parts[i], lengths[i]);
	}
	
	_pairWpn w = { src, AllWpnsOriginals[id_wpn] };
	Changed[id_wpn] = w;

	printf_s("changing: source = %s\n             new = %s\n", AllWpnsOriginals[id_wpn]->data + 0xE, src + 0xE);
	printf_s("-------------------------------------------------\n");
}

void Patcher::AddNewWpnTest(std::string path)
{
	int16_t id_wpn, id_zamena;
	PBYTE src, newOne;
	std::ifstream file(path);

	file >> id_wpn >> id_zamena;

	if (Changed.find(id_wpn) != Changed.end() || Changed.find(id_zamena) != Changed.end() )
	{
		//printf_s("Wpn with id %d//%d already exist. continue...\n", id_wpn, id_zamena);
	}
	else
	{
		src = (PBYTE)fGetWpnById(AdrOfGetWpnById)(id_wpn);
		newOne = (PBYTE)fGetWpnById(AdrOfGetWpnById)(id_zamena);

		if (src == nullptr || newOne == nullptr) { printf_s("wpn with id %d or %d doesnt exist. continue...\n", id_wpn, id_zamena); return; }

		memcpy_s(src + 0x2, sizeof(Weapon) - 2, AllWpnsOriginals[id_zamena]->data + 0x2, sizeof(Weapon) - 2);
		memcpy_s(newOne + 0x2, sizeof(Weapon) - 2, AllWpnsOriginals[id_wpn]->data + 0x2, sizeof(Weapon) - 2);
		memcpy_s(src + 0x99F, 0x10, AllWpnsOriginals[id_wpn]->data + 0x99F, 0x10);
		memcpy_s(src + 0x1091, 0x16, AllWpnsOriginals[id_wpn]->data + 0x1091, 0x16);

		_pairWpn w = { src, AllWpnsOriginals[id_wpn] };
		_pairWpn w2 = { newOne, AllWpnsOriginals[id_zamena] };
		Changed[id_wpn] = w;
		Changed[id_zamena] = w2;

		//printf_s("changing: source = %s\n    test     new = %s\n", AllWpnsOriginals[id_wpn]->data + 0xE, src + 0xE);
	}

	//printf_s("-------------------------------------------------\n");
}

void Patcher::SetPatches(DWORD adrNew)
{
	for (PBYTE adr : _adrOfDetours)
	{
		DetourFunc(adr, adrNew);
	}

	for (int i = 0; i < 4000; i++)
	{
		auto ptr = (PBYTE)fGetWpnById(AdrOfGetWpnById)(i);
		if (ptr == nullptr) continue;

		int16_t id = ((int16_t)(ptr)[1] << 8) | (int16_t)((ptr)[0]);
		PWeapon p = new Weapon();

		memcpy_s(p, sizeof(Weapon), ptr, sizeof(Weapon));
		AllWpnsOriginals[id] = p;
	}

	printf_s("--< Patched 31_0 28_3 >--\n");
}

void Patcher::RemovePatches()
{
	if (!_isPatched) return;
	for (PBYTE adr : _adrOfDetours)
	{
		DetourFunc(adr, (DWORD)(AdrOfGetWpnById));
	}
	Recover();
	/*for (auto w : Changed)
	{
		if (w.second.first == nullptr) continue;
		//printf_s("Removed changes: id = %d | name = %s | ptr = %X\n", w.first, w.second.second->data + 0xE, w.second.first);
		memcpy_s(w.second.first, sizeof(Weapon), w.second.second->data, sizeof(Weapon));
	}*/
}

void Patcher::Recover()
{
	for (auto w : Changed)
	{
		if (w.second.first == nullptr) continue;
		memcpy_s(w.second.first, sizeof(Weapon), w.second.second->data, sizeof(Weapon));
	}
	Changed.clear();
}

void Patcher::MakeDumpAllWpns(std::string path)
{
	//printf_s("--< start dump...");
	for (int i = 0; i < 4000; i++)
	{
		auto ptr = (PBYTE)fGetWpnById(AdrOfGetWpnById)(i);
		if (ptr == nullptr) continue;
		auto s = path + std::string((char*)(ptr + 0xE)) +  "  [" + std::to_string(i) + "].bin";
		MakeBin(ptr, sizeof(Weapon), s.c_str());
	}
	//printf_s("done >--\n");
}