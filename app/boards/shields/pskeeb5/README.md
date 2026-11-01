# pskeeb5

This shield is the 3x5 layout. For the 3x6 variant (one extra column per half),
see [`pskeeb5_3x6`](../pskeeb5_3x6/).

## One-line build

From the workspace root (the directory containing `.west`):

```bash
# right / central side, with ZMK Studio over USB
west build -s app -d build -b nice_nano//zmk -p -S studio-rpc-usb-uart -- -DSHIELD=pskeeb5_right
```

The firmware is written to `build/zephyr/zmk.uf2`.

> Zephyr 4.1 renamed the board target: `nice_nano_v2` is now `nice_nano//zmk`
> (or explicitly `nice_nano@2.0.0//zmk`).

## toolchain setup

fish config:
```fish
set -x ZEPHYR_TOOLCHAIN_VARIANT zephyr
set -x ZEPHYR_SDK_INSTALL_DIR /home/klesh/zephyr-sdk-0.16.3
```

## production build
```
# left
west build -s app -d build -b nice_nano//zmk -p -- -DSHIELD=pskeeb5_left
# right
west build -s app -d build -b nice_nano//zmk -S studio-rpc-usb-uart -p -- -DSHIELD=pskeeb5_right
```

## debugging build
```
west build -s app -d build -b nice_nano//zmk -S zmk-usb-logging -p -- -DSHIELD=pskeeb5_left
west build -s app -d build -b nice_nano//zmk -S zmk-usb-logging -S studio-rpc-usb-uart -p -- -DSHIELD=pskeeb5_right
```

## flash
Double-tap reset and copy the UF2 to the nice!nano drive:

```
cp build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
```
