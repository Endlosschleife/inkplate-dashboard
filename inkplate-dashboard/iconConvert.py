# -----------
# Simple python script to
# create icon header files for Inkplate 6
# Arduino sketches
#
# Takes all files from /icons and saves them to /binary_icons
#
# -----------

from PIL import Image
import os, sys

size = 56

if not os.path.isdir("./binary_icons"):
    os.mkdir(os.path.abspath(os.getcwd()) + "/binary_icons")

for file in os.listdir("./icons"):
    im = Image.open("./icons/" + file)
    im = im.resize((size, size))
    alp = im.split()[-1]
    s = [0 for x in range(size * size)]
    for y in range(size):
        for x in range(size):
            # print(im.getpixel((x, y)))
            if alp.getpixel((x, y)) > 128:
                s[(x + size * y) // 8] |= 1 << (7 - (x + size * y) % 8)

    with open("./binary_icons/icon_" + file[:-4] + "_" + str(size) + ".h", "w") as f:
        print("const uint8_t icon_" + file[:-4] + "_" + str(size) + "[] PROGMEM = {", file=f)
        print(",".join(list(map(hex, s))), file=f)
        print("};", file=f)
