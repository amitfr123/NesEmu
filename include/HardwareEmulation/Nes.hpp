#pragma once

#include <thread>

#include "Cpu.hpp"
#include "Bus.hpp"
#include "Ppu.hpp"
#include "EmuWindows/WindowManager.hpp"

class Nes
{
public:
	Nes();
	void StartNesEmulation();
	~Nes() = default;
private:
	void InsertNewCartridge(std::string file_path);
	Bus _bus;
    WindowManager _wm;
	std::thread _wmThread;
	bool _runMasterClock;
};
