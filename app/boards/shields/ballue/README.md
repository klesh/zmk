
# Setup

1. install [dependencies](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-dependencies)
2. install [zephyr sdk](https://docs.zephyrproject.org/3.5.0/develop/getting_started/index.html#install-zephyr-sdk)
3. install virtualenv: `sudo apt install python3-virtualenv `
4. create virtualenv: `virtualenv .venv`
5. activate virtualenv: `source .venv/bin/activate`
6. install python packages: `pip install -r zephyr/scripts/requirements-base.txt`
7. install west: `pip install west`
8. initialize west: `west init -l app`
9. download/update modules: `west update && west zephyr-export`
10. build the shield:
```
cd app
west build -b nice_nano_v2 -- -DSHIELD=ballue
# USB logging: west build -b nice_nano_v2 -S zmk-usb-logging -- -DSHIELD=ballue
```
11. flash:
```
powershell.exe "cp build/zephyr/zmk.uf2 d:/"
```