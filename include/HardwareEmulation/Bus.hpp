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
    using WriteFunction = std::function<void (const uint16_t address, const uint8_t data)>;
    using ReadFunction = std::function<uint8_t (const uint16_t address)>;
    
    struct phis_translation
    {
        uint32_t offset;
        uint32_t size;
        WriteFunction writeFunc;
        ReadFunction readFunc;
    };

    Bus();

    void busWrite(const uint16_t address, const uint8_t data);

    uint8_t busRead(const uint16_t address);

    void insertCartridge(std::fstream file);

    void removeCartridge();

    std::span<const uint8_t> getRamView() const
    {
        return std::span<const uint8_t>(_ram.begin(), _ram.size());
    }

private:
    friend class Nes;
    static constexpr uint32_t TRUE_RAM_SIZE = 0x800;
    static constexpr uint32_t RAM_MEMORY_RANGE = 0x2000;

    void ramWrite(const uint16_t address, const uint8_t data);
    uint8_t ramRead(const uint16_t address);

    std::list<phis_translation> _memoryMapper;
    std::array<uint8_t, 0x800> _ram;
    std::shared_ptr<Cartridge> _cartridge;
    Cpu _cpu;
    Ppu _ppu;
};