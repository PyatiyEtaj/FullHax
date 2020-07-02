#include "HitBox.h"


void HitBox::__valueChanging(PBYTE ptr, float x, float y, float z, float hboffset)
{	
	Node* node = (Node*)(ptr);
	//node->RelativePosX = hboffset; // 0x28
	//node->Radius = hboffset;
	node->DimensionX = x; // 0x38
	node->DimensionY = y; //
	node->DimensionZ = z; //
	/*
	// OLD VERSION
	LTVector* offset = (LTVector*)(ptr + 0x24);
	offset->z = hboffset;

	LTVector* v = (LTVector*)(ptr + 0x38);
	v->x = x;
	v->y = y;
	v->z = z;
	*/
}

HitBox::MyModelForHitBoxes* HitBox::HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float g_x, float g_y, float g_z, float s_x, float s_y, float s_z, float hboffset)
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
	__valueChanging((PBYTE)f(0), g_x, g_y, g_z, hboffset);
	__valueChanging((PBYTE)f(1), s_x, s_y, s_z, hboffset);

	return mymodels;
}

void HitBox::SetDetour(const std::vector<int>& offs, DWORD adrOhHook)
{
	DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
	DetourFunc((PBYTE)(h + offs[OffsEnum::HitBoxPatch]), adrOhHook);
}



/*
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
}*/