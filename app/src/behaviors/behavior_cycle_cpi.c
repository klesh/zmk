/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_cycle_cpi

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/behavior.h>
#include <dt-bindings/zmk/cycle_cpi.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct cycle_cpi_config {
    const struct device *device;
    uint8_t initial_cpi_index;
    int16_t sensor_channel;
    int16_t sensor_attr;
    uint8_t cpis_len;
    uint16_t cpis[];
};

struct cycle_cpi_data {
    uint8_t cpi_idx;
};

static int behavior_cycle_cpi_init(const struct device *dev) { return 0; };

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {

    int8_t delta = binding->param1 == CCPI_NEXT ? 1 : -1;
    LOG_INF("Cycling CPI, delta: %d", delta);
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct cycle_cpi_config *cfg = dev->config;
    struct cycle_cpi_data *data = dev->data;

    data->cpi_idx = (data->cpi_idx + delta) % cfg->cpis_len;
    struct sensor_value val = {.val1 = cfg->cpis[data->cpi_idx], .val2 = 0};
    int err = sensor_attr_set(cfg->device, cfg->sensor_channel, cfg->sensor_attr, &val);
    if (err) {
        LOG_ERR("Failed to set CPI");
        return err;
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_cycle_cpi_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

#define FALLBACK(val, def) ((val == -1) ? def : val)

#define CYCLE_CPI_INST(n)                                                                          \
    const struct cycle_cpi_config ccpi_config_##n = {                                              \
        .device = DEVICE_DT_GET(DT_INST_PHANDLE(n, device)),                                       \
        .sensor_channel = FALLBACK(DT_INST_PROP(n, sensor_channel), SENSOR_CHAN_ALL),              \
        .sensor_attr = FALLBACK(DT_INST_PROP(n, sensor_attr), SENSOR_ATTR_SAMPLING_FREQUENCY),     \
        .cpis_len = DT_INST_PROP_LEN(n, cpis),                                                     \
        .cpis = DT_INST_PROP(n, cpis),                                                             \
    };                                                                                             \
    static struct cycle_cpi_data ccpi_data_##n = {                                                 \
        .cpi_idx = DT_INST_PROP(n, initial_cpi_index),                                             \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_cycle_cpi_init, NULL, &ccpi_data_##n, &ccpi_config_##n,    \
                            POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,                         \
                            &behavior_cycle_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CYCLE_CPI_INST)
