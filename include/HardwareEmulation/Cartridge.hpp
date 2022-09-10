#pragma once

#include <fstream>
#include <memory>

#include "Mappers/Mapper.hpp"

class Cartridge {
public:
    Cartridge(std::fstream cartridge_file);

    void CpuWrite(const uint16_t address, const uint8_t data);

    uint8_t CpuRead(const uint16_t address);
private:
    struct CartridgeHeader
    {
      char nes_name[4];
      uint8_t prg_rom_bank_count;
      uint8_t chr_rom_bank_count;
      uint8_t flags6; // Mapper, mirroring, battery, trainer
      uint8_t flags7; // Mapper, VS/Playchoice, NES 2.0
      uint8_t flags8; // PRG-RAM size
      uint8_t flags9; // TV system
      uint8_t flags10; // TV system, PRG-RAM presence
      char reserved[5];
    };

    std::shared_ptr<Mapper> _mapper;
    CartridgeHeader _cartridge_header;
};