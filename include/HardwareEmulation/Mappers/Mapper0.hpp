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

	void CpuWriteMap(const uint16_t address, const uint8_t data) override;
	uint8_t CpuReadMap(const uint16_t address) override;

	~Mapper0() = default;
};
