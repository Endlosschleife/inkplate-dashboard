#include "Inkplate.h"

class ImageScreen {
    private:
        Inkplate &display;
    public:
        ImageScreen(Inkplate &d): display(d) {};
        void draw();
};