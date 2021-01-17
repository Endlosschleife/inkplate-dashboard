#include "Dashboard.h"

DynamicJsonDocument doc(ESP.getMaxAllocHeap());

void Dashboard::drawDate()
{
  JsonObject currentDateObject = doc["currentDate"];
  String day_of_week = currentDateObject["dayOfWeek"].as<String>();
  String day = currentDateObject["day"].as<String>();
  String month = currentDateObject["month"].as<String>();

  display.setTextSize(1);

  // day
  display.setTextColor(WHITE);
  display.setFont(&Roboto_Bold_96);
  DisplayHelpers::printCenteredText(display, day, 60, 90);

  // day of week
  display.setFont(&Roboto_Light_48);
  display.setTextColor(WHITE);
  display.setCursor(128, 48);
  display.print(day_of_week);

  // month
  display.setFont(&Roboto_Light_48);
  display.setTextColor(WHITE);
  display.setCursor(128, 46 * 2);
  display.println(month);
}

void Dashboard::fetchData()
{
  HTTPClient http;
  http.begin(API_URL);
  int httpCode = http.GET();

  if (httpCode != 200)
  {
    Serial.println("Could not load data from API");
    return;
  }

  // Parse JSON object and load into global doc
  DeserializationError error = deserializeJson(doc, http.getStream());
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
}

void Dashboard::drawCalendarEvents()
{

  JsonArray eventGroups = doc["events"].as<JsonArray>(); // todo rename events property
  int y = 120;                                           // start position for this calendar

  for (int i = 0; i < eventGroups.size(); i++)
  {
    String title = eventGroups[i]["title"].as<String>();
    JsonArray events = eventGroups[i]["events"].as<JsonArray>();

    // print title
    y += 48;
    display.setFont(&Roboto_Bold_48);
    display.setTextColor(WHITE);
    display.setCursor(10, y);
    display.print(title);

    y += 5; // bottom margin after title

    // print events
    for (int k = 0; k < events.size(); k++)
    {
      String title = events[k]["title"].as<String>();
      boolean isFullDayEvent = events[k]["fullDayEvent"].as<boolean>();

      y += 42;

      if (!isFullDayEvent)
      {
        String startTime = events[k]["start"].as<String>();
        display.setFont(&Roboto_28);
        display.setTextColor(WHITE);
        display.setCursor(10, y);
        display.print(startTime.substring(11, 16));

        display.setFont(&Roboto_28);
        display.setTextColor(WHITE);
        display.setCursor(110, y);
        DisplayHelpers::printTextWithMaxWidth(display, title, 105, y, 290);
      }
      else
      { // full day event
        display.setFont(&Roboto_28);
        display.setTextColor(WHITE);
        display.setCursor(10, y);
        display.print(title + " (ganzt.)");
      }
      
    }

    // no events
    if (events.size() == 0)
    {
      y += 38;
      display.setFont(&Roboto_Italic_28);
      display.setTextColor(WHITE);
      display.setCursor(10, y);
      display.print("Keine Termine");
    }

    y += 40; // some bottom margin
  }
}

void Dashboard::drawConditionIcon(String &condition, int size, int x, int y)
{
  condition.toLowerCase();
  String filename = "weather/" + condition + "_" + size + ".bmp";
  char filenameCharArray[filename.length() + 1];
  filename.toCharArray(filenameCharArray, filename.length() + 1);

  Serial.println(filenameCharArray);

  display.drawBitmapFromSd(filenameCharArray, x, y);
}

void Dashboard::drawWeather()
{

  const int x = 420;
  const int y = 10;
  const int width = 370;
  const int forecast_line_y = y + 165;

  JsonObject weatherJson = doc["weather"];
  String condition = weatherJson["condition"].as<String>();
  String condition_text = weatherJson["conditionText"].as<String>();
  float temperature_float = weatherJson["temperature"].as<float>();
  String temperature = String(temperature_float, 0); // second param is precision

  // condition icon
  drawConditionIcon(condition, 120, x + 10, y + 2);
  display.setTextSize(1);

  // temperature
  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Bold_96);
  display.setCursor(x + 140, y + 85);
  display.println(temperature);
  // degree symbol
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(temperature, x + 140, y + 85, &x1, &y1, &w, &h);
  display.setCursor(x1 + w + 6, y1 + 21);
  display.setFont(&Roboto_36);
  display.println("o");

  // condtition text
  display.setFont(&Roboto_36);
  display.setCursor(x + 146, y + 130);
  display.println(condition_text);

  // forecast
  JsonArray forecastArray = weatherJson["forecast"].as<JsonArray>();
  for (int i = 0; i < 3; i++)
  {
    String forecast_condition = forecastArray[i]["condition"].as<String>();
    double forecast_temperature = forecastArray[i]["temperature"].as<double>();
    String forecast_time = forecastArray[i]["datetime"].as<String>();
    int startX = x + i * (width / 3);

    display.setFont(&Roboto_28);
    display.setCursor(startX + 20, forecast_line_y + 30);
    display.println(forecast_time.substring(11, 13) + " Uhr");

    drawConditionIcon(forecast_condition, 56, startX + 35, forecast_line_y + 40);

    display.setFont(&Roboto_28);
    DisplayHelpers::printCenteredText(display, String(forecast_temperature, 1), startX + 58, forecast_line_y + 130);
  }
}

void Dashboard::drawWasteCalendar() {
  int dividier_width = 6;
  for(int i = 0; i < dividier_width; i++) {
    display.drawFastHLine(400, 340 + i, 800, BLACK);
  }


  JsonArray wasteCalendarArray = doc["wasteCalendar"].as<JsonArray>();
  for(int i = 0; i < wasteCalendarArray.size(); i++) {
    String name = wasteCalendarArray[i]["name"].as<String>();
    String dateString = wasteCalendarArray[i]["dateString"].as<String>();

    int y = 420 + (i / 2) * 115;
    int x = 440 + (i % 2) * 200;
  
    if(dateString == "morgen" || dateString == "heute") {
      display.setTextColor(WHITE);
      display.fillRoundRect(x - 10, y - 40, 140, 90, 3, BLACK);
    } else {
      display.setTextColor(BLACK);
    }

    display.setFont(&Roboto_Bold_36);
    display.setCursor(x, y);
    display.print(name);

    display.setFont(&Roboto_28);
    display.setCursor(x, y + 35);
    display.print(dateString);
  }


}

void Dashboard::drawTime() {
  JsonObject weatherJson = doc["currentDate"];
  String timeString = weatherJson["time"].as<String>();

  display.setFont(&Roboto_22);
  display.setTextColor(GRAY_5);
  display.setCursor(10, 590);
  display.print(timeString);
}

void Dashboard::draw()
{
  fetchData();

  display.fillRect(0, 0, 400, 600, BLACK);


  drawDate();
  drawCalendarEvents();
  drawWeather();
  drawWasteCalendar();
  drawTime();

  display.display();
}