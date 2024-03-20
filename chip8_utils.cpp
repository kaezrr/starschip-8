#include <fstream>
#include <iostream>

#include "chip-8.h"

static array<uint8_t, 80> font {
     0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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


static array<uint8_t, 160> super_font{
    0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, // 0
    0x18, 0x78, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, // 1
    0x7E, 0xFF, 0xC3, 0x03, 0x07, 0x1E, 0x78, 0xE0, 0xFF, 0xFF, // 2
    0x7E, 0xFF, 0xC3, 0x03, 0x0E, 0x0E, 0x03, 0xC3, 0xFF, 0x7E, // 3
    0xC3, 0xC3, 0xC3, 0xC3, 0xFF, 0x7F, 0x03, 0x03, 0x03, 0x03, // 4
    0xFF, 0xFF, 0xC0, 0xC0, 0xFE, 0x7F, 0x03, 0x03, 0xFF, 0xFE, // 5
    0x7F, 0xFF, 0xC0, 0xC0, 0xFE, 0xFF, 0xC3, 0xC3, 0xFF, 0x7E, // 6
    0xFF, 0xFF, 0x03, 0x03, 0x07, 0x0E, 0x1C, 0x18, 0x18, 0x18, // 7 
    0x7E, 0xFF, 0xC3, 0xC3, 0x7E, 0x7E, 0xC3, 0xC3, 0xFF, 0x7E, // 8
    0x7E, 0xFF, 0xC3, 0xC3, 0xFF, 0x7F, 0x03, 0x07, 0x7E, 0x7C, // 9
    0x18, 0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, // A
    0xFE, 0xFF, 0xC3, 0xC3, 0xFE, 0xFE, 0xC3, 0xC3, 0xFF, 0xFE, // B
    0x3F, 0x7F, 0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0, 0x7F, 0x3F, // C
    0xFC, 0xFE, 0xC7, 0xC3, 0xC3, 0xC3, 0xC3, 0xC7, 0xFE, 0xFC, // D
    0x7F, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xFF, 0x7F, // E
    0x7F, 0xFF, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0};// F


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
    for (uint16_t i = 0x00; i < 0xA0; ++i) write(SUPER_FONT_LOCATION + i, super_font[i]); 

    // Set program counter to RAM.
    pc = RAM_LOCATION;
    draw_screen = false;

    last_key = 0xFF;

    super = false;
    active = true;
    stopped = false;
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


void Chip_8::load_texture(SDL_Renderer* renderer, SDL_Texture* texture, const vector<vector<bool>>& disp, int height, int width) {
    int pitch = width * height;
    uint32_t* pixels{};
    void* pix_ptr{pixels};

    SDL_LockTexture(texture, nullptr, static_cast<void**>(&pix_ptr), &pitch);
    pixels = static_cast<uint32_t*>(pix_ptr);

    for (const auto& row : disp) {
        for (const auto& on : row) {
            *pixels++ = (on) ? ON_COLOR : OFF_COLOR;
        }
    }
    pixels -= pitch;
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}


void Chip_8::draw_screen_array(vector<vector<bool>>& disp, int height, int width, uint8_t X, uint8_t Y, uint8_t N) {
    uint8_t x_coords = V[X] % width;
    uint8_t y_coords = V[Y] % height;
    V[0xF] = 0x00; // Set collision flag to false

    for (uint8_t n = 0; n < N; ++n, ++y_coords) {
        if (!clip) y_coords %= height;
        if (clip && y_coords >= height) break;
        auto x = x_coords;

        // Iterate through each bit left to right in sprite data
        for (uint8_t sp = read(I + n); sp > 0; (sp <<= 1), ++x) {
            if (!clip) x %= width;
            if (clip && x >= width) break;
            if (!(sp & 0x80)) continue;

            if (disp[y_coords][x]) V[0xF] = 0x01; // set collison flag to true
            disp[y_coords][x] = !disp[y_coords][x];
        }
    }
}


void Chip_8::scroll_down(vector<vector<bool>>& disp, int height, uint8_t N) {
    if (!N) return;
    for (int i = height - 1; i >= 0; --i) {
        if (i < N) std::fill(disp[i].begin(), disp[i].end(), 0);
        else disp[i] = disp[i - N];
    }
}


void Chip_8::scroll_up(vector<vector<bool>>& disp, int height, uint8_t N) {
    if (!N) return;
    for (int i = 0; i < height; ++i) {
        if (i > (height - 1 - N)) std::fill(disp[i].begin(), disp[i].end(), 0);
        else disp[i] = disp[i + N];
    }
}


void Chip_8::scroll_left(vector<vector<bool>>& disp, int width) {
    for (auto& row : disp) {
        for (int i = 0; i < width; ++i) {
            if (i > (width - 5)) row[i] = 0;
            else row[i] = row[i + 4];
        }
    }
}


void Chip_8::scroll_right(vector<vector<bool>>& disp, int width) {
    for (auto& row : disp) {
        for (int i = width - 1; i >= 0; --i) {
            if (i < 4) row[i] = 0;
            else row[i] = row[i - 4];
        }
    }
}


void Chip_8::draw_screen_array_sprite(vector<vector<bool>>& disp, int height, int width, uint8_t X, uint8_t Y) {
    uint8_t x_coords = V[X] % width;
    uint8_t y_coords = V[Y] % height;
    V[0xF] = 0x00; // Set collision flag to false

    for (uint16_t n = 0; n < 32; n += 2, ++y_coords) {
        if (!clip) y_coords %= height;
        if (clip && y_coords >= height) break;

        uint8_t left_half = read(I + n);
        uint8_t right_half = read(I + n + 1);

        auto x = x_coords;
        // Iterate through each bit left to right in left_half data
        for (uint8_t sp = left_half; sp > 0; (sp <<= 1), ++x) {
            if (!clip) x %= width;
            if (clip && x >= width) break;
            if (!(sp & 0x80)) continue;

            if (disp[y_coords][x]) V[0xF] = 0x01; // set collison flag to true
            disp[y_coords][x] = !disp[y_coords][x];
        }
        
        x = x_coords + 8;
        // Iterate through each bit left to right in right_half data
        for (uint8_t sp = right_half; sp > 0; (sp <<= 1), ++x) {
            if (!clip) x %= width;
            if (clip && x >= width) break;
            if (!(sp & 0x80)) continue;

            if (disp[y_coords][x]) V[0xF] = 0x01; // set collison flag to true
            disp[y_coords][x] = !disp[y_coords][x];
        }
    }
}
