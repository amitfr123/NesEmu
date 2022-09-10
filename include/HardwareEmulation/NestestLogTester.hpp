#pragma once

#include <exception>
#include <array>
#include <string>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <vector>
#include <memory>

#include "Cpu.hpp"

#include "NumToHexStringConvertor.hpp"

// This file uses a known good log file for the nestest rom
// The nestest rom is a rom file created to test NES emulatores
// This project was created as the first part of a NES emulator so i use the nestest and others to test my cpu
// This emulator starts failing at the E545 address which attempts to preform the LAX instruction that isn't implmented

class NestestLogTester
{
public:
    static std::shared_ptr<NestestLogTester> GetInstance()
    {
        static std::shared_ptr<NestestLogTester> instance(new NestestLogTester());
        return instance;
    }

    void DebugInstruction(const Cpu& cpu, uint8_t opcode, uint32_t index)
    {
        if (index > _cmp_vector.max_size())
        {
            throw std::runtime_error("NestestLogTester received invalid index");
        }
        // This is used for debug prints
        std::string str = NumToHexStringConvertor::Convert(cpu._pc - 1, 4) + "    " + 
            NumToHexStringConvertor::Convert(opcode, 2) + 
            "   A:" + NumToHexStringConvertor::Convert(cpu._a, 2) +
            "   X:" + NumToHexStringConvertor::Convert(cpu._x, 2) +
            "   Y:" + NumToHexStringConvertor::Convert(cpu._y, 2) +
            "   P:" + NumToHexStringConvertor::Convert(cpu._p | cpu.RESERVED_FLAG_MASK, 2) +
            "   SP:" + NumToHexStringConvertor::Convert(cpu._sp, 2);
        if (!StringComperator(opcode, index, str))
        {
            if (index > 0)
            {
                std::cout << "Error in previous instruction" << std::endl;
                std::cout << _cmp_vector[index - 1] << std::endl;
            }
            getchar();
        }
        
    }

private:
    NestestLogTester()
    {
        uint32_t index = 0;
        std::string line;
        std::ifstream log(NESTEST_ROM_LOG_PATH);
        while (std::getline(log,line))
        {
            _cmp_vector.insert(_cmp_vector.end(),std::string(line)); 
        }
        
    }

    bool StringComperator(uint8_t opcode, uint32_t index, std::string debugString)
    {
        bool flag = true;
        std::string line(_cmp_vector[index]);

        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        debugString.erase(std::remove_if(debugString.begin(), debugString.end(), ::isspace), debugString.end());

        std::string::iterator line_it = line.begin();
        std::string::iterator debug_it = debugString.begin();

        // match addr
        flag &= StringCmp(std::string(line.substr(0,4)), debugString.substr(0,4), "Checking address");
        flag &= StringCmp(std::string(line.substr(4,2)), debugString.substr(4,2), "Checking opcode");

        flag &= StringByIteratorCmp(line_it, debug_it, line.end(), debugString.end(), "Checking Acumm");
        flag &= StringByIteratorCmp(line_it, debug_it, line.end(), debugString.end(), "Checking X");
        flag &= StringByIteratorCmp(line_it, debug_it, line.end(), debugString.end(), "Checking Y");
        flag &= StringByIteratorCmp(line_it, debug_it, line.end(), debugString.end(), "Checking P");
        flag &= StringByIteratorCmp(line_it, debug_it, line.end(), debugString.end(), "Checking SP");
        return flag;
    }

    static bool IsSeperator(char c) // static cause i am lazy and its an easy fix
    {
        return (c == ':');
    }

    void PrintResult(std::string e, std::string r, std::string desc)
    {
        std::cout << desc << std::endl;
        std::cout << "expected: " << e << ", result: "<< r << std::endl;
    }

    bool StringByIteratorCmp(std::string::iterator& line_it, std::string::iterator& debug_it, 
        const std::string::iterator& line_end, const std::string::iterator& debug_end, std::string desc)
    {
        line_it = std::find_if(line_it, line_end, IsSeperator) + 1;
        debug_it = std::find_if(debug_it, debug_end, IsSeperator) + 1;
        return StringCmp(std::string(line_it, line_it + 2), std::string(debug_it, debug_it + 2), desc);;
    }

    bool StringCmp(std::string e, std::string r, std::string desc)
    {
        if (e != r)
        {
            PrintResult(e, r, desc);
            return false;
        }
        return true;
    }

   std::vector<std::string> _cmp_vector;
};