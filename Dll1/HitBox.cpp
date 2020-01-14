#include "HitBox.h"

void HitBox::__valueChanging(PBYTE ptr, float valGolden, float valSilver)
{
	float* v = (float*)(ptr + 0x38);
	for (int i = 0; i < 3; i++)
		v[i] = valGolden;

	v = (float*)(ptr + 0xd4);
	for (int i = 0; i < 3; i++)
		v[i] = valSilver;
}


void HitBox::__valueChanging(PBYTE ptr, float val)
{
	float* v = (float*)(ptr + 0x38);
	for (int i = 0; i < 3; i++)
		v[i] = val;
}

void HitBox::__valueChanging(PBYTE ptr, float x, float y, float z)
{
	
	/*
	// хз что это
	*(float*)(ptr + 0x44) = 0.1f;
	*(float*)(ptr + 0x34) = 0.1f;
	*(float*)(ptr + 0x24) = 0.1f;
	*(float*)(ptr + 0x50) = 0.1f;
	*(float*)(ptr + 0x30) = 0.1f;
	*/
	// как-то влияет на размер хитбокса
	// *(float*)(ptr + 0x28) = 0.1f;
	LTVector* v = (LTVector*)(ptr + 0x38);
	v->x = x;
	v->y = y;
	v->z = z;
}

HitBox::MyModelForHitBoxes* HitBox::HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	typedef PBYTE(*fGetModelById)(int32_t id);
	MyModelForHitBoxes* mymodels = (MyModelForHitBoxes*)malloc(sizeof(MyModelForHitBoxes));
	mymodels->origGetModelById = (PBYTE)(h + offs[OffsEnum::HitBoxGetModelById]);
	mymodels->Backup = (PBYTE*)malloc(2 * sizeof(PBYTE));
	fGetModelById f = (fGetModelById)(mymodels->origGetModelById);

	for (int i = 0; i < 2; i++)
	{
		mymodels->Backup[i] = (PBYTE)malloc(0x9C);
		memcpy(mymodels->Backup[i], f(i), 0x9C);
	}
	   
	DetourFunc((PBYTE)(h + offs[OffsEnum::HitBoxPatch]), adrOhHook);
	__valueChanging((PBYTE)f(0), valGolden);
	__valueChanging((PBYTE)f(1), valSilver);

	return mymodels;
}

HitBox::MyModelForHitBoxes* HitBox::HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float g_x, float g_y, float g_z, float s_x, float s_y, float s_z)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	typedef PBYTE(*fGetModelById)(int32_t id);
	MyModelForHitBoxes* mymodels = (MyModelForHitBoxes*)malloc(sizeof(MyModelForHitBoxes));
	mymodels->origGetModelById = (PBYTE)(h + offs[OffsEnum::HitBoxGetModelById]);
	mymodels->Backup = (PBYTE*)malloc(2 * sizeof(PBYTE));
	fGetModelById f = (fGetModelById)(mymodels->origGetModelById);

	for (int i = 0; i < 2; i++)
	{
		mymodels->Backup[i] = (PBYTE)malloc(0x9C);
		memcpy(mymodels->Backup[i], f(i), 0x9C);
	}

	DetourFunc((PBYTE)(h + offs[OffsEnum::HitBoxPatch]), adrOhHook);
	__valueChanging((PBYTE)f(0), g_x, g_y, g_z);
	__valueChanging((PBYTE)f(1), s_x, s_y, s_z);

	return mymodels;
}

PBYTE HitBox::HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver, size_t sizeOfModelNode)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	ModelNode* mn = (ModelNode*)((PBYTE)(h + offs[OffsEnum::HitBoxModels]));
	PBYTE modelNodeBackup = (PBYTE)malloc(sizeOfModelNode);
	if (modelNodeBackup)
	{
		memcpy(modelNodeBackup, mn->ptr1->ptr2, sizeOfModelNode);
		DetourFunc((PBYTE)(h + offs[OffsEnum::HitBoxPatch]), adrOhHook);
		__valueChanging((PBYTE)mn->ptr1->ptr2, valGolden, valSilver);
		return modelNodeBackup;
	}

	return nullptr;
}