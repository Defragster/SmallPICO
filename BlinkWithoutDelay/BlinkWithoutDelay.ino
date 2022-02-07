
const int led1Pin =  18;
const int led2Pin =  26;
int led1State = LOW;
int led2State = LOW;
uint32_t previousMillis1 = 0;
uint32_t previousMillis2 = 0;
const long interval1 = 1000;           // interval at which to blink (milliseconds)
const long interval2 = 2000;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led1Pin, HIGH);
  delay(200);
  digitalWrite(led2Pin, HIGH);
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
#endif
}
uint32_t lCnt = 0;
void loop() {
  lCnt++;
  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis;
    if (led1State == LOW) led1State = HIGH;
    else led1State = LOW;
    digitalWrite(led1Pin, led1State);
  }
  if (currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    if (led2State == LOW) led2State = HIGH;
    else led2State = LOW;
    digitalWrite(led2Pin, led2State);
    Serial.printf( "loop()/sec = %u\t", lCnt / 2 );
    if ( HIGH == led2State ) Serial.println();
    lCnt = 0;
  }
}
