# Pukeeb1 Vinkey

## One-line build

From the workspace root (the directory containing `.west`):

```bash
# central side, with ZMK Studio over USB
west build -s app -d build -b nice_nano//zmk -p -S studio-rpc-usb-uart -- -DSHIELD=pukeeb1
```

The firmware is written to `build/zephyr/zmk.uf2`.

> Zephyr 4.1 renamed the board target: `nice_nano_v2` is now `nice_nano//zmk`
> (or explicitly `nice_nano@2.0.0//zmk`).

## production build
```
west build -s app -d build -b nice_nano//zmk -S studio-rpc-usb-uart -p -- -DSHIELD=pukeeb1
```

## debugging build
```
west build -s app -d build -b nice_nano//zmk -S zmk-usb-logging -S studio-rpc-usb-uart -p -- -DSHIELD=pukeeb1
```

## flash
Double-tap reset and copy the UF2 to the nice!nano drive:

```
cp build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
```
