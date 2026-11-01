# ZMK Studio rotary encoder customization

This repo carries the runtime-customizable rotary encoder feature (commit
`820b70fb`). It exposes each encoder direction as a synthetic key position in
the Studio keymap and physical layout, backed by per-layer sensor binding
overrides that persist through settings.

A shield needs three pieces:

## 1. Sensors node

Defined once (shared `*-common.dtsi` is fine):

```dts
sensors: sensors {
    status = "okay";
    compatible = "zmk,keymap-sensors";
    sensors = <&left_encoder &right_encoder>;
    triggers-per-rotation = <9>;
};
```

The order of `sensors` is the sensor index order used by both the keymap
`sensor-bindings` and the physical layout `sensor-key-positions`.

## 2. Per-layer sensor bindings

Every keymap layer must have one two-cell binding per sensor, using a
`zmk,behavior-sensor-rotate` or `-var` behavior (e.g. `&inc_dec_kp`):

```dts
sensor-bindings = <&inc_dec_kp C_VOL_DN C_VOL_UP &inc_dec_kp LEFT RIGHT>;
```

These are the devicetree defaults; Studio edits become per-layer overrides.
`CONFIG_ZMK_STUDIO` selects `CONFIG_ZMK_KEYMAP_SETTINGS_STORAGE`, so overrides
survive reboots.

## 3. Physical layout `sensor-key-positions`

Add it to the `zmk,physical-layout` node:

```dts
// Encoder push switches are at RC(3,4) (position 36) and RC(3,7)
// (position 43). Studio places the synthetic CW/CCW keys there.
sensor-key-positions = <36 43>;
```

### Position semantics (the common mistake)

`sensor-key-positions` is **indices into the physical layout's `keys` array**
(which for a single layout is the transform `map` order), in `sensors` order.
It is **not** `RC(row, col)` and **not** `row * columns + col`.

Compute the value by counting entries in the transform map:

```dts
map = <
    RC(0, 0) ... RC(0,11)   // 0..11
    RC(1, 0) ... RC(1,11)   // 12..23
    RC(2, 0) ... RC(2,11)   // 24..35
              RC(3, 4) RC(3, 1) RC(3, 2) RC(3, 3)     RC(3, 8) RC(3, 9) RC(3,10) RC(3, 7)
              //       36       37       38       39           40       41       42       43
>;
```

So `RC(3,4)` is position **36**, and `RC(3,7)` is position **43**. Using the
matrix index `40` for `RC(3,4)` would anchor the left encoder's CW/CCW keys
under `keys[40]` (the other half's thumb key).

Reference values:

| shield | encoders | `sensor-key-positions` |
| --- | --- | --- |
| `pskeeb5` (3x5) | `RC(3,3)`, `RC(3,6)` | `<30 37>` |
| `pskeeb5_3x6` | `RC(3,4)`, `RC(3,7)` | `<36 43>` |
| `pukeeb1_vinkey` | `RC(3,1)`, `RC(3,8)` | `<30 37>` |

## How the Studio side works

- `app/src/studio/keymap_subsystem.c` sends `layout_keys_len + 2 * sensors`
  layer bindings and layout keys. Encoder direction positions are
  `layout_keys_len + 2 * sensor + direction` (0 = CW, 1 = CCW).
- Synthetic direction keys are 70% size (70 units), centered below the anchor
  key (`SENSOR_DIRECTION_KEY_SIZE/GAP/OFFSET` in `keymap_subsystem.c`). If no
  `sensor-key-positions` entry exists, they fall back to a cluster to the right
  of the keyboard.
- `zmk_keymap_set_layer_binding_at_idx()` treats positions at or above
  `layout_keys_len` as sensor overrides.

## Verify

After building, confirm the property reached the devicetree:

```bash
grep -n "sensor-key-positions" build/zephyr/zephyr.dts
# e.g. sensor-key-positions = < 0x24 0x2b >;   -> 36, 43
```

Also check that `sensors` is `status = "okay"` and lists the encoders in the
intended order. If Studio does not show the CW/CCW pairs, first suspect a
firmware built before `sensor-key-positions` was added, not the layout file.
