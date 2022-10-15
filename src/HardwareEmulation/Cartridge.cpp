#include <stdexcept>

#include "HardwareEmulation/Cartridge.hpp"

#include "HardwareEmulation/Mappers/Mapper0.hpp"

Cartridge::Cartridge(std::fstream cartridge_file)
{
    uint8_t mapperId;
    Mapper::PrgRomBankMapper prgRomBank;
    Mapper::ChrRomBankMapper chrRomBank;

    if (!cartridge_file.is_open())
    {
        throw std::runtime_error("Failed to read cartridge_file");
    }
    cartridge_file.read(reinterpret_cast<char*>(&_cartridgeHeader), sizeof(_cartridgeHeader));
    prgRomBank.resize(_cartridgeHeader.prg_rom_bank_count);
    chrRomBank.resize(_cartridgeHeader.chr_rom_bank_count);
    mapperId = (_cartridgeHeader.flags7 & 0xf0) | ((_cartridgeHeader.flags6 & 0xf0) >> 4);
    if (_cartridgeHeader.flags6 & 0x4)
    {
        cartridge_file.seekg(512, std::ios_base::cur);
    }
    for (uint32_t i = 0; i < _cartridgeHeader.prg_rom_bank_count; ++i)
    {
        cartridge_file.read(reinterpret_cast<char*>(prgRomBank[i].data()), prgRomBank[i].size());
    }
    for (uint32_t i = 0; i < _cartridgeHeader.chr_rom_bank_count; ++i)
    {
        cartridge_file.read(reinterpret_cast<char*>(chrRomBank[i].data()), chrRomBank[i].size());
    }
    cartridge_file.close();
    switch (mapperId)
    {
    case 0:
        _mapper = std::make_shared<Mapper0>(std::move(prgRomBank), std::move(chrRomBank));
        break;
    default:
        break;
    }
}

bool Cartridge::cpuWrite(uint16_t address, uint8_t data)
{
    return _mapper->cpuWriteMap(address, data);
}

bool Cartridge::cpuRead(uint16_t address, uint8_t& data)
{
    return _mapper->cpuReadMap(address, data);
}

bool Cartridge::ppuWrite(uint16_t address, uint8_t data)
{
    return _mapper->ppuWriteMap(address, data);
}

bool Cartridge::ppuRead(uint16_t address, uint8_t& data)
{
    return _mapper->ppuReadMap(address, data);
}

uint8_t Cartridge::getMirroringMode()
{
    uint8_t mode;
    if (_cartridgeHeader.flags6 & 0x4)
    {
        mode = 0;
    }
    else if (!(_cartridgeHeader.flags6 & 0x1))
    {
        mode = 1;
    }
    else
    {
        mode = 2;
    }
    return mode;
}