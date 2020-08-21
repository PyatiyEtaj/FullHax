#include "HitBox.h"
#include <iostream>

void HitBox::__valueChanging(PBYTE ptr, float x, float y, float z)
{
	Node* node = (Node*)(ptr);
	node->DimensionX = x;
	node->DimensionY = y;
	node->DimensionZ = z;
}

DWORD hbPatchAddress = NULL;

HitBox::MyModelForHitBoxes* HitBox::HitBoxes(const std::vector<int>& offs, DWORD adrOhHook)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	typedef PBYTE(*fGetModelById)(int32_t id);
	MyModelForHitBoxes* mymodels = (MyModelForHitBoxes*)malloc(sizeof(MyModelForHitBoxes));
	mymodels->origGetModelById = (PBYTE)(h + offs[OffsEnum::HitBoxGetModelById]);
	mymodels->Backup = (PBYTE*)malloc(23 * sizeof(PBYTE));
	fGetModelById f = (fGetModelById)(mymodels->origGetModelById);
	hbPatchAddress = offs[OffsEnum::HitBoxPatch];
	for (int i = 0; i < 23; i++)
	{
		mymodels->Backup[i] = (PBYTE)malloc(0x9C);
		memcpy(mymodels->Backup[i], f(i), 0x9C);
	}
	DetourFunc((PBYTE)(h + hbPatchAddress), adrOhHook);

	return mymodels;
}

void HitBox::SetDetour(DWORD adrOhHook)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	DetourFunc((PBYTE)(h + hbPatchAddress), adrOhHook);
}