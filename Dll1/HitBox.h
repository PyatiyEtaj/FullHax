#pragma once
#include "HelpfulFuncs.h"
#include "OffsEnum.h"

namespace HitBox
{
	struct LTVector {
		float x, z, y;
	};

	struct Node
	{
		int ModelType;
		char ModelNodeName [32];
		float DamageFactor; // заглушка

		float RelativePosX;
		float RelativePosZ;
		float RelativePosY;

		float Radius; // заглушка

		float DimensionX;
		float DimensionZ;
		float DimensionY;

		float ObjectColorA;
		float ObjectColorR;
		float ObjectColorG;
		float ObjectColorB;

		int ModelNodeType;
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

	void   __valueChanging(PBYTE ptr, float x, float y, float z, float hboffset);
	MyModelForHitBoxes* HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float g_x, float g_y, float g_z, float s_x, float s_y, float s_z, float hboffset);
	void SetDetour(const std::vector<int>& offs, DWORD adrOhHook);
	//void   __valueChanging(PBYTE ptr, float valGolden, float valSilver);
	//void   __valueChanging(PBYTE ptr, float val);
	//MyModelForHitBoxes* HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver);
	//PBYTE  HitBoxes(const std::vector<int>& offs, DWORD adrOhHook, float valGolden, float valSilver, size_t sizeOfModelNode);
}