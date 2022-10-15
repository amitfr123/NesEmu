#include <iostream>

#include "HardwareEmulation/Bus.hpp"

Bus::Bus() : 
    _cpu(std::bind(&Bus::cpuWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::cpuRead, this, std::placeholders::_1)),
    _ppu(std::bind(&Bus::ppuWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::ppuRead, this, std::placeholders::_1, std::placeholders::_2))
{
    std::fill(std::begin(_ram), std::end(_ram), 0);
    _memoryMapper.emplace_front(phis_translation({0, RAM_MEMORY_RANGE, std::bind(&Bus::ramWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Bus::ramRead, this, std::placeholders::_1)}));
    _memoryMapper.emplace_front(phis_translation({0x2000, 0x2000, std::bind(&Ppu::writeToRegister, &_ppu, std::placeholders::_1, std::placeholders::_2), std::bind(&Ppu::readFromRegister, &_ppu, std::placeholders::_1)}));
}

std::span<const uint8_t> Bus::getRamView() const
{
    return std::span<const uint8_t>(_ram.begin(), _ram.size());
}

void Bus::cpuWrite(uint16_t address, uint8_t data)
{
    if (_cartridge.get() != nullptr && _cartridge->cpuWrite(address, data))
    {
        
    }
    else
    {
        for (auto &target : _memoryMapper)
        {  
            if (target.offset + target.size > address && address >= target.offset)
            {
                target.writeFunc(address, data);
            }
        }
    }
}

uint8_t Bus::cpuRead(uint16_t address)
{
    uint8_t data = 0;
    if (_cartridge.get() != nullptr && _cartridge->cpuRead(address, data))
    {
    }
    else
    {
        for (auto &target : _memoryMapper)
        {  
            if (target.offset + target.size > address && address >= target.offset)
            {
                return target.readFunc(address);
            }
        }
    }
    return data;
}

void Bus::insertCartridge(std::fstream file)
{
    _cartridge = std::make_shared<Cartridge>(std::move(file));
    _ppu.setMirroringMode(_cartridge->getMirroringMode());
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

void Bus::ramWrite(uint16_t address, uint8_t data)
{
    _ram[address % TRUE_RAM_SIZE] = data;
}

uint8_t Bus::ramRead(uint16_t address)
{
    return _ram[address % TRUE_RAM_SIZE];
}

bool Bus::ppuWrite(uint16_t address, uint8_t data)
{
    if (_cartridge.get() != nullptr && _cartridge->ppuWrite(address, data))
    {
        return true;
    }
    return false;
}

bool Bus::ppuRead(uint16_t address, uint8_t& data)
{
    if (_cartridge.get() != nullptr && _cartridge->ppuRead(address, data))
    {
        return true;
    }
    return false;
}