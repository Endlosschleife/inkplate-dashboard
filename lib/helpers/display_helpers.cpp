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

  void printTextWithMaxWidth(Inkplate &display, const String text, int x, int y, int maxWidth)
  {
    int16_t x1, y1;
    uint16_t w, h;
    String nText = text;
    String ellipsisText = text;
    display.getTextBounds(ellipsisText, x, y, &x1, &y1, &w, &h);

    while (w > maxWidth && nText.length() > 2)
    {
      nText = nText.substring(0, nText.length() - 1);
      ellipsisText = nText + "...";
      display.getTextBounds(ellipsisText, x, y, &x1, &y1, &w, &h);
    }

    display.setCursor(x, y);
    display.print(ellipsisText);
  }

} // namespace DisplayHelpers