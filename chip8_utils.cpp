#include <fstream>
#include <iostream>

#include "chip-8.h"

static array<uint8_t, 80> font =
    {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
     0x20, 0x60, 0x20, 0x20, 0x70, // 1
     0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
     0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
     0x90, 0x90, 0xF0, 0x10, 0x10, // 4
     0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
     0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
     0xF0, 0x10, 0x20, 0x40, 0x40, // 7
     0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
     0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
     0xF0, 0x90, 0xF0, 0x90, 0x90, // A
     0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
     0xF0, 0x80, 0x80, 0x80, 0xF0, // C
     0xE0, 0x90, 0x90, 0x90, 0xE0, // D
     0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
     0xF0, 0x80, 0xF0, 0x80, 0x80};// F


// Read byte at memory location 'at'. Has OOB check.
uint8_t Chip_8::read(uint16_t at) const {
    if (at >= MEM_SIZE) return 0x00;
    return memory[at];
}


// Write byte 'data' at memory location 'at'. Has OOB check.
void Chip_8::write(uint16_t at, uint8_t data) {
    if (at >= MEM_SIZE) return;
    memory[at] = data;
}


// Default constructor.
Chip_8::Chip_8() {
    // Load font into memory
    for (uint16_t i = 0x00; i < 0x50; ++i) write(i, font[i]); 

    // Set program counter to RAM.
    pc = RAM_LOCATION;
    draw_screen = false;
}


// Load program into memory.
void Chip_8::load_program(const std::string& path) {
    std::ifstream prog{ path };
    if (!prog) {
        std::cerr << "ERROR: File not found!\n";
        std::exit(1);
    }

    prog.seekg(0, std::ios::end);
    std::size_t rom_size = prog.tellg();
    prog.seekg(0, std::ios::beg);

    if (rom_size > MEM_SIZE - RAM_LOCATION) {
        std::cerr << "ERROR: File too big!\n";
        std::exit(1);
    }

    prog.read(reinterpret_cast<char*>(&memory[RAM_LOCATION]), rom_size);
}


// Pretty self explanatory
bool Chip_8::get_pixel(size_t x, size_t y) const {
    if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return false;
    return display[y][x];

}


