#include "chip-8.h"
#include "debug.h"
#include "audio.h"

static constexpr auto PIXEL_SIZE = 10;
static int pitch = SCREEN_WIDTH * 4;
static constexpr uint32_t ON_COLOR = 0xCB'87'00'FF;
static constexpr uint32_t OFF_COLOR = 0x52'1A'00'FF;


static Chip_8 chip8{
    Chip_8::RESET_VF |
    Chip_8::MEM_INCREMENT |
    Chip_8::CLIPPING 
};


int main(int, char*[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        PIXEL_SIZE * SCREEN_WIDTH, PIXEL_SIZE * SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_AudioSpec spec{
    .freq = SAMPLE_RATE,
    .format = AUDIO_F32,
    .channels = 1,
    .samples = BUFFER_SIZE,
    .callback = callback
    };

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);

    chip8.load_program("roms/games/br8kout.ch8");

    bool active = true;
    SDL_Event event{};
    uint32_t* pixels{};


    while (active) {
        // Main loop runs at 60Hz
        SDL_Delay(16);

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

        SDL_PauseAudioDevice(audio_device, (chip8.sound <= 0));

        // Timers decrease at 60Hz
        chip8.delay -= (chip8.delay > 0);
        chip8.sound -= (chip8.sound > 0);

        // CPU runs at 720Hz
        for (int _ = 0; _ < 12; ++_) {
            chip8.fetch();
            chip8.decode_and_execute();
            if (chip8.disp_wait && chip8.draw_screen) break;
        }

        // Draw screen if draw flag is set
        if (!chip8.draw_screen) continue;
        chip8.draw_screen = false;

        void* pix_ptr{ pixels };
        SDL_LockTexture(texture, nullptr, reinterpret_cast<void**>(&pix_ptr), &pitch);
        pixels = reinterpret_cast<uint32_t*>(pix_ptr);

        for (const auto& row : chip8.display) {
            for (const auto& on : row) {
                *pixels++ = (on)? ON_COLOR : OFF_COLOR;
            }
        } 

        pixels -= SCREEN_WIDTH * SCREEN_HEIGHT;
        SDL_UnlockTexture(texture);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}