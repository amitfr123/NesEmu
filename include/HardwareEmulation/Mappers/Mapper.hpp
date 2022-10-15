#pragma once

#include <cstdint>
#include <vector>
#include <array>

class Mapper
{
public: 
	static constexpr uint32_t PRG_ROM_BANK_SIZE = 16384;
    static constexpr uint32_t CHR_ROM_BANK_SIZE = 8192;
	using PrgRomBankMapper = std::vector<std::array<uint8_t, PRG_ROM_BANK_SIZE>>;
	using ChrRomBankMapper = std::vector<std::array<uint8_t, CHR_ROM_BANK_SIZE>>;

	Mapper(PrgRomBankMapper&& prg_rom_bank_vector, ChrRomBankMapper&& chr_rom_bank_vector)
		:
		_prgRomBankVector(std::move(prg_rom_bank_vector)),
		_chrRomBankVector(std::move(chr_rom_bank_vector))
	{
	}

	virtual bool cpuWriteMap(uint16_t address, uint8_t data) = 0;
	virtual bool cpuReadMap(uint16_t address, uint8_t& data) = 0;

	virtual bool ppuWriteMap(uint16_t address, uint8_t data) = 0;
	virtual bool ppuReadMap(uint16_t address, uint8_t& data) = 0;

	virtual ~Mapper() = default;
protected:
    // In my emulator i will implemnt all rom as read only until i will need to change that
    PrgRomBankMapper _prgRomBankVector;
    ChrRomBankMapper _chrRomBankVector;
};
