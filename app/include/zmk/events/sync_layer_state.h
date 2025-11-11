/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_sync_layer_state {
    uint8_t index;
    // char label[3];
};

ZMK_EVENT_DECLARE(zmk_sync_layer_state);