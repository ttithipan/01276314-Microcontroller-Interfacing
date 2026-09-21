#!/usr/bin/env python3
"""Convert the team photo into an RGB565 C array for the Lab 8 Screen 2 layout.

Usage:  python3 lab08/tools/make_team_photo.py [source.jpg]

The source defaults to lab08/picture/155015.jpg, which is git-ignored.  The
generated *.c file holds the pixels and is git-ignored as well, so the photo
never reaches the repository; this script and the small header are committed.
Run this script once after cloning, or whenever the photo changes.
"""

import os
import sys

from PIL import Image

# Layout of the photo area in screen2.c: 3:4 portrait, left column of the
# 320x240 landscape screen.
TARGET_W = 132
TARGET_H = 176

HERE = os.path.dirname(os.path.abspath(__file__))
LAB08 = os.path.dirname(HERE)
MICROCON = os.path.dirname(LAB08)
PICTURE_DIR = os.path.join(MICROCON, "picture")

PROJECTS = ("lab08-special-1", "lab08-special-2")

DEFAULT_SOURCE = os.path.join(PICTURE_DIR, "155015.jpg")


def crop_to_aspect(image):
    """Centre crop to the target aspect ratio, trimming the longer side."""
    width, height = image.size
    target_ratio = TARGET_W / TARGET_H
    source_ratio = width / height

    if source_ratio > target_ratio:
        new_width = int(round(height * target_ratio))
        offset = (width - new_width) // 2
        return image.crop((offset, 0, offset + new_width, height))

    new_height = int(round(width / target_ratio))
    offset = (height - new_height) // 2
    return image.crop((0, offset, width, offset + new_height))


def to_rgb565_bytes(image):
    """Return the image as big-endian RGB565 bytes, panel byte order."""
    out = bytearray()
    pixels = image.load()
    for y in range(TARGET_H):
        for x in range(TARGET_W):
            red, green, blue = pixels[x, y]
            value = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)
            out.append((value >> 8) & 0xFF)
            out.append(value & 0xFF)
    return bytes(out)


def describe(image, label):
    """Cheap sanity check: does this look like a photo of people?"""
    small = image.resize((48, 48))
    pixels = list(small.getdata())
    skintone = sum(1 for r, g, b in pixels
                   if r > 95 and g > 40 and b > 20
                   and max(r, g, b) - min(r, g, b) > 15
                   and abs(r - g) > 15 and r > g and r > b)
    mean = tuple(sum(c[i] for c in pixels) // len(pixels) for i in range(3))
    print("    %s: mean RGB %s, skin-tone pixels %.1f%%"
          % (label, mean, 100.0 * skintone / len(pixels)))


def main():
    source = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_SOURCE
    print("source: %s" % source)

    image = Image.open(source).convert("RGB")
    print("    size %dx%d" % image.size)
    describe(image, "whole image")

    cropped = crop_to_aspect(image)
    resized = cropped.resize((TARGET_W, TARGET_H), Image.LANCZOS)
    data = to_rgb565_bytes(resized)
    print("    emitted %dx%d = %d bytes" % (TARGET_W, TARGET_H, len(data)))

    rows = []
    for i in range(0, len(data), 16):
        chunk = ", ".join("0x%02X" % b for b in data[i:i + 16])
        rows.append("  " + chunk + ",")
    array_body = "\n".join(rows)

    for project in PROJECTS:
        src_path = os.path.join(LAB08, project, "Core", "Src", "team_photo.c")
        with open(src_path, "w") as handle:
            handle.write(
                "/*\n"
                " * RGB565 pixels of the team photo, used by Screen 2.\n"
                " *\n"
                " * GENERATED FILE - do not edit, and deliberately kept out of git\n"
                " * (see .gitignore) so the photo is never published.  Recreate it\n"
                " * with:  python3 lab08/tools/make_team_photo.py\n"
                " *\n"
                " * Byte order is big-endian per pixel, which is what the ILI9341\n"
                " * expects on the wire.  Source: %s\n"
                " */\n\n"
                "#include \"team_photo.h\"\n\n"
                "const uint8_t team_photo_rgb565[TEAM_PHOTO_WIDTH * TEAM_PHOTO_HEIGHT * 2U] =\n"
                "{\n%s\n};\n" % (os.path.basename(source), array_body))

        hdr_path = os.path.join(LAB08, project, "Core", "Inc", "team_photo.h")
        with open(hdr_path, "w") as handle:
            handle.write(
                "/**\n"
                "  ******************************************************************************\n"
                "  * @file    team_photo.h\n"
                "  * @brief   Geometry of the Screen 2 team photo bitmap.\n"
                "  ******************************************************************************\n"
                "  *\n"
                "  * The pixels live in team_photo.c, which is generated and git-ignored.\n"
                "  * This header only carries the dimensions, so the layout constants in\n"
                "  * screen2.c can be checked against the bitmap at compile time.\n"
                "  */\n\n"
                "#ifndef __TEAM_PHOTO_H__\n"
                "#define __TEAM_PHOTO_H__\n\n"
                "#include <stdint.h>\n\n"
                "#define TEAM_PHOTO_WIDTH   %dU\n"
                "#define TEAM_PHOTO_HEIGHT  %dU\n\n"
                "/* Big-endian RGB565, TEAM_PHOTO_WIDTH * TEAM_PHOTO_HEIGHT pixels */\n"
                "extern const uint8_t team_photo_rgb565[TEAM_PHOTO_WIDTH * TEAM_PHOTO_HEIGHT * 2U];\n\n"
                "#endif /* __TEAM_PHOTO_H__ */\n" % (TARGET_W, TARGET_H))
        print("    wrote %s" % src_path)


if __name__ == "__main__":
    main()
