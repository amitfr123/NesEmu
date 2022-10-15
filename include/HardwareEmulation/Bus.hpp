#pragma once

#include <array>
#include <list>
#include <functional>
#include <memory>
#include <stdint.h>
#include <span>

#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Cartridge.hpp"

class Bus {
public:
    using WriteFunction = std::function<void (uint16_t address, uint8_t data)>;
    using ReadFunction = std::function<uint8_t (uint16_t address)>;
    
    struct phis_translation
    {
        uint32_t offset;
        uint32_t size;
        WriteFunction writeFunc;
        ReadFunction readFunc;
    };

    Bus();

    void insertCartridge(std::fstream file);

    void removeCartridge();

    std::span<const uint8_t> getRamView() const;

private:
    friend class Nes;
    static constexpr uint32_t TRUE_RAM_SIZE = 0x800;
    static constexpr uint32_t RAM_MEMORY_RANGE = 0x2000;

    void cpuWrite(uint16_t address, uint8_t data);
    uint8_t cpuRead(uint16_t address);

    bool ppuWrite(uint16_t address, uint8_t data);
    bool ppuRead(uint16_t address, uint8_t& data);

    void ramWrite(uint16_t address, uint8_t data);
    uint8_t ramRead(uint16_t address);

    std::list<phis_translation> _memoryMapper;
    std::array<uint8_t, 0x800> _ram;
    std::shared_ptr<Cartridge> _cartridge;
    Cpu _cpu;
    Ppu _ppu;
};