#include <iostream>

#include "HardwareEmulation/Bus.hpp"

Bus::Bus() : 
    _cpu(std::bind(&Bus::busWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::busRead, this, std::placeholders::_1)),
    _ppu(std::bind(&Bus::busWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::busRead, this, std::placeholders::_1))
{
    std::fill(std::begin(_ram), std::end(_ram), 0);
    _memoryMapper.emplace_front(phis_translation({0, RAM_MEMORY_RANGE, std::bind(&Bus::ramWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::ramRead, this, std::placeholders::_1)}));
}

std::span<const uint8_t> Bus::getRamView() const
{
    return std::span<const uint8_t>(_ram.begin(), _ram.size());
}


void Bus::busWrite(const uint16_t address, const uint8_t data)
{
    for (auto &target : _memoryMapper)
    {  
        if (target.offset + target.size > address && address >= target.offset)
        {
            target.writeFunc(address, data);
        }
    }
}

uint8_t Bus::busRead(const uint16_t address)
{
    for (auto &target : _memoryMapper)
    {  
        if (target.offset + target.size > address && address >= target.offset)
        {
            return target.readFunc(address);
        }
    }
    return 0; // If we reach here read operation failed
}

void Bus::insertCartridge(std::fstream file)
{
    _cartridge = std::make_shared<Cartridge>(std::move(file));
    _memoryMapper.emplace_front(phis_translation({0x6000, 0xffff - 0x6000, std::bind(&Cartridge::CpuWrite, _cartridge, std::placeholders::_1, std::placeholders::_2), std::bind(&Cartridge::CpuRead, _cartridge, std::placeholders::_1)}));
}

void Bus::removeCartridge()
{
    if (_cartridge.get() != nullptr)
    {
        // The cartridge will alwase be at the front beacause it is loaded after the mmu construction and "hot swapable"
        _memoryMapper.pop_front();
        _cartridge.reset();
    }
}

void Bus::ramWrite(const uint16_t address, const uint8_t data)
{
    _ram[address % TRUE_RAM_SIZE] = data;
}

uint8_t Bus::ramRead(const uint16_t address)
{
    return _ram[address % TRUE_RAM_SIZE];
}