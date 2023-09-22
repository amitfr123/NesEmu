#include <fstream>
#include <cstring>
#include "HardwareEmulation/Ppu.hpp"

Ppu::Ppu(WriteFunction bus_write, ReadFunction bus_read) :
    _busRead(std::move(bus_read)), _busWrite(std::move(bus_write))
{
    std::memset(_workPaletteSet.data(), 0, sizeof(_workPaletteSet));
    std::memset(_paletteTable.data(), 0, sizeof(_paletteTable));
    std::memset(_patternTable.data(), 0, sizeof(_patternTable));
    std::memset(_screen.data(), 0, sizeof(_screen));
    
    std::ifstream pFile(std::string(RESOURCE_PATH) + "/palettes/NES_Classic.pal", std::ios::binary);
    std::array<uint8_t, 3> pBuffer;
    size_t i = 0;
    for (;i < 64 && !pFile.read(reinterpret_cast<char*>(pBuffer.data()), pBuffer.size()).eof(); i++)
    {
        _paletteTable[i % 64] = (static_cast<uint32_t>(pBuffer[0]) << 16);
        _paletteTable[i % 64] |= (static_cast<uint32_t>(pBuffer[1]) << 8);
        _paletteTable[i % 64] |= pBuffer[2];
    }
    pFile.close();
    if (i < 64)
    {
        throw std::runtime_error("Palette file is too small");
    }
    reset();
}

void Ppu::reset()
{
    _ignoreCtrlFlagW = false;
    _oddFrame = true;
    _scanLine = 0;
    _cycle = 0;
    _loPtShift = 0;
    _hiPtShift = 0;
    _loAtShift = 0;
    _hiAtShift = 0;
    _v.data = 0;
    _t.data = 0;
    _fineX = 0;
    _loBgTileByte = 0;
    _hiBgTileByte = 0;
    _nmi = false;
    _ntByte = 0;
    _atByte = 0;
}

std::span<const uint32_t> Ppu::getPalette()
{
    return std::span<uint32_t>(_paletteTable.begin(), _paletteTable.size());
}

std::array<std::array<uint32_t, 0x4000>, 2> Ppu::getPatternTable(uint8_t paletteId)
{
    std::array<std::array<uint32_t, 0x4000>, 2> arr;
    uint8_t vecI = 0;
    uint32_t tileY = 0;
    for (uint32_t i = PPU_PATTERN_ADDR_START; i < PPU_PATTERN_ADDR_END;)
    {
        for (uint32_t tile = 0; tile < 16; tile++)
        {
            for (uint32_t y = 0; y < 8; y++)
            {
                uint8_t lo = ppuRead(i, false);
                uint8_t hi = ppuRead(i + 8, false);
                for (uint32_t x = 0; x < 8; x++)
                {
                    uint8_t colorInPal = (lo & 0x1) | ((hi & 0x1) << 1);
                    arr[vecI][7 - x + tile * 8 + 128 * y + tileY * 0x400] = getRgbForPixel(paletteId, colorInPal);
                    lo >>= 1;
                    hi >>= 1;
                }
                i++;
            }
            i += 8;
        }
        tileY++;
        if (i == 0x1000)
        {
            vecI = 1;
            tileY = 0;
        }
    }
    return arr;
}

std::array<uint32_t, 4> Ppu::getWorkPaletteRgb(uint8_t paletteId)
{
    std::array<uint32_t, 4> arr;
    paletteId %= 8;
    uint16_t palAddr = PPU_PALETTE_ADDR_START + paletteId * 4;
    arr[0] = _paletteTable[ppuRead(palAddr, false)];
    arr[1] = _paletteTable[ppuRead(palAddr + 1, false)];
    arr[2] = _paletteTable[ppuRead(palAddr + 2, false)];
    arr[3] = _paletteTable[ppuRead(palAddr + 3, false)];
    return arr;
}

std::span<const uint32_t> Ppu::getScreen()
{
    return std::span<uint32_t>(_screen.begin(), _screen.size());
}

void Ppu::writeToRegister(uint16_t address, uint8_t data)
{
    address &= 0xf;
    switch (address)
    {
    case PPU_CTRL_OFFSET:
        if (!_ignoreCtrlFlagW)
        {
            _ctrl.data = data;
            _t.ntX = _ctrl.ntX;
            _t.ntY = _ctrl.ntY;
            // TODO: check for nmi immediate activation
        }
        break;
    case PPU_MASK_OFFSET:
        _mask.data = data;
        break;
    case PPU_OAM_ADDR_OFFSET:
        // TODO: impl
        break;
    case PPU_OAM_DATA_OFFSET:
        // TODO: impl
        break;
    case PPU_SCROLL_OFFSET:
        if (_w == 0)
        {
            _t.coarseX = data >> 3;
            _fineX = data & 0x7;
            _w = 1;
        }
        else
        {
            _t.coarseY = data >> 3;
            _t.fineY = data & 0x7;
            _w = 0;
        }
        break;
    case PPU_ADDR_OFFSET:
        if (_w == 0)
        {
            _t.data = (static_cast<uint16_t>(data & 0x3f) << 8) | (_t.data & 0x00FF);
            _w = 1;
        }
        else
        {
            _t.data = (_t.data & 0xff00) | data;
            _v.data = _t.data;
            _w = 0;
        }
        break;
    case PPU_DATA_OFFSET:
        ppuWrite(_v.data, data);
        _v.data += (_ctrl.vramInc)? 32 : 1;
        break;
    default:
        break;
    }
}

uint8_t Ppu::readFromRegister(uint16_t address)
{
    uint8_t data = _readBuffer;
    address &= 0xf;
    switch (address)
    {
    case PPU_STATUS_OFFSET:
        data = _status.data & 0xE0;
        data |= _readBuffer & 0x1f;
        _w = 0;
        _status.verticalBank = 0;
        break;
    case PPU_OAM_DATA_OFFSET:
        // TODO: impl
        break;
    case PPU_DATA_OFFSET:
        data = _readBuffer;
        _readBuffer = ppuRead(_v.data);
        _v.data += (_ctrl.vramInc)? 32 : 1;
        if (_immediateRead)
        {
            data = _readBuffer;
            _immediateRead = false;
        }
        break;
    default:
        break;
    }
    return data;
}


void Ppu::ppuWrite(uint16_t address, uint8_t data)
{
    address &= 0x3FFF;
    if (_busWrite(address, data))
    {
        // overriden by the bus
    }
    else if (address >= PPU_PATTERN_ADDR_START && address <= PPU_PATTERN_ADDR_END)
    {
        PatternTableAddr addr(address);
        _patternTable[addr.h][addr.r * 16 + addr.c][addr.p][addr.t] = data;
    }
    else if (address >= PPU_NAME_TABLE_ADDR_START && address <= PPU_NAME_TABLE_ADDR_M_END)
    {
        address &= 0x0fff;
        uint8_t nameTableId = (address & 0x0c00) >> 10;
        if (_mirroringMode == 1) // horizontal
        {
            _internalNameTableMem[nameTableId / 2][address & 0x03FF] = data;
        }
        else if (_mirroringMode == 2) // vertical
        {
            _internalNameTableMem[nameTableId % 2][address & 0x03FF] = data;
        }
    }
    else if (address >= PPU_PALETTE_ADDR_START && address <= PPU_PALETTE_ADDR_END)
    {
        address = address % _workPaletteSet.size(); // mirroring 
        if (address % 4 == 0)
        {
            address &= 0xf; // internal mirroring for 3f10 3f14 3f18 3f1c
        }
        _workPaletteSet[address] = data;
    }
}

uint8_t Ppu::ppuRead(uint16_t address, bool active)
{
    uint8_t data = 0;
    address &= 0x3fff;
    if (_busRead(address, data))
    {
        // overriden by the bus
    }
    else if (address >= PPU_PATTERN_ADDR_START && address <= PPU_PATTERN_ADDR_END)
    {
        PatternTableAddr addr(address);
        data = _patternTable[addr.h][addr.r * 16 + addr.c][addr.p][addr.t];
    }
    else if (address >= PPU_NAME_TABLE_ADDR_START && address <= PPU_NAME_TABLE_ADDR_M_END)
    {
        address &= 0x0fff;
        uint8_t nameTableId = (address & 0x0c00) >> 10;
        if (_mirroringMode == 1) // horizontal
        {
            data = _internalNameTableMem[nameTableId / 2][address & 0x03FF];
        }
        else if (_mirroringMode == 2) // vertical
        {
             data = _internalNameTableMem[nameTableId % 2][address & 0x03FF];
        }
    }
    else if (address >= PPU_PALETTE_ADDR_START && address <= PPU_PALETTE_ADDR_END)
    {
        address &= 0x001F;
        if (address % 4 == 0)
        {
            address &= 0xf; // internal mirroring for 3f10 3f14 3f18 3f1c
        }
        data = _workPaletteSet[address];
        if (active)
        {
            _immediateRead = true;
        }
    }
    return data;
}

void Ppu::executeCycle()
{
    if (_scanLine == 261 && _cycle == 1)
    {
        // Effectively start of new frame, so clear vertical blank flag
        _status.verticalBank = 0;
    }
    if (_scanLine == 0 && _cycle == 0)
    {
        _cycle = 1;
    }
    if (_scanLine == 261 && _cycle == 1)
    {
        _status.verticalBank = 0;
        _status.spriteHit0 = 0;
        _status.spriteOverflow = 0;
        _oddFrame = !_oddFrame;
    }
    if (_scanLine >= 0 && _scanLine < 240)
    {
        if (_cycle >= 1 && _cycle <= 256)
        {
            // normal fetch
            activeCycleSwitch();
            // render the pixels to the screen
            renderPixelsToScreen();
        }
        else if (_cycle >= 321 && _cycle <= 336)
        {
            activeCycleSwitch();
        }
        else if (_cycle >= 337 && _cycle <= 340)
        {
            if (_cycle == 338 || _cycle == 340)
            {
                _ntByte = ppuRead(PPU_NAME_TABLE_ADDR_START + (_v.data & 0xfff));
            }
        }
    }
    progressY();
    if (_cycle == 257)
    {
        // reset to row start
        _v.coarseX = _t.coarseX;
        _v.ntX = _t.ntX;
    }
    if (_scanLine == 256 && _cycle <= 280 && _cycle >= 304)
    {
        _v.coarseY = _t.coarseY;
        _v.ntY = _t.ntY;
    }
    if (_scanLine == 241 && _cycle == 1)
    { 
        // frame complete
        _status.verticalBank = 1;
        if (_ctrl.genNmi)
        {
            _nmi = true;
        }
    }
    _cycle++;
    if (_cycle == 341)
    {
        _cycle = 0;
        _scanLine++;
        if (_scanLine == 262)
        {
            _scanLine = 0;
        }
    }
}

bool Ppu::getNmiStatus()
{
    return _nmi;
}

void Ppu::clearNmiStatus()
{
    _nmi = false;
}

void Ppu::setMirroringMode(uint8_t mode)
{
    _mirroringMode = mode;
}

uint32_t Ppu::getRgbForPixel(uint8_t paletteId, uint8_t pixelValue)
{
    paletteId %= 8;
    pixelValue &= 0x3;
    uint16_t palAddr = PPU_PALETTE_ADDR_START + paletteId * 4;
    return _paletteTable[ppuRead(palAddr + pixelValue, false)];
}

void Ppu::fetchNextTile()
{
    // load old data into the shift registers
    _loPtShift = (_loPtShift & 0xff00) | _loBgTileByte;
    _hiPtShift = (_hiPtShift & 0xff00) | _hiBgTileByte;
    _loAtShift = (_loAtShift & 0xff00) | ((_atByte & 0x1)? 0xff : 0);
    _hiAtShift = (_hiAtShift & 0xff00) | ((_atByte & 0x2)? 0xff : 0);

    // get new pattern id
    _ntByte = ppuRead(PPU_NAME_TABLE_ADDR_START + (_v.data & 0xfff));
}

void Ppu::fetchNextTileAttribute()
{
    uint8_t shift = 0;
    _atByte = ppuRead((PPU_NAME_TABLE_ADDR_START + PPU_ATTRIBUTE_TABLE_OFFSET) | 
                (_v.ntX << 10) | (_v.ntY << 11) |
                ((_v.coarseY >> 2) << 3) | (_v.coarseX >> 2));
    if (_v.coarseX % 4 > 1)
    {
        shift += 2;
    }
    if (_v.coarseY % 4 > 1)
    {
        shift += 4;
    }
    _atByte = (_atByte >> shift) & 0x3;
}

void Ppu::fetchPatternForTile(bool isLow)
{
    PatternTableAddr addr(0);
    addr.c = _ntByte % 0x32;
    addr.r = _ntByte / 32;
    addr.h = _ctrl.bptAddr;
    addr.t = _v.fineY;
    if (isLow)
    {
        _loBgTileByte = ppuRead(addr.data);
    }
    else
    {
        addr.p = 1;
        _hiBgTileByte = ppuRead(addr.data);
    }
}

void Ppu::updateShiftRegisters()
{
    if (_mask.shBackground)
    {
        _loPtShift <<= 1;
        _hiPtShift <<= 1;
        _loAtShift <<= 1;
        _hiAtShift <<= 1;
    }
}

void Ppu::progressX()
{
    if (_mask.shBackground | _mask.shSprite)
    {
        if (_v.coarseX == 31)
        {
            _v.coarseX = 0;
            _v.ntX ^= 1;
        }
        else
        {
            _v.coarseX++;
        }
    }
}

void Ppu::progressY()
{
    if (_mask.shBackground | _mask.shSprite)
    {
        if (_cycle == 256)
        {
            _v.fineY++;
            if (_v.fineY == 8)
            {
                _v.fineY = 0;
                if (_v.coarseY == 29)
                {
                    _v.coarseY = 0;
                    _v.ntY ^= 1;
                }
                else if (_v.coarseY == 31)
                {
                    _v.coarseY = 0;
                }
                else
                {
                    _v.coarseY++;
                }
            }
        }
    }
}

void Ppu::activeCycleSwitch()
{
    updateShiftRegisters();
    switch ((_cycle - 1) % 8)
    {
    case 0:
        fetchNextTile();
        break;
    case 2:
    fetchNextTileAttribute();
        break;
    case 4:
        fetchPatternForTile(true);
        break;
    case 6:
        fetchPatternForTile(false);
    case 7:
        progressX();
        break;
    }
}

void Ppu::renderPixelsToScreen()
{
    if (_mask.shBackground)
    {
        uint16_t mux = 0x8000 >> _fineX;
        uint8_t lPix = (mux & _loPtShift) >> (15 - _fineX);
        uint8_t hPix = ((mux & _hiPtShift) >> (15 - _fineX)) << 1;
        uint8_t lp = (mux & _loAtShift) >> (15 - _fineX);
        uint8_t hp = ((mux & _hiAtShift) >> (15 - _fineX)) << 1;
        static int i = 0;
        i++;
        _screen[_cycle - 1 + _scanLine * 256] = getRgbForPixel(lp | hp, lPix | hPix);
        // _screen[_cycle - 1 + _scanLine * 256] = _paletteTable[(rand() % 2) ? 0x3F : 0x30];
    }
}