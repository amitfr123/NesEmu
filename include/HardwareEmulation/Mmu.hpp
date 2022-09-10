#pragma once

#include <array>
#include <list>
#include <functional>
#include <memory>
#include <stdint.h>

#include "Cartridge.hpp"

class Mmu {
public:
    using WriteFunction = std::function<void (const uint16_t address, const uint8_t data)>;
    using ReadFunction = std::function<uint8_t (const uint16_t address)>;
    
    struct phis_transltion
    {
        uint32_t offset;
        uint32_t size;
        WriteFunction writeFunc;
        ReadFunction readFunc;
    };

    Mmu();

    void MmuWrite(const uint16_t address, const uint8_t data);

    uint8_t MmuRead(const uint16_t address);

    void InsertCartridge(std::shared_ptr<Cartridge> cartridge);

    void RemoveCartridge();

    const std::array<uint8_t, 0x800>& GetRam()
    {
        return _ram;
    }

    ~Mmu() = default;
private:
    static constexpr uint32_t TRUE_RAM_SIZE = 0x800;
    static constexpr uint32_t RAM_MEMORY_RANGE = 0x2000;

    void RamWrite(const uint16_t address, const uint8_t data);
    uint8_t RamRead(const uint16_t address);

    std::list<phis_transltion> _memoryMapper;
    std::array<uint8_t, 0x800> _ram;
    std::shared_ptr<Cartridge> _cartridge;
};