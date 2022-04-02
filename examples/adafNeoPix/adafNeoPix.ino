#include <Adafruit_NeoPixel.h>

#define PIN 9

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

/*
  255,0,0  What Color ?RED
  0,255, 0  What Color ?GREEN
  0,0, 255  What Color ?BLUE
*/

void showIn( char const *szDoc, bool bWait ) {
  Serial.print( szDoc );
  if ( bWait ) {
    Serial.print( " What Color ? " );
    while ( !Serial.available() );
    while ( Serial.available() ) {
      char cc = Serial.read();
      Serial.print( cc );
      if ( 13 == cc ) break;
    }
  }
  else
    delay(2000);
  Serial.println( "\n" );
}

void setup()
{
  Serial.begin(115200);
  testOrderRGB( false );
}

void testOrderRGB( bool bWait ) {
  delay(200); // Delay for a period of time (in milliseconds).
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setPixelColor(0, pixels.Color(25, 25, 25));
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(2000); // Delay for a period of time (in milliseconds).

  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show(); // This sends the updated pixel color to the hardware.
  showIn( "RED :: 255, 0, 0 ", bWait );
  delay(200);

  pixels.setPixelColor(0, pixels.Color( 0, 255, 0));
  pixels.show(); // This sends the updated pixel color to the hardware.
  showIn( "GREEN:: 0, 255, 0 ", bWait );
  delay(200); // Delay for a period of time (in milliseconds).

  pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  pixels.show(); // This sends the updated pixel color to the hardware.
  showIn( "BLUE:: 0, 0, 255 ", bWait );
  Serial.println( "\n: each keystroke received increments brightness, until MAX reset!" );
  delay(200); // Delay for a period of time (in milliseconds).
}

uint32_t doBright = 3;
void loop()
{
  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        pixels.setPixelColor(0, pixels.Color(i * doBright, j * doBright, k * doBright)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(200); // Delay for a period of time (in milliseconds).
      }
    }
  }
  if ( Serial.available() ) {
    while ( Serial.available() ) {
      Serial.read();
      delay(500);
      doBright += 15;
      if ( doBright > 127 )
        doBright = 3;
    }
    Serial.println( doBright );
  }
}
