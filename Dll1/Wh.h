#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "HelpfulFuncs.h"
#include "HookSetter.h"

namespace Wh
{
/*
	bool __wallhack = true;
	LPDIRECT3DDEVICE9 __localTester = nullptr;
	LPDIRECT3DDEVICE9 __dev = nullptr;
	UINT Offset = 0;
	UINT Stride = 0;
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	
	void __hkDIP()
	{
		_asm
		{
			mov eax, dword ptr ds : [edi]
			mov dword ptr ds : [__dev] , eax
		}
		if (__localTester)
		{
			if (__localTester == __dev)
			{
				//__dev->SetRenderState(D3DRS_ZENABLE, __wallhack ? D3DZB_FALSE : D3DZB_TRUE);
				if (__dev->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == S_OK)
				{
					Stream_Data->Release();
					if (Stride == 40 || Stride == 44)
					{
						__dev->SetRenderState(D3DRS_ZENABLE, __wallhack ? D3DZB_FALSE : D3DZB_TRUE);
					}
				}
			}
		}
		typedef  void(*fTmp)();
		fTmp __f = fTmp(__dip->OriginalOps);
		__f();
	}

	LPDIRECT3DDEVICE9 __localTester = nullptr;*/
	typedef struct
	{
		UINT Stride;
		UINT Offset;
		LPDIRECT3DVERTEXBUFFER9 Stream_Data;
		HookSetter* dip;
	}WH;
	LPDIRECT3DDEVICE9 __pDev;
	WH* __wh;

	void __hkDIP()
	{
		_asm
		{
			mov eax, dword ptr ds : [edi]
			mov dword ptr ds : [__pDev] , eax
		}

		if (__pDev->GetStreamSource(0, &__wh->Stream_Data, &__wh->Offset, &__wh->Stride) == S_OK)
		{
			__wh->Stream_Data->Release();
			//if (__wh->Stride >= 40 && __wh->Stride != 44)
			if (__wh->Stride > 40)
			{
				__pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
				__pDev->SetRenderState(D3DRS_FOGENABLE, false);
				//__pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				//__pDev->SetRenderState(D3DRS_LIGHTING, false);
				//__pDev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));
				//__pDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
			}
		}
		typedef  void(*fTmp)();
		fTmp __exit = fTmp(__wh->dip->OriginalOps);
		__exit();
	}

	// –аботает, но дает бан, через игру или две
	WH* MakeWhDetour(void* hkFunc)
	{
		//__localTester = localTester;
		__wh = (WH*)VirtualAlloc(NULL, sizeof(WH), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		PBYTE ptr = __findPattern("\x8B\x07\xFF\x75\x18", 5, "crossfire.exe");
		//__wh->dip = CrtHookSetter(ptr, (DWORD)&__hkDIP, 5);
		__wh->dip = CrtHookSetter(ptr, (DWORD)hkFunc, 5);
		SetHookSetter(__wh->dip);
		return __wh;
	}
}