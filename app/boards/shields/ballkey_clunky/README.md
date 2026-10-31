# Ballkey Clunky

## One-line build

From the workspace root (the directory containing `.west`):

```bash
# right / default side, with ZMK Studio over USB
west build -s app -d build -b nice_nano//zmk -p -S studio-rpc-usb-uart -- -DSHIELD=ballkey_clunky
```

The firmware is written to `build/zephyr/zmk.uf2`.

> Zephyr 4.1 renamed the board target: `nice_nano_v2` is now `nice_nano//zmk`
> (or explicitly `nice_nano@2.0.0//zmk`).

# Setup

1. install [dependencies](https://docs.zephyrproject.org/4.1.0/develop/getting_started/index.html#install-dependencies)
2. install [zephyr sdk](https://docs.zephyrproject.org/4.1.0/develop/getting_started/index.html#install-zephyr-sdk)
3. install virtualenv: `sudo apt install python3-virtualenv protobuf-compiler`
4. create virtualenv: `virtualenv .venv`
5. activate virtualenv: `source .venv/bin/activate`
6. install west: `pip install west protobuf grpcio-tools setuptools<=81.0.0`
7. initialize west: `west init -l app`
8. download/update modules: `west update`, `west zephyr-export`
9. install python packages: `pip install -r zephyr/scripts/requirements-base.txt`
10. build the shield (from the workspace root):
```
# default
west build -s app -d build -b nice_nano//zmk -p -- -DSHIELD=ballkey_clunky
# left-hand variant
west build -s app -d build -b nice_nano//zmk -p -- -DSHIELD=ballkey_clunky_left
# USB logging
west build -s app -d build -b nice_nano//zmk -p -S zmk-usb-logging -- -DSHIELD=ballkey_clunky
# ZMK Studio
west build -s app -d build -b nice_nano//zmk -p -S studio-rpc-usb-uart -- -DSHIELD=ballkey_clunky
```
11. flash:
```
powershell.exe "cp build/zephyr/zmk.uf2 d:/"
cp build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
```
