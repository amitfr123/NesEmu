#include <iostream>
#include <fstream>

#include "HardwareEmulation/Nes.hpp"

#include "EmuWindows/FileLoadingWindow.hpp"
#include "EmuWindows/MemoryWindow.hpp"

#include <unistd.h>

Nes::Nes() :
    _mmu(),
    _cpu(std::bind(&Mmu::MmuWrite, &_mmu, std::placeholders::_1, std::placeholders::_2), std::bind(&Mmu::MmuRead, &_mmu, std::placeholders::_1))
{
    _wm.AddNewWindow(std::make_shared<FileLoadingWindow>(std::bind(&Nes::InsertNewCartridge, this ,std::placeholders::_1)));
    _wm.AddNewWindow(std::make_shared<MemoryWindow>(std::bind(&Mmu::GetRam, &_mmu)));
    _run_master_clock = false;
}

void Nes::StartNesEmultion()
{
    bool run_flag = true;
    _wm_thread = std::thread([&]()
    {
        _wm.EmuWindowManagerEventLoop();
        run_flag = false;
    });
    _wm_thread.detach();

    #ifdef NESTEST_DEBUG
    Nes::InsertNewCartridge(NESTEST_ROM_PATH);
    #endif // NESTEST_DEBUG

    while (run_flag)
    {
        if (_run_master_clock)
        {
            _cpu.CpuExecuteInstruction();
        }
    }
}

void Nes::InsertNewCartridge(std::string file_path)
{
    std::fstream file(file_path, std::fstream::in | std::fstream::out | std::fstream::binary);
    // stop cpu
    // window stop
    if (_cartridge.get() != nullptr)
    {
        _mmu.RemoveCartridge();

        _cartridge.reset();
    }
    _cartridge = std::make_shared<Cartridge>(std::move(file));
    _mmu.InsertCartridge(_cartridge);
    auto diss = _cpu.Disassemble();
    // replace with dissasmbly window
    std::ofstream fs(DISSASMBLY_LOG_PATH);
    for (auto line = diss.begin(); line != diss.end(); ++line)
    {  
        //std::cout << line->second << std::endl;
        fs << line->second << std::endl;
    }
    fs.close();
    _cpu.CpuReset();
    _run_master_clock = true;
    // reset cpu
    // cpu dissasmble
    // window start
    // cpu start
}