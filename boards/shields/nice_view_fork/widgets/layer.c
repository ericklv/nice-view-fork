#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "layer.h"
#include "../assets/custom_fonts.h"

void draw_layer_status(const struct canvas *canvas, const struct status_state *state) {
    char text[10] = {};

    if (state->layer_label == NULL || state->layer_label[0] == '\0') {
        snprintf(text, sizeof(text), "L#%u", state->layer_index);
    } else {
        strncpy(text, state->layer_label, sizeof(text) - 1);
        to_uppercase(text);
    }

    // 4 characters fit the large font, 5 the small one
    const lv_font_t *font = &quinquefive_12;
    if (strlen(text) > 4) {
        font = &quinquefive_8;
        text[5] = '\0';
    }

    canvas_text(canvas, 0, 110, 57, font, LV_TEXT_ALIGN_RIGHT, text);
}
