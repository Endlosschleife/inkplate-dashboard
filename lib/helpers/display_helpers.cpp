#include "display_helpers.h"

namespace DisplayHelpers
{
  void printCenteredText(Inkplate &display, const String &buf, int x, int y)
  {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);
    display.setCursor(x - w / 2, y);
    display.print(buf);
  }
}