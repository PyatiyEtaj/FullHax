#pragma once
#include "HookSetter.h"
#include "JustOneHeader.h"
#include "TextDrawer.h"
#include <d3d9.h>
//#include <d3dx9.h>

//#pragma comment(lib, "d3d9")
//#pragma comment(lib, "d3dx9")

#define DRAW_TEXT true


// по большей части тут старый код который нужно удалить

namespace D3D
{
#define CROSSHIRE TRUE

	PDWORD VMT;
	PBYTE _oEndScene;
	PBYTE _oDrawIP;
	UINT StrideNum = 0;

	const DWORD _endScene = 42;
	const DWORD _drawIndexedPrimitive = 82;
	const DWORD _reset = 16;
	//Stride 24 - Map / Walls
	//Stride 28 - Sky
	//Stride 32 - Guns
	//Stride 44 - Players
	const UINT Strides[6] = { 24, 28, 32, 36, 40, 44 };

	bool IsDrawCH = false;
	bool IsCreated = false;
	bool WH = false, GM = false, FullBright = false, Asus = false, BH = false, Esp = false;
	bool AreTexturesCreated = false;
	UINT IsTexturing = 1;

	LPDIRECT3DTEXTURE9 _textureBlue = nullptr;
	LPDIRECT3DTEXTURE9 _textureYellow = nullptr;
	BOOL IstFirstTime = true;
	D3DVIEWPORT9 _viewPort;
	////////////////////////////////////////////////

	inline HRESULT GenerateTexture(IDirect3DDevice9 *pDevice, IDirect3DTexture9 **ppTexture, DWORD color)
	{
		BYTE white[58] = {
		   0x42, 0x4D, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
		   0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF
		};
		memcpy_s((void*)&white[54], 4, (void*)&color, 4);
		return D3DXCreateTextureFromFileInMemory(pDevice, reinterpret_cast<const void*>(&white), sizeof(white), ppTexture);
	}

	WORD _32To16(DWORD color)
	{
		DWORD red = ((((color >> 0x10) & 0xFF) * 0x1F) + 0x7F) / 0xFF;
		DWORD green = ((((color >> 0x8) & 0xFF) * 0x1F) + 0x7F) / 0xFF;
		DWORD blue = (((color & 0xFF) * 0x1F) + 0x7F) / 0xFF;

		return (WORD)(0x8000 | (red << 0xA) | (green << 0x5) | blue);
	}

	HRESULT GenerateTextureNewVer(IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
	{
		if (FAILED(pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
			return E_FAIL;

		WORD colour16 = _32To16(colour32);
		D3DLOCKED_RECT d3dlr;
		(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
		WORD *pDst16 = (WORD *)d3dlr.pBits;
		for (int xy = 0; xy < 8 * 8; xy++)
			*pDst16++ = colour16;
		(*ppD3Dtex)->UnlockRect(0);
		return S_OK;
	}

	////////////////////////////////////////////////
	UINT Offset = 0;
	UINT Stride = 0;
	HookSetter * hsDIP;
	HookSetter * hsES;
	D3DTextDrawer* td;
	bool IsTdCreated = false;
	bool ShowMenu = false;
	bool IsSoundOff = false;

	HRESULT __stdcall hkDrawPrimitive(
		LPDIRECT3DDEVICE9 pDev,
		D3DPRIMITIVETYPE  d3dPrimitive,
		INT               BaseVertexIndex,
		UINT              MinVertexIndex,
		UINT              NumVertices,
		UINT              startIndex,
		UINT              primCount
	)
	{
		typedef HRESULT(__stdcall * dip)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);

		//dip f = dip(_oDrawIP+5); // работает быстро, но чамсы не работают
		dip f = dip(hsDIP->OriginalOps);
		LPDIRECT3DVERTEXBUFFER9 Stream_Data;

		if (!AreTexturesCreated)
		{
			if (_textureYellow != nullptr)
			{
				delete _textureBlue;
				delete _textureYellow;
			}
			if (GenerateTextureNewVer(pDev, &_textureYellow, D3DCOLOR_ARGB(255, 255, 255, 0)) == E_FAIL) printf_s("pzdc\n");
			if (GenerateTextureNewVer(pDev, &_textureBlue, 0x0FE354FF) == E_FAIL) printf_s("pzdc 2\n");
			AreTexturesCreated = true;
		}

		if (WH || GM)
		{
			_textureBlue->PreLoad();
			_textureYellow->PreLoad();
			if (pDev->GetStreamSource(0, &Stream_Data, &Offset, &Stride) == S_OK)
			{
				Stream_Data->Release();
				if (Stride == 40 || Stride == 44)
				{
					if (GM)
					{
						if (_textureBlue != nullptr) pDev->SetTexture(0, _textureYellow);
					}
					else
					{
						if (IsTexturing == 1)
						{
							pDev->SetTexture(0, _textureBlue);
						}

						pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

						f(pDev, d3dPrimitive, BaseVertexIndex,
							MinVertexIndex, NumVertices, startIndex,
							primCount);

						if (IsTexturing == 1)
						{
							pDev->SetTexture(0, _textureYellow);
						}

						pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
					}
				}
			}
		}

		return f(pDev, d3dPrimitive, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	}


	PDWORD __testDev1 = NULL;
	PDWORD __testDev2 = NULL;
	LPDIRECT3DDEVICE9 forTest = NULL;
	/*
	HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev	)
	{
		typedef HRESULT(__stdcall * es)(LPDIRECT3DDEVICE9);
		es f = es(hsES->OriginalOps);

		forTest = pDev;

		if (IsDrawCH)
		{
			pDev->GetViewport(&_viewPort);
			float CenterX = (float)_viewPort.Width / 2;
			float CenterY = (float)_viewPort.Height / 2;
			D3DRECT rec1 = { CenterX - 8, CenterY, CenterX + 8 + 1, CenterY + 1 };
			D3DRECT rec2 = { CenterX, CenterY - 8, CenterX + 1, CenterY + 8 + 1 };
			pDev->Clear(1, &rec1, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			pDev->Clear(1, &rec2, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			D3DRECT rec3 = { CenterX - 2, CenterY, CenterX + 2 + 1, CenterY + 1 };
			D3DRECT rec4 = { CenterX, CenterY - 2, CenterX + 1, CenterY + 2 + 1 };
			pDev->Clear(1, &rec3, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
			pDev->Clear(1, &rec4, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
		}

#if DRAW_TEXT
		if (!IsTdCreated)
		{
			printf_s("--< Re//Create UI >--\n");
			td = CrtD3DTextDrawer(pDev, 50, 300, D3DCOLOR_ARGB(255, 255, 255, 0), "Consolas",18);
			IsTdCreated = true;
			ShowMenu = true;
		}
		if (ShowMenu)
		{
#define TEXT_LENGTH 80
			char s[TEXT_LENGTH];
			//pDev->Clear(1, (D3DRECT*)&td->Rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(100, 0, 0, 0), 0, 0);
			sprintf_s(s, TEXT_LENGTH, "WH = %d Chamses = %d\nBH = %d   Sound = %d\nCH = %d      GM = %d\nEsp = %d", WH, IsTexturing, BH, IsSoundOff, IsDrawCH, GM, Esp);
			xDrawText(td, s);
		}
#endif
	end_es:
		return f(pDev);
	}*/

	struct forch
	{
		HookSetter* hs;
		bool ison;
	};

	forch* ch;

	forch* InitCrossHair(void* hook) {
		ch = (forch*)malloc(sizeof(forch));
		ch->hs = CrtHookSetter(_oEndScene, (DWORD)hook, 7);
		ch->ison = true;
		return ch;
	}

	HRESULT __stdcall hkEndSceneTestVer(LPDIRECT3DDEVICE9 pDev)
	{
		typedef HRESULT(__stdcall* es)(LPDIRECT3DDEVICE9);
		es f = es(ch->hs->OriginalOps);
		if (ch->ison)
		{
			D3DVIEWPORT9 viewPort;
			pDev->GetViewport(&viewPort);
			long CenterX = viewPort.Width / 2;
			long CenterY = viewPort.Height / 2;
			D3DRECT rec1 = { CenterX - 8, CenterY, CenterX + 8 + 1, CenterY + 1 };
			D3DRECT rec2 = { CenterX, CenterY - 8, CenterX + 1, CenterY + 8 + 1 };
			pDev->Clear(1, &rec1, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			pDev->Clear(1, &rec2, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			D3DRECT rec3 = { CenterX - 2, CenterY, CenterX + 2 + 1, CenterY + 1 };
			D3DRECT rec4 = { CenterX, CenterY - 2, CenterX + 1, CenterY + 2 + 1 };
			pDev->Clear(1, &rec3, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
			pDev->Clear(1, &rec4, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
		}

		return f(pDev);
	}

	////////////////////////////////////////////////
	
	std::string OutputString = "build date: 17.08.2020";

	HRESULT __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDev)
	{
		typedef HRESULT(__stdcall* es)(LPDIRECT3DDEVICE9);
		es f = es(hsES->OriginalOps);
		if (IsDrawCH)
		{
			D3DVIEWPORT9 viewPort;
			pDev->GetViewport(&viewPort);
			long CenterX = viewPort.Width / 2;
			long CenterY = viewPort.Height / 2;
			D3DRECT rec1 = { CenterX - 8, CenterY, CenterX + 8 + 1, CenterY + 1 };
			D3DRECT rec2 = { CenterX, CenterY - 8, CenterX + 1, CenterY + 8 + 1 };
			pDev->Clear(1, &rec1, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			pDev->Clear(1, &rec2, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 255, 255), 0, 0);
			D3DRECT rec3 = { CenterX - 2, CenterY, CenterX + 2 + 1, CenterY + 1 };
			D3DRECT rec4 = { CenterX, CenterY - 2, CenterX + 1, CenterY + 2 + 1 };
			pDev->Clear(1, &rec3, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
			pDev->Clear(1, &rec4, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 0, 0);
			
		}
		//if (!IsTdCreated)
		//{
		//	//printf_s("--< Re//Create UI >--\n");
		//	td = CrtD3DTextDrawer(pDev, 50, 300, D3DCOLOR_ARGB(255, 255, 255, 0), "Consolas", OutputString.length());
		//	IsTdCreated = true;
		//	//ShowMenu = true;
		//}
		//if (ShowMenu)
		//{
		//	xDrawText(td, OutputString.c_str());
		//}
		return f(pDev);
	}

	void Constructor(LPCSTR wndName)
	{
		typedef LPDIRECT3D9(__stdcall*D3DCreate)(UINT);

		HMODULE hDLL = GetModuleHandleA("d3d9");
		if (hDLL == nullptr) { printf_s("Can't find module d3d9.dll\nAbort...\n"); return; }
		D3DCreate pDirect3DCreate9 = (D3DCreate)GetProcAddress(hDLL, "Direct3DCreate9");

		LPDIRECT3D9 pD3D = pDirect3DCreate9(D3D_SDK_VERSION);
		D3DDISPLAYMODE d3ddm;
		HRESULT hRes = pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = true;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = d3ddm.Format;
		IDirect3DDevice9 * ppReturnedDeviceInterface;
		HWND hWnd = FindWindowA(NULL, wndName);

		hRes = pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &ppReturnedDeviceInterface
		);

		PDWORD vtable = (PDWORD)(*((PDWORD)ppReturnedDeviceInterface));
		_oDrawIP = (PBYTE)vtable[_drawIndexedPrimitive];
		_oEndScene = (PBYTE)vtable[_endScene];
		
		//hsDIP = CrtHookSetter(_oDrawIP  , (DWORD)&hkDrawPrimitive, 5);
		hsES  = CrtHookSetter(_oEndScene, (DWORD)&hkEndScene, 7);
		//SetHookSetter(hsDIP);
		SetHookSetter(hsES);
		ppReturnedDeviceInterface->Release();
		pD3D->Release();
		IsCreated = true;
	}

	////////////////////////////////////////////////

	void Destructor()
	{
		//memcpy_s(_oDrawIP, 5, OriginalCode, 5);
		//UnsetHook(hsDIP);
		UnsetHook(hsES);
	}

	////////////////////////////////////////////////


}

