#include "HitBox.h"

void HitBox::__valueChanging(PBYTE ptr, float valGolden, float valSilver)
{
	float* v = (float*)(ptr + 0x38);
	for (int i = 0; i < 3; i++)
		v[i] = valGolden;

	v = (float*)(ptr + 0xd4);
	for (int i = 0; i < 3; i++)
		v[i] = valSilver;
	/*LTVector* v = (LTVector*)(ptr + 0x38);
	v->x = valGolden;
	v->y = valGolden;
	v->z = valGolden;

	v = (LTVector*)(ptr + 0xd4);
	v->x = valSilver;
	v->y = valSilver;
	v->z = valSilver;*/
}

PBYTE HitBox::HitBoxes(DWORD adrOhHook, float valGolden, float valSilver, size_t sizeOfMOdelNode)
{
	PBYTE ptr = FindPatternInModule(CrtVec("\x8B\x0D\x00\x00\x00\x00\x81\xFE", 8), "CShell.dll");
	ModelNode* mn = (ModelNode*)((PBYTE)ptr + 2);
	PBYTE modelNodeBackup = (PBYTE)malloc(0x33000);
	if (modelNodeBackup)
	{
		memcpy(modelNodeBackup, mn->ptr1->ptr2, 0x33000);
		ptr = FindPatternInModule(
			CrtVec("\x69\xC1\x9C\x00\x00\x00\x03\x05\x00\x00\x00\x00\x5D\xC3\x33\xC0\x5D\xC3", 18),
			"CShell.dll"
		);
		if (ptr)
		{
			ptr = ptr - 0x1C + 0x190196;
			DetourFunc(ptr, adrOhHook);
			__valueChanging((PBYTE)mn->ptr1->ptr2, valGolden, valSilver);
			return modelNodeBackup;
		}
	}

	return nullptr;
}
