/* Simple "Hello World" example.

   After uploading this to your board, use Serial Monitor
   to view the message.  When Serial is selected from the
   Tools > USB Type menu, the correct serial port must be
   selected from the Tools > Serial Port AFTER Teensy is
   running this code.  Teensy only becomes a serial device
   while this code is running!  For non-Serial types,
   the Serial port is emulated, so no port needs to be
   selected.

   This example code is in the public domain.
*/
#include "LittlePICO.h"
#define LED_PICO 18
#include <Adafruit_NeoPixel.h>
#define PIN 9
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
int lColors[8] = { 0, 3618615, 3604480, 14080, 55, 3618560, 14135, 3604535 };
int lBlink = 0;
int led[] = { 5, 14, 23, 18, 12, 19, 4, 13, 25, 15, 26, 27, 33, 32, 21, 22 }; // 2201

void setup()
{
  for (int i = 0; i < 16; i++) {
    pinMode(led[i], INPUT);
  }
  pinMode(LED_PICO, OUTPUT);
  digitalWrite(LED_PICO, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.begin(115200);
  delay(1000);
  digitalWrite(LED_PICO, LOW);   // turn the LED on (HIGH is the voltage level)
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
  boardInfo();

#ifdef BOARD_HAS_PSRAM
  Serial.println("BOARD_HAS_PSRAM");
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  delay(100);
  logMemory();
#endif
  uint32_t cycTime;
  Serial.printf("\tCycle count before delay(100): %u\n", cycTime = ESP.getCycleCount() );
  delay(100);
  cycTime = ESP.getCycleCount() - cycTime;
  Serial.printf("\tCycle count DIFF delay(100): %u\n", cycTime );
  Serial.printf("\tCycle count before delay(100): %u\n", cycTime = ESP.getCycleCount() );
  uint32_t jj;
  int ii;
  cycTime = ESP.getCycleCount();
  for ( ii = 0; ii < 1000; ii++ ) {
    jj += ESP.getCycleCount();
  }
  cycTime = ESP.getCycleCount() - cycTime;
  Serial.printf("\tCycle count for %d calls : %u [noise %u]\n", ii , cycTime, jj );
  cycTime = ESP.getCycleCount();
  for ( ii = 0; ii < 1000; ii++ ) {
    jj = ESP.getCycleCount();
  }
  cycTime = ESP.getCycleCount() - cycTime;
  Serial.printf("\tCycle count for %d calls : %u [noise %u]\n", ii , cycTime, jj );

#include "esp_timer.h"

  //void measure_important_function(void) {
  const unsigned MEASUREMENTS = 5;
  uint64_t start = esp_timer_get_time();

  for (int retries = 0; retries < MEASUREMENTS; retries++) {
    //important_function(); // This is the thing you need to measure
    logMemory();
  }

  uint64_t end = esp_timer_get_time();

  printf("%u iterations took %ull milliseconds (%ull microseconds per invocation)\n",
         MEASUREMENTS, (end - start) / 1000, (end - start) / MEASUREMENTS);
  //}

}

uint32_t myTime = millis();
uint32_t lCnt = 0;
//void IRAM_ATTR loop() // code in RAM
void loop()
{
  static uint32_t ledState = 0;
  static uint32_t cycTime = 0;
  static uint32_t cycTimeInDiff = 23982909;

  /*
    if ( !(lCnt % 100) )
      Serial.printf("#%d\n", lCnt);
    else
      Serial.print('.');
  */
  lCnt++;
  /*
    if ( !(lCnt % 1000) ) {
      digitalWrite(18, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(100);
    }
    else
      digitalWrite(18, LOW);    // turn the LED off by making the voltage LOW
  */
  //  if ( Serial.available() ) { // Cuts cycle count in half: Loops/sec=71747
  if ( ( (ESP.getCycleCount() - cycTime) + cycTimeInDiff >= 239990000 ) ) { // faster : Loops/sec=136984
    while ( Serial.available() ) {
      Serial.print( (char)Serial.read() );
      //delay(1);
    }
    cycTimeInDiff -= 2398290;
    ledToggle();
  }
  //if ( (millis() - myTime) > 1000 ) { // Cycle count 1 sec?: 239983710 Loops/sec=122490
  if ( (ESP.getCycleCount() - cycTime) >= 239984000 ) { // Loops/sec=137999 @240000000
    // @239990000: Cycle count 1 sec?: 239990555 Loops/sec=137993
    cycTime = ESP.getCycleCount() - cycTime;
    Serial.printf("Loop()s/~sec: %u\t", cycTime );
    Serial.printf( "Loops/sec=%d\t", lCnt );
    // Convert raw temperature in F to Celsius degrees
    //Serial.print((temprature_sens_read() - 32) / 1.8);
    Serial.printf( "ms=%d\tTemp °C=%f\n", millis(), ((temprature_sens_read() - 32) / 1.8) );
    cycTime = ESP.getCycleCount();
    cycTimeInDiff = 23982909;
    //Serial.println("\n ... Hello World...\n");
    myTime += 1000;
    lCnt = 0;
    ledState++;
    if ( ledState % 2 )
      digitalWrite(LED_PICO, HIGH);   // turn the LED on (HIGH is the voltage level)
    else
      digitalWrite(LED_PICO, LOW);    // turn the LED off by making the voltage LOW
  }
}

void ledToggle() {
  pixels.setPixelColor(0, lColors[lBlink]);
  pixels.show(); // This sends the updated pixel color to the hardware.
  lBlink++;
  if ( lBlink > 6 ) lBlink = 0;
}
