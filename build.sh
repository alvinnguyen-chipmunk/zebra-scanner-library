#!/bin/bash

make clean
make lib
make
export LD_LIBRARY_PATH=$PWD/image/usr/lib
export ZEBRA_SCANNER=/dev/ttyACM0
ln -sf ./image/usr/bin/zebra-demo 
