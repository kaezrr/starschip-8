#pragma once

#include<array>
#include<stack>
#include<string>
#include<cstddef>


using std::array;
using std::uint8_t;
using std::uint16_t;
using std::size_t;

constexpr size_t MEM_SIZE = 4096;
constexpr size_t REG_SIZE = 16;
constexpr size_t SCREEN_HEIGHT = 32;
constexpr size_t SCREEN_WIDTH = 64;
constexpr size_t TIMER_FREQ = 60;
constexpr size_t RAM_LOCATION = 0x200;


class Chip_8 {
private:
    uint16_t pc{}, I{};
    uint8_t delay{}, sound{};

    std::stack<uint16_t> stack{};
    array<uint8_t, REG_SIZE> V{};
    array<uint8_t, MEM_SIZE> memory{};
    array<array<bool, SCREEN_WIDTH>, SCREEN_HEIGHT> display{};

public:
    bool draw_screen{};
    uint16_t opcode{};

    Chip_8();
    uint8_t read(uint16_t at) const;
    void write(uint16_t at, uint8_t data);
    bool get_pixel(size_t x, size_t y) const;

    void fetch();
    void decode_and_execute();
    void load_program(const std::string& path);
};


