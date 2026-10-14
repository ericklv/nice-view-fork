#pragma once

#include <lvgl.h>
#include <zmk/endpoints.h>

#define SCREEN_WIDTH 68
#define SCREEN_HEIGHT 160

// The portrait screen is drawn on square canvases stacked from the top, each one
// rotated onto the landscape display. The last one overlaps the one before it.
#define CANVAS_SIZE 68
#define CANVAS_COUNT 3
#define CANVAS_TOP(i) ((i) == CANVAS_COUNT - 1 ? SCREEN_HEIGHT - CANVAS_SIZE : (i) * CANVAS_SIZE)
#define CANVAS_COLOR_FORMAT LV_COLOR_FORMAT_L8
#define CANVAS_BUF_SIZE                                                                            \
    LV_CANVAS_BUF_SIZE(CANVAS_SIZE, CANVAS_SIZE, LV_COLOR_FORMAT_GET_BPP(CANVAS_COLOR_FORMAT),     \
                       LV_DRAW_BUF_STRIDE_ALIGN)

typedef uint8_t canvas_buf_t[CANVAS_BUF_SIZE];

#define LVGL_BACKGROUND                                                                            \
    (IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_white() : lv_color_black())
#define LVGL_FOREGROUND                                                                            \
    (IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_black() : lv_color_white())

#define WPM_HISTORY 16

struct status_state {
    uint8_t battery;
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    uint8_t layer_index;
    const char *layer_label;
    uint8_t wpm[WPM_HISTORY]; // oldest first
#else
    bool connected;
#endif
};

// A canvas and the screen row its first row is drawn at. Widgets draw in screen
// coordinates, anything outside the canvas is clipped.
struct canvas {
    lv_obj_t *obj;
    int32_t top;
};

void to_uppercase(char *str);
void canvas_create(lv_obj_t *parent, canvas_buf_t buf, int index);
void canvas_clear(const struct canvas *canvas);
void canvas_rotate(const struct canvas *canvas);
void canvas_fill_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      int32_t radius);
void canvas_stroke_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                        int32_t radius);
void canvas_text(const struct canvas *canvas, int32_t x, int32_t y, int32_t w,
                 const lv_font_t *font, lv_text_align_t align, const char *text);
