#pragma once

#include "Mapper.hpp"

class Mapper0 : public Mapper
{
public:
	Mapper0(PrgRomBankMapper&& prg_rom_bank_vector, ChrRomBankMapper&& chr_rom_bank_vector)
		:
		Mapper(std::move(prg_rom_bank_vector), std::move(chr_rom_bank_vector))
	{
	}
	bool cpuWriteMap(uint16_t address, uint8_t data) override;
	bool cpuReadMap(uint16_t address, uint8_t& data) override;

	bool ppuWriteMap(uint16_t address, uint8_t data) override;
	bool ppuReadMap(uint16_t address, uint8_t& data) override;

	~Mapper0() = default;
};
