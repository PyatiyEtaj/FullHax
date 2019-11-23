#pragma once
#include "HelpfulFuncs.h"

namespace HitBox
{
	struct LTVector {
		float x, y, z;
	};

	struct ModelNode {
		struct
		{
			void* ptr2;
		} *ptr1;
	};

	void   __valueChanging(PBYTE ptr, float valGolden, float valSilver);
	PBYTE  HitBoxes(DWORD adrOhHook, float valGolden, float valSilver, size_t sizeOfMOdelNode = 0x33000);
}