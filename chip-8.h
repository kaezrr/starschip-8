#pragma once

#include <SDL.h>

#include<array>
#include<vector>
#include<stack>
#include<string>
#include<cstddef>


using std::array;
using std::vector;
using std::size_t;

constexpr size_t MEM_SIZE = 4096;
constexpr size_t REG_SIZE = 16;
constexpr size_t NUM_KEYS = 16;

constexpr uint16_t RAM_LOCATION = 0x200;
constexpr uint16_t SUPER_FONT_LOCATION = 0x50;

constexpr int SCREEN_HEIGHT = 32;
constexpr int SCREEN_WIDTH = 64;

constexpr int SUPER_HEIGHT = 64;
constexpr int SUPER_WIDTH = 128;

constexpr uint32_t ON_COLOR = 0xCB'87'00'FF;
constexpr uint32_t OFF_COLOR = 0x52'1A'00'FF;

class Chip_8 {
private:
    uint16_t opcode{};
    uint16_t pc{}, I{};

    std::stack<uint16_t> stack{};
    array<uint8_t, REG_SIZE> V{};
    array<bool, NUM_KEYS> keyboard{};
    array<uint8_t, MEM_SIZE> memory{};
    
public:
    bool active{};
    bool super{};
    bool stopped{};

    enum Flags {
        RESET_VF = 1,
        MEM_INCREMENT = 2,
        DISPLAY_WAIT = 4,
        CLIPPING = 8,
        SHIFTING = 16,
        JUMPING = 32,
    };

    // Flags:
    bool vf_reset{};
    bool mem_incr{};
    bool disp_wait{};
    bool clip{};
    bool shift{};
    bool jump{};

    uint16_t last_key{};
    bool draw_screen{};
    uint8_t delay{}, sound{};

    vector<vector<bool>> display{SCREEN_HEIGHT, vector<bool>(SCREEN_WIDTH, 0)};
    vector<vector<bool>> super_display{SUPER_HEIGHT, vector<bool>(SUPER_WIDTH, 0)};

    Chip_8() = delete;
    Chip_8(const Chip_8&) = delete;

    Chip_8(uint32_t argument);
    uint8_t read(uint16_t at) const;
    void write(uint16_t at, uint8_t data);

    void fetch();
    void decode_and_execute();
    void load_program(const std::string& path);
    void toggle_key(const SDL_Scancode& sc, bool on);

    void draw_screen_array(vector<vector<bool>>& disp, int height, int width, uint8_t X, uint8_t Y, uint8_t N);
    void draw_screen_array_sprite(vector<vector<bool>>& disp, int height, int width, uint8_t X, uint8_t Y);
    void load_texture(SDL_Renderer* renderer, SDL_Texture* texture, const vector<vector<bool>>& disp, int height, int width);

    void scroll_down(vector<vector<bool>>& disp, int height, uint8_t N);
    void scroll_up(vector<vector<bool>>& disp, int height, uint8_t N);
    void scroll_left(vector<vector<bool>>& disp, int width);
    void scroll_right(vector<vector<bool>>& disp, int width);

    // Debug
    friend void current_op(const Chip_8& chip8);
    friend void print_registers(const Chip_8& chip8);
    friend void print_keys(const Chip_8& chip8);
};




