/*
  Blink
  Turns on an LED on and off one at a time.
  This example code is in the public domain.
*/

#ifdef ARDUINO_ESP32S2_DEV
#define LED_PICO 36
#define PIN 21
byte led[] = { 9, 8, 0, 6, 7, 14, 17, 18, 44, 43, 33, 37, 38, 36, 35, 4, 5 }; // 2203 :: S2
#else
#define LED_PICO 18
#define PIN 9
byte led[] = { 5, 14, 23, 18, 12, 19, 4, 13, 25, 15, 26, 27, 33, 32, 21, 22 }; // 2201&2202 :: PICO D4
#endif

#define PIN_CNT sizeof( led )

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

int i, jj;
int lColors[8];
int lBlink = 0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  pixels.begin(); // This initializes the NeoPixel library.
  lColors[0] = pixels.Color(55, 55, 55);
  lColors[1] = pixels.Color(55, 0, 0);
  lColors[2] = pixels.Color(0, 55, 0);
  lColors[3] = pixels.Color(0, 0, 55);
  lColors[4] = pixels.Color(55, 55, 0);
  lColors[5] = pixels.Color(0, 55, 55);
  lColors[6] = pixels.Color(55, 0, 55);
  lColors[7] = pixels.Color(0, 0, 0);
  for (jj = 0; jj < 8; jj++) {
    Serial.print( "0x" );
    Serial.print( lColors[jj], HEX );
    Serial.print( ", " );
    delay(200);
    ledToggle();
  }
  Serial.println( " " );
  // initialize the digital pin as an output.
  for (i = 0; i < PIN_CNT; i++) {
    pinMode(led[i], OUTPUT);
  }
  for (jj = 0; jj < 7; jj++) {
    delay(200);
    ledToggle();
  }
}

void altToggle() {
  for (jj = 0; jj < 3; jj++) {
    for (i = 0; i < PIN_CNT; i++) {
      if (i % 2)
        digitalWrite(led[i], LOW);  // turn the LED off by making the voltage LOW
      else
        digitalWrite(led[i], HIGH);  // turn the LED off by making the voltage LOW
    }
    delay(500);
    ledToggle();
    for (i = 0; i < PIN_CNT; i++) {
      if (i % 2)
        digitalWrite(led[i], HIGH);  // turn the LED off by making the voltage LOW
      else
        digitalWrite(led[i], LOW);  // turn the LED off by making the voltage LOW
    }
    delay(500);
    ledToggle();
  }
  for (i = 0; i < PIN_CNT; i++) {
    digitalWrite(led[i], LOW);  // turn the LED off by making the voltage LOW
    //  delay(100);
  }
}

void loop() {
  // the loop routine runs over and over again forever
  for (jj = 0; jj < 2; jj++) {
    for (i = 0; i < PIN_CNT; i++) {
      digitalWrite(led[i], HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(100);                  // wait for a 1/10 second
      digitalWrite(led[i], LOW);   // turn the LED off by making the voltage LOW
      delay(100);
    }
    ledToggle();
  }
  altToggle();
  for (jj = 0; jj < 7; jj++) {
    delay(100);
    ledToggle();
  }
  delay(100);
}

void ledToggle() {
  pixels.setPixelColor(0, lColors[lBlink]);
  pixels.show(); // This sends the updated pixel color to the hardware.
  lBlink++;
  if ( lBlink > 7 ) lBlink = 0;
}
