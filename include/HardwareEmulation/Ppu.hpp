#pragma once

#include <functional>
#include <cstdint>
#include <span>

class Ppu
{
public:
    using WriteFunction = std::function<void (uint16_t address, uint8_t data)>;
    using ReadFunction = std::function<uint8_t (uint16_t address)>;
    Ppu(WriteFunction bus_write, ReadFunction bus_read);

    void writeToRegister(const uint8_t offset, const uint8_t data);

    std::span<const std::tuple<uint8_t,uint8_t,uint8_t>> getPalette();
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

    using PaletteTable = std::array<std::tuple<uint8_t,uint8_t,uint8_t>, 64>;
    struct PatternTableAddr
    {
        uint16_t t :3;
        uint16_t p :1;
        uint16_t c :4;
        uint16_t r :4;
        uint16_t h :1;
    };
    

    void ppuWrite(uint16_t address, uint8_t data);
    uint8_t ppuRead(uint16_t address);

    std::array<uint8_t, 8> _registers;
    std::array<PatternSection, 2> _patternTable;

    // Using a default palette i took from the nesdev wiki
    PaletteTable _paletteTable = {{{84,84,84},{0,30,116},{8,16,144},{48,0,136},{68,0,100},{92,0,48},{84,4,0},{60,24,0},{32,42,0},{8,58,0},{0,64,0},{0,60,0},{0,50,60},{0,0,0},
        {152,150,152},{8,76,196},{48,50,236},{92,30,228},{136,20,176},{160,20,100},{152,34,32},{120,60,0},{84,90,0},{40,114,0},{8,124,0},{0,118,40},{0,102,120},{0,0,0},
        {236,238,236},{76,154,236},{120,124,236},{176,98,236},{228,84,236},{236,88,180},{236,106,100},{212,136,32},{160,170,0},{116,196,0},{76,208,32},{56,204,108},{56,180,204},{60,60,60},
        {236,238,236},{168,204,236},{188,188,236},{212,178,236},{236,174,236},{236,174,212},{236,180,176},{228,196,144},{204,210,120},{180,222,120},{168,226,144},{152,226,180},{160,214,228},{160,162,160}}};
    WriteFunction _busWrite;
    ReadFunction _busRead;
};