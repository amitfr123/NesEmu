#include "HardwareEmulation/Mappers/Mapper0.hpp"

bool Mapper0::cpuWriteMap(uint16_t address, uint8_t data)
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        // write to ram in family basic
        // TODO: think about impl it once i know what it isd
        return true;
    }
    if (address >= 0x8000 && address <= 0xffff)
    {
        // My roms are all read only until ill need to change it
        return true;
    }
    return false;
}

bool Mapper0::cpuReadMap(uint16_t address, uint8_t& data)
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        // read from ram in family basic
        // TODO: think about impl it once i know what it isd
        data = 0;
        return true;
    }
    if (address >= 0x8000 && address <= 0xffff)
    {
        data = _prgRomBankVector[((_prgRomBankVector.size() > 1 && address >= 0xc000)? 1 : 0)][((address - 0x8000) % 0x4000)];
        // My roms are all read only until ill need to change it
        return true;
    }
    return false;
}

bool Mapper0::ppuWriteMap(uint16_t address, uint8_t data)
{
    if (address >= 0 && address <= 0x1fff)
    {
        _chrRomBankVector[0][address] = data;
        return true;
    }
    return false;
}
bool Mapper0::ppuReadMap(uint16_t address, uint8_t& data)
{
    if (address >= 0 && address <= 0x1fff)
    {
        data = _chrRomBankVector[0][address];
        return true;
    }
    return false;
}