#include "chip-8.h"
#include "debug.h"

constexpr auto PIXEL_SIZE = 10;

static Chip_8 chip8{};

int main(int, char* []) {
    SDL_Window* window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        PIXEL_SIZE * SCREEN_WIDTH, PIXEL_SIZE * SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    chip8.load_program("ROMs/spaceracer.ch8");

    bool active = true;
    SDL_Event event;

    while (active) {
        // Handling input events
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                active = false;
                break;
            case SDL_KEYDOWN:
                chip8.toggle_key(event.key.keysym.scancode, true);
                break;
            case SDL_KEYUP:
                chip8.toggle_key(event.key.keysym.scancode, false);
                break;
            }
        }

        // Main loop runs at 60Hz
        SDL_Delay(16);

        // Timers decrease at 60Hz
        chip8.delay -= (chip8.delay > 0);
        chip8.sound -= (chip8.sound > 0);

        // CPU runs at 720Hz
        for (int _ = 0; _ < 12; ++_) {
            chip8.fetch();
            current_op(chip8);
            chip8.decode_and_execute();
        }

        // Draw screen if draw flag is set
        if (!chip8.draw_screen) continue;
        chip8.draw_screen = false;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                if (!chip8.display[y][x]) continue;
                SDL_Rect rect{ x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}