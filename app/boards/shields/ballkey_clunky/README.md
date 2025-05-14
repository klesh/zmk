
# Setup

1. install [dependencies](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-dependencies)
2. install [zephyr sdk](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-zephyr-sdk)
3. install virtualenv: `sudo apt install python3-virtualenv protobuf-compiler`
4. create virtualenv: `virtualenv .venv`
5. activate virtualenv: `source .venv/bin/activate`
6. install west: `pip install west protobuf grpcio-tools`
7. initialize west: `west init -l app`
8. download/update modules: `west update`, `west zephyr-export`
9. install python packages: `pip install -r zephyr/scripts/requirements-base.txt`
10. build the shield:
```
cd app
# default
west -s app build -b nice_nano_v2 -- -DSHIELD=ballkey_clunky
# USB logging
west -s app build -b nice_nano_v2 -S zmk-usb-logging -- -DSHIELD=ballkey_clunky
# ZMK Studio
west -s app build -p -b nice_nano_v2 -S zmk-usb-logging -S studio-rpc-usb-uart -- -DSHIELD=ballkey_clunky -DCONFIG_ZMK_STUDIO=y
```
11. flash:
```
powershell.exe "cp build/zephyr/zmk.uf2 d:/"
cp build/zephyr/zmk.uf2 /run/media/klesh/NICENANO
```