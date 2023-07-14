// SPDX-FileCopyrightText: 2022 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include "Adafruit_MAX1704X.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_TestBed.h"
#include <Adafruit_ST7789.h> 
#include <Fonts/FreeSans12pt7b.h>
#include <WiFiClientSecure.h>
#include "RTClib.h"
#include "secrets.h"

RTC_PCF8523 rtc;
char daysOfTheWeek[7][5] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

#define SERVER "transit-board-be.up.railway.app"
#define PATH "/api"
WiFiClientSecure client;

extern Adafruit_TestBed TB;

Adafruit_MAX17048 lipo;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

GFXcanvas16 canvas(240, 135);
String dtString = "Date and Time";
String statusString = "Network";

void setup() {
  Serial.begin(115200);
  //while (! Serial) delay(10);

  delay(100);

  initClock();
 
  TB.neopixelPin = PIN_NEOPIXEL;
  TB.neopixelNum = 1; 
  TB.begin();
  TB.setColor(WHITE);

  display.init(135, 240);           // Init ST7789 240x135
  display.setRotation(3);
  canvas.setFont(&FreeSans12pt7b);
  canvas.setTextColor(ST77XX_WHITE); 

  if (!lipo.begin()) {
    Serial.println(F("Couldn't find Adafruit MAX17048?\nMake sure a battery is plugged in!"));
    while (1) delay(10);
  }
    
  Serial.print(F("Found MAX17048"));
  Serial.print(F(" with Chip ID: 0x")); 
  Serial.println(lipo.getChipID(), HEX);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLDOWN);
  pinMode(2, INPUT_PULLDOWN);

  statusString = "Scanning Wifi...";
  initWifi();
}

uint8_t j = 0;

void loop() {
  Serial.println("**********************");

  TB.printI2CBusScan();
  dtString = getTimeString();

  if (j % 2 == 0) {
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setCursor(0, 17);
    canvas.setTextColor(ST77XX_RED);
    canvas.println(dtString);
    canvas.setTextColor(ST77XX_YELLOW);
    canvas.println(statusString);
    canvas.setTextColor(ST77XX_GREEN); 
    canvas.print("Battery: ");
    canvas.setTextColor(ST77XX_WHITE);
    canvas.print(lipo.cellVoltage(), 1);
    canvas.print(" V  /  ");
    canvas.print(lipo.cellPercent(), 0);
    canvas.println("%");
    canvas.setTextColor(ST77XX_BLUE); 
    canvas.print("I2C: ");
    canvas.setTextColor(ST77XX_WHITE);
    for (uint8_t a=0x01; a<=0x7F; a++) {
      if (TB.scanI2CBus(a, 0))  {
        canvas.print("0x");
        canvas.print(a, HEX);
        canvas.print(", ");
      }
    }
    canvas.println("");
    canvas.print("Buttons: ");
    Serial.println(digitalRead(0));
    Serial.println(digitalRead(1));
    Serial.println(digitalRead(2));
    if (!digitalRead(0)) {
      canvas.print("D0, ");
    }
    if (digitalRead(1)) {
      canvas.print("D1, ");
    }
    if (digitalRead(2)) {
      canvas.print("D2, ");
    }
    // -- Go to Sleep --
    if (digitalRead(1) && digitalRead(2)) {
      Serial.println("Sleep mode requested");
      // esp_sleep_enable_timer_wakeup(1000000 * 10); // 10 sec
      esp_deep_sleep_start(); 
    }
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);
  }
  
  TB.setColor(TB.Wheel(j++));
  delay(10);
  return;
}

void initClock(void) {
  if (! rtc.begin()) {
    dtString = "RTC not found";
    while (1) delay(100);
    Serial.println("Waiting for RTC");
  }
  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, setting to compile time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.start();
  dtString = "RTC Ready";
}

String getTimeString(void) {
  DateTime now = rtc.now();
  String retVal = daysOfTheWeek[now.dayOfTheWeek()];
  retVal.concat(" ");
  retVal.concat(now.month());
  retVal.concat("/");
  retVal.concat(now.day());
  retVal.concat("  ");
  retVal.concat(now.hour());
  retVal.concat(":");
  int minute = now.minute();
  if (minute < 10) retVal.concat("0");
  retVal.concat(String(minute));
  return retVal;
}

void initWifi(void) {
  statusString = ("Conn to " + wifi_ssid); // What gets displayed on the TFT
  
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  statusString = WiFi.SSID();
  statusString.concat(" ");
  statusString.concat(rssi);
}
