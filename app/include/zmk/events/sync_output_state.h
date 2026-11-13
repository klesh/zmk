/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints_types.h>

struct zmk_sync_output_state {
    struct zmk_endpoint_instance selected_endpoint;
    enum zmk_transport preferred_transport;
    uint8_t active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
};

ZMK_EVENT_DECLARE(zmk_sync_output_state);
