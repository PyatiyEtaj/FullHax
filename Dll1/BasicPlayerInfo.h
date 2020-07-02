#pragma once
#include "JustOneHeader.h"
#include "OffsEnum.h"
#include "HelpfulFuncs.h"
namespace BPI {
    struct BasicPlayerInfo
    {
        int BPIType;
        // error 1_%x esli dolgo ispolzovat
        float MovementSpeed; 
        float WalkRate; 
        float DuckWalkRate; 
        float SideMoveRate; 
        float FBRunAnimRate; 
        float LRRunAnimRate;
        float FBWalkAnimRate; 
        float LRWalkAnimRate; 
        //------------------
        float Acceleration;
        float Friction;
        float _missedfloatvalues[18];
        float DamagePenaltyTime;
        float DamagePenaltyMoveRate;
        float C4PlantTime; // ne rabotaet iz-za animacii
        float C4DefuseTime; // rabotaet
        // voobshe nichego ne delaet
        float MaxCanDefuseDistance;
        float CharacterHiddenAlpha;
        float CharacterHiddenWalkAlpha;
        float CharacterHiddenRunAlpha;
        float MovementHiddenRate;
        int CrossHairColor;
        float CrosshairColorRedChangeRate;
        float CrosshairColorGreenChangeRate;
        float CrosshairColorBlueChangeRate;
        //---------------------------------
    };

    struct BPIForHook {
        PBYTE origFunc;
        PBYTE* backup;
    };

    void __changeValue(PBYTE ptr) {
        BasicPlayerInfo* bpi = (BasicPlayerInfo*)ptr;
        bpi->C4DefuseTime = 5.0f;
        //bpi->CrossHairColor = 3;
        //bpi->WalkRate = 0.90f;
        //bpi->DuckWalkRate = 1.5f;
        //bpi->CharacterHiddenRunAlpha = 0.5f;
        //bpi->CharacterHiddenWalkAlpha = 0.5f;
    }

    BPIForHook* Init(const std::vector<int>& offs) {
#define BPI_SIZE 0xA8
        DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
        typedef PBYTE(*fGetBPIById)(int32_t id);
        fGetBPIById f = (fGetBPIById)((PBYTE)(h + offs[OffsEnum::BPIGetById]));

        BPIForHook* bpi = (BPIForHook*)malloc(sizeof(BPIForHook));
        bpi->origFunc = (PBYTE)(h + offs[OffsEnum::BPIGetById]);
        bpi->backup = (PBYTE*)malloc(sizeof(PBYTE));
        bpi->backup[0] = (PBYTE)malloc(BPI_SIZE);
        memcpy(bpi->backup[0], f(0), BPI_SIZE);

        __changeValue(f(0));

        return bpi;
    }

    void SetDetour(const std::vector<int>& offs, DWORD adrOhHook)
    {
        DWORD h = (DWORD)GetModuleHandleA("CShell.dll");
        DetourFunc((PBYTE)(h + offs[OffsEnum::BPIPatch]), adrOhHook);
    }
};
