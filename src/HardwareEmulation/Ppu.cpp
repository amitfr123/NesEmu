#include <fstream>
#include "HardwareEmulation/Ppu.hpp"

Ppu::Ppu(WriteFunction bus_write, ReadFunction bus_read) :
    _busRead(std::move(bus_read)), _busWrite(std::move(bus_write))
{
    // TODO: bring a different palette file cuz this file is super dark
    
    std::ifstream pFile(std::string(RESOURCE_PATH) + "/palettes/NES_Classic.pal", std::ios::binary);
    std::array<char, 3> pBuffer;
    size_t i = 0;
    for (;i < 64 && !pFile.read(pBuffer.data(), pBuffer.size()).eof(); i++)
    {
        _paletteTable[i % 64] = {pBuffer[0], pBuffer[1], pBuffer[2]};
    }
    pFile.close();
    if (i < 64)
    {
        throw std::runtime_error("Palette file is too small");
    }
    
}

std::span<const std::tuple<uint8_t,uint8_t,uint8_t>> Ppu::getPalette()
{
    return std::span<std::tuple<uint8_t,uint8_t,uint8_t>>(_paletteTable.begin(), _paletteTable.size());
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