# Getting Started

### Pre-requisites

1.

First download the Arm GNU toolchain `arm-none-eabi` 14.2.Rel1 from here. Make sure to make it available on your path PATH and then check GCC version is reported correctly:

```
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

2.
```shell
sudo apt update
sudo apt install clang gcc gcc-multilib git gettext python3-dev python3-venv
```

3.

Install Emscripten SDK as detailed [here](https://emscripten.org/docs/getting_started/downloads.html). Pick version `4.0.7` instead of `latest` when running `./emsdk install` or `./emsdk activate`. To conveniently access Emscripten SDK tools, the activate command will offer some suggestions — it is recommended to follow them.


### Get the source code

```shell
git clone --recurse-submodules https://github.com/asilz/pebble-firmware.git
cd pebble-firmware
```

### Python dependencies
A series of additional Python dependencies are also required. Follow the next steps to install them in a [Python virtual environment](https://docs.python.org/3/library/venv.html).
```shell
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Building

```shell
./waf configure --board=nrf52840dk --nojs --gdb
./waf build
```
### Build bootloader
```shell
cd platform/nrf52840dk/boot
./waf configure
./waf build
```

### Flashing
This flashes both the bootloader and the firmware.
```shell
openocd -f openocd.cfg -c "init; reset halt; program platform/nrf52840dk/boot/build/tintin_boot.elf;program build/src/fw/tintin_fw.hex reset; ; shutdown" 2>&1 | tee .waf.openocd.log
```
If you only need to flash the firmware you can use the following command:
```shell
./waf flash
```
# Changes
- Edited [third_party/nimble/wscript](third_party/nimble/wscript) to link Nordic Softdevice Controller link layer instead of Nimble's link lyaer.
- Added new target `nrf52840dk`, changes and additions can be found in directiories: [src/fw/mfg/nrf52840dk](src/fw/mfg/nrf52840dk), [platform/nrf52840dk](platform/nrf52840dk), and  [resources/normal/nrf52840dk](resources/normal/nrf52840dk).
- Added Nordic Softdevice Controller Link Layer to Nimble. Changes can be found in [third_party/nimble/mynewt-nimble](https://github.com/asilz/mynewt-nimble/tree/0c47225d0fcf2bf410c84c64a8934aa4bff6d2e5) which is a submodule of this repo — for more details follow the link.
- Various big fixes.
