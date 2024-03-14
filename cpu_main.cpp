#include <iostream>
#include "chip-8.h"


// Fetch opcode, increment program counter by 2.
void Chip_8::fetch() {
    opcode = (read(pc) << 8) | read(pc + 1);
    pc += 2;
}

// Decode the opcode and execute it.
void Chip_8::decode_and_execute() {
    uint8_t byte0  = (opcode & 0xF000) >> 12;
    uint8_t X      = (opcode & 0x0F00) >> 8;
    uint8_t Y      = (opcode & 0x00F0) >> 4;
    uint8_t N      = (opcode & 0x000F);
    uint8_t NN     = (opcode & 0x00FF);
    uint16_t NNN   = (opcode & 0x0FFF);

    switch (byte0) {
    case 0x0:
        switch (NNN) {
        case 0x0E0: // Clear screen
            for (auto& row : display) row.fill(0); 
            draw_screen = true;
            break;
        }
        break;

    case 0x1: // Jump to NNN
        pc = NNN;
        break;
        
    case 0x6: // Set V[X] register to NN
        V[X] = NN;
        break;
        
    case 0x7: // Add NN to V[X] register
        V[X] += NN;
        break;
        
    case 0xA: // Set index register to NNN
        I = NNN;
        break;

    case 0xD: // Draw sprite on screen
        uint8_t x_coords = V[X] % SCREEN_WIDTH;
        uint8_t y_coords = V[Y] % SCREEN_HEIGHT;
        V[0xF] = 0x00; // Set collision flag to false

        for (uint8_t n = 0; n < N; ++n, ++y_coords) {
            if (y_coords >= SCREEN_HEIGHT) break;
            auto x = x_coords;

            // Iterate through each bit left to right in sprite data
            for (uint8_t sp = read(I + n); sp > 0; (sp <<= 1), ++x) {
                if (x >= SCREEN_WIDTH) break;
                if (!(sp & 0x80)) continue;

                if (display[y_coords][x]) V[0xF] = 0x01; // set collison flag to true
                display[y_coords][x] = !display[y_coords][x];
            }
        }
        draw_screen = true;
    }
}
