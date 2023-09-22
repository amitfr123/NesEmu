#include <iostream>
#include <fstream>

#include "HardwareEmulation/Nes.hpp"

#include "EmuWindows/FileLoadingWindow.hpp"
#include "EmuWindows/MemoryWindow.hpp"
#include "EmuWindows/PaletteWindow.hpp"
#include "EmuWindows/PatternWindow.hpp"

#include <unistd.h>

Nes::Nes() :
    _bus()
{
    _wm.AddNewWindow(std::make_shared<FileLoadingWindow>(std::bind(&Nes::InsertNewCartridge, this ,std::placeholders::_1)));
    _wm.AddNewWindow(std::make_shared<MemoryWindow>(_bus.getRamView()));
    //_wm.AddNewWindow(std::make_shared<PaletteWindow>(_bus._ppu.getPalette(), std::bind(&Ppu::getWorkPaletteRgb, &(_bus._ppu),std::placeholders::_1)));
    _screen = std::make_shared<ScreenWindow>(_bus._ppu.getScreen());
    _wm.AddNewWindow(_screen);
    //_wm.AddNewWindow(std::make_shared<PatternWindow>(std::bind(&Ppu::getPatternTable, &(_bus._ppu),std::placeholders::_1)));
    _runMasterClock = false;
}

void Nes::StartNesEmulation()
{
    bool run_flag = true;
    uint32_t clc = 0;
    _wmThread = std::thread([&]()
    {
        _wm.EmuWindowManagerEventLoop();
        run_flag = false;
    });
    _wmThread.detach();

    //#ifdef NESTEST_DEBUG
    //Nes::InsertNewCartridge(std::string(RESOURCE_PATH) + "/nestest_rom/nestest.nes");
    //Nes::InsertNewCartridge(std::string(RESOURCE_PATH) + "/nestest_rom/smb.nes");
    Nes::InsertNewCartridge(std::string(RESOURCE_PATH) + "/nestest_rom/DK.nes");
    //Nes::InsertNewCartridge("/home/a/Desktop/smb.nes");
    //#endif // NESTEST_DEBUG

    while (run_flag)
    {
        if (_runMasterClock)
        {
            _bus._ppu.clock();
            if (clc % 3 == 0)
            {
                _bus._cpu.cpuExecuteInstruction();
            }
            if (_bus._ppu.nmi)
            {
                _bus._ppu.nmi = false;
                _bus._cpu.Nmi();
            }
            clc++;
        }
    }
}

void Nes::InsertNewCartridge(std::string file_path)
{
    std::fstream file(file_path, std::fstream::in | std::fstream::out | std::fstream::binary);
    // stop cpu
    // window stop
    _bus.removeCartridge();
    _bus.insertCartridge(std::move(file));
    
    auto diss = _bus._cpu.disassemble();
    // replace w_bus.ith dissasmbly window
    std::ofstream fs(DISSASMBLY_LOG_PATH);
    for (auto line = diss.begin(); line != diss.end(); ++line)
    {  
        //std::cout << line->second << std::endl;
        fs << line->second << std::endl;
    }
    fs.close();
    _bus._cpu.cpuReset();
    _runMasterClock = true;
    // reset cpu
    // cpu dissasmble
    // window start
    // cpu start
}