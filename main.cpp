// Remove this flag in the final code.
#pragma warning(push)
#pragma warning(disable : 26819)
#include <SDL.h>
#pragma warning(pop)

#include "chip-8.h"
#include "debug.h"

constexpr auto PIXEL_SIZE = 10;

static Chip_8 chip8{};

int main(int, char* []) {
    SDL_Window* window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, PIXEL_SIZE * SCREEN_WIDTH, PIXEL_SIZE * SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    chip8.load_program("programs/IBM_Logo.ch8");

    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;

        chip8.fetch();
        chip8.decode_and_execute();

        if (!chip8.draw_screen) continue;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                if (!chip8.get_pixel(x, y)) continue;
                SDL_Rect rect{ x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);
        chip8.draw_screen = false;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}