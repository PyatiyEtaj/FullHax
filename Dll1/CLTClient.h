#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

namespace nCLTClient {

	HookSetter* hs;

	bool __IntersectSegment(PBYTE a1, PBYTE a2) {
		typedef bool(*original)(PBYTE, PBYTE);
		original intersectSegment = original(hs->OriginalOps);
		bool res = intersectSegment(a1, a2);
		D3D::OutputString.clear();
		for (int i = 0; i < 17; i++)
			D3D::OutputString += std::to_string(int(a1[i])) + " ";
		D3D::OutputString += "\n\n";
		for (int i = 0; i < 12; i++)
			D3D::OutputString += std::to_string((int)(a2[i])) + " ";
		return res;
	}

	class CLTClient
	{
	public:
		CLTClient(const std::vector<int>& offs);
		~CLTClient();
		void SetHooks();

	private:
		PDWORD* _thisCF;
		PDWORD* _thisCShell;
		//DWORD _cf;
	};

	// vptr[0] == ((const char* (*)())(PDWORD)(_this[0][0]))()

	CLTClient::CLTClient(const std::vector<int>& offs)
	{
		PBYTE adr = FindPatternLight(
			CrtVec("\x55\x8B\xEC\x51\x83\x3D\x00\x00\x00\x00\x00\x75\x11\x83\x3D", 15), 
			"crossfire.exe"
		);
		_thisCF = (PDWORD*)(*(PDWORD)(*(PDWORD)(adr+6)));
		hs = CrtHookSetter((PBYTE)_thisCF[35], (DWORD)&__IntersectSegment, 6);
		SetHookSetter(hs);


		/*adr = FindPatternLight(
			CrtVec("\x8B\x0D\x00\x00\x00\x00\x3A\xC3\x6A\x00\x68\x00\x00\x00\x00\x8D\x45\xEC", 18),
			"CShell.dll"
		);

		_thisCF = (PDWORD*)(*(PDWORD)(*(PDWORD)(adr + 2)));*/
	}

	// vptr hook = BAN
	//__origIntersectSegment = (PBYTE)(_this[35]);
	//_this[35] = (PDWORD)&__IntersectSegment;
	void CLTClient::SetHooks() {
	}

	CLTClient::~CLTClient()
	{
		UnsetHook(hs);
		//_this[35] = (PDWORD)&__IntersectSegment;
	}
}