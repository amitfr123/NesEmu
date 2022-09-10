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
		_prg_rom_bank_vector(std::move(prg_rom_bank_vector)),
		_chr_rom_bank_vector(std::move(chr_rom_bank_vector))
	{
	}

	virtual void CpuWriteMap(const uint16_t address, const uint8_t data) = 0;
	virtual uint8_t CpuReadMap(const uint16_t address) = 0;

	virtual ~Mapper() = default;
protected:
    // In my emulator i will implemnt all rom as read only until i will need to change that
    PrgRomBankMapper _prg_rom_bank_vector;
    ChrRomBankMapper _chr_rom_bank_vector;
};
