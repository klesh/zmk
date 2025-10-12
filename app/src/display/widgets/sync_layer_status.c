/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/sync_layer_status.h>
#include <zmk/events/sync_layer_state.h>
#include <zmk/event_manager.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void set_layer_symbol(lv_obj_t *label, struct zmk_sync_layer_state state) {
    // if (state.label == NULL || strlen(state.label) == 0) {
    char text[8] = {};

    snprintf(text, sizeof(text), LV_SYMBOL_KEYBOARD " %i", state.index);

    lv_label_set_text(label, text);
    // } else {
    //     char text[14] = {};

    //     snprintf(text, sizeof(text), LV_SYMBOL_KEYBOARD " %s", state.label);

    //     lv_label_set_text(label, text);
    // }
}

static void layer_status_update_cb(struct zmk_sync_layer_state state) {
    struct zmk_widget_sync_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_symbol(widget->obj, state); }
}

static struct zmk_sync_layer_state layer_status_get_state(const zmk_event_t *eh) {
    struct zmk_sync_layer_state *ev = as_zmk_sync_layer_state(eh);
    return *ev;
    // return (struct zmk_sync_layer_state){.index = evt->layer, .label = ""};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct zmk_sync_layer_state,
                            layer_status_update_cb, layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_sync_layer_state);

int zmk_widget_sync_layer_status_init(struct zmk_widget_sync_layer_status *widget,
                                      lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    sys_slist_append(&widgets, &widget->node);

    widget_layer_status_init();
    return 0;
}

lv_obj_t *zmk_widget_sync_layer_status_obj(struct zmk_widget_sync_layer_status *widget) {
    return widget->obj;
}
