#pragma once
#include "JustOneHeader.h"
#include "OffsEnum.h"
#include "HelpfulFuncs.h"

class Skins
{
private:
    DWORD cshell;
    std::vector<int> offs;
    typedef void* (*fGetSkinById)(int32_t);
    fGetSkinById get;
public:
    Skins(const std::vector<int>& offs) {
        cshell = (DWORD)GetModuleHandleA("CShell.dll");
        this->offs = offs;
        get = fGetSkinById(cshell + offs[OffsEnum::GetSkinById]);
    }
    void DumpAllSkins()
    {
        for (int i = 1; i < 109; i++)
        {
            DWORD name = 0x79C;
            PBYTE ptr = (PBYTE)get(i);
            if (ptr)
            {
                auto s = "Bytes//skins//" + std::string((char*)(ptr + name)) + "  [" + std::to_string(i) + "].data";
                MakeBin(ptr, 3340, s.c_str());
            }
        }
    }
    void SkinChange(int to = 35, int from = 99/*89 67*/)
    {
        size_t sz = 3340;
        PBYTE src = (PBYTE)get(to);
        PBYTE newOne = (PBYTE)get(from);
        PBYTE tmp = (PBYTE)malloc(sz);
        if (src == nullptr || newOne == nullptr) { /*printf_s("wpn with id %d or %d doesnt exist. continue...\n", id_wpn, id_zamena);*/ return; }
        memcpy_s(tmp, sz, src, sz);
        memcpy_s(src, sz, newOne, sz);
        memcpy_s(src + 0x7c4, 0xb, tmp + 0x7c4, 0xb);
        memcpy_s(newOne + 0x7cf, 0x9, tmp + 0x7cf, 0x9);
        /*memcpy_s(src + 0x7c4, 0xb, tmp + 0x7c4, 0xb);
        memcpy_s(newOne+0x7c4, 0xb, tmp+0x7c4, 0xb);*/
        free(tmp);
    }
};

