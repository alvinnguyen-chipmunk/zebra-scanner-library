//
//  control.hpp
//  zebra_scanner
//
//  Created by Hoàng Trung Huy on 8/19/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#ifndef ssi_utils_h
#define ssi_utils_h

#include <stdint.h>
#include <termios.h>
#include "ssi.h"

int defconfig_ssi_dev(int fd, struct termios *dev_conf);
int config_scanner(int fd);

int prepare_pkg(byte *pkg, byte opcode);

int wakeup_scanner(int fd);
int ssi_read(int fd, byte *buff);

byte *extract_barcode(byte *pkg);

void display_pkg(byte *pkg);
void display_barcode(byte *barcode);
byte *getSymbology(byte barcodeType);

int length_of_(byte *pkg);

#endif /* ssi_utils_h */
