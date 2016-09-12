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

int OpenTTY();
int ConfigTTY(int fd);
int ConfigSSI(int fd);
int WriteSSI(int fd, byte opcode, byte *param, byte paramLen);
int ReadSSI(int fd, byte *buff);
void DisplayPkg(byte *pkg);

#endif /* ssi_utils_h */
