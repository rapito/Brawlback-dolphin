#include "Savestate.h"

#include <Core/HW/Memmap.h>
#include "common/Logging/Log.h"
#include <Common/MemoryUtil.h>
#include <Core/HW/EXI/EXI.h>
#include <thread>

#define LOW_BOUND_MEM 0x80000000


// lots of code here is heavily derived from Slippi's Savestates.cpp

BrawlbackSavestate::BrawlbackSavestate()
{
    // init member list with proper addresses
    initBackupLocs();
    // iterate through address ranges and allocate mem for our savestates
    for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it) {
        auto size = it->endAddress - it->startAddress;
        it->data = static_cast<u8*>(Common::AllocateAlignedMemory(size, 64));
    }
}

BrawlbackSavestate::~BrawlbackSavestate()
{
    for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
    {
        Common::FreeAlignedMemory(it->data);
    }
}


void BrawlbackSavestate::getDolphinState(PointerWrap& p)
{
    // p.DoArray(Memory::m_pRAM, Memory::RAM_SIZE);
    // p.DoMarker("Memory");
    // VideoInterface::DoState(p);
    // p.DoMarker("VideoInterface");
    // SerialInterface::DoState(p);
    // p.DoMarker("SerialInterface");
    // ProcessorInterface::DoState(p);
    // p.DoMarker("ProcessorInterface");
    // DSP::DoState(p);
    // p.DoMarker("DSP");
    // DVDInterface::DoState(p);
    // p.DoMarker("DVDInterface");
    // GPFifo::DoState(p);
    // p.DoMarker("GPFifo");
    ExpansionInterface::DoState(p);
    p.DoMarker("ExpansionInterface");
    // AudioInterface::DoState(p);
    // p.DoMarker("AudioInterface");
}

void BrawlbackSavestate::initBackupLocs()
{
    static std::vector<ssBackupLoc> allMem = {
        {0x805b5160, 0x817da5a0, nullptr},  // all of mem1
        {0x90000800, 0x935e0000, nullptr},  // all of mem2
    };

    static std::vector<ssBackupLoc> fullBackupRegions = {
        // {start address, end address, nullptr},

        // data/bss sections
        
        // 805b8a00
        
        {0x800064E0, 0x800064E0 + 0x3280, nullptr}, // 0
        {0x80009760, 0x80009760 + 0x3100, nullptr}, // 1
        {0x804064E0, 0x804064E0 + 0x300, nullptr}, // 2
        {0x804067E0, 0x804067E0 + 0x20, nullptr}, // 3
        {0x80406800, 0x80406800 + 0x19E80, nullptr}, // 4
        {0x80420680, 0x80420680 + 0x741C0, nullptr}, // 5
        {0x8059C420, 0x8059C420 + 0x3B60, nullptr}, // 6
        {0x805A1320, 0x805A1320 + 0x3E00, nullptr}, // 7 
        
        // data sections 8-10 are size 0

        // bss

        //{0x80494880, 0x8059c41f, nullptr}, // MAIN_uninitialized0
        //{0x8059ff80, 0x805a131f, nullptr}, // MAIN_uninitialized1
        //{0x805a5120, 0x805a5153, nullptr}, // MAIN_uninitialized2

        //{0x80494880, 0x80494880 + 0x1108D4}, // bss  0x80494880 - 0x805A5154  (according to brawlcrate)
        


        {0x80001800, 0x80003000, nullptr}, // default gecko code region
        
        // mem1

        //{0x805b5160, 0x805ca260, nullptr }, // System FW
        {0x80611f60, 0x80673460, nullptr }, // System
        {0x80b8db60, 0x80c23a60, nullptr }, // Effect
        //{0x805d1e60, 0x80611f60, nullptr }, // RenderFifo
        //{0x80c23a60, 0x80da3a60, nullptr }, // InfoResource
        //{0x815edf60, 0x817bad60, nullptr}, // InfoExtraResource
        //{0x80da3a60, 0x80fd6260, nullptr }, // CommonResource
        //{0x81049e60, 0x81061060, nullptr }, // Tmp // ?
        //{0x8154e560, 0x81601960, nullptr }, // Physics
        //{0x81382b60, 0x814ce460, nullptr }, // ItemInstance
        //{0x814ce460, 0x8154e560, nullptr }, // StageInstance // ?
        {0x8123ab60, 0x8128cb60, nullptr }, // Fighter1Instance
        {0x8128cb60, 0x812deb60, nullptr }, // Fighter2Instance
        //{0x812deb60, 0x81330b60, nullptr }, // Fighter3Instance
        //{0x81330b60, 0x81382b60, nullptr }, // Fighter4Instance
        {0x81601960, 0x81734d60, nullptr }, // InfoInstance
        {0x81734d60, 0x817ce860, nullptr }, // MenuInstance
        //{0x80fd6260, 0x81049e60, nullptr }, // MeleeFont
        {0x80673460, 0x80b8db60, nullptr }, // OverlayCommon
        //{0x810f1a60, 0x81162560, nullptr }, // OverlayStage
        //{0x81162560, 0x811aa160, nullptr }, // OverlayMenu
        //{0x81061060, 0x810a9560, nullptr }, // OverlayFighter1
        //{0x810a9560, 0x810f1a60, nullptr }, // OverlayFighter2
        //{0x811aa160, 0x811f2660, nullptr }, // OverlayFighter3
        //{0x811f2660, 0x8123ab60, nullptr }, // OverlayFighter4
        //{0x805ca260, 0x805d1e60, nullptr }, // Thread

        // mem2

        // not 100% sure these are right, and these classes might also use other sections of mem
        // taskScheduler 805b5160 size: 0x178
        // efManager:
        // @ 80611f60 size: 0x13c
        // @ 80b8db60 size: 0x456e0
        // @ 80611f60 size: 0x4c
        // @ 80b8db60 size: 0x6000



        //{0x90199800, 0x90e61400, nullptr }, // Sound
        //{0x90e77500, 0x90fddc00, nullptr }, // Network
        {0x90e61400, 0x90e77500, nullptr }, // WiiPad
        //{0x91018b00, 0x91301b00, nullptr }, // IteamResource
        //{0x91301b00, 0x9134cc00, nullptr }, // Replay
        //{0x92f34700, 0x9359ae00, nullptr }, // StageResoruce
        {0x9151fa00, 0x91a72e00, nullptr }, // Fighter1Resoruce  (crashes without this, but it's so big... need to cut this down)
        {0x91b04c80, 0x92058080, nullptr }, // Fighter2Resoruce
        //{0x920e9f00, 0x9263d300, nullptr }, // Fighter3Resoruce
        //{0x926cf180, 0x92c22580, nullptr }, // Fighter4Resoruce
        //{0x91a72e00, 0x91b04c80, nullptr }, // Fighter1Resoruce2
        //{0x92058080, 0x920e9f00, nullptr }, // Fighter2Resoruce2
        //{0x9263d300, 0x926cf180, nullptr }, // Fighter3Resoruce2
        //{0x92c22580, 0x92cb4400, nullptr }, // Fighter4Resoruce2
        {0x91478e00, 0x914d2900, nullptr }, // FighterEffect
        {0x92cb4400, 0x92dcdf00, nullptr }, // FighterTechqniq
        //{0x914d2900, 0x914ec400, nullptr }, // FighterKirbyResource1
        //{0x914ec400, 0x91505f00, nullptr }, // FighterKirbyResource2
        //{0x91505f00, 0x9151fa00, nullptr }, // FighterKirbyResource3
        //{0x92dcdf00, 0x92e34600, nullptr }, // AssistFigureResource
        //{0x9359ae00, 0x935ce200, nullptr }, // ItemExtraResource
        //{0x92e34600, 0x92f34700, nullptr }, // PokemonResource
        {0x9134cc00, 0x91478e00, nullptr }, // CopyFB
        {0x90167400, 0x90199800, nullptr }, // GameGlobal
        //{0x90fddc00, 0x91018b00, nullptr }, // GlobalMode
    };



    // wip
    static std::vector<PreserveBlock> excludeSections = {
        // {start address, size}

        {0x935d7660, 0x000089a0}, // CPP Framework heap (subject to change...??)

        // {0x805bacc0+0x40, 0x40*4} // gfPadSystem

        {0x80663e00, 0x1a4}, // CameraController
        {0x80663b40, 0x198}, // cmAiController
        {0x805b6d20, 0x740}, // gfCameraManager
    };

    //SlippiInitBackupLocations(this->backupLocs, allMem, excludeSections);
    SlippiInitBackupLocations(this->backupLocs, fullBackupRegions, excludeSections);
  
    static bool once = true;
    if (once) {
        u64 totalsize = 0;
        for (auto& loc : this->backupLocs) {
            u32 size = loc.endAddress-loc.startAddress;
            INFO_LOG(BRAWLBACK, "Savestate region: %p - %p : size 0x%x\n", loc.startAddress, loc.endAddress, size);
            totalsize += size;
        }
        INFO_LOG(BRAWLBACK, "Savestates total size: 0x%llx\n", totalsize);
    }
    once = false;
}

typedef std::vector<SlippiUtility::Savestate::ssBackupLoc>::iterator backupLocIterator;

void captureMemRegions(backupLocIterator start, backupLocIterator end) {
    for (auto it = start; it != end; ++it) {
        auto size = it->endAddress - it->startAddress;
        Memory::CopyFromEmu(it->data, it->startAddress, size);  // game -> emu
    }
}

void BrawlbackSavestate::Capture()
{
    captureMemRegions(backupLocs.begin(), backupLocs.end());

    /*
    // copy game mem
    for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
    {
        auto size = it->endAddress - it->startAddress;
        // since no addresses will be lower than this, if this is the case, the backupLoc endAddress
        // represents a size rather than an end address
        //if (it->endAddress < LOW_BOUND_MEM) 
        //{
        //    Memory::CopyFromEmu(it->data, it->startAddress, it->endAddress);
        //}
        //else
        //{
            Memory::CopyFromEmu(it->data, it->startAddress, size);  // game -> emu
        //}
    }
    */

    // copy dolphin states
    //u8 *ptr = &dolphinSsBackup[0];
    //PointerWrap p(&ptr, PointerWrap::MODE_WRITE);
    //getDolphinState(p);

}

void BrawlbackSavestate::Load(std::vector<PreserveBlock> blocks)
{

    // Back up regions of game that should stay the same between savestates

    for (auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        if (!preservationMap.count(*it)) // if this PreserveBlock is NOT in our preservationMap
        {
            // TODO: Clear preservation map when game ends
            preservationMap[*it] = std::vector<u8>(it->length); // init new entry at this PreserveBlock key
        }

        Memory::CopyFromEmu(&preservationMap[*it][0], it->address, it->length);
    }


    // Restore memory blocks
    for (auto it = backupLocs.begin(); it != backupLocs.end(); ++it)
    {
        auto size = it->endAddress - it->startAddress;
        //if (it->endAddress < LOW_BOUND_MEM)
        //{
        //    Memory::CopyToEmu(it->startAddress, it->data, it->endAddress);  // emu -> game
        //}
        //else
        //{
            Memory::CopyToEmu(it->startAddress, it->data, size);  // emu -> game
        //}
    }

    //// Restore audio
    //u8 *ptr = &dolphinSsBackup[0];
    //PointerWrap p(&ptr, PointerWrap::MODE_READ);
    //getDolphinState(p);

    // Restore preservation blocks
    for (auto it = blocks.begin(); it != blocks.end(); ++it)
    {
        Memory::CopyToEmu(it->address, &preservationMap[*it][0], it->length);
    }
  

}
