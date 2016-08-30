#!/bin/bash

make clean
make lib
make
export LD_LIBRARY_PATH=$PWD/image/usr/lib
ln -sf ./image/usr/bin/zebra-demo 
