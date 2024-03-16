#pragma once

// Remove this flag in the final code.
#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL.h>
#pragma warning(pop)

#include<array>
#include<stack>
#include<string>
#include<cstddef>


using std::array;
using std::size_t;

constexpr size_t MEM_SIZE = 4096;
constexpr size_t REG_SIZE = 16;
constexpr size_t SCREEN_HEIGHT = 32;
constexpr size_t SCREEN_WIDTH = 64;
constexpr size_t RAM_LOCATION = 0x200;
constexpr size_t NUM_KEYS = 16;

class Chip_8 {
private:
    uint16_t opcode{};
    uint16_t pc{}, I{};

    std::stack<uint16_t> stack{};
    array<uint8_t, REG_SIZE> V{};
    array<bool, NUM_KEYS> keyboard{};
    array<uint8_t, MEM_SIZE> memory{};

public:
    bool draw_screen{};
    uint8_t delay{}, sound{};
    array<array<bool, SCREEN_WIDTH>, SCREEN_HEIGHT> display{};

    Chip_8();
    uint8_t read(uint16_t at) const;
    void write(uint16_t at, uint8_t data);

    void fetch();
    void decode_and_execute();
    void load_program(const std::string& path);
    void toggle_key(const SDL_Scancode& sc, bool on);

    // Debug
    friend void current_op(const Chip_8& chip8);
    friend void print_registers(const Chip_8& chip8);
    friend void print_keys(const Chip_8& chip8);
};


