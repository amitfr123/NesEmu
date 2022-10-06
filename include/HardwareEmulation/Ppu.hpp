#pragma once

class Ppu
{
public:
    using WriteFunction = std::function<void (const uint16_t address, const uint8_t data)>;
    using ReadFunction = std::function<uint8_t (const uint16_t address)>;
    Ppu(WriteFunction mmu_write, ReadFunction mmu_read);

    
private:
    /* data */
};