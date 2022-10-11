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

	void CpuWriteMap(uint16_t address, uint8_t data) override;
	uint8_t CpuReadMap(uint16_t address) override;

	~Mapper0() = default;
};
