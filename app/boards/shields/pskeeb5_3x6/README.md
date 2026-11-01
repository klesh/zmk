# pskeeb5 3x6

A 3x6 variant of the [pskeeb5](../pskeeb5/) split keyboard. Compared with the
3x5 shield, each half gains one matrix column (six columns instead of five),
for 44 keys total: 3 rows of 6 keys per half plus 4 thumb keys per half.

The 3x5 shield is unchanged and still builds as `pskeeb5_left` /
`pskeeb5_right`.

## One-line build

From the workspace root (the directory containing `.west`):

```bash
# right / central side, with ZMK Studio over USB
west build -s app -d build -b nice_nano//zmk -p -S studio-rpc-usb-uart -- -DSHIELD=pskeeb5_3x6_right
```

The firmware is written to `build/zephyr/zmk.uf2`.

## production build

```
# left
west build -s app -d build -b nice_nano//zmk -p -- -DSHIELD=pskeeb5_3x6_left
# right
west build -s app -d build -b nice_nano//zmk -S studio-rpc-usb-uart -p -- -DSHIELD=pskeeb5_3x6_right
```

## debugging build

```
west build -s app -d build -b nice_nano//zmk -S zmk-usb-logging -p -- -DSHIELD=pskeeb5_3x6_left
west build -s app -d build -b nice_nano//zmk -S zmk-usb-logging -S studio-rpc-usb-uart -p -- -DSHIELD=pskeeb5_3x6_right
```

## matrix wiring

Each side uses six column GPIOs. The extra pin (added compared with the 3x5
shield) is `pro_micro 4`:

| side  | column GPIOs (left to right)                          |
| ----- | ----------------------------------------------------- |
| left  | 4, 8, 7, 6, 14, 15                                    |
| right | 15, 14, 6, 7, 8, 4                                     |

Encoders are still wired to `pro_micro 14` on each half, which puts the
encoder push switches at matrix positions `RC(3,4)` (position 36) and
`RC(3,7)` (position 43).

## flash

Double-tap reset and copy the UF2 to the nice!nano drive:

```
cp build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
```
