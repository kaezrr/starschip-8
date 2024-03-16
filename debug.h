#pragma once

#include <iostream>
#include "chip-8.h"


constexpr auto nl = '\n';

// DEBUGGING ONLY
// prints memory bytes in hexadecimal format in the range [from, to)
void print_memory(const Chip_8& chip8, uint16_t from = 0, uint16_t to = MEM_SIZE) {
    std::cout << std::hex;
    for (auto i = from; i < to; ++i) {
        std::cout << static_cast<int>(chip8.read(i)) << ' ';
    }
}


void print_registers(const Chip_8& chip8) {
    std::cout << std::hex;
    std::cout << "REGISTERS: " << nl;
    for (int i = 0; i < REG_SIZE; ++i) {
        std::cout << "V[" << i << "]= " << static_cast<int>(chip8.V[i]) << ' ';
        if (chip8.V[i] <= 0xF) std::cout << ' ';
        if ((i+1) % 4 == 0) std::cout << nl;
    }
}


void current_op(const Chip_8& chip8) {
    std::cout << "Current opcode: " << std::hex << chip8.opcode << nl;
}


void print_keys(const Chip_8& chip8) {
    std::cout << "Keys turned on: ";
    std::cout << std::hex;
    for (uint16_t i = 0x0; i <= 0xF; i++) {
        if (!chip8.keyboard[i]) continue;
        std::cout << i << ' ';
    }
    std::cout << nl;
}
