#pragma once

#include <iostream>
#include "chip-8.h"


constexpr auto nl = '\n';

// DEBUGGING ONLY
// prints memory bytes in hexadecimal format in the range [from, to)
void print_memory(Chip_8& chip8, uint16_t from = 0, uint16_t to = MEM_SIZE) {
    std::cout << std::hex;
    for (auto i = from; i < to; ++i) {
        std::cout << static_cast<int>(chip8.read(i)) << ' ';
    }
}

