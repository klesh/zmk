/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/widgets/cpi_status.h>
#include <zmk/display/widgets/sync_layer_status.h>
#include <zmk/display/widgets/sync_output_status.h>
#include <zmk/display/widgets/sync_battery_status.h>
#include <zmk/display/status_screen.h>

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CPI_STATUS)
static struct zmk_widget_cpi_status cpi_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_LAYER_STATUS)
static struct zmk_widget_sync_layer_status sync_layer_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_OUTPUT_STATUS)
static struct zmk_widget_sync_output_status sync_output_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_BATTERY_STATUS)
static struct zmk_widget_sync_battery_status sync_battery_widget;
#endif

static lv_obj_t *screen;
static lv_obj_t *splash_label;

static void show_status_screen(lv_timer_t *timer) {
    lv_obj_del(splash_label);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
    zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
    lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT, 0,
                 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                               lv_theme_get_font_small(screen), LV_PART_MAIN);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_CPI_STATUS)
    zmk_widget_cpi_status_init(&cpi_status_widget, screen);
    lv_obj_align(zmk_widget_cpi_status_obj(&cpi_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_OUTPUT_STATUS)
    zmk_widget_sync_output_status_init(&sync_output_widget, screen);
    lv_obj_align(zmk_widget_sync_output_status_obj(&sync_output_widget), LV_ALIGN_BOTTOM_LEFT, 0,
                 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_LAYER_STATUS)
    zmk_widget_sync_layer_status_init(&sync_layer_widget, screen);
    lv_obj_align(zmk_widget_sync_layer_status_obj(&sync_layer_widget), LV_ALIGN_BOTTOM_MID, 15, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_SYNC_BATTERY_STATUS)
    zmk_widget_sync_battery_status_init(&sync_battery_widget, screen);
    lv_obj_align(zmk_widget_sync_battery_status_obj(&sync_battery_widget), LV_ALIGN_BOTTOM_RIGHT, 0,
                 0);
#endif
}

lv_obj_t *zmk_display_status_screen() {
    screen = lv_obj_create(NULL);

    splash_label = lv_label_create(screen);
    lv_obj_set_style_text_font(splash_label, lv_theme_get_font_large(screen), LV_PART_MAIN);
    lv_label_set_text(splash_label, "https://pskeeb.top");
    lv_obj_center(splash_label);

    lv_timer_t *timer = lv_timer_create(show_status_screen, 5000, NULL);
    lv_timer_set_repeat_count(timer, 1);

    return screen;
}
