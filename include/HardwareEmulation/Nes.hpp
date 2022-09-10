#pragma once

#include <thread>

#include "Cpu.hpp"
#include "Mmu.hpp"
#include "EmuWindows/WindowManager.hpp"

class Nes
{
public:
	Nes();
	void StartNesEmultion();
	~Nes() = default;
private:
	void InsertNewCartridge(std::string file_path);
	std::shared_ptr<Cartridge> _cartridge;
	Cpu _cpu;
	Mmu _mmu;
    WindowManager _wm;
	std::thread _wm_thread;
	bool _run_master_clock;
};
