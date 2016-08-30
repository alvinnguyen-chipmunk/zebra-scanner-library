0. REQUIREMENT

Scanner must be configured in "SSI over USB CDC" mode.
Please use Windows SDK provided by Zebra to scan "SSI over USB CDC" barcode.

The setting barcode could be found in:
PL3307 IG (72E-149624-02rA).pdf - PL3307 Decoder Intergration Guide  

1. BUILD

$ source ./build.sh

OUTPUT: ./image
	App: ./image/usr/bin/zebra
	Lib: ./image/usr/lib/libssi.so

2. DEPLOY on board

$ cp ./image/* <Board rootfs>/

3. CONFIGURE device:
Please use barcode to configure decoder to transmit formatted package of barcode before using this application.

The setting barcode could be found in:
PL3307 IG (72E-149624-02rA).pdf - PL3307 Decoder Intergration Guide

4. RUN

$ export ZEBRA_SCANNER=/dev/ttyACMx (x: number)
$ zebra

NOTE: please note that device is not recognized as modem so udev rules does not work probably. Please check boot log (dmesg) to know "x"

