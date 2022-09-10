#include <iostream>
#include <memory.h>

#include "HardwareEmulation/Nes.hpp"

static void func(std::string file_path)
{
    std::cout << file_path << std::endl;
}

int main()
{
    Nes nes;
    nes.StartNesEmultion();
    return 0;
}