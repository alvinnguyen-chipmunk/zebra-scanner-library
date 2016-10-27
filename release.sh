#!/bin/bash

function build()
{
	sync
	make clean
	make lib
	make
	export LD_LIBRARY_PATH=$PWD/image/usr/lib
	export ZEBRA_SCANNER=/dev/ttyACM0
	ln -sf ./image/usr/bin/zebra-demo 
}

CROSS_COMPILE=/opt/poky/2.1.1/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
if [[ ! -d ./release ]]; then
	mkdir release
fi
cd ./image
tar cJf ./mlsScanner.tar.xz ./*
cd ..
mv ./image/mlsScanner.tar.xz ./release

