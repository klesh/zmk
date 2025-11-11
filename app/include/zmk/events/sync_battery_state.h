/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_sync_battery_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool is_charging;
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
};

ZMK_EVENT_DECLARE(zmk_sync_battery_state);
