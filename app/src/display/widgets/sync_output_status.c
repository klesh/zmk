/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/sync_output_status.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/events/sync_output_state.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static struct zmk_sync_output_state get_state(const zmk_event_t *eh) {
    const struct zmk_sync_output_state *ev = as_zmk_sync_output_state(eh);
    return *ev;
    // return (struct zmk_sync_output_state){.selected_endpoint = ev->selected_endpoint,
    //                                       .active_profile_connected =
    //                                       ev->active_profile_connected, .active_profile_bonded =
    //                                       ev->active_profile_bonded};
}

static void set_status_symbol(lv_obj_t *label, struct zmk_sync_output_state state) {
    char text[20] = {};

    switch (state.selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        strcat(text, LV_SYMBOL_USB);
        break;
    case ZMK_TRANSPORT_BLE:
        if (state.active_profile_bonded) {
            if (state.active_profile_connected) {
                snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_OK,
                         state.selected_endpoint.ble.profile_index + 1);
            } else {
                snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_CLOSE,
                         state.selected_endpoint.ble.profile_index + 1);
            }
        } else {
            snprintf(text, sizeof(text), LV_SYMBOL_WIFI " %i " LV_SYMBOL_SETTINGS,
                     state.selected_endpoint.ble.profile_index + 1);
        }
        break;
    }

    lv_label_set_text(label, text);
}

static void sync_output_status_update_cb(struct zmk_sync_output_state state) {
    struct zmk_widget_sync_output_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_sync_output_status, struct zmk_sync_output_state,
                            sync_output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_sync_output_status, zmk_sync_output_state);

int zmk_widget_sync_output_status_init(struct zmk_widget_sync_output_status *widget,
                                       lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_sync_output_status_init();
    return 0;
}

lv_obj_t *zmk_widget_sync_output_status_obj(struct zmk_widget_sync_output_status *widget) {
    return widget->obj;
}
