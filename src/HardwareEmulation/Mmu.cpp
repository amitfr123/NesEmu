#include <iostream>

#include "HardwareEmulation/Mmu.hpp"

Mmu::Mmu()
{
    std::fill(std::begin(_ram), std::end(_ram), 0);
    _memoryMapper.emplace_front(phis_transltion({0, RAM_MEMORY_RANGE, std::bind(&Mmu::RamWrite, this, std::placeholders::_1, std::placeholders::_2), std::bind(&Mmu::RamRead, this, std::placeholders::_1)}));
}

void Mmu::MmuWrite(const uint16_t address, const uint8_t data)
{
    for (auto &target : _memoryMapper)
    {  
        if (target.offset + target.size > address && address >= target.offset)
        {
            target.writeFunc(address, data);
        }
    }
}

uint8_t Mmu::MmuRead(const uint16_t address)
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

void Mmu::InsertCartridge(std::shared_ptr<Cartridge> cartridge)
{
    _cartridge = cartridge;
    _memoryMapper.emplace_front(phis_transltion({0x6000, 0xffff - 0x6000, std::bind(&Cartridge::CpuWrite, _cartridge, std::placeholders::_1, std::placeholders::_2), std::bind(&Cartridge::CpuRead, _cartridge, std::placeholders::_1)}));
}

void Mmu::RemoveCartridge()
{
    if (_cartridge.get() != nullptr)
    {
        // The cartridge will alwase be at the front beacause it is loaded after the mmu construction and "hot swapable"
        _memoryMapper.pop_front();
    }
    _cartridge.reset();
}

void Mmu::RamWrite(const uint16_t address, const uint8_t data)
{
    _ram[address % TRUE_RAM_SIZE] = data;
}

uint8_t Mmu::RamRead(const uint16_t address)
{
    return _ram[address % TRUE_RAM_SIZE];
}