#include <random>
#include <climits>
#include "chip-8.h"


static std::random_device rd;
static std::mt19937 mt{ rd() };
static std::uniform_int_distribution<int> dist{0, INT_MAX};

static array<uint8_t, 16> save_data{};

// Fetch opcode, increment program counter by 2.
void Chip_8::fetch() {
    opcode = (read(pc) << 8) | read(pc + 1);
    pc += 2;
}

// Decode the opcode and execute it.
void Chip_8::decode_and_execute() {
    uint8_t byte0  = (opcode & 0xF000) >> 12;   // The first nibble
    uint8_t X      = (opcode & 0x0F00) >> 8;    // The second nibble
    uint8_t Y      = (opcode & 0x00F0) >> 4;    // The third nibble
    uint8_t N      = (opcode & 0x000F);         // The fourth nibble
    uint8_t NN     = (opcode & 0x00FF);         // The last two nibbles
    uint16_t NNN   = (opcode & 0x0FFF);         // The last three nibbles

    switch (byte0) {
    case 0x0:
        switch (NN) {
        case 0xE0: // Clear screen
            for (auto& row : display) std::fill(row.begin(), row.end(), 0);
            for (auto& row : super_display) std::fill(row.begin(), row.end(), 0);
            draw_screen = true;
            break;

        case 0xEE: // Return from a subroutine
            pc = stack.top();
            stack.pop();
            break;

        case 0xFF: // Enable high resolution mode
            super = true;
            break;

        case 0xFE: // Enable low resolution mode
            super = false;
            break;

        case 0xFB: // Scroll display right by 4 pixels
            if (super) scroll_right(super_display, SUPER_WIDTH);
            else scroll_right(display, SCREEN_WIDTH);
            draw_screen = true;
            break;

        case 0xFC: // Scroll display left by 4 pixels
            if (super) scroll_left(super_display, SUPER_WIDTH);
            else scroll_left(display, SCREEN_WIDTH);
            draw_screen = true;
            break;

        case 0xFD: // Exit the interpreter
            active = false;
            break;

        case 0x00: // Halt the program 
            stopped = true;
            break;

        default: // 00CN Scroll display down by N pixels
            if (Y == 0xC) {
                if (super) scroll_down(super_display, SUPER_HEIGHT, N);
                else scroll_down(display, SCREEN_HEIGHT, N);
            } else if (Y == 0xD) {
                if (super) scroll_up(super_display, SUPER_HEIGHT, N);
                else scroll_up(display, SCREEN_HEIGHT, N);
            }
            draw_screen = true;
            break;
        }
        break;

    case 0x1: // Jump to NNN
        pc = NNN;
        break;

    case 0x2: // Jump to subroutine at NNN
        stack.push(pc);
        pc = NNN;
        break;

    case 0x3: // Skip conditionally
        if (V[X] == NN) pc += 2;
        break;

    case 0x4: // Skip conditionally
        if (V[X] != NN) pc += 2;
        break;

    case 0x5: // Skip conditionally
        if (V[X] == V[Y]) pc += 2;
        break;

    case 0x6: // Set V[X] register to NN
        V[X] = NN;
        break;

    case 0x7: // Add NN to V[X] register
        V[X] += NN;
        break;

    case 0x9: // Skip conditionally
        if (V[X] != V[Y]) pc += 2;
        break;

    case 0x8: //Logical and Arithmetic opcodes
        switch (N) {
        case 0x0:
            V[X] = V[Y];
            break;

        case 0x1: 
            V[X] |= V[Y];
            if (vf_reset) V[0xF] = 0;
            break;

        case 0x2:
            V[X] &= V[Y];
            if (vf_reset) V[0xF] = 0;
            break;

        case 0x3:
            V[X] ^= V[Y];
            if (vf_reset) V[0xF] = 0;
            break;

        case 0x4: { // Adds V[Y] to V[X], sets carry flag
            uint16_t result = V[X] + V[Y];
            V[X] = static_cast<uint8_t>(result);
            V[0xF] = (result > 0xFF);
            break;
        }

        case 0x5: { // Substracts V[Y] from V[X], sets carry flag
            bool temp = (V[X] >= V[Y]);
            V[X] = V[X] - V[Y];
            V[0xF] = temp;
            break;
        }

        case 0x6: { // Set carry flag to the right-most bit and shift V[X] one bit to the right
            if (!shift) V[X] = V[Y];
            bool bit = V[X] & 0x01;
            V[X] >>= 1;
            V[0xF] = bit;
            break;
        }

        case 0x7: { // Substracts V[X] from V[Y], sets carry flag
            bool temp = (V[Y] >= V[X]);
            V[X] = V[Y] - V[X];
            V[0xF] = temp;
            break;
        }

        case 0xE: { // Set carry flag to the left-most bit and shift V[X] one bit to the left
            if (!shift) V[X] = V[Y];
            bool bit = V[X] & 0x80;
            V[X] <<= 1;
            V[0xF] = bit;
            break;
        }
        }
        break;

    case 0xA: // Set index register to NNN
        I = NNN;
        break;

    case 0xB: {
        uint8_t jmp = (jump)? V[X] : V[0]; // make this toggleable (V[X] / V[0])
        pc = NNN + jmp;
        break;
    }

    case 0xC: { // Generates a random number and binary ANDs it with NN
        auto rand = static_cast<uint8_t>(dist(mt));
        V[X] = rand & NN;
        break;
    }

    case 0xD: { // Draw sprite on screen
        if (super) {
            if (N == 0) draw_screen_array_sprite(super_display, SUPER_HEIGHT, SUPER_WIDTH, X, Y);
            else draw_screen_array(super_display, SUPER_HEIGHT, SUPER_WIDTH, X, Y, N);
        } else {
            if (N == 0) draw_screen_array_sprite(display, SCREEN_HEIGHT, SCREEN_WIDTH, X, Y);
            else draw_screen_array(display, SCREEN_HEIGHT, SCREEN_WIDTH, X, Y, N);
        }
        draw_screen = true;
        break;
    }

    case 0xE: // Skips instructions based on key press
        switch (NN) {
        case 0x9E:
            pc += (keyboard[V[X] & 0xF]) ? 2 : 0;
            break;

        case 0xA1:
            pc += (!keyboard[V[X] & 0xF]) ? 2 : 0;
            break;
        }
        break;

    case 0xF:
        switch (NN) {
        case 0x07:
            V[X] = delay;
            break;

        case 0x15:
            delay = V[X];
            break;

        case 0x18:
            sound = (V[X] == 1)? 2 : V[X];
            break;

        case 0x1E: { // Add V[X] to I, sets carry flag
            uint32_t res = I + V[X];
            I += V[X];
            V[0xF] = res > 0x0FFF;
            break;
        }

        case 0x0A: // Stop until a key is pressed and released, then set V[X] to the key set.
            if (last_key > 0xF) {
                for (uint16_t i = 0x0; i <= 0xF; ++i) {
                    if (!keyboard[i]) continue;
                    last_key = i; break;
                }
            } else if(!keyboard[last_key]) {
                V[X] = static_cast<uint8_t>(last_key);
                last_key = 0xFF; break;
            }
            pc -= 2;
            break;

        case 0x29: // Set I to font character at V[X]
            I = (V[X] & 0x0F) * 5;
            break;

        case 0x33: // Binary to Decimal conversions added to successive addresses from I
            write(I, (V[X] / 100) % 10);
            write(I + 1, (V[X] / 10) % 10);
            write(I + 2, V[X] % 10);
            break;

        case 0x55: // Store V[0] to V[X] registers at successive memory addresses from I 
            for (uint16_t i = 0; i <= X; ++i) {
                if (mem_incr) write(I++, V[i]); 
                else write(I + i, V[i]);
            }
            break;

        case 0x65: // Store successive memory addresses from I at registers V[0] to V[X]
            for (uint16_t i = 0; i <= X; ++i) {
                if (mem_incr) V[i] = read(I++); 
                else V[i] = read(I + i);
            }
            break;

        case 0x30: // Set I to a large hexadecimal character based on the value of V[X]
            I = SUPER_FONT_LOCATION + (V[X] & 0x0F) * 10;
            break;

        case 0x75: // Save V[0] - V[X] to flag registers
            for (uint16_t i = 0; i <= X; ++i) 
                save_data[i] = V[i]; 
            break;

        case 0x85: // Restore V[0] - V[X] from flag registers
            for (uint16_t i = 0; i <= X; ++i) 
                V[i] = save_data[i]; 
            break;
        }
        break;
    }
}
