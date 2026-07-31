#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "assets/logo_assets.h"

static const palette_color_t palettes[] = {
    RGB8(248, 248, 232), RGB8(168, 188, 136), RGB8(76, 92, 80), RGB8(8, 12, 14)
};

static const uint8_t blank_tile[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static void wait_frames(uint16_t frames) {
    while (frames--) {
        wait_vbl_done();
    }
}

static void blank_screen(void) {
    HIDE_BKG;
    set_bkg_data(0, 1, blank_tile);
    fill_bkg_rect(0, 0, 20, 18, 0);
    SHOW_BKG;
}

static void show_logo(const uint8_t *tiles, uint8_t tile_count, const uint8_t *map) {
    HIDE_BKG;
    set_bkg_data(0, tile_count, tiles);
    set_bkg_tiles(0, 0, LOGO_MAP_WIDTH, LOGO_MAP_HEIGHT, map);
    SHOW_BKG;
}

void main(void) {
    set_bkg_palette(0, 1, palettes);
    BGP_REG = 0xE4U;
    DISPLAY_ON;

    show_logo(modretro_tiles, MODRETRO_TILE_COUNT, modretro_map);
    wait_frames(150);

    blank_screen();
    wait_frames(18);

    show_logo(openai_tiles, OPENAI_TILE_COUNT, openai_map);
    wait_frames(150);

    blank_screen();

    while (1) {
        wait_vbl_done();
    }
}
