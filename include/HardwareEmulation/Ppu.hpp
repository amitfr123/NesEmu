#pragma once

#include <functional>
#include <cstdint>
#include <span>
#include <vector>

class Ppu
{
public:
    using WriteFunction = std::function<bool (uint16_t address, uint8_t data)>;
    using ReadFunction = std::function<bool (uint16_t address, uint8_t& data)>;

    Ppu(WriteFunction bus_write, ReadFunction bus_read);

    void writeToRegister(uint16_t address, uint8_t data);
    uint8_t readFromRegister(uint16_t address);

    void executeCycle();

    bool getNmiStatus();
    void clearNmiStatus();

    void setMirroringMode(uint8_t mode);

    void reset();

    std::span<const uint32_t> getPalette();
    std::array<std::array<uint32_t, 0x4000>, 2> getPatternTable(uint8_t paletteId);
    std::array<uint32_t, 4> getWorkPaletteRgb(uint8_t paletteId);
    std::span<const uint32_t> getScreen();
private:
    static constexpr uint8_t PPU_CTRL_OFFSET = 0; // W
    static constexpr uint8_t PPU_MASK_OFFSET = 1; // W
    static constexpr uint8_t PPU_STATUS_OFFSET = 2; // R
    static constexpr uint8_t PPU_OAM_ADDR_OFFSET = 3; // W
    static constexpr uint8_t PPU_OAM_DATA_OFFSET = 4; // WR
    static constexpr uint8_t PPU_SCROLL_OFFSET = 5; // W
    static constexpr uint8_t PPU_ADDR_OFFSET = 6; // W
    static constexpr uint8_t PPU_DATA_OFFSET = 7; //WR

    static constexpr uint16_t PPU_PATTERN_ADDR_START = 0; //WR
    static constexpr uint16_t PPU_PATTERN_ADDR_END = 0X1FFF; //WR

    static constexpr uint16_t PPU_NAME_TABLE_ADDR_START = 0X2000; //WR
    static constexpr uint16_t PPU_NAME_TABLE_ADDR_END = 0X2fff; //WR
    static constexpr uint16_t PPU_NAME_TABLE_ADDR_M_START = 0X3000; //WR
    static constexpr uint16_t PPU_NAME_TABLE_ADDR_M_END = 0X3eff; //WR
    static constexpr uint16_t PPU_NAME_TABLE_SIZE = 0X400; //WR

    static constexpr uint16_t PPU_PALETTE_ADDR_START = 0X3F00; //WR
    static constexpr uint16_t PPU_PALETTE_ADDR_END = 0X3FFF; //WR

    static constexpr uint16_t PPU_ATTRIBUTE_TABLE_OFFSET = 0x3c0; //WR

    using TilePlane = std::array<uint8_t, 8>;
    using Tile = std::array<TilePlane, 2>;
    using PatternSection = std::array<std::array<TilePlane, 2>, 256>;
    using PatternTable = std::array<PatternSection, 2>;

    using PaletteTable = std::array<uint32_t, 64>;

    using WorkPaletteSet = std::array<uint8_t, 32>;

    using NameTable = std::array<uint8_t, PPU_NAME_TABLE_SIZE>;

    union PatternTableAddr
    {
        struct
        {
            uint16_t t :3;
            uint16_t p :1;
            uint16_t c :4;
            uint16_t r :4;
            uint16_t h :1;
        };
        uint16_t data;
    };
    

    union InternalVramRegister
    {
        struct
        {
            uint16_t coarseX :5;
            uint16_t coarseY :5;
            uint16_t ntX :1;
            uint16_t ntY :1;
            uint16_t fineY :3;
        };
        uint16_t data;
    };

    void ppuWrite(uint16_t address, uint8_t data);

    uint8_t ppuRead(uint16_t address, bool active = true);

    uint32_t getRgbForPixel(uint8_t paletteId, uint8_t pixelValue);

    void progressNt();

    void fetchNextTile();
    void fetchNextTileAttribute();
    void fetchPatternForTile(bool isLow);
    void updateShiftRegisters();
    void progressX();
    void progressY();
    void activeCycleSwitch();
    void renderPixelsToScreen();

    // registers
    union
    {
        struct
        {
            uint8_t ntX : 1;
            uint8_t ntY : 1;
            uint8_t vramInc : 1;
            uint8_t sptAddr : 1;
            uint8_t bptAddr : 1;
            uint8_t spriteSize : 1;
            uint8_t masterSlave : 1;
            uint8_t genNmi : 1;
        };
        uint8_t data;
    } _ctrl;

    union
    {
        struct
        {
            uint8_t gScale : 1;
            uint8_t shBackgroundL8 : 1;
            uint8_t shSpriteL8 : 1;
            uint8_t shBackground : 1;
            uint8_t shSprite : 1;
            uint8_t empRed : 1;
            uint8_t empGreen : 1;
            uint8_t empBlue : 1;
        };
        uint8_t data;
    } _mask;

    union
    {
        struct
        {
            uint8_t openBus : 5;
            uint8_t spriteOverflow : 1;
            uint8_t spriteHit0 : 1;
            uint8_t verticalBank : 1;
        };
        uint8_t data;
    } _status;

    uint8_t _readBuffer;
    InternalVramRegister _v;
    InternalVramRegister _t;
    uint8_t _fineX;
    bool _w;
    bool _immediateRead;

    int16_t _cycle;
    int16_t _scanLine;
    bool _nmi;
    bool _oddFrame;

    PatternTable _patternTable;
    PaletteTable _paletteTable;
    std::array<NameTable, 2> _internalNameTableMem;

    WorkPaletteSet _workPaletteSet;

    WriteFunction _busWrite;
    ReadFunction _busRead;

    bool _ignoreCtrlFlagW;
    uint8_t _mirroringMode;

    uint8_t _ntByte;
    uint8_t _atByte;
    uint8_t _loBgTileByte;
    uint8_t _hiBgTileByte;

    uint16_t _loPtShift;
    uint16_t _hiPtShift;
    uint16_t _loAtShift;
    uint16_t _hiAtShift;

    std::array<uint32_t, 0xf000> _screen;
};