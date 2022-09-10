#include <stdexcept>

#include "HardwareEmulation/Cartridge.hpp"

#include "HardwareEmulation/Mappers/Mapper0.hpp"

Cartridge::Cartridge(std::fstream cartridge_file)
{
    uint8_t mapper_id;
    Mapper::PrgRomBankMapper prg_rom_bank;
    Mapper::ChrRomBankMapper chr_rom_bank;

    if (!cartridge_file.is_open())
    {
        throw std::runtime_error("Failed to read cartridge_file");
    }
    cartridge_file.read(reinterpret_cast<char*>(&_cartridge_header), sizeof(_cartridge_header));
    prg_rom_bank.resize(_cartridge_header.prg_rom_bank_count);
    chr_rom_bank.resize(_cartridge_header.chr_rom_bank_count);
    mapper_id = (_cartridge_header.flags7 & 0xf0) | ((_cartridge_header.flags6 & 0xf0) >> 4);
    if (_cartridge_header.flags6 % 0x4)
    {
        cartridge_file.seekg(512, std::ios_base::cur);
    }
    for (uint32_t i = 0; i < _cartridge_header.prg_rom_bank_count; ++i)
    {
        cartridge_file.read(reinterpret_cast<char*>(prg_rom_bank[i].data()), prg_rom_bank[i].size());
    }
    for (uint32_t i = 0; i < _cartridge_header.chr_rom_bank_count; ++i)
    {
        cartridge_file.read(reinterpret_cast<char*>(chr_rom_bank[i].data()), chr_rom_bank[i].size());
    }
    cartridge_file.close();
    switch (mapper_id)
    {
    case 0:
        _mapper = std::make_shared<Mapper0>(std::move(prg_rom_bank), std::move(chr_rom_bank));
        break;
    default:
        break;
    }
}

void Cartridge::CpuWrite(const uint16_t address, const uint8_t data)
{
    _mapper->CpuWriteMap(address, data);
}

uint8_t Cartridge::CpuRead(const uint16_t address)
{
    return _mapper->CpuReadMap(address);
}