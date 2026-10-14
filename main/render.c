#include "render.h"
#include <stdint.h>
#include <string.h>
#include "font.h"
// -----------------------------------------------------------------------------
// Drawing functions
// -----------------------------------------------------------------------------

void pixel_set(
    uint8_t *framebuffer,
    int width,
    int height,
    int x,
    int y)
{
    if (x < 0 || x >= width ||
        y < 0 || y >= height) {
        return;
    }

    // SSD1306 framebuffer: 8 vertical pixels per byte.
    framebuffer[(y / 8) * width + x] |=
        (uint8_t)(1U << (y % 8));
}

void draw_char(
    uint8_t *framebuffer,
    int width,
    int height,
    int x,
    int y,
    char c,
    int scale)
{
    const uint8_t *glyph = font5x7(c);

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            if ((glyph[col] & (1U << row)) == 0) {
                continue;
            }

            for (int dx = 0; dx < scale; dx++) {
                for (int dy = 0; dy < scale; dy++) {
                    pixel_set(
                        framebuffer,
                        width,
                        height,
                        x + col * scale + dx,
                        y + row * scale + dy);
                }
            }
        }
    }
}

void draw_text_centered(
    uint8_t *framebuffer,
    int width,
    int height,
    int y,
    const char *text,
    int scale)
{
    int text_width = (int)strlen(text) * 6 * scale - scale;
    int x = (width - text_width) / 2;

    for (size_t i = 0; text[i] != '\0'; i++) {
        draw_char(
            framebuffer,
            width,
            height,
            x,
            y,
            text[i],
            scale);

        x += 6 * scale;
    }
}