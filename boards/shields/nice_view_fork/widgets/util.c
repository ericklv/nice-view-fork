#include <ctype.h>
#include <string.h>
#include <zephyr/kernel.h>
#include "util.h"

void to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

void canvas_create(lv_obj_t *parent, canvas_buf_t buf, int index) {
    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_obj_align(canvas, LV_ALIGN_TOP_RIGHT, -CANVAS_TOP(index), 0);
    lv_canvas_set_buffer(canvas, buf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);
}

void canvas_clear(const struct canvas *canvas) {
    lv_canvas_fill_bg(canvas->obj, LVGL_BACKGROUND, LV_OPA_COVER);
}

// Rotates by hand: lv_canvas_transform() needs a display being refreshed, which there
// isn't yet when the widgets draw for the first time
void canvas_rotate(const struct canvas *canvas) {
    static canvas_buf_t buf_tmp;
    lv_color_t *buf = (lv_color_t *)lv_canvas_get_img(canvas->obj)->data;
    memcpy(buf_tmp, buf, sizeof(buf_tmp));

    for (int y = 0; y < CANVAS_SIZE; y++) {
        for (int x = 0; x < CANVAS_SIZE; x++) {
            buf[y * CANVAS_SIZE + x] = buf_tmp[(CANVAS_SIZE - 1 - x) * CANVAS_SIZE + y];
        }
    }

    lv_obj_invalidate(canvas->obj);
}

static bool is_visible(const struct canvas *canvas, int32_t y, int32_t h) {
    return y + h > canvas->top && y < canvas->top + CANVAS_SIZE;
}

static void draw_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      const lv_draw_rect_dsc_t *dsc) {
    if (!is_visible(canvas, y, h)) {
        return;
    }

    lv_canvas_draw_rect(canvas->obj, x, y - canvas->top, w, h, dsc);
}

void canvas_fill_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      int32_t radius) {
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = LVGL_FOREGROUND;
    dsc.radius = radius;

    draw_rect(canvas, x, y, w, h, &dsc);
}

void canvas_stroke_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                        int32_t radius) {
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = LVGL_BACKGROUND;
    dsc.border_color = LVGL_FOREGROUND;
    dsc.border_width = 1;
    dsc.radius = radius;

    draw_rect(canvas, x, y, w, h, &dsc);
}

void canvas_text(const struct canvas *canvas, int32_t x, int32_t y, int32_t w,
                 const lv_font_t *font, lv_text_align_t align, const char *text) {
    if (!is_visible(canvas, y, font->line_height)) {
        return;
    }

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = LVGL_FOREGROUND;
    dsc.font = font;
    dsc.align = align;

    lv_canvas_draw_text(canvas->obj, x, y - canvas->top, w, &dsc, text);
}
