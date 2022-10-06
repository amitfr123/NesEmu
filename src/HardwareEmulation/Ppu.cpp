
#include "HardwareEmulation/Ppu.hpp"

Ppu::Ppu(WriteFunction bus_write, ReadFunction bus_read) :
    _busRead(std::move(bus_read)), _busWrite(std::move(bus_write))
{
}

void Ppu::ppuWrite(const uint16_t address, const uint8_t data)
{
    
    if (address >= 0 && address <= 0x1fff)
    {
        PatternTableAddr addr(address);
        _patternTable[addr.h][((addr.r << 4) | (addr.c))][addr.p][addr.t] = data;
    }
}

uint8_t Ppu::ppuRead(const uint16_t address)
{
    uint8_t data;
    if (address >= 0 && address <= 0x1fff)
    {
        PatternTableAddr addr(address);
        data = _patternTable[addr.h][((addr.r << 4) | (addr.c))][addr.p][addr.t];
    }
    return data;
}