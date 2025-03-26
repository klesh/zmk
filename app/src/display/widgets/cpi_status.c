/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/cpi_status.h>
#include <zmk/events/cpi_state_changed.h>
#include <zmk/event_manager.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct cpi_status_state {
    uint16_t cpi;
};

struct cpi_status_state cpi_status_get_state(const zmk_event_t *eh) {
    const struct zmk_cpi_state_changed *data = as_zmk_cpi_state_changed(eh);
    if (data == NULL) {
        return (struct cpi_status_state){.cpi = 0};
    }
    return (struct cpi_status_state){.cpi = data->cpi};
};

void set_cpi_symbol(lv_obj_t *label, struct cpi_status_state state) {
    LOG_DBG("cpi changed to %i", state.cpi);

    char text[6] = {};
    if (state.cpi > 0) {
        snprintf(text, sizeof(text), "%i", state.cpi);
    } else {
        text[0] = '\0';
    }

    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}

void cpi_status_update_cb(struct cpi_status_state state) {
    struct zmk_widget_cpi_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_cpi_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_cpi_status, struct cpi_status_state, cpi_status_update_cb,
                            cpi_status_get_state)
ZMK_SUBSCRIPTION(widget_cpi_status, zmk_cpi_state_changed);

int zmk_widget_cpi_status_init(struct zmk_widget_cpi_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_align(widget->obj, LV_ALIGN_RIGHT_MID, 0, 0);

    sys_slist_append(&widgets, &widget->node);

    widget_cpi_status_init();
    return 0;
}

lv_obj_t *zmk_widget_cpi_status_obj(struct zmk_widget_cpi_status *widget) { return widget->obj; }
