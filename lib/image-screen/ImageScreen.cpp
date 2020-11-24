#include "ImageScreen.h"

void ImageScreen::draw() {
  display.drawBitmapFromSd("image1.bmp", 0, 0);
  display.display();
}