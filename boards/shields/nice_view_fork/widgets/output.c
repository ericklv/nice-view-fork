#include <zephyr/kernel.h>
#include "output.h"
#include "../assets/custom_fonts.h"

void draw_output_status(const struct canvas *canvas, const struct status_state *state) {
    const char *text;

#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    switch (state->selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        text = "USB";
        break;
    case ZMK_TRANSPORT_BLE:
        text = "BLE";
        break;
    default:
        text = "NULL";
        break;
    }
#else
    text = state->connected ? "BLE" : "NULL";
#endif

    canvas_text(canvas, 0, 139, 57, &quinquefive_8, LV_TEXT_ALIGN_RIGHT, text);
}
