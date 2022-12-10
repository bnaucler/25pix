
# 25pix game console
A simple game console with a 25 pixel 'screen' built of WS2812 LEDs

## Created by
Björn Westerberg Nauclér (mail@bnaucler.se) 2022

## Installation
The software is created with [PlatformIO](https://platformio.org) core and can be installed with `pio run -t upload`  
Edit `platformio.ini` to set upload port

## Software
Configuration can be done by editing macros in `src/25pix_config.h`.  

## Hardware
In the `kicad` sub-directory you can find [KiCad](https://kicad.org) files. If you prefer to send to production as is, the `hw_production` sub-directory has all gerbers, POS/CPL file and BOM in the correct format for ordering from [JLCPCB](https://jlcpcb.com).

## Contributing
Submit an issue or send a pull request if you feel inclined to get involved.

## Disclaimer
This project is in alpha version and, albeit usable, far from polished.  
Feel free to explore (and edit!) the code base, but expect bugs.

## License
MIT (do whatever you want)
