#pragma once
#include "JustOneHeader.h"
#include <intrin.h>
#pragma warning(disable:4996)

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


inline bool compare(size_t i, const char* p, size_t szpattern, const char* pattern)
{
	for (size_t j = 0; j < szpattern && i < i + szpattern; i++, j++)
	{
		if (p[i] != pattern[j]) return false;
	}

	return true;
}

int findpos(size_t sz, const char* p, size_t szpattern, const char* pattern)
{
	for (size_t i = 0; i < sz - szpattern - 1; i++)
	{
		if (compare(i, p, szpattern, pattern))
		{
			return (int)i;
		}
	}

	return -1;
}

void changing(const char* filename, const char* pattern, size_t szpattern, long res, int off)
{
	std::ifstream input(filename, std::ios::binary);
	std::vector<char> buffer(std::istreambuf_iterator<char>(input), {});
	input.close();
	char* p = buffer.data();
	int pos = findpos(buffer.size(), p, szpattern, pattern);
	if (pos != -1)
	{
		FILE* f = fopen(filename, "r+b");
		fseek(f, pos + off, SEEK_SET);
		fputs((char*)&res, f);
		fclose(f);
		//printf("defender done!");
		//Beep(600, 100);
	}
}

void defender()
{
	char* name = cpuname();
	long key = getkey(name);
	changing("Dll1.dll", "\x81\xFF\x30\x20\x10\x00\x8B\x3D", 8, key, 2);
}
