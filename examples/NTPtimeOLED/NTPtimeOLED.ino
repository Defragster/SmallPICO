#include <WiFi.h>
#include <SSD1306Wire.h>
#include <time.h>

 //  https://blog.devgenius.io/5-experiments-with-wifi-using-esp32-8d40dddc538b
 
//const char* ssid     = "MYWIFI";     
//const char* password = "12345678";

const char* ssid       = "netEagle_GuestNG";
const char* password   = "20222022";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;
// OLED Display 128x64
SSD1306Wire  display(0x3c, 21, 22);
 
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
  display.init();
  display.flipScreenVertically(); // Comment to leave it other side up
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Access Point connected");
  display.drawString(0, 24, "AP IP address: ");
  display.drawString(0, 36, WiFi.localIP().toString());
  display.display();
  delay(1000);
}
void draw_time(char *msg) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_24);
  display.drawString(display.getWidth()/2, 0, msg);
  display.display();
  Serial.println(msg);
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
