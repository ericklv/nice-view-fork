#include <zephyr/kernel.h>
#include "profile.h"

#define PROFILE_X 60
#define PROFILE_Y 101
#define PROFILE_SIZE 8
#define PROFILE_STEP 10
#define PROFILE_COUNT 5

void draw_profile_status(const struct canvas *canvas, const struct status_state *state) {
    for (int i = 0; i < PROFILE_COUNT; i++) {
        int32_t y = PROFILE_Y + i * PROFILE_STEP;

        if (i == state->active_profile_index) {
            canvas_fill_rect(canvas, PROFILE_X, y, PROFILE_SIZE, PROFILE_SIZE, 0);
        } else {
            canvas_stroke_rect(canvas, PROFILE_X, y, PROFILE_SIZE, PROFILE_SIZE, 2);
        }
    }
}
