#include "ReaderWriterBin.h"

void ReaderWriterBin::Write(const char* path, std::vector<int> offs)
{
	std::ofstream f(path, std::ios::binary);

	for (int i : offs)
	{
		f.write((char*)&i, sizeof(int));
	}

	f.close();
}

std::vector<int> ReaderWriterBin::Read(const char* path)
{
	std::vector<int> res;
	std::ifstream f(path, std::ios::binary);
	if (f.is_open())
	{
		int i;
		while (f.read((char*)&i, sizeof(int)))
		{
			res.push_back(i);
		}
	}

	return res;
}
