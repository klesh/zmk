/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_cpi_state_changed {
    uint16_t cpi;
};

ZMK_EVENT_DECLARE(zmk_cpi_state_changed);
