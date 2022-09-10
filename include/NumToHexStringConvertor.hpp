#pragma once 

#include <string>
#include <string_view>

/*
    This class was inspired by OLC hex convertion method
*/
class NumToHexStringConvertor
{
private:
    static constexpr std::string_view HEX_CHARACTER_TEMPLATE = "0123456789ABCDEF";
public:
    static std::string Convert(uint32_t number, uint8_t hex_digit_count)
    {
		std::string s(hex_digit_count, '0');
		for (int i = hex_digit_count - 1; i >= 0; i--, number >>= 4)
			s[i] = HEX_CHARACTER_TEMPLATE[number & 0xF];
		return std::move(s);
	}
};