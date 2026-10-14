#pragma once

#include "util.h"

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
};

struct peripheral_status_state {
    bool connected;
};

void draw_output_status(const struct canvas *canvas, const struct status_state *state);
