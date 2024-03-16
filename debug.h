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


void print_registers(Chip_8& chip8) {
    std::cout << std::hex;
    std::cout << "REGISTERS: " << nl;
    for (int i = 0; i < REG_SIZE; ++i) {
        std::cout << "V[" << i << "]= " << static_cast<int>(chip8.V[i]) << ' ';
        if (chip8.V[i] <= 0xF) std::cout << ' ';
        if ((i+1) % 4 == 0) std::cout << nl;
    }
}


void current_op(Chip_8& chip8) {
    std::cout << "Current opcode: " << std::hex << chip8.opcode << nl;
}
