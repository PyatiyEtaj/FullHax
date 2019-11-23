#pragma once
#include <windows.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")

typedef struct
{
	LPD3DXFONT D3D_Font;
	RECT Rect;
	D3DCOLOR Color;
	int Size;
}D3DTextDrawer;

D3DTextDrawer* CrtD3DTextDrawer(LPDIRECT3DDEVICE9 pDev, int x, int y, D3DCOLOR color, LPCSTR fontName, int Size = 8)
{
	D3DTextDrawer* td = (D3DTextDrawer*)malloc(sizeof(D3DTextDrawer));
	if (td)
	{
		RECT einheit;
		einheit.left = x;
		einheit.right = x + 200;
		einheit.top = y;
		einheit.bottom = einheit.top + 100;
		td->Rect = einheit;
		td->Color = color;
		td->Size = Size;
		D3DXCreateFontA(pDev, td->Size, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontName, &td->D3D_Font);
	}	
	return td;
}

void xDrawText(D3DTextDrawer* td,LPCSTR xText)
{
	td->D3D_Font->DrawTextA(NULL, xText, -1, &td->Rect, DT_TOP | DT_LEFT, td->Color);
	td->D3D_Font->OnLostDevice();
	td->D3D_Font->OnResetDevice();
	//td->D3D_Font->Release();
	//delete D3D_Font;
}

