/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/sync_battery_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/sync_battery_state.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void set_battery_symbol(lv_obj_t *label, struct zmk_sync_battery_state state) {
    char text[9] = {};

    uint8_t level = state.level;

    bool hide_level = false;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    if (state.is_charging) {
        strcpy(text, LV_SYMBOL_CHARGE " ");
        hide_level = true;
    }
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

    if (!hide_level) {
#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_SHOW_PERCENTAGE)
        char perc[5] = {};
        snprintf(perc, sizeof(perc), "%3u%%", level);
        strcat(text, perc);
#else
        if (level > 95) {
            strcat(text, LV_SYMBOL_BATTERY_FULL);
        } else if (level > 65) {
            strcat(text, LV_SYMBOL_BATTERY_3);
        } else if (level > 35) {
            strcat(text, LV_SYMBOL_BATTERY_2);
        } else if (level > 5) {
            strcat(text, LV_SYMBOL_BATTERY_1);
        } else {
            strcat(text, LV_SYMBOL_BATTERY_EMPTY);
        }
#endif
    }
    lv_label_set_text(label, text);
}

void sync_battery_status_update_cb(struct zmk_sync_battery_state state) {
    struct zmk_widget_sync_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, state); }
}

static struct zmk_sync_battery_state sync_battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_sync_battery_state *ev = as_zmk_sync_battery_state(eh);
    return *ev;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_sync_battery_status, struct zmk_sync_battery_state,
                            sync_battery_status_update_cb, sync_battery_status_get_state)

ZMK_SUBSCRIPTION(widget_sync_battery_status, zmk_sync_battery_state);

int zmk_widget_sync_battery_status_init(struct zmk_widget_sync_battery_status *widget,
                                        lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_sync_battery_status_init();
    return 0;
}

lv_obj_t *zmk_widget_sync_battery_status_obj(struct zmk_widget_sync_battery_status *widget) {
    return widget->obj;
}
