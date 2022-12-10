/*
 *  25pix game console
 *  Björn Westerberg Nauclér 2022
 *  MIT License
 *
 *  Core functionality
 *
 */

#include "25pix_config.h"
#include "25pix.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

uint8_t INPUTS[] = {UBTN, LBTN, RBTN, DBTN, SBTN};
uint8_t OUTPUTS[] = {PPIN, AIPIN, AEPIN, TLPIN};
uint8_t BTNS[] = {UBTN, LBTN, RBTN, DBTN, SBTN};

uint8_t gm;                     // Current mode
uint8_t pgm;                    // Previous mode
uint8_t pos;                    // Cursor position

uint8_t apin;                   // Current audio pin
uint8_t brtns;                  // Screen brightness

uint8_t grid[PXSZY][PXSZX];     // State of pixel [Y][X]

uint8_t r_seq[NUMPX];           // RMBR sequence
uint8_t r_clev, r_ctr;          // RMBR current level & counter
uint8_t r_gctr;                 // RMBR uessing counter

uint32_t gptimer = 0;           // RMBR sequence timer
uint32_t bttimer = 0;           // button press timer
uint32_t dtimer = 0;            // draw timer

Adafruit_NeoPixel px(NUMPX, PPIN, NEO_GRB + NEO_KHZ800);

// Debug func - prints grid to serial
void d_printgrid(const size_t dvar) {

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            if(y * 5 + x == pos) Serial.print('X');
            else Serial.print(grid[y][x]);
            if(x < PXSZX - 1) Serial.print('\t');
        }
        Serial.print('\n');
    }

    Serial.print('\n');
    Serial.print("gm: ");
    Serial.println(gm);
    Serial.print("pgm: ");
    Serial.println(pgm);
    Serial.print("debug var: ");
    Serial.println(dvar);

}

// Clears grid
void cleargrid() {

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            grid[y][x] = 0;
        }
    }
}

// draw individual pixel for menu
void m_drawpix(const uint8_t pix) {

    uint32_t c = C_OFF;

    if(pix == pos) c = C_BLUE;

    px.setPixelColor(pix, c);
}

// draw individual pixel for win animation
void w_drawpix(const uint8_t pix) {

    uint32_t c = C_OFF;

    if(grid[pix / PXSZX][pix % PXSZX] % 2) c = C_BLUE;
    else if(grid[pix / PXSZX][pix % PXSZX] % 4) c = C_GREEN;
    else if(grid[pix / PXSZX][pix % PXSZX] % 8) c = C_PINK;

    px.setPixelColor(pix, c);
}

// RMBR: Generates new dequen
void r_genseq(const uint8_t lev) {

    Serial.print("Sequence: ");

    for(int x = 0; x < lev; x++) {
        r_seq[x] = random(PXSZX);
        Serial.print(r_seq[x]);
        Serial.print(" ");
    }

    Serial.print('\n');
}

// draw individual pixel for RMBR
void r_drawpix(const uint8_t pix, const uint8_t col) {

    uint32_t c = C_OFF;

    if(grid[pix / PXSZX][pix % PXSZX]) {
        switch(col) {
            case 0:
                c = C_ORANGE;
                break;

            case 1:
                c = C_PURPLE;
                break;

            case 2:
                c = C_GREEN;
                break;

            case 3:
                c = C_PINK;
                break;

            case 4:
                c = C_BLUE;
                break;

            default:
                break;
        }
    }

    px.setPixelColor(pix, c);
}

// RMBR input guess
uint8_t r_guess() {

    uint8_t guess = pos % PXSZX;
    char buf[32]; // TODO: define buffer size

    snprintf(buf, 32, "Guess: %d, Corr: %d\n", guess, r_seq[r_gctr]);
    Serial.print(buf);

    if(guess == r_seq[r_gctr]) {

        r_gctr++;

        if(r_gctr == r_clev) {
            r_clev++;
            r_ctr = r_gctr = 0;
            r_genseq(r_clev);
            // TODO: Win melody / animation
        }

    } else {

        gm = M_MENU;            // Return to menu
        /* pgm = M_UNUSED; */   // Restart game
    }



    return 0;
}

// draw individual pixel for flashlight
void fl_drawpix(const uint8_t pix) {

    px.setPixelColor(pix, C_WHITE);
}

// Wrapper for draw functions
void draw() {

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            switch(gm) {
                case(M_FLIP):
                    f_drawpix(y * PXSZX + x);
                    break;

                case(M_RMBR):
                    r_drawpix(y * PXSZX + x, x);
                    break;

                case(M_WIN):
                    w_drawpix(y * PXSZX + x);
                    break;

                case(M_BRTNS): // fallthrough
                case(M_FLASH):
                    fl_drawpix(y * PXSZX + x);
                    break;

                default: // Default to menu screen
                    m_drawpix(y * PXSZX + x);
                    break;

            }
        }
    }

    px.setBrightness(brtns);
    px.show();
}

// set fixed cursor position
void setpos(const uint8_t y, const uint8_t x) {

    pos = (y * PXSZX) + x;
}

// update relative cursor position
uint8_t chpos(const int y, const int x) {

    if((y * PXSZX + pos) < PXSZX * PXSZY &&
       (y * PXSZX + pos) >= 0) pos += (y * PXSZX);
    else return 1;

    if((pos % PXSZX + x) < PXSZX &&
       (pos % PXSZX + x) >= 0 ) pos += x;
    else return 1;

    return 0;
}

// Return new brightness level
uint8_t b_set(const int n) {

    int ret = brtns + n;

    if(ret < MN_BRTNS) ret = MN_BRTNS;
    else if(ret > MX_BRTNS) ret = MX_BRTNS;

    Serial.print("Current brightness: ");
    Serial.println(ret);

    return ret;
}

uint8_t b_select() {

    d_printgrid(0);

    tone(apin, 440, 20);

    if(gm == M_MENU && pos <= M_MAX) {
        gm = pos;

    } else if(gm == M_FLIP) {
        f_flip(pos / PXSZX, pos % PXSZX);
        if(f_win()) {
            pgm = gm;
            gm = M_WIN;
        }

    } else if(gm == M_RMBR) {
        r_guess();

    } else if(gm == M_FLASH || gm == M_BRTNS) {
        gm = M_MENU;
    }

    return 0;
}

uint8_t b_up() {

    Serial.println("UP");

    if(gm == M_FLIP || gm == M_MENU) chpos(-1, 0);
    if(gm == M_BRTNS) brtns = b_set(+5);

    return 0;
}

uint8_t b_down() {

    Serial.println("DOWN");

    if(gm == M_FLIP || gm == M_MENU) chpos(1, 0);
    if(gm == M_BRTNS) brtns = b_set(-5);

    return 0;
}

uint8_t b_left() {

    Serial.println("LEFT");

    if(gm == M_FLIP || gm == M_MENU || gm == M_RMBR) chpos(0, -1);

    return 0;
}

uint8_t b_right() {

    Serial.println("RIGHT");

    if(gm == M_FLIP || gm == M_MENU || gm == M_RMBR) chpos(0, 1);

    return 0;
}

// Restore grid state from EEPROM on reset
void restore_grid() {

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            grid[y][x] = EEPROM.read(y * PXSZX + x + ADDR_GRID);
        }
    }
}

// Read button states into individual bits
uint8_t readbtns() {

    uint8_t ret = 0;

    for(int i = 0; i < NUMBTNS; i++) {
        if(!digitalRead(BTNS[i])) ret |= 1 << i;
    }

    return ret;
}

// Generate logic pattern for animation at game win
uint16_t animate() {

    static uint16_t ctr = 0;
    uint8_t dx, dy;

    if(!ctr) cleargrid();
    grid[CPOSY][CPOSX]++; // need to update center pixel first

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            if(y == CPOSY && x == CPOSX) continue;

            dy = abs(y - CPOSY);
            dx = abs(x - CPOSX);

            grid[y][x] = grid[CPOSY][CPOSX] - (dy > dx ? dy : dx);

            if(grid[y][x] > grid[CPOSY][CPOSX]) grid[y][x] = 0;
        }
    }

    uint16_t tn = map(ctr, 0, A_DONE, 50, 800);
    tone(apin, tn, A_DEL);

    if(++ctr >= A_DONE) ctr = 0;

    draw();
    return ctr;
}

// Compare current and previous button states
void procinput(const uint8_t btnst, const uint16_t diff) {

    if(diff < DBNC) return;

    else if(diff > DBNC && diff < LPTIME) {
        if(btnst > 15) b_select();
        else if(btnst > 7) b_down();
        else if(btnst > 3) b_right();
        else if(btnst > 1) b_left();
        else if(btnst != 0) b_up();
    }

    else if(btnst > 15) {
        Serial.println("Long press SELECT");
        gm = M_MENU;
    }

    return;
}

// Main menu loop
void m_loop() {

    if(pgm != M_MENU) {
        Serial.println("Entering main menu");
        cleargrid();
        setpos(0,0);
        pgm = M_MENU;
    }
    draw();
    dtimer = millis();
}

// Animation loop
void a_loop() {

    if(!animate()) {
        gm = pgm;
        pgm = M_WIN;
    }
    dtimer = millis();
}

// RMBR: Plays sequence
void r_pseq(const uint8_t pix) {

    for(int y = 0; y < PXSZY; y++) {
        for(int x = 0; x < PXSZX; x++) {
            if(x == pix) grid[y][x] = 1;
            else grid[y][x] = 0;
        }
    }

    uint16_t tn;

    switch(pix) {
        case(0):
            tn = NOTE_C;
        break;

        case(1):
            tn = NOTE_D;
        break;

        case(2):
            tn = NOTE_E;
        break;

        case(3):
            tn = NOTE_F;
        break;

        case(4):
        default:
            tn = NOTE_G;
        break;
    }

    tone(apin, tn, R_SSTP - R_NTOFF);
}

// Brightness setting + main loop
void b_loop() {

    if(pgm != M_BRTNS) {
        cleargrid();
        pgm = M_BRTNS;
    }

    draw();
}

// RMBR: Main loop
void r_loop() {

    static uint8_t lpix = 0; // Currently displaying last pixel

    if(pgm != M_RMBR) {
        r_clev = R_SLEV;
        r_ctr = 0;

        cleargrid();
        r_genseq(r_clev);

        pgm = M_RMBR;
    }

    if(lpix && millis() > gptimer + R_SSTP) lpix = 0;

    // Playing sequence
    if(r_ctr < r_clev && millis() > gptimer + R_SSTP) {
        r_pseq(r_seq[r_ctr]);
        r_ctr++;
        gptimer = millis();

        if(r_ctr == r_clev) {
            setpos(0, CPOSX);
            r_gctr = 0;
            lpix = 1;
        }

    // Guessing
    } else if(r_ctr == r_clev && !lpix) {
        cleargrid();
        for(int y = 0; y < PXSZY; y ++) grid[y][pos % PXSZX] = 1;
    }

    draw();
}

// Start flashlight - not a loop, but following naming convention
void fl_loop() {

    Serial.println("Starting flashlight");

    pgm = M_FLASH;
    draw();
}

// Save state to EEPROM
void save_state() {

    EEPROM.write(ADDR_GM, pgm); // TODO: currently saves menu
    EEPROM.write(ADDR_APIN, apin);
    EEPROM.write(ADDR_BRTNS, brtns);

    for(int i = 0; i < NUMPX; i++)
        EEPROM.write(i + ADDR_GRID, grid[i / PXSZX][i % PXSZX]);

    gm = pgm;
    pgm = M_SAVE;
    Serial.println("State saved");

}

// Toggle audio output
void apin_toggle() {

    noTone(AEPIN);
    noTone(AIPIN);
    apin = apin == AEPIN ? AIPIN : AEPIN;
    tone(apin, 440, 100);
    gm = pgm;
    pgm = M_ATOG; // ?

    d_printgrid(apin);

    if(apin == AEPIN) Serial.println("Audio: External output");
    else Serial.println("Audio: Built-in buzzer");
}

void setup() {

    randomSeed(analogRead(A2));

    gm = EEPROM.read(ADDR_GM);
    pgm = M_UNUSED;
    apin = EEPROM.read(ADDR_APIN);
    brtns = EEPROM.read(ADDR_BRTNS);
    restore_grid(); // TODO: test

    if(pgm == gm) pgm = M_UNUSED; // TODO: will clear board on reset - fix
    if(brtns < MN_BRTNS) brtns = MN_BRTNS;
    if(brtns > MX_BRTNS) brtns = MX_BRTNS;

    bttimer = 0;
    dtimer = 0;

    int len = sizeof(INPUTS);
    for(int i = 0; i < len; i++) pinMode(INPUTS[i], INPUT_PULLUP);

    len = sizeof(OUTPUTS);
    for(int i = 0; i < len; i++) pinMode(OUTPUTS[i], OUTPUT);

    Serial.begin(S_SPEED);
    px.begin();

    Serial.println("Boot sequence complete - entering main loop");
}

void loop() {

    static uint8_t btnst, pbtnst;
    static uint16_t btdiff;

    btnst = readbtns();

    if(btnst && !pbtnst) {
        bttimer = millis();
        pbtnst = btnst;
    }

    if(!btnst && pbtnst) {
        btdiff = millis() - bttimer;
        procinput(pbtnst, btdiff);
        pbtnst = btnst;
    }

    // Mode loop selector
    if(gm == M_MENU && millis() > dtimer + D_DEL) m_loop();
    else if(gm == M_FLIP && millis() > dtimer + D_DEL) f_loop();
    else if(gm == M_RMBR && millis() > dtimer + D_DEL) r_loop();
    else if(gm == M_BRTNS && millis() > dtimer + D_DEL) b_loop();
    else if(gm == M_FLASH && pgm != M_FLASH) fl_loop();
    else if(gm == M_2048 && millis() > dtimer + D_DEL) gm = M_FLIP; // TODO
    else if(gm == M_WIN && millis() > dtimer + A_DEL) a_loop();
    else if(gm == M_ATOG) apin_toggle();
    else if(gm == M_SAVE) save_state();
}
