#pragma once

#include <functional>
#include <cstdint>

class Ppu
{
public:
    using WriteFunction = std::function<void (const uint16_t address, const uint8_t data)>;
    using ReadFunction = std::function<uint8_t (const uint16_t address)>;
    Ppu(WriteFunction bus_write, ReadFunction bus_read);

    void writeToRegister(const uint8_t offset, const uint8_t data);
private:
    static constexpr uint8_t PPU_CTRL_OFFSET = 0; // W
    static constexpr uint8_t PPU_MASK_OFFSET = 1; // W
    static constexpr uint8_t PPU_STATUS_OFFSET = 2; // R
    static constexpr uint8_t PPU_OAM_ADDR_OFFSET = 3; // W
    static constexpr uint8_t PPU_OAM_DATA_OFFSET = 4; // WR
    static constexpr uint8_t PPU_SCROLL_OFFSET = 5; // W
    static constexpr uint8_t PPU_ADDR_OFFSET = 6; // W
    static constexpr uint8_t PPU_DATA_OFFSET = 7; //W

    // CTRL
    static constexpr uint8_t PPU_CTRL_NAME_TABLE_ADDR_MASK = 0x03;
    static constexpr uint8_t PPU_CTRL_VRAM_INC_MASK = 0x04;
    static constexpr uint8_t PPU_CTRL_SPRITE_TABLE_ADDR_MASK = 0x08;
    static constexpr uint8_t PPU_CTRL_SPRITE_SIZE_MASK = 0x10;
    static constexpr uint8_t PPU_CTRL_BACKGROUND_TABLE_ADDR_MASK = 0x20;
    static constexpr uint8_t PPU_CTRL_MASTER_SLAVE_SELECT_MASK = 0x40;
    static constexpr uint8_t PPU_CTRL_GEN_NMI_MASK = 0x80;

    // MASK
    static constexpr uint8_t PPU_GRAYSCALE_MASK = 0x01;
    static constexpr uint8_t PPU_SHOW_BACKGROUND_LEFT_8_MASK = 0x02;
    static constexpr uint8_t PPU_SHOW_SPRITE_LEFT_8_MASK = 0x04;
    static constexpr uint8_t PPU_SHOW_BACKGROUND_MASK = 0x08;
    static constexpr uint8_t PPU_SHOW_SPRITE_MASK = 0X10;
    static constexpr uint8_t PPU_EMPHASIZE_RED_MASK = 0X20;
    static constexpr uint8_t PPU_EMPHASIZE_GREEN_MASK = 0X40;
    static constexpr uint8_t PPU_EMPHASIZE_BLUE_MASK = 0X80;

    // STATUS
    static constexpr uint8_t PPU_OPEN_BUS_MASK = 0x1F;
    static constexpr uint8_t PPU_SPRITE_OVERFLOW_MASK = 0x20;
    static constexpr uint8_t PPU_SPRITE_HIT_0_MASK = 0X40;
    static constexpr uint8_t PPU_VERTICAL_BANK_MASK = 0X80;

    using TilePlane = std::array<uint8_t, 8>;
    using Tile = std::array<TilePlane, 2>;
    using PatternSection = std::array<std::array<TilePlane, 2>, 256>;

    struct PatternTableAddr
    {
        uint16_t t :3;
        uint16_t p :1;
        uint16_t c :4;
        uint16_t r :4;
        uint16_t h :1;
    };
    

    void ppuWrite(const uint16_t address, const uint8_t data);
    uint8_t ppuRead(const uint16_t address);

    std::array<uint8_t, 8> _registers;
    std::array<PatternSection, 2> _patternTable;
    WriteFunction _busWrite;
    ReadFunction _busRead;

    // 0 - PPUCTRL
    // 1 - PPUMASK
    // 2 - PPUSTATUS
    // 3 - OAMADDR
    // 4 - OAMDATA
    // 5 - PPUSCROLL
    // 6 - PPUADDR
    // 7 - PPUDATA
};