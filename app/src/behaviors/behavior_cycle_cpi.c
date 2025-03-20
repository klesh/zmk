/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_cycle_cpi

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <string.h>

#include <zmk/behavior.h>
#include <dt-bindings/zmk/cycle_cpi.h>
#include <zmk/events/cpi_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static const struct behavior_parameter_value_metadata param_values[] = {
    {
        .display_name = "Next",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = CCPI_NEXT,
    },
    {
        .display_name = "Prev",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_VALUE,
        .value = CCPI_PREV,
    },
};

static const struct behavior_parameter_metadata_set param_metadata_set[] = {{
    .param1_values = param_values,
    .param1_values_len = ARRAY_SIZE(param_values),
}};

static const struct behavior_parameter_metadata metadata = {
    .sets_len = ARRAY_SIZE(param_metadata_set),
    .sets = param_metadata_set,
};

#endif

struct cycle_cpi_config {
    const struct device *device;
    uint8_t default_cpi_index;
    uint16_t init_delay;
    int16_t sensor_channel;
    int16_t sensor_attr;
    uint8_t cpis_len;
    uint16_t cpis[];
};

struct cycle_cpi_data {
    uint8_t cpi_idx;
    // #if IS_ENABLED(CONFIG_SETTINGS)
    struct k_work_delayable init_work;
    struct k_work_delayable setup_work;
    struct k_work_delayable save_work;
    const struct device *behavior;
    // #endif
};

struct cycle_cpi_setting_cb_param {
    const struct cycle_cpi_config *config;
    struct cycle_cpi_data *data;
};

static int set_cpi(const struct cycle_cpi_config *cfg, uint16_t cpi) {
    LOG_DBG("set %s cpi to %d", cfg->device->name, cpi);
    struct sensor_value val = {.val1 = cpi, .val2 = 0};
    int err = sensor_attr_set(cfg->device, cfg->sensor_channel, cfg->sensor_attr, &val);
    if (err) {
        LOG_ERR("Failed to set CPI");
        return err;
    }
    raise_zmk_cpi_state_changed((struct zmk_cpi_state_changed){.cpi = cpi});
    return 0;
}

// #if IS_ENABLED(CONFIG_SETTINGS)

static int cycle_cpi_settings_load_cb(const char *key, size_t len, settings_read_cb read_cb,
                                      void *cb_arg, void *param) {
    LOG_DBG("setting loaded for key %s", key);
    struct cycle_cpi_data *data = param;
    const struct cycle_cpi_config *cfg = data->behavior->config;
    const char *next;
    int ret = 0;
    if (settings_name_steq(key, "cpi_idx", &next) && !next) {
        if (len != sizeof(data->cpi_idx)) {
            ret = -EINVAL;
        } else {
            int rc = read_cb(cb_arg, &data->cpi_idx, sizeof(data->cpi_idx));
            if (rc < 0) {
                ret = -EINVAL;
            }
            ret = MIN(rc, 0);
        }
    }
    if (ret == -EINVAL) {
        data->cpi_idx = cfg->default_cpi_index;
    }
    uint16_t cpi = cfg->cpis[data->cpi_idx];
    set_cpi(cfg, cpi);
    return ret;
}

// static void cycle_cpi_setup_work_handler(struct k_work *work) {
//     LOG_DBG("device initial cpi setup");
//     struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
//     struct cycle_cpi_data *data = CONTAINER_OF(dwork, struct cycle_cpi_data, init_work);
//     const struct cycle_cpi_config *cfg = data->behavior->config;
//     uint16_t cpi = cfg->cpis[data->cpi_idx];
//     LOG_DBG("setting initial cpi to %d for %s", cpi, cfg->device->name);
// }

static void cycle_cpi_save_work_handler(struct k_work *work) {
    struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
    struct cycle_cpi_data *data = CONTAINER_OF(dwork, struct cycle_cpi_data, save_work);
    const struct cycle_cpi_config *cfg = data->behavior->config;
    const struct device *device = cfg->device;
    char setting_name[32];
    sprintf(setting_name, "cycle_cpi/%s/cpi_idx", device->name);
    LOG_DBG("saving setting %s to %d", setting_name, data->cpi_idx);
    settings_save_one(setting_name, &data->cpi_idx, sizeof(data->cpi_idx));
}
// #endif

static void cycle_cpi_init_work_handler(struct k_work *work) {
    LOG_DBG("async init");
    struct k_work_delayable *dwork = CONTAINER_OF(work, struct k_work_delayable, work);
    struct cycle_cpi_data *data = CONTAINER_OF(dwork, struct cycle_cpi_data, init_work);
    const struct cycle_cpi_config *cfg = data->behavior->config;
    const struct device *device = cfg->device;

    // load setting
    char setting_name[32];
    sprintf(setting_name, "cycle_cpi/%s", device->name);
    LOG_DBG("start loading settings for %s", setting_name);
    settings_load_subtree_direct(setting_name, cycle_cpi_settings_load_cb, data);
}

static int behavior_cycle_cpi_init(const struct device *dev) {
    LOG_DBG("init");
    // #if IS_ENABLED(CONFIG_SETTINGS)
    const struct cycle_cpi_config *cfg = dev->config;
    struct cycle_cpi_data *data = dev->data;
    data->behavior = dev;
    k_work_init_delayable(&data->init_work, cycle_cpi_init_work_handler);
    k_work_init_delayable(&data->save_work, cycle_cpi_save_work_handler);
    LOG_DBG("schedule async intialization, dealy %d", cfg->init_delay);
    k_work_schedule(&data->init_work, K_MSEC(cfg->init_delay));
    // #endif
    return 0;
};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {

    int8_t delta = binding->param1 == CCPI_NEXT ? 1 : -1;
    LOG_INF("Cycling CPI, delta: %d", delta);
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct cycle_cpi_config *cfg = dev->config;
    struct cycle_cpi_data *data = dev->data;

    data->cpi_idx = (data->cpi_idx + delta) % cfg->cpis_len;
    uint16_t cpi = cfg->cpis[data->cpi_idx];
    int err = set_cpi(cfg, cpi);
    if (err) {
        return err;
    }
    // #if IS_ENABLED(CONFIG_SETTINGS)
    // k_work_reschedule(&data->save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
    k_work_reschedule(&data->save_work, K_MSEC(1000));
    // #endif
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
    .parameter_metadata = &metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

#define FALLBACK(val, def) ((val == -1) ? def : val)

#define CYCLE_CPI_INST(n)                                                                          \
    const struct cycle_cpi_config ccpi_config_##n = {                                              \
        .device = DEVICE_DT_GET(DT_INST_PHANDLE(n, device)),                                       \
        .sensor_channel = FALLBACK(DT_INST_PROP(n, sensor_channel), SENSOR_CHAN_ALL),              \
        .sensor_attr = FALLBACK(DT_INST_PROP(n, sensor_attr), SENSOR_ATTR_SAMPLING_FREQUENCY),     \
        .init_delay = DT_INST_PROP(n, init_delay),                                                 \
        .cpis_len = DT_INST_PROP_LEN(n, cpis),                                                     \
        .default_cpi_index = DT_INST_PROP(n, default_cpi_index),                                   \
        .cpis = DT_INST_PROP(n, cpis),                                                             \
    };                                                                                             \
    static struct cycle_cpi_data ccpi_data_##n = {                                                 \
        .cpi_idx = DT_INST_PROP(n, default_cpi_index),                                             \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_cycle_cpi_init, NULL, &ccpi_data_##n, &ccpi_config_##n,    \
                            POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY,                         \
                            &behavior_cycle_cpi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CYCLE_CPI_INST)