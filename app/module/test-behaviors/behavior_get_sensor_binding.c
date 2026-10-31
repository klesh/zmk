/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_get_sensor_binding

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/keymap.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if ZMK_KEYMAP_HAS_SENSORS && DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

static void log_direction(zmk_keymap_layer_id_t layer, uint8_t sensor, uint8_t direction,
                          const char *label) {
    struct zmk_behavior_binding binding;
    int ret = zmk_keymap_get_sensor_binding(layer, sensor, direction, &binding);

    if (ret < 0) {
        LOG_DBG("get_sensor_binding: layer=%d sensor=%d %s err=%d", layer, sensor, label, ret);
        return;
    }

    LOG_DBG("get_sensor_binding: layer=%d sensor=%d %s behavior=%s p1=%d p2=%d", layer, sensor,
            label, binding.behavior_dev ? binding.behavior_dev : "(null)", binding.param1,
            binding.param2);
}

static int on_get_sensor_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    log_direction(binding->param1, binding->param2, 0, "cw");
    log_direction(binding->param1, binding->param2, 1, "ccw");
    return 0;
}

static int on_get_sensor_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    return 0;
}

static const struct behavior_driver_api behavior_get_sensor_binding_driver_api = {
    .binding_pressed = on_get_sensor_binding_pressed,
    .binding_released = on_get_sensor_binding_released};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_get_sensor_binding_driver_api);

#endif // ZMK_KEYMAP_HAS_SENSORS && DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
