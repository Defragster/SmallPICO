#include <rom/rtc.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#ifdef LED_BUILTIN
#undef LED_BUILTIN
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 18
#endif

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogReadA3( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  delay(200); // Pause for 2 seconds
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);
#ifdef ARDUINO_TINYPICO
  Serial.println("TinyPICO!");
#else
  Serial.println("LittlePICO!");
#endif

#ifdef BOARD_HAS_PSRAM
  delay(100);
  Serial.println("BOARD_HAS_PSRAM");
  Serial.printf("Total heap: %d\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
  delay(100);
  logMemory();
  delay(100);
#endif

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskBlink
    ,  "TaskBlink"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    TaskAnalogReadA3
    ,  "AnalogReadA3"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{ // Empty. Things are done in Tasks.

  // THIS IS ACTUALLY STILL CALLED!
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /*
    Blink
    Turns on an LED on for one second, then off for one second, repeatedly.

    If you want to know what pin the on-board LED is connected to on your ESP32 model, check
    the Technical Specs of your board.
  */

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    //    vTaskDelay(100);  // one tick delay (15ms) in between reads for stability
    delay( 200 );
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    //    vTaskDelay(100);  // one tick delay (15ms) in between reads for stability
    delay( 200 );
  }
}

void TaskAnalogReadA3(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /*
    AnalogReadSerial
    Reads an analog input on pin A3, prints the result to the serial monitor.
    Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
    Attach the center pin of a potentiometer to pin A3, and the outside pins to +5V and ground.

    This example code is in the public domain.
  */
  int sensorValueA3_OLD = 9999999;
  for (;;)
  {
    // read the input on analog pin A3:
    int sensorValueA3 = analogRead(A3);
    if ( sensorValueA3 != sensorValueA3_OLD ) {
      // print out the value you read:
      Serial.println(sensorValueA3);
      sensorValueA3_OLD = sensorValueA3;
    }
    vTaskDelay(10);  // one tick delay (15ms) in between reads for stability
  }
}

void logMemory() {
#ifdef BOARD_HAS_PSRAM
  uint32_t allocSize = ESP.getFreePsram() - 16;
  Serial.printf( "%d Bytes PSRAM to test ps_malloc()\n", allocSize );
  uint32_t* psdRamBuffer = (uint32_t*)ps_malloc( allocSize );
  testMemory( psdRamBuffer, allocSize / sizeof(uint32_t ) );
  free(psdRamBuffer);
#elif BOARD_HAS_PSRAM2
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
  uint32_t allocSize = ESP.getFreePsram() - 32;
  Serial.printf( "%d Bytes PSRAM to test ps_malloc()\n", allocSize );
  uint32_t* psdRamBuffer = (uint32_t*)ps_malloc( allocSize );
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
  testMemory( psdRamBuffer, allocSize / sizeof(uint32_t ) );
  free(psdRamBuffer);
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
#endif
}
void testMemory( uint32_t* psdRamBuffer, uint32_t testSize ) {
#ifdef BOARD_HAS_PSRAM
  for ( uint32_t ii = 0; ii < testSize; ii += 1 ) {
    psdRamBuffer[ii] = ii;
  }
  int jj = 0;
  for ( uint32_t ii = 0; ii < testSize; ii += 1 ) {
    if ( psdRamBuffer[ii] != ii ) jj++;
  }
  if ( 0 == jj ) {
    Serial.printf( "All Good! testSize DWORDS=%u\n", testSize );
  }
  else {
    Serial.printf( "%d Fails to compare! (test)\n", jj );
  }
#endif
}
