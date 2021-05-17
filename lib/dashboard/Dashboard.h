#include "Inkplate.h"
#include "fonts.h"
#include "display_helpers.h"
#include "HTTPClient.h"
#include <ArduinoJson.h>
#include "icons.h"

class Dashboard {
    private:
        Inkplate &display;
        void drawDate();
        int drawCalendarEvents();
        void drawWeather();
        void fetchData(String endpoint);
        void drawConditionIcon(String &condition, int size, int x, int y);
        void drawWasteCalendar();
        void drawTime();
        void drawReminders(int y);
        void draw(String endpoint);
    public:
        Dashboard(Inkplate &d): display(d) {};
        void update();
        void partialUpdate();
};