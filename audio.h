#pragma once

constexpr int SAMPLE_RATE = 48000;
constexpr int BUFFER_SIZE = 512;

constexpr int WAVE_FREQ = 750;
constexpr int WAVE_AMP = 1;
constexpr float VOLUME = 0.1f;

constexpr int FULL_PERIOD = (SAMPLE_RATE / WAVE_FREQ);
constexpr int HALF_PERIOD = FULL_PERIOD / 2;

size_t current_cycle = 0;

float get_next_sample() {
    float sample = (current_cycle <= HALF_PERIOD)? WAVE_AMP : -WAVE_AMP;
    current_cycle = (current_cycle >= FULL_PERIOD) ? 0 : current_cycle + 1;
    return sample * VOLUME;
}

void callback(void*, uint8_t* stream, int len) {
    auto* fstream{ reinterpret_cast<float*>(stream) };
    len /= sizeof(float);
    for (int i = 0; i < len; ++i) {
        fstream[i] = get_next_sample();
    }
}
