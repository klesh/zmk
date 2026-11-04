/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

enum zmk_activity_state { ZMK_ACTIVITY_ACTIVE, ZMK_ACTIVITY_IDLE, ZMK_ACTIVITY_SLEEP };

enum zmk_activity_state zmk_activity_get_state(void);

/**
 * @brief Note user activity and restart the idle timer.
 *
 * Transitions back to @ref ZMK_ACTIVITY_ACTIVE and raises a
 * @ref zmk_activity_state_changed event when the state actually changes, so
 * listeners (e.g. display blanking) can react.
 */
void zmk_activity_note_activity(void);
