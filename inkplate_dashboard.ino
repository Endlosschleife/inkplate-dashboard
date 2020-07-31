#include "Inkplate.h"               //Include Inkplate library to the sketch
#include "HTTPClient.h"             //Include library for HTTPClient
#include "WiFi.h"                   //Include library for WiFi
#include <ArduinoJson.h>

// Including fonts used
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_120.h"
#include "Fonts/Roboto_36.h"
#include "Fonts/Roboto_Bold_96.h"

#include "icons.h"
#include "config.h"

Inkplate display(INKPLATE_3BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)

// Use arduinojson.org/v6/assistant to compute the capacity.
const size_t capacity = JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(4) + 5*JSON_OBJECT_SIZE(3) + 4*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + 1000;
DynamicJsonDocument doc(capacity);

void connectWifi();
void fetchData();
void drawDate();
void drawWeather();

void setup() {
  Serial.begin(9600);
  
  display.begin();
  display.clearDisplay();
  display.display();

  connectWifi();
  display.clearDisplay();

  fetchData();

  drawDate();
  drawWeather();
  display.display();
}

void connectWifi() {
  display.print("Connecting to WiFi...");
  display.partialUpdate();
  
  //Connect to the WiFi network.
  int attempts = 0;
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED && attempts < 5) {
    attempts++;
    delay(500);
    display.print(".");
    display.partialUpdate();
  }

  // still not connected: reset wifi see issue: https://github.com/espressif/arduino-esp32/issues/2501
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi Connection could not be established. Will reset Wifi and try again.");
    WiFi.persistent(false);
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(3000);
  }
}

void fetchData() {
  HTTPClient http;
  http.begin("http://192.168.178.87:8080/dashboard");
  int httpCode = http.GET();

  if (httpCode != 200) {
    Serial.println("Could not load data from API");
    return;
  }

  // Parse JSON object and load into global doc
  DeserializationError error = deserializeJson(doc, http.getStream());
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
}

void drawDate() {

  JsonObject currentDateObject = doc["currentDate"];
  String day_of_week = currentDateObject["dayOfWeek"].as<String>();
  String day = currentDateObject["day"].as<String>();
  String month = currentDateObject["month"].as<String>();

  display.setTextSize(1);

  // day
  display.fillRoundRect(5, 10, 120, 90, 10, 6); //Arguments are: start X, start Y, width, height, radius, color
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Bold_96);
  display.setCursor(10, 90);
  display.print(day);

  // day of week
  display.setFont(&Roboto_Light_48);
  display.setTextColor(BLACK);
  display.setCursor(130, 48);
  display.print(day_of_week);

  // month
  display.setFont(&Roboto_Light_48);
  display.setTextColor(BLACK);
  display.setCursor(130, 48 * 2);
  display.println(month);
}

void drawWeather() {
  JsonObject weatherJson = doc["weather"];
  String condition_text = weatherJson["conditionText"].as<String>();
  float temperature_float = weatherJson["temperature"].as<float>();
  String temperature = String(temperature_float, 0); // second param is precision

  display.drawRoundRect(470, 5, 310, 300, 10, BLACK); //Arguments are: start X, start Y, width, height, radius, color
  display.drawLine(470, 150, 780, 150, BLACK);
  
  // condition icon
  display.drawBitmap(480, 10, icon_c, 150, 150, BLACK);

  display.setTextSize(1);

  // temperature
  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Bold_96);
  display.setCursor(800 - 160, 90);
  display.println(temperature);

  // condtition text
  display.setFont(&Roboto_36);
  display.setCursor(800 - 160, 140);
  display.println(condition_text);
  
}

void loop() {
  //Nothing...
}
