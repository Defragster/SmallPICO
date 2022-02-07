#include <rom/rtc.h>
#include "soc/rtc_wdt.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
#define ARDUINO_PRO_CORE 0
#define ARDUINO_USR_CORE 1
#ifdef LED_BUILTIN
#undef LED_BUILTIN
#endif


#ifdef ARDUINO_TINYPICO
#define LED_PIN 13 // TinyPICO
#else
#define LED_PIN 18 // PICO Kit
#endif

#include "esp_task_wdt.h"
// define two tasks for Blink & loopCounting
void TaskBlink( void *pvParameters );
void loopCounts( void *pvParameters );

uint32_t myTimeL = millis();
volatile uint32_t lCntL = 0;
uint32_t myTimeA = millis();
uint32_t myTimeB = millis();
volatile uint32_t lCntA = 0;
volatile uint32_t lCntB = 0;


int jj = 0;
bool setupWait = true;
TaskHandle_t xHandle0;
TaskHandle_t xHandle1;
// the setup function runs once when you press reset or power the board
void setup() {
  // rtc_wdt_protect_off(); // https://stackoverflow.com/questions/51750377/how-to-disable-interrupt-watchdog-in-esp32-or-increase-isr-time-limit
  // rtc_wdt_disable();
  Serial.begin(115200);   // initialize serial communication at 115200 bits per second:
  setupSSD1306();
  delay(200);
  checkReset();
  Serial.print("setup() is running on core ");
  Serial.println(xPortGetCoreID());
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
#ifdef ARDUINO_TINYPICO
  Serial.println("TinyPICO!");
#else
  Serial.println("LittlePICO!");
#endif
  delay(100);

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore( // B
    TaskBlink
    ,  "TaskBlink"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &xHandle0
    , ARDUINO_PRO_CORE );
  //    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore( // A
    loopCounts
    ,  "loopCounts"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  &xHandle1
    ,  ARDUINO_USR_CORE);

#ifdef BOARD_HAS_PSRAM
  delay(100);
  rtc_wdt_feed();
  Serial.println("BOARD_HAS_PSRAM");
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  delay(100);
  rtc_wdt_feed();
  //Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
  delay(100);
  rtc_wdt_feed();
  //Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  delay(100);
  logMemory();
#endif

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
  setupWait = false; // Allow Tasks to Run on setup() exit
}

void loop()
{ // Empty. Things are done in Tasks.
  lCntL++;
  if ( millis() - myTimeL > 1000 ) {
    myTimeL += 1000;
    Serial.println(" LOOP ... Hello World...");
    runSSD1306(); // never returns
  }
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  uint32_t ledState = 0;
  // Blink Turns on an LED on for one second, then off for one second, repeatedly.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.print("TaskBlink() [B] is running on core ");
  Serial.println(xPortGetCoreID());
  while ( setupWait ) {
    delay(100);
    ledState++;
    if ( ledState % 2 )
      digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    else
      digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
  }
  for (;;) // A Task shall never return or exit.
  {
    lCntB++;
    if ( millis() - myTimeB > 1000 ) {
      myTimeB += 1000;
      ledState++;
      if ( ledState % 2 )
        digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      else
        digitalWrite(LED_PIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1);
    }
    if ( Serial.available() ) {
      char szFoo[100];
      int ii = 0;
      Serial.println();
      while ( Serial.available() && ii < 100 ) {
        szFoo[ii++] = Serial.read();
      }
      Serial.print( szFoo );
    }
  } // for (;;) // A Task shall never return or exit.
}

void loopCounts(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  Serial.print("loopCounts() [A] is running on core ");
  Serial.println(xPortGetCoreID());
  while ( setupWait ) {
    delay(100);
  }
  for (;;)
  {
    lCntA++;
    if ( millis() - myTimeA > 1000 ) {
      Serial.print( lCntA );
      Serial.print(" ... << A cnt...");
      Serial.print( lCntB );
      Serial.print(" ... << B cnt... == ");
      Serial.print( lCntB + lCntA );
      Serial.print(" ... L cnt >> ");
      Serial.print( lCntL );
      Serial.print(" ...  ALL cnt... == ");
      Serial.print( lCntB + lCntA + lCntL);
      Serial.print( "\t sec=" );
      Serial.print( millis() / 1000 );
      Serial.println( );
      myTimeA += 1000;
      lCntA = 0;
      lCntB = 0;
      lCntL = 0;
      delay(2);
    }
  }
}




/**************************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

  This example is for a 128x32 pixel display using I2C to communicate
  3 pins are required to interface (two I2C and one reset).

  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license, check license.txt for more information
  All text above, and the splash screen below must be
  included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#ifdef ARDUINO_TINYPICO
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#else
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#endif

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};

void setupSSD1306() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  //  delay(200); // Pause for 2 seconds
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
#ifdef ARDUINO_TINYPICO
  Serial.println("TinyPICO!");
#else
  Serial.println("LittlePICO!");
#endif

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
}

void runSSD1306() {
  // delay(200);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  testdrawline();      // Draw many lines

  testdrawrect();      // Draw rectangles (outlines)

  testfillrect();      // Draw rectangles (filled)

  testdrawcircle();    // Draw circles (outlines)

  testfillcircle();    // Draw circles (filled)

  testdrawroundrect(); // Draw rounded rectangles (outlines)

  testfillroundrect(); // Draw rounded rectangles (filled)

  testdrawtriangle();  // Draw triangles (outlines)

  testfilltriangle();  // Draw triangles (filled)

  testdrawchar();      // Draw characters of the default font

  testdrawstyles();    // Draw 'stylized' characters

  testscrolltext();    // Draw scrolling text

  testdrawbitmap();    // Draw a small bitmap image

  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(100);
  display.invertDisplay(false);
  delay(100);
  testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}


void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(0, 0, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(25);

  display.clearDisplay();

  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(0, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(25);

  display.clearDisplay();

  for (i = display.width() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4) {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(25);

  display.clearDisplay();

  for (i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for (i = 0; i < display.width(); i += 4) {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(200); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(200);
}

void testfillrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(200);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(200);
}

void testfillcircle(void) {
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(200);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(200);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(200);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(
      display.width() / 2  , display.height() / 2 - i,
      display.width() / 2 - i, display.height() / 2 + i,
      display.width() / 2 + i, display.height() / 2 + i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(200);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width() / 2  , display.height() / 2 - i,
      display.width() / 2 - i, display.height() / 2 + i,
      display.width() / 2 + i, display.height() / 2 + i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(200);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++) {
    if (i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(200);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(200);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(200);
  display.stopscroll();
  delay(100);
  display.startscrollleft(0x00, 0x0F);
  delay(200);
  display.stopscroll();
  delay(100);
  display.startscrolldiagright(0x00, 0x07);
  delay(200);
  display.startscrolldiagleft(0x00, 0x07);
  delay(200);
  display.stopscroll();
  delay(100);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(100);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for (f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }
  for (;;) { // Loop forever...
    lCntL++;
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for (f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(10);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for (f = 0; f < NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}

/*
    rtc_wdt_protect_off(); // https://stackoverflow.com/questions/51750377/how-to-disable-interrupt-watchdog-in-esp32-or-increase-isr-time-limit
    rtc_wdt_disable();
    rtc_wdt_feed();
    // vTaskDelay(100);
    //esp_task_wdt_reset();

*/
