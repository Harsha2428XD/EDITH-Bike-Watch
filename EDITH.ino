#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <math.h>
#include <ChronosESP32.h>
#include "spidy_wallpaper.h"

// ==================== PIN DEFINITIONS ====================
// Generic ESP32-C3 DevKitM-1 TFT wiring.
// TFT SCK=GPIO4, MOSI=GPIO6, CS=GPIO7, DC=GPIO10.
// TFT RST is connected to GPIO2 in this version.
#define TFT_SCK         4
#define TFT_MISO        5  // TFT does not use MISO; SPI.begin still needs a value.
#define TFT_MOSI        6
#define TFT_CS          7
#define TFT_DC          10
#define TFT_RST          2

// Push Buttons (Connect between pin and GND)
#define BTN_MODE        0    // Switch screens
#define BTN_PLAY_PAUSE  1    // Play/Pause through Chronos
#define BTN_NEXT        3    // Next track through Chronos
#define BTN_PREVIOUS   11    // Previous track through Chronos

// ==================== OBJECTS & STATE ====================
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
ChronosESP32 chronos("BikeDevice");

enum AppMode { MODE_CLOCK, MODE_DIAL, MODE_MUSIC };
AppMode currentMode = MODE_CLOCK;
bool navigationActive = false;
bool chronosConnected = false;

// Navigation Data State
String navIcon = "straight";
String navDistance = "Waiting...";
String navStreet = "Open Maps to start";
bool navUpdated = true;

// Music Data State
String trackTitle = "No Track";
String trackArtist = "Bluetooth Ready";
String trackApp = "Music";
bool isPlaying = false;
bool musicUpdated = true;

// Notification / now-playing popup
bool popupActive = false;
bool popupDirty = false;
bool incomingCallPopup = false;
String popupApp = "";
String popupTitle = "";
String popupMessage = "";
unsigned long popupStarted = 0;
unsigned long popupDuration = 4500;
String previousDigitalTime = "";
bool analogHandsDrawn = false;
int oldHourX = 80, oldHourY = 66;
int oldMinuteX = 80, oldMinuteY = 66;
int oldSecondX = 80, oldSecondY = 66;
bool navigationScreenRendered = false;
String renderedNavIcon = "";
String renderedNavDistance = "";
String renderedNavStreet = "";

void drawMusicScreen();

// ==================== UI DRAWING ROUTINES ====================

// Draw directional arrows for Navigation (Turn right, left, straight)
void drawArrow(int cx, int cy, String type) {
  tft.fillRect(0, 0, 70, 128, 0x07E0); // Bright Green split background (Image 2)
  
  if (type.indexOf("right") >= 0) {
    // Arrow pointing right with 90 deg bend
    tft.fillRect(cx - 20, cy + 5, 25, 10, ST77XX_WHITE);
    tft.fillRect(cx - 5, cy - 15, 10, 30, ST77XX_WHITE);
    tft.fillTriangle(cx - 5, cy - 25, cx - 5, cy - 5, cx + 18, cy - 15, ST77XX_WHITE);
  } else if (type.indexOf("left") >= 0) {
    // Arrow pointing left
    tft.fillRect(cx - 5, cy + 5, 25, 10, ST77XX_WHITE);
    tft.fillRect(cx - 5, cy - 15, 10, 30, ST77XX_WHITE);
    tft.fillTriangle(cx + 5, cy - 25, cx + 5, cy - 5, cx - 18, cy - 15, ST77XX_WHITE);
  } else {
    // Straight arrow
    tft.fillRect(cx - 5, cy - 10, 10, 35, ST77XX_WHITE);
    tft.fillTriangle(cx, cy - 25, cx - 18, cy - 10, cx + 18, cy - 10, ST77XX_WHITE);
  }
}

// Format a number as two digits, for example 7 -> "07".
String twoDigits(int value) {
  String result;
  if (value < 10) result = "0";
  result += String(value);
  return result;
