#!/bin/sh
# this file will compile and send the binary to an AtTiny45 on the USB ISP programmer

mkdir -p build
cd build
cmake -G "CodeBlocks - Unix Makefiles" ..
make
make flash


