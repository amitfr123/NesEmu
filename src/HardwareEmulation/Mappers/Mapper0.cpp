#include "HardwareEmulation/Mappers/Mapper0.hpp"

void Mapper0::CpuWriteMap(const uint16_t address, const uint8_t data) 
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        // write to ram in family basic
        // TODO: think about impl it once i know what it isd
    }
    if (address >= 0x8000 && address <= 0xffff)
    {
        // My roms are all read only until ill need to change it
    }
}

uint8_t Mapper0::CpuReadMap(const uint16_t address) 
{
    if (address >= 0x6000 && address <= 0x7fff)
    {
        // read from ram in family basic
        // TODO: think about impl it once i know what it isd
    }
    if (address >= 0x8000 && address <= 0xffff)
    {
        // My roms are all read only until ill need to change it
        return _prg_rom_bank_vector[((_prg_rom_bank_vector.size() > 1 && address >= 0xc000)? 1 : 0)][((address - 0x8000) % 0x4000)];
    }
    return 0;
}