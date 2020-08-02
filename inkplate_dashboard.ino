#include "Inkplate.h"               //Include Inkplate library to the sketch
#include "HTTPClient.h"             //Include library for HTTPClient
#include "WiFi.h"                   //Include library for WiFi
#include <ArduinoJson.h>

// Including fonts used
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_120.h"
#include "Fonts/Roboto_18.h"
#include "Fonts/Roboto_28.h"
#include "Fonts/Roboto_36.h"
#include "Fonts/Roboto_Bold_96.h"

#include "icons.h"
#include "config.h"

Inkplate display(INKPLATE_3BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)

// Use arduinojson.org/v6/assistant to compute the capacity.
const size_t capacity = JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(4) + 5*JSON_OBJECT_SIZE(3) + 4*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + 1000;
DynamicJsonDocument doc(capacity);

// weather icon mapping
String conditions[] = {"CLEAR_NIGHT", "CLOUDY", "FOG", "HAIL", "LIGHTNING", "LIGHTNING_RAINY", "PARTLY_CLOUDY", "POURING", "RAINY", "SNOWY", "SNOWY_RAINY", "SUNNY", "WINDY", "WINDY_VARIANT", "EXCEPTIONAL"};
const uint8_t *logos[15] = {icon_clear_night, icon_cloudy, icon_fog, icon_hail, icon_lightning, icon_lightning_rainy, icon_partly_cloudy, icon_pouring, icon_rainy, icon_snowy, icon_snowy_rainy, icon_sunny, icon_windy, icon_windy_variant, icon_exceptional};

void connectWifi();
void fetchData();
void drawDate();
void drawWeather();
const uint8_t* find_weather_icon(String condition);

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

  const int x = 470;
  const int y = 5;
  const int width = 320;
  const int height = 300;
  const int forecast_line_y = y + 155;
  const int forecast_end_y = forecast_line_y + height - 155;
  
  JsonObject weatherJson = doc["weather"];
  String condition = weatherJson["condition"].as<String>();
  String condition_text = weatherJson["conditionText"].as<String>();
  float temperature_float = weatherJson["temperature"].as<float>();
  String temperature = String(temperature_float, 0); // second param is precision

  // draw box
  display.drawRoundRect(x, y, width, height, 10, BLACK); //Arguments are: start X, start Y, width, height, radius, color
  display.drawLine(x, forecast_line_y, x + width, forecast_line_y, BLACK);
  display.drawLine(x + width / 3, forecast_line_y, x + width / 3, forecast_end_y, BLACK); // first column
  display.drawLine(x + (width / 3) * 2, forecast_line_y, x + (width / 3) * 2, forecast_end_y, BLACK); // second column
  
  // condition icon
  display.drawBitmap(x + 10, y + 2, find_weather_icon(condition), 150, 150, BLACK);
  display.setTextSize(1);

  // temperature
  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Bold_96);
  display.setCursor(x + width - 150, y + 85);
  display.println(temperature);

  // condtition text
  display.setFont(&Roboto_36);
  display.setCursor(x + width - 150, y + 130);
  display.println(condition_text);

  // forecast
  JsonArray forecastArray = weatherJson["forecast"].as<JsonArray>();
  for(int i = 0; i < 3; i++) {
    String forecast_1_condition = forecastArray[i]["condition"].as<String>();
    String forecast_1_temperature = forecastArray[i]["temperature"].as<String>();
    String forecast_1_time = forecastArray[i]["datetime"].as<String>();
    //display.drawBitmap(x + 10, forecast_line_y, icon_cloudy_50, 150, 150, BLACK);
    int startX = x + i * (width / 3);
    
    display.setFont(&Roboto_18);
    display.setCursor(startX + 25, forecast_line_y + 20);
    display.println(forecast_1_time.substring(11, 13) + " Uhr");    

    display.setFont(&Roboto_28);
    display.setCursor(startX + 25, forecast_end_y - 10);
    display.println(forecast_1_temperature);    
  }
  
}



const uint8_t* find_weather_icon(String condition) {
  for (int i = 0; i < 15; ++i) { 
    if (conditions[i].equals(condition)) {
      return logos[i];
    }
  }
}

void loop() {
  //Nothing...
}
