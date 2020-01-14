#pragma once
#include <intrin.h>
#include <iostream>

#define CPU_NAME_LENGTH 0x40
#define PRIME_NUMBER 1151

char* cpuname()
{
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	// Get the information associated with each extended ID.
	char CPUBrandString[CPU_NAME_LENGTH] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);

		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
	}

	return CPUBrandString;
}

long getkey(char* in)
{
	long key = 0;
	for (int i = 0; i < CPU_NAME_LENGTH; i++)
		key += (long)in[i] * PRIME_NUMBER;
	return key;
}