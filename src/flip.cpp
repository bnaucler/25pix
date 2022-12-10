/*
 *  25pix game console
 *  Björn Westerberg Nauclér 2022
 *  MIT License
 *
 *  Game: flip
 *  Macro prefix: F
 *  Function prefix: f
 *
 */

#include "25pix_config.h"
#include "25pix.h"
#include <Arduino.h>

#define F_SCRAM 8           // Number of flips for initial scramble

// flip individual bit
void flipbit(const uint8_t y, const uint8_t x) {

    if(x >= PXSZX || y >= PXSZY || x < 0 || y < 0) return;

    grid[y][x] = 1 - grid[y][x];
}

// flip: flip bit and adjacent bits
void f_flip(const uint8_t ty, const uint8_t tx) {

    if(tx >= PXSZX || ty >= PXSZY || tx < 0 || ty < 0) return; // TODO: needed?

    for(int y = ty - 1; y < ty + 2; y++) {
        for(int x = tx - 1; x < tx + 2; x++) {
            flipbit(y, x);
        }
    }
}

// flip: returns 1 if winning condition is met
uint8_t f_win() {

    for(int i = 0; i < NUMPX; i++) {
        if(grid[i / PXSZX][i % PXSZX]) return 0;
    }

    return 1;
}

// Initializes flip grid
void f_initgrid() {

    cleargrid();
    uint8_t x, y;

    for(uint8_t i = 0; i < F_SCRAM; i++) {
      x = (random(PXSZX));
      y = (random(PXSZY));
      f_flip(y, x);
    }
}

// draw individual pixel for flip
void f_drawpix(const uint8_t pix) {

    uint32_t c = C_OFF;
    uint8_t p = grid[pix / PXSZX][pix % PXSZX];

    c = p ? C_RED : C_BLUE;

    if(pix == pos && p) c = C_YELLOW;
    else if(pix == pos) c = C_PURPLE;

    px.setPixelColor(pix, c);
}

// Flip: Main loop
void f_loop() {

    if(pgm != M_FLIP) {
        Serial.println("Launching game: Flip");
        f_initgrid();
        setpos(CPOSY, CPOSX);
        pgm = M_FLIP;
    }

    draw();
    dtimer = millis();
}

