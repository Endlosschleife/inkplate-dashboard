#include "Inkplate.h"
#include "fonts.h"
#include "colors.h"
#include "display_helpers.h"
#include "HTTPClient.h"
#include <ArduinoJson.h>
#include "icons.h"

class Dashboard {
    private:
        Inkplate &display;
        void drawDate();
        void drawCalendarEvents();
        void drawWeather();
        void fetchData();
        void drawConditionIcon(String &condition, int size, int x, int y);
        void drawWasteCalendar();
    public:
        Dashboard(Inkplate &d): display(d) {};
        void draw();
};