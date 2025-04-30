
# Setup

1. install [dependencies](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-dependencies)
2. install [zephyr sdk](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-zephyr-sdk)
3. install virtualenv: `sudo apt install python3-virtualenv protobuf-compiler`
4. create virtualenv: `virtualenv .venv`
5. activate virtualenv: `source .venv/bin/activate`
6. install west: `pip install west protobuf grpcio-tools west`
7. initialize west: `west init -l app`
8. download/update modules: `west update && west zephyr-export`
9. install python packages: `pip install -r zephyr/scripts/requirements-base.txt`
10. build the shield:
```
$env.ZEPHYR_SDK_INSTALL_DIR = "/home/klesh/.local/opt/zephyr-sdk-0.16.3"
$env.ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
cd app
# default
west build -b nice_nano_v2 -- -DSHIELD=ballkey_clunky
# USB logging
west build -b nice_nano_v2 -S zmk-usb-logging -- -DSHIELD=ballkey_clunky
# ZMK Studio
west build -p -b nice_nano_v2 -S zmk-usb-logging -S studio-rpc-usb-uart -- -DSHIELD=ballkey_clunky -DCONFIG_ZMK_STUDIO=y


# wired
west build -p -b waveshare_rp2040_zero -S zmk-usb-logging -S studio-rpc-usb-uart -- -DSHIELD=ballkey_clunky_usb -DCONFIG_ZMK_STUDIO=y
```
11. flash:
```
powershell.exe "cp build/zephyr/zmk.uf2 d:/"
```
