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
    lv_canvas_set_buffer(canvas, buf, CANVAS_SIZE, CANVAS_SIZE, CANVAS_COLOR_FORMAT);
}

void canvas_clear(const struct canvas *canvas) {
    lv_canvas_fill_bg(canvas->obj, LVGL_BACKGROUND, LV_OPA_COVER);
}

void canvas_rotate(const struct canvas *canvas) {
    static uint8_t buf_tmp[CANVAS_BUF_SIZE];
    uint8_t *buf = lv_canvas_get_draw_buf(canvas->obj)->data;
    memcpy(buf_tmp, buf, CANVAS_BUF_SIZE);

    lv_canvas_fill_bg(canvas->obj, LVGL_BACKGROUND, LV_OPA_COVER);

    const int32_t stride = lv_draw_buf_width_to_stride(CANVAS_SIZE, CANVAS_COLOR_FORMAT);
    lv_draw_sw_rotate(buf_tmp, buf, CANVAS_SIZE, CANVAS_SIZE, stride, stride,
                      LV_DISPLAY_ROTATION_270, CANVAS_COLOR_FORMAT);
}

static bool is_visible(const struct canvas *canvas, int32_t y, int32_t h) {
    return y + h > canvas->top && y < canvas->top + CANVAS_SIZE;
}

static void draw_rect(const struct canvas *canvas, int32_t x, int32_t y, int32_t w, int32_t h,
                      const lv_draw_rect_dsc_t *dsc) {
    if (!is_visible(canvas, y, h)) {
        return;
    }

    lv_layer_t layer;
    lv_canvas_init_layer(canvas->obj, &layer);

    y -= canvas->top;
    lv_area_t coords = {x, y, x + w - 1, y + h - 1};
    lv_draw_rect(&layer, dsc, &coords);

    lv_canvas_finish_layer(canvas->obj, &layer);
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

    lv_layer_t layer;
    lv_canvas_init_layer(canvas->obj, &layer);

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = LVGL_FOREGROUND;
    dsc.font = font;
    dsc.align = align;
    dsc.text = text;

    y -= canvas->top;
    lv_area_t coords = {x, y, x + w - 1, y + font->line_height};
    lv_draw_label(&layer, &dsc, &coords);

    lv_canvas_finish_layer(canvas->obj, &layer);
}
