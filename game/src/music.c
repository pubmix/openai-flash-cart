#include <gb/gb.h>
#include <stdint.h>

#include "music.h"

static const uint8_t soft_wave[] = {
    0x8A, 0xCD, 0xFE, 0xDC, 0x98, 0x76, 0x54, 0x32,
    0x10, 0x12, 0x45, 0x79, 0xBD, 0xFF, 0xDB, 0x97
};

static const uint16_t bass_notes[] = {
    1424, 0, 1424, 0, 1490, 0, 1490, 0,
    1568, 0, 1534, 0, 1490, 0, 1602, 0
};

static const uint16_t lead_notes[] = {
    1812, 1890, 1936, 1890, 1856, 1936, 1984, 1936,
    1890, 1968, 2016, 1968, 1936, 1890, 1856, 1812
};

static const uint16_t wave_notes[] = {
    1776, 0, 1856, 0, 1936, 0, 1856, 0,
    1812, 0, 1890, 0, 1968, 0, 1890, 0
};

static const uint8_t kick_steps[] = {
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 1
};

static const uint8_t snare_steps[] = {
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0
};

static uint8_t music_frame;
static uint8_t music_step;

static void trigger_bass(uint16_t note) {
    if (!note) return;
    NR10_REG = 0x00U;
    NR11_REG = 0x80U;
    NR12_REG = 0x84U;
    NR13_REG = (uint8_t)(note & 0xFFU);
    NR14_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_wave(uint16_t note) {
    if (!note) return;
    NR30_REG = 0x00U;
    for (uint8_t i = 0; i != 16; ++i) {
        *((uint8_t *)0xFF30U + i) = soft_wave[i];
    }
    NR30_REG = 0x80U;
    NR31_REG = 0x80U;
    NR32_REG = 0x40U;
    NR33_REG = (uint8_t)(note & 0xFFU);
    NR34_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_lead(uint16_t note) {
    if (!note) return;
    NR21_REG = 0x40U;
    NR22_REG = 0x74U;
    NR23_REG = (uint8_t)(note & 0xFFU);
    NR24_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_noise(uint8_t snare) {
    NR41_REG = snare ? 0x12U : 0x08U;
    NR42_REG = snare ? 0x62U : 0x72U;
    NR43_REG = snare ? 0x35U : 0x63U;
    NR44_REG = 0x80U;
}

void music_init(void) {
    NR52_REG = 0x80U;
    NR50_REG = 0x77U;
    NR51_REG = 0xFFU;

    music_frame = 0;
    music_step = 0;
    trigger_bass(bass_notes[0]);
    trigger_wave(wave_notes[0]);
    trigger_lead(lead_notes[0]);
    trigger_noise(0);
}

void music_update(void) {
    ++music_frame;
    if (music_frame < 10U) return;

    music_frame = 0;
    music_step = (music_step + 1U) & 0x0FU;
    trigger_bass(bass_notes[music_step]);
    trigger_wave(wave_notes[music_step]);
    trigger_lead(lead_notes[music_step]);
    if (kick_steps[music_step]) {
        trigger_noise(0);
    } else if (snare_steps[music_step]) {
        trigger_noise(1);
    } else if ((music_step & 0x01U) != 0) {
        NR41_REG = 0x04U;
        NR42_REG = 0x12U;
        NR43_REG = 0x22U;
        NR44_REG = 0x80U;
    }
}
