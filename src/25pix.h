/*
 *
 *  25pix game console
 *  Björn Westerberg Nauclér 2022
 *  MIT License
 *
 *  25pix.h - Includes, global variables  and forward declarations
 *
 */

#include "25pix_config.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

extern uint8_t gm;                      // Current mode
extern uint8_t pgm;                     // Previous mode
extern uint8_t pos;                     // Cursor position

extern uint8_t apin;                    // Current audio pin
extern uint8_t sndst;                   // Sound state (on/off)
extern uint8_t sndlen;                  // Note length (in ms)

extern uint8_t brtns;                   // Screen brightness

extern uint8_t grid[PXSZY][PXSZX];      // State of pixel [Y][X]

extern uint8_t r_seq[NUMPX];            // RMBR sequence
extern uint8_t r_clev, r_ctr;           // RMBR current level & counter

extern uint32_t gptimer;                // RMBR sequence timer
extern uint32_t bttimer;                // button press timer
extern uint32_t dtimer;                 // draw timer

extern Adafruit_NeoPixel px;

// Forward declarations - main.cpp
extern void cleargrid();
extern void setpos(const uint8_t y, const uint8_t x);
extern void draw();

// Forward declarations - flip.cpp
extern void flipbit(const uint8_t y, const uint8_t x);
extern void f_flip(const uint8_t ty, const uint8_t tx);
extern uint8_t f_win();
extern void f_initgrid();
extern void f_drawpix(const uint8_t pix);
extern void f_loop();

