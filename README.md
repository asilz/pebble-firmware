# Getting Started

## Ubuntu

### Pre-requisites

1.

First download the Arm GNU toolchain arm-none-eabi 14.2.Rel1 from here. Make sure to make it available on your path PATH and then check GCC version is reported correctly:

```
$ arm-none-eabi-gcc --version
arm-none-eabi-gcc (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

2.

```
sudo apt update
sudo apt install clang gcc gcc-multilib git gettext python3-dev python3-venv
```
3.

Install Emscripten SDK as detailed [here](https://emscripten.org/docs/getting_started/downloads.html). Pick version 4.0.7 instead of latest when running ./emsdk install or ./emsdk activate. To conveniently access Emscripten SDK tools, the activate command will offer some suggestions. It is recommended to follow them.


### Get the source code

```
git clone --recurse-submodules https://github.com/asilz/pebble-firmware.git
cd pebble-firmware
```

### Python dependencies

```
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Building

```
./waf configure --board=nrf52840dk --nojs --gdb
./waf build
```

### Flashing

```
./waf flash
```