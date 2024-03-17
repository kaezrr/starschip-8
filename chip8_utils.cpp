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


Chip_8::Chip_8(uint32_t argument) {
    // Set Flags
    vf_reset    = (argument & RESET_VF);
    mem_incr    = (argument & MEM_INCREMENT);
    disp_wait   = (argument & DISPLAY_WAIT);
    clip        = (argument & CLIPPING);
    shift       = (argument & SHIFTING);
    jump        = (argument & JUMPING);

    // Load font into memory
    for (uint16_t i = 0x00; i < 0x50; ++i) write(i, font[i]); 

    // Set program counter to RAM.
    pc = RAM_LOCATION;
    draw_screen = false;

    last_key = 0xFF;
}


// Load program into memory.
void Chip_8::load_program(const std::string& path) {
    std::ifstream prog{ path, std::ios::binary };
    if (!prog) {
        std::cerr << "ERROR: File not found!\n";
        std::exit(1);
    }
    // Calculating ROM size.
    prog.seekg(0, std::ios::end);
    std::size_t rom_size = prog.tellg();
    prog.seekg(0, std::ios::beg);

    if (rom_size > MEM_SIZE - RAM_LOCATION) {
        std::cerr << "ERROR: File too big!\n";
        std::exit(1);
    }
    prog.read(reinterpret_cast<char*>(&memory[RAM_LOCATION]), rom_size);
}


// Handling key presses.
void Chip_8::toggle_key(const SDL_Scancode& sc, bool on) {
    switch (sc) {
    case SDL_SCANCODE_1:
        keyboard[0x1] = on;
        break;

    case SDL_SCANCODE_2:
        keyboard[0x2] = on;
        break;

    case SDL_SCANCODE_3:
        keyboard[0x3] = on;
        break;

    case SDL_SCANCODE_4:
        keyboard[0xC] = on;
        break;

    case SDL_SCANCODE_Q:
        keyboard[0x4] = on;
        break;

    case SDL_SCANCODE_W:
        keyboard[0x5] = on;
        break;

    case SDL_SCANCODE_E:
        keyboard[0x6] = on;
        break;

    case SDL_SCANCODE_R:
        keyboard[0xD] = on;
        break;
        
    case SDL_SCANCODE_A:
        keyboard[0x7] = on;
        break;

    case SDL_SCANCODE_S:
        keyboard[0x8] = on;
        break;

    case SDL_SCANCODE_D:
        keyboard[0x9] = on;
        break;

    case SDL_SCANCODE_F:
        keyboard[0xE] = on;
        break;

    case SDL_SCANCODE_Z:
        keyboard[0xA] = on;
        break;

    case SDL_SCANCODE_X:
        keyboard[0x0] = on;
        break;

    case SDL_SCANCODE_C:
        keyboard[0xB] = on;
        break;

    case SDL_SCANCODE_V:
        keyboard[0xF] = on;
        break;
    }
}


