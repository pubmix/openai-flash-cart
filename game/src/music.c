#include <gb/gb.h>
#include <stdint.h>

#include "music.h"

#define N_REST 0
#define N_C3   1424
#define N_D3   1490
#define N_E3   1534
#define N_G3   1602
#define N_A3   1648
#define N_C4   1712
#define N_D4   1776
#define N_E4   1812
#define N_G4   1890
#define N_A4   1936
#define N_B4   1968
#define N_C5   1984
#define N_D5   2016
#define N_E5   2028

typedef struct music_row_t {
    uint16_t bass;
    uint16_t lead;
    uint16_t wave;
    uint8_t drum;
    uint8_t lead_env;
} music_row_t;

enum {
    DRUM_NONE,
    DRUM_KICK,
    DRUM_SNARE,
    DRUM_HAT,
    DRUM_SPLASH
};

static const uint8_t soft_wave[] = {
    0x89, 0xBC, 0xEF, 0xFE, 0xCB, 0x86, 0x42, 0x10,
    0x12, 0x46, 0x8B, 0xDF, 0xFF, 0xDB, 0x84, 0x20
};

static const music_row_t song_rows[] = {
    {N_C3, N_E4, N_C4, DRUM_KICK,   0x74}, {0,    N_G4, 0,    DRUM_HAT,    0x53},
    {N_C3, N_A4, N_E4, DRUM_HAT,    0x63}, {0,    N_G4, 0,    DRUM_HAT,    0x53},
    {N_D3, N_E4, N_D4, DRUM_SNARE,  0x72}, {0,    N_G4, 0,    DRUM_HAT,    0x53},
    {N_D3, N_B4, N_A4, DRUM_HAT,    0x63}, {0,    N_A4, 0,    DRUM_SPLASH, 0x53},

    {N_E3, N_G4, N_E4, DRUM_KICK,   0x74}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_E3, N_C5, N_G4, DRUM_HAT,    0x63}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_D3, N_G4, N_D4, DRUM_SNARE,  0x72}, {0,    N_E4, 0,    DRUM_HAT,    0x53},
    {N_G3, N_D5, N_B4, DRUM_HAT,    0x63}, {0,    N_C5, 0,    DRUM_SPLASH, 0x53},

    {N_C3, N_E4, N_C4, DRUM_KICK,   0x74}, {0,    N_G4, 0,    DRUM_HAT,    0x53},
    {N_C3, N_A4, N_E4, DRUM_HAT,    0x63}, {0,    N_C5, 0,    DRUM_HAT,    0x53},
    {N_A3, N_B4, N_A4, DRUM_SNARE,  0x72}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_G3, N_G4, N_D4, DRUM_HAT,    0x63}, {0,    N_E4, 0,    DRUM_SPLASH, 0x53},

    {N_E3, N_G4, N_E4, DRUM_KICK,   0x74}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_D3, N_C5, N_G4, DRUM_HAT,    0x63}, {0,    N_D5, 0,    DRUM_HAT,    0x53},
    {N_C3, N_C5, N_C4, DRUM_SNARE,  0x72}, {0,    N_G4, 0,    DRUM_HAT,    0x53},
    {N_G3, N_A4, N_D4, DRUM_HAT,    0x63}, {0,    N_E4, 0,    DRUM_SPLASH, 0x53},

    {N_C3, N_C5, N_E4, DRUM_KICK,   0x75}, {0,    N_D5, 0,    DRUM_HAT,    0x54},
    {N_D3, N_E5, N_A4, DRUM_HAT,    0x64}, {0,    N_D5, 0,    DRUM_HAT,    0x54},
    {N_E3, N_C5, N_G4, DRUM_SNARE,  0x73}, {0,    N_A4, 0,    DRUM_HAT,    0x54},
    {N_G3, N_D5, N_B4, DRUM_HAT,    0x64}, {0,    N_C5, 0,    DRUM_SPLASH, 0x54},

    {N_A3, N_A4, N_A4, DRUM_KICK,   0x75}, {0,    N_C5, 0,    DRUM_HAT,    0x54},
    {N_G3, N_D5, N_G4, DRUM_HAT,    0x64}, {0,    N_C5, 0,    DRUM_HAT,    0x54},
    {N_E3, N_A4, N_E4, DRUM_SNARE,  0x73}, {0,    N_G4, 0,    DRUM_HAT,    0x54},
    {N_D3, N_E4, N_D4, DRUM_HAT,    0x64}, {0,    N_G4, 0,    DRUM_SPLASH, 0x54},

    {N_C3, N_E4, N_C4, DRUM_KICK,   0x74}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_E3, N_C5, N_G4, DRUM_HAT,    0x64}, {0,    N_D5, 0,    DRUM_HAT,    0x54},
    {N_G3, N_E5, N_B4, DRUM_SNARE,  0x73}, {0,    N_D5, 0,    DRUM_HAT,    0x54},
    {N_A3, N_C5, N_A4, DRUM_HAT,    0x64}, {0,    N_A4, 0,    DRUM_SPLASH, 0x54},

    {N_G3, N_G4, N_D4, DRUM_KICK,   0x74}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_E3, N_C5, N_G4, DRUM_HAT,    0x64}, {0,    N_A4, 0,    DRUM_HAT,    0x53},
    {N_D3, N_D5, N_D4, DRUM_SNARE,  0x73}, {0,    N_C5, 0,    DRUM_HAT,    0x53},
    {N_C3, N_G4, N_C4, DRUM_HAT,    0x63}, {0,    N_E4, 0,    DRUM_SPLASH, 0x53}
};

static uint8_t music_frame;
static uint8_t music_row;

static void trigger_bass(uint16_t note) {
    if (!note) return;
    NR10_REG = 0x00U;
    NR11_REG = 0xC0U;
    NR12_REG = 0x93U;
    NR13_REG = (uint8_t)(note & 0xFFU);
    NR14_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_lead(uint16_t note, uint8_t env) {
    if (!note) return;
    NR21_REG = 0x40U;
    NR22_REG = env;
    NR23_REG = (uint8_t)(note & 0xFFU);
    NR24_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_wave(uint16_t note) {
    if (!note) return;
    NR30_REG = 0x00U;
    for (uint8_t i = 0; i != 16; ++i) {
        *((uint8_t *)0xFF30U + i) = soft_wave[i];
    }
    NR30_REG = 0x80U;
    NR31_REG = 0xA0U;
    NR32_REG = 0x40U;
    NR33_REG = (uint8_t)(note & 0xFFU);
    NR34_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_drum(uint8_t drum) {
    switch (drum) {
        case DRUM_KICK:
            NR41_REG = 0x08U;
            NR42_REG = 0x82U;
            NR43_REG = 0x63U;
            break;
        case DRUM_SNARE:
            NR41_REG = 0x14U;
            NR42_REG = 0x74U;
            NR43_REG = 0x34U;
            break;
        case DRUM_HAT:
            NR41_REG = 0x03U;
            NR42_REG = 0x23U;
            NR43_REG = 0x12U;
            break;
        case DRUM_SPLASH:
            NR41_REG = 0x18U;
            NR42_REG = 0x53U;
            NR43_REG = 0x42U;
            break;
        default:
            return;
    }
    NR44_REG = 0x80U;
}

static void play_row(uint8_t row_index) {
    const music_row_t *row = &song_rows[row_index];
    trigger_bass(row->bass);
    trigger_lead(row->lead, row->lead_env);
    trigger_wave(row->wave);
    trigger_drum(row->drum);
}

void music_init(void) {
    NR52_REG = 0x80U;
    NR50_REG = 0x77U;
    NR51_REG = 0xFFU;

    music_frame = 0;
    music_row = 0;
    play_row(0);
}

void music_update(void) {
    ++music_frame;
    if (music_frame < 7U) return;

    music_frame = 0;
    music_row = (music_row + 1U) & 0x3FU;
    play_row(music_row);
}
