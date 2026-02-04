/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_input_processor_thresholder

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/input_processor.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct thresholder_config {
    uint8_t type;
    size_t codes_len;
    uint16_t codes[];
};

static int filter_val(struct input_event *event, uint32_t threshold,
                      struct zmk_input_processor_state *state) {
    if (state == NULL) {
        return 0;
    }

    int32_t value = event->value + *state->remainder;
    if (abs(value) > (int)threshold) {
        *state->remainder = 0;
    } else {
        *state->remainder = value;
        value = 0;
    }

    LOG_DBG("thresholded %d with %d to %d with remainder %d", event->value, threshold, value,
            *state->remainder);

    event->value = value;

    return 0;
}

static int thresholder_handle_event(const struct device *dev, struct input_event *event,
                                    uint32_t param1, uint32_t param2,
                                    struct zmk_input_processor_state *state) {
    const struct thresholder_config *cfg = dev->config;

    if (event->type != cfg->type) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    for (int i = 0; i < cfg->codes_len; i++) {
        if (cfg->codes[i] == event->code) {
            return filter_val(event, param1, state);
        }
    }

    return ZMK_INPUT_PROC_CONTINUE;
}

static struct zmk_input_processor_driver_api thresholder_driver_api = {
    .handle_event = thresholder_handle_event,
};

#define FILTER_INST(n)                                                                             \
    static const struct thresholder_config thresholder_config_##n = {                              \
        .type = DT_INST_PROP_OR(n, type, INPUT_EV_REL),                                            \
        .codes_len = DT_INST_PROP_LEN(n, codes),                                                   \
        .codes = DT_INST_PROP(n, codes),                                                           \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, &thresholder_config_##n, POST_KERNEL,               \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &thresholder_driver_api);

DT_INST_FOREACH_STATUS_OKAY(FILTER_INST)
#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */