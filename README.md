# EDITH Map Navigator

A smartwatch-style ESP32-C3 project with a 1.8-inch ST7735 TFT. It connects to the Chronos companion app over Bluetooth Low Energy and displays a digital clock, analog dial, navigation information, music metadata, and notification popups. The digital clock uses a custom Spider-Man wallpaper.

## Features

- Large 24-hour digital clock with weekday and date
- Analog clock face
- Navigation arrows, distance, street instruction, and corner clock
- Music title, artist, playback status, and Chronos media controls
- Notification popup and incoming-call-style popup when Chronos forwards call details
- Partial display updates to reduce screen flicker
- RGB565 wallpaper stored in flash in `spidy_wallpaper.h`

## Project files

```text
Chronos_C3_Bike_Watch/
├── Chronos_C3_Bike_Watch.ino
├── spidy_wallpaper.h
└── README.md
```

Keep `spidy_wallpaper.h` next to the `.ino`; the sketch includes it directly.

## Parts

- ESP32-C3 DevKitM-1 (pin mapping below assumes this board)
- 1.8-inch ST7735 SPI TFT, 160×128
- Up to four momentary push buttons
- Optional protected 1-cell 3.7 V LiPo battery and regulated boost supply

The ESP32-C3 supports BLE, which this project uses for Chronos. It does not support Bluetooth Classic A2DP, so this sketch does not stream audio to a MAX98357A amplifier. Music buttons send media controls to the phone through Chronos.

## Wiring

### TFT to ESP32-C3 DevKitM-1

| TFT pin | ESP32-C3 pin |
|---|---:|
| SCK / SCL / CLK | GPIO 4 |
| SDA / MOSI / DIN | GPIO 6 |
| CS | GPIO 7 |
| DC / A0 | GPIO 10 |
| RST | GPIO 2 (see reset note below) |
| VCC | 3V3 |
| GND | GND |
| LED / BL | 3V3 for full backlight |
| MISO / SDO | Not connected |

GPIO 2 is a boot-strapping pin on the ESP32-C3. If the board does not boot reliably, connect TFT RST to the board's RST pin and change `#define TFT_RST 2` to `#define TFT_RST -1` in the sketch.

Backlight brightness is set by the TFT module's LED/BL wiring, not by `Adafruit_ST7735`. For full brightness, connect LED/BL to 3V3 if that is supported by your display module. Do not connect it to 5 V unless the module documentation explicitly allows that.

### Buttons

Each button is a normally-open momentary switch between its GPIO and GND. The sketch enables the internal pull-up, so no external resistor or 3V3 connection is required.

| Button | GPIO |
|---|---:|
| Mode | GPIO 0 |
| Play / Pause | GPIO 1 |
| Next track | GPIO 3 |
| Previous track | GPIO 11 |

A 4-leg tactile switch has two internally connected leg pairs. Connect one pair to the GPIO and the other pair to GND. Confirm the pairs with a multimeter in continuity mode if the switch orientation is unclear.

### Battery power

A 1-cell LiPo marked 3.7 V reaches about 4.2 V when fully charged. Do not connect it directly to the ESP32-C3 3V3 pin or 5V input. Use a proper LiPo charger/protection circuit and a regulated boost converter. A 5 V boost output can power the DevKit's 5V input; keep grounds common. Choose a supply rated for the ESP32-C3's current peaks and any attached peripherals. Do not charge a bare LiPo without a suitable charger/protection circuit.

For first bring-up, power the board from USB. Disconnect other power sources unless the exact dev board supports simultaneous USB and external power.

## Arduino IDE setup

1. Install the Espressif **esp32** board package and select an ESP32-C3 board matching your hardware.
2. Install these libraries using Library Manager or their official sources:
   - Adafruit GFX Library
   - Adafruit ST7735 and ST7789 Library
   - ChronosESP32
3. Open `Chronos_C3_Bike_Watch.ino` with `spidy_wallpaper.h` in the same folder.
4. Check the pin definitions against your exact board and wiring.
5. Select the correct port and upload.
6. In the Chronos phone app, pair the watch from the app's watch/pairing screen. Allow notification access if you want notification popups.

The mode button cycles through digital clock, analog dial, and music screens. The remaining buttons send play/pause, next, and previous commands through Chronos.

## Wallpaper

`spidy_wallpaper.h` contains a 160×128 RGB565 image array in program flash. The sketch draws it on the digital clock screen. To use another wallpaper, convert/crop an image to 160×128 RGB565 and regenerate this header with the symbol name `spidyWallpaper`.

## Notes

- This project displays music metadata and sends phone playback controls; it does not play audio from the ESP32-C3 speaker amplifier.
- Incoming calls appear as a call-style popup only when Chronos forwards call details as a notification.
- The pin map is for ESP32-C3 DevKitM-1. Other ESP32-C3 boards may use different exposed pins.
- The wallpaper was derived from an image supplied for this project. Include/distribute it publicly only if you have rights to do so.
