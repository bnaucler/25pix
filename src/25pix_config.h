/*
 *
 *  25pix game console
 *  Björn Westerberg Nauclér 2022
 *  MIT License
 *
 *  25pix_config.h - configuration and proprocessor directives
 *
 */

#define NUMPX 25            // Nuber of pixels in grid
#define PXSZY 5             // Y grid size
#define PXSZX 5             // X grid size
#define CPOSY 2             // Central Y position
#define CPOSX 2             // Central X position

#define S_SPEED 9600        // Serial terminal speed

#define UBTN 5             // Up button pin
#define LBTN 2              // Left button pin
#define RBTN 8             // Right button pin
#define DBTN 7             // Down button pin
#define SBTN 10             // Select button pin

#define NUMBTNS 5           // Total number of physical buttons

#define PPIN 3              // Pixel pin
#define AIPIN 9            // Audio pin - internal
#define AEPIN 6            // Audio pin - external
#define TLPIN 7             // Test LED pin // UNUSED

#define DBNC 20             // Debounce time (ms)
#define LPTIME 1000         // Long press time (ms)

#define M_UNUSED 255        // Non-existing mode
#define M_MENU 254          // Main menu / mode select
#define M_WIN 253           // Won game animation
#define M_FLIP 0            // Mode: Flip
#define M_RMBR 1            // Mode: Memory game
#define M_2048 2            // Mode: 2048
#define M_FLASH 3           // Flashlight - all pixels white
#define M_SAVE 4            // Save states to EEPROM
#define M_BRTNS 5           // Set brightness
#define M_ATOG 6            // Toggle audio pin
#define M_MAX 6             // Highest mode available in menu

#define ADDR_GM 0           // EEPROM address for gm
#define ADDR_APIN 1         // EEPROM address for audio pin
#define ADDR_BRTNS 2        // EEPROM address for brightness
#define ADDR_GRID 3         // EEPROM address for gridst
                            // <div id="logwrapper">

#define R_SLEV 3            // Starting level for RMBR
#define R_SSTP 600          // Time to show sequence step (in ms)
#define R_NTOFF 100         // Time to turn off notes at step end

#define D_DEL 10            // draw delay in ms
                            // </div>
#define A_DEL 50            // draw delay during animation

#define A_DONE 20           // # cycles for win animation

#define MN_BRTNS 3          // Minimum LED brightness
#define MX_BRTNS 100        // Maximum LED brightness

// Colors
#define C_OFF 0
#define C_WHITE 16250871
#define C_ORANGE 15157760
#define C_YELLOW 12415752
#define C_RED 16189440
#define C_PINK 14033259
#define C_PURPLE 12976375
#define C_GREEN 11925272
#define C_BLUE 4361463
#define C_CYAN 57054

// Notes
#define NOTE_C 262
#define NOTE_D 294
#define NOTE_E 330
#define NOTE_F 349
#define NOTE_G 392
#define NOTE_A 440
#define NOTE_B 494

