#pragma once
#include "JustOneHeader.h"
#include "OffsEnum.h"
#include "HelpfulFuncs.h"

struct TextureStruct {
    int  TextureType;
    int  ImpactFXNum;
    char ImpactFX1 [0x7F];
    char ImpactFX2 [0x7F];
    char ImpactFX3 [0x7F];
    char ImpactFX4 [0x7F];
    char ImpactFX5 [0x80];
    int  ScalableImpactFX;
    char BulletMarkFX[0x7F];
    char LeftFootSoundName[0x20];
    char RightFootSoundName[0x20];
    char LandingSoundName[0x20];
    char __temp1[0x20];
    char HitSoundName1[0x20];
    char HitSoundName2[0x20];
    char HitSoundName3[0x20];
    char __temp2[0x181];
    int EdgeShotEnabled; // 1384
    int WallShotEnabled; // 1388
    int PerfectWallShotEnabled;// 1392
};

class Texture
{
private:
    DWORD cshell;
    std::vector<int> offs;
    typedef void* (*fGetTextureById)(int32_t);
    fGetTextureById get;
public:
    Texture(const std::vector<int>& offs) {
        cshell = (DWORD)GetModuleHandleA("CShell.dll");
        this->offs = offs;
        get = fGetTextureById(cshell + offs[OffsEnum::GetTextureById]);

    }

    void DumpAllTextures()
    {
        for (int i = 1; i < 19; i++)
        {
            DWORD sz = 0x594;
            PBYTE ptr = (PBYTE)get(i);
            if (ptr)
            {
                auto s = "Bytes//textures//texture-[" + std::to_string(i) + "].data";
                MakeBin(ptr, sz, s.c_str());
            }
        }
    }

    void SoundOff()
    {
        for (int i = 1; i < 19; i++)
        {
            TextureStruct* ptr = (TextureStruct*)get(i);
            if (ptr)
            {
                *(ptr->HitSoundName1) = 0;
                *(ptr->HitSoundName2) = 0;
                *(ptr->HitSoundName3) = 0;
            }
        }
    }

    void WallShot()
    {
        for (int i = 1; i < 19; i++)
        {
            TextureStruct* ptr = (TextureStruct*)get(i);
            if (ptr)
            {
                ptr->WallShotEnabled = 1;
                //ptr->PerfectWallShotEnabled = 1;
                ptr->EdgeShotEnabled = 1;
                //ptr->TextureType = 18;
            }
        }

    }
};