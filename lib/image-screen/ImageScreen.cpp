#include "ImageScreen.h"

void ImageScreen::draw() {
  display.drawBitmapFromSD("image1.bmp", 0, 0);
  display.display();
}