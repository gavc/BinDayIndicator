#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"

// WiFi credentials from secrets.h
const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;

// NTP client for time fetching
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// NeoPixel strip configuration
#define LED_PIN    D2
#define LED_COUNT  1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global variables and constants for timing and bin management
int currentWeek = 0;
int binNight = 1; // 1 for Monday...

unsigned long previousMillisTimeUpdate = 0; 
const long timeUpdateInterval = 24 * 60 * 60 * 1000;  // Update time every 24 hours

unsigned long previousMillis = 0; 
const long interval = 30000;  // LED blink interval in milliseconds
bool colorSwitch = false; 
int brightness = 20; // LED brightness

int startHour = 17; // Start time for flashing
int endHour = 20;  // End time for flashing

// Bin types
enum BinType {
  NONE,
  GREEN_AND_RED,
  YELLOW_AND_RED
};

BinType currentBinType = NONE;

void setup() {
  Serial.begin(115200);

  // Initialize NeoPixel strip
  strip.begin(); 
  strip.show();  
  strip.setBrightness(brightness); 

  // Connect to WiFi
  Serial.print("\nConnecting ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  blinkLed(0, 255, 0, 3, 200);
  Serial.println(" connected to WiFi");

  // Initialize NTP client
  timeClient.begin();
  timeClient.setTimeOffset(10 * 60 * 60); // 10 Hour offset for Brisbane Au.
  timeClient.update();
  blinkLed(0, 0, 255, 3, 200);
}

void loop() {
  unsigned long currentMillis = millis();

  // Update time every 24 hours
  if (currentMillis - previousMillisTimeUpdate >= timeUpdateInterval) {
    previousMillisTimeUpdate = currentMillis;
    timeClient.update();
    blinkLed(0, 0, 255, 3, 200);
    Serial.println("TimeClient Udpated");
  }

  // Handle LED blinking and color
  if (currentMillis - previousMillis >= interval && isWithinBlinkingPeriod()) {
    previousMillis = currentMillis;
    colorSwitch = !colorSwitch;

    // Choose color based on bin type
    if (currentBinType == GREEN_AND_RED) {
      setLedColor(colorSwitch ? 255 : 0, colorSwitch ? 0 : 255, 0);
    } else if (currentBinType == YELLOW_AND_RED) {
      setLedColor(colorSwitch ? 255 : 255, colorSwitch ? 0 : 255, 0);
    } else {
      setLedColor(0, 0, 0);
    }
  }

  // Check bin day
  checkBinDay();
}

void checkBinDay() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti = localtime (&rawtime);
  int currentDayOfYear = ti->tm_yday;
  currentWeek = currentDayOfYear / 7 + 1;
  int referenceGreenWeek = 23;

  // Determine current bin type
  if (ti->tm_wday == binNight) {
    currentBinType = (currentWeek - referenceGreenWeek) % 2 == 0 ? GREEN_AND_RED : YELLOW_AND_RED;
  } else {
    currentBinType = NONE;
  }
}

void setLedColor(int red, int green, int blue) {
  strip.setPixelColor(0, strip.Color(red, green, blue));
  strip.show();
}

void blinkLed(int red, int green, int blue, int times, int wait) {
  for(int i=0; i<times; i++) {
    setLedColor(red, green, blue);
    delay(wait);
    setLedColor(0, 0, 0);
    delay(wait);
  }
}

bool isWithinBlinkingPeriod() {
  int currentHour = timeClient.getHours();
  return currentHour >= startHour && currentHour < endHour;
}
