

## production build
```
# left
west build -s app -d build -b nice_nano_v2  -p  -- -DSHIELD=pskeeb5_left
# right
west build -s app -d build -b nice_nano_v2 -S studio-rpc-usb-uart -p  -- -DSHIELD=pskeeb5_right -DCONFIG_ZMK_STUDIO=y
```

## debugging build
```
west build -s app -d build -b nice_nano_v2 -S zmk-usb-logging -p -- -DSHIELD=pskeeb5_left
west build -s app -d build -b nice_nano_v2 -S zmk-usb-logging -S studio-rpc-usb-uart -p  -- -DSHIELD=pskeeb5_right -DCONFIG_ZMK_STUDIO=y
```