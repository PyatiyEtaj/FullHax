#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

namespace HitBox
{
	struct LTVector {
		float x, z, y;
	};

	struct ModelNode {
		struct
		{
			void* ptr2;
		} *ptr1;
	};

	struct MyModelForHitBoxes {
		PBYTE* Backup;
		PBYTE origGetModelById;
	};

	void   __valueChanging(PBYTE ptr, float valGolden, float valSilver);
	void   __valueChanging(PBYTE ptr, float val);
	void   __valueChanging(PBYTE ptr, float x, float y, float z);
	MyModelForHitBoxes*  HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver);
	MyModelForHitBoxes* HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float g_x, float g_y, float g_z, float s_x, float s_y, float s_z);
	PBYTE  HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver, size_t sizeOfModelNode);
}