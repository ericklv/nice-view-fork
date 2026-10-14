#pragma once

#include "util.h"

struct layer_status_state {
    uint8_t index;
    const char *label;
};

void draw_layer_status(const struct canvas *canvas, const struct status_state *state);
