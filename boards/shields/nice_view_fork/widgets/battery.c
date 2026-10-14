#include <stdio.h>
#include <zephyr/kernel.h>
#include "battery.h"
#include "../assets/custom_fonts.h"

#define DOT_RADIUS 3
#define BATTERY_DOTS 10

// From the lower left, over the top, to the lower right
static const struct {
    int8_t x, y;
} dots[BATTERY_DOTS] = {
    {12, 49}, {10, 39}, {12, 28}, {19, 20}, {29, 16},
    {39, 16}, {49, 20}, {56, 28}, {58, 39}, {56, 49},
};

void draw_battery_status(const struct canvas *canvas, const char *letter, uint8_t level) {
    int filled = MIN((level + 9) / 10, BATTERY_DOTS);

    for (int i = 0; i < BATTERY_DOTS; i++) {
        int32_t x = dots[i].x - DOT_RADIUS;
        int32_t y = dots[i].y - DOT_RADIUS;

        if (i < filled) {
            canvas_fill_rect(canvas, x, y, DOT_RADIUS * 2, DOT_RADIUS * 2, DOT_RADIUS);
        } else {
            canvas_stroke_rect(canvas, x, y, DOT_RADIUS * 2, DOT_RADIUS * 2, DOT_RADIUS);
        }
    }

    char text[4];
    snprintf(text, sizeof(text), "%u", level);

    canvas_text(canvas, 6, 27, 56, &quinquefive_12, LV_TEXT_ALIGN_CENTER, letter);
    canvas_text(canvas, 6, 41, 56, &quinquefive_8, LV_TEXT_ALIGN_CENTER, text);
}
