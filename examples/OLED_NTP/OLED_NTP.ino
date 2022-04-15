#include <WiFi.h>
//xx #include <SSD1306Wire.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#ifdef ARDUINO_TINYPICO
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#else
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#endif

#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     -1 // 4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <time.h>

// const char* ssid     = "MYWIFI";
// const char* password = "12345678";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;
// OLED Display 128x64
//xx SSD1306Wire  display(0x3c, 22, 21);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
  // Connect to the network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.println("Connection established");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  // Get the NTP time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // OLED display init
  //xx  display.init();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.clearDisplay(); //xx display.clear();
  //xx display.setTextAlignment(TEXT_ALIGN_LEFT);
  //xx display.setFont(ArialMT_Plain_10);
  drawString(0, 0, "Access Pnt connected", 1);
  drawString(0, 18, "AP IP address: ", 1);
  char szIP[48];
  sprintf( szIP, "%lu", WiFi.localIP() );
  drawString( 0, 36, szIP, 1);
  display.display();
  delay(2000);
}
void draw_time(char *msg) {
  display.clearDisplay(); //xx display.clear();
  //xx display.setTextAlignment(TEXT_ALIGN_CENTER);
  //xx display.setFont(ArialMT_Plain_24);
  drawString(14, 32, msg, 2);
  display.display();
  Serial.println(msg);
}

void drawString(int xx , int yy , char *msg, int tSize) {
  display.setTextSize(tSize); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(xx, yy);
  display.println(msg);
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char time_str[16];
    strftime(time_str, 16, "%H:%M:%S", &timeinfo);
    draw_time(time_str);
  }
  delay(500);
}
