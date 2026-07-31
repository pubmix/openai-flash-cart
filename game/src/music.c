#include <gb/gb.h>
#include <stdint.h>

#include "music.h"

static const uint8_t soft_wave[] = {
    0x89, 0xAB, 0xCD, 0xEE, 0xFE, 0xDC, 0xBA, 0x98,
    0x76, 0x54, 0x32, 0x11, 0x01, 0x23, 0x45, 0x67
};

static const uint16_t pad_notes[] = {
    1776, 1812, 1856, 1812, 1890, 1856, 1812, 1776,
    1746, 1776, 1812, 1856, 1890, 1856, 1812, 1776
};

static const uint16_t bell_notes[] = {
    1890, 0, 1918, 0, 1856, 0, 1936, 0,
    1812, 0, 1890, 0, 1856, 0, 1918, 0
};

static uint8_t music_frame;
static uint8_t music_step;

static void trigger_wave(uint16_t note) {
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

static void trigger_bell(uint16_t note) {
    if (!note) return;
    NR21_REG = 0x80U;
    NR22_REG = 0x63U;
    NR23_REG = (uint8_t)(note & 0xFFU);
    NR24_REG = 0x80U | (uint8_t)(note >> 8);
}

static void trigger_bubble(void) {
    NR41_REG = 0x08U;
    NR42_REG = 0x21U;
    NR43_REG = 0x52U;
    NR44_REG = 0x80U;
}

void music_init(void) {
    NR52_REG = 0x80U;
    NR50_REG = 0x66U;
    NR51_REG = 0xFFU;

    music_frame = 0;
    music_step = 0;
    trigger_wave(pad_notes[0]);
}

void music_update(void) {
    ++music_frame;
    if (music_frame < 30U) return;

    music_frame = 0;
    music_step = (music_step + 1U) & 0x0FU;
    trigger_wave(pad_notes[music_step]);
    trigger_bell(bell_notes[music_step]);
    if ((music_step & 0x03U) == 0x02U) {
        trigger_bubble();
    }
}
