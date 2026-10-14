#include <string.h>
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

#include "battery.h"
#include "chart.h"
#include "layer.h"
#include "output.h"
#include "profile.h"
#include "screen.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void draw(struct zmk_widget_screen *widget) {
    for (int i = 0; i < CANVAS_COUNT; i++) {
        struct canvas canvas = {.obj = lv_obj_get_child(widget->obj, i), .top = CANVAS_TOP(i)};
        canvas_clear(&canvas);

        draw_battery_status(&canvas, "L", widget->state.battery);
        draw_chart_status(&canvas, &widget->state);
        draw_layer_status(&canvas, &widget->state);
        draw_output_status(&canvas, &widget->state);
        draw_profile_status(&canvas, &widget->state);

        canvas_rotate(&canvas);
    }
}

/**
 * Battery status
 **/

struct battery_status_state {
    uint8_t level;
};

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->state.battery = state.level;
        draw(widget);
    }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_status_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

/**
 * Layer status
 **/

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->state.layer_index = state.index;
        widget->state.layer_label = state.label;
        draw(widget);
    }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();

    return (struct layer_status_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)
ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

/**
 * Output status
 **/

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->state.selected_endpoint = state.selected_endpoint;
        widget->state.active_profile_index = state.active_profile_index;
        draw(widget);
    }
}

static struct output_status_state output_status_get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoints_selected(),
        .active_profile_index = zmk_ble_active_profile_index(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, output_status_get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);
#endif
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif

/**
 * WPM status
 **/

struct wpm_status_state {
    uint8_t wpm;
};

// ZMK only raises the WPM event when the value changes, so the chart samples the WPM every
// second until its history is all zeros again
static void wpm_sample_cb(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(wpm_sample_work, wpm_sample_cb);

static void wpm_sample_cb(struct k_work *work) {
    uint8_t wpm = zmk_wpm_get_state();
    bool active = false;

    struct zmk_widget_screen *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        memmove(widget->state.wpm, widget->state.wpm + 1, sizeof(widget->state.wpm) - 1);
        widget->state.wpm[WPM_HISTORY - 1] = wpm;
        draw(widget);

        for (int i = 0; i < WPM_HISTORY; i++) {
            active |= widget->state.wpm[i] > 0;
        }
    }

    if (active) {
        k_work_schedule_for_queue(zmk_display_work_q(), &wpm_sample_work, K_SECONDS(1));
    }
}

static void wpm_status_update_cb(struct wpm_status_state state) {
    // No-op while a sample is already scheduled
    k_work_schedule_for_queue(zmk_display_work_q(), &wpm_sample_work, K_NO_WAIT);
}

static struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

/**
 * Initialization
 **/

int zmk_widget_screen_init(struct zmk_widget_screen *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, SCREEN_HEIGHT, SCREEN_WIDTH);
    lv_obj_set_style_bg_color(widget->obj, LVGL_BACKGROUND, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, LV_PART_MAIN);

    for (int i = 0; i < CANVAS_COUNT; i++) {
        canvas_create(widget->obj, widget->cbuf[i], i);
    }

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
    widget_layer_status_init();
    widget_output_status_init();
    widget_wpm_status_init();

    return 0;
}

lv_obj_t *zmk_widget_screen_obj(struct zmk_widget_screen *widget) { return widget->obj; }
