/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_keymap_settings

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/keymap.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

enum keymap_settings_op {
    KEYMAP_SETTINGS_CHECK_UNSAVED = 0,
    KEYMAP_SETTINGS_SAVE = 1,
    KEYMAP_SETTINGS_DISCARD = 2,
    KEYMAP_SETTINGS_RESET = 3,
};

static int on_keymap_settings_binding_pressed(struct zmk_behavior_binding *binding,
                                              struct zmk_behavior_binding_event event) {
    switch (binding->param1) {
    case KEYMAP_SETTINGS_CHECK_UNSAVED: {
        int ret = zmk_keymap_check_unsaved_changes();
        LOG_DBG("keymap_settings: check_unsaved_changes=%d", ret);
        return ret < 0 ? ret : 0;
    }
    case KEYMAP_SETTINGS_SAVE: {
        int ret = zmk_keymap_save_changes();
        LOG_DBG("keymap_settings: save_changes=%d", ret);
        return ret < 0 ? ret : 0;
    }
    case KEYMAP_SETTINGS_DISCARD: {
        int ret = zmk_keymap_discard_changes();
        LOG_DBG("keymap_settings: discard_changes=%d", ret);
        return ret < 0 ? ret : 0;
    }
    case KEYMAP_SETTINGS_RESET: {
        int ret = zmk_keymap_reset_settings();
        LOG_DBG("keymap_settings: reset_settings=%d", ret);
        return ret < 0 ? ret : 0;
    }
    default:
        LOG_ERR("Unknown keymap settings operation %d", binding->param1);
        return -EINVAL;
    }
}

static int on_keymap_settings_binding_released(struct zmk_behavior_binding *binding,
                                               struct zmk_behavior_binding_event event) {
    return 0;
}

static const struct behavior_driver_api behavior_keymap_settings_driver_api = {
    .binding_pressed = on_keymap_settings_binding_pressed,
    .binding_released = on_keymap_settings_binding_released};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                        &behavior_keymap_settings_driver_api);

#endif // DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
