#pragma once
#include "JustOneHeader.h"

class Patcher 
{
private:
	using _pairWpn = std::pair<PBYTE, PWeapon>;
	std::vector<PBYTE> _adrOfDetours;
	bool _isPatched = false;
public:
	std::unordered_map<int16_t, _pairWpn> Changed;
	PWeapon* AllWpnsOriginals;
	PBYTE AdrOfGetWpnById = nullptr;

public:
	void Init();
	void OffAllSounds(bool isSoundOff = false);
	void AddNewWpn(std::string path);
	void AddNewWpnByIds(std::string path, bool izyMode = false);
	void AddNewWpnTest(std::string path);
	void MakeDumpAllWpns(std::string path = "E://GamesMailRu//NewDumps//Wpns//");
	void SetPatches(DWORD adrNew);
	void RemovePatches();
	void Recover();
	~Patcher()
	{
		std::ofstream f("Log.txt");
		f << "DESTRUCTION";
		f.close();
	}
};

