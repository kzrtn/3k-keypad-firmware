## Downloading & Compiling
### Requisites
Dependencies for Ubuntu based systems:
```shell
sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```
### Compiling
```shell
git clone --recursive https://github.com/kzrtn/3k-keypad-firmware
cd 3k-keypad-firmware
mkdir build && cd build
cmake ..
make
```