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

int ssi_config(int fd);
int prepare_pkg(byte *pkg, byte opcode, const byte *param, unsigned int param_len);
int wakeup_scanner(int fd);
int ssi_write(int fd, byte opcode, const byte *param, unsigned int param_len);
int ssi_read(int fd, byte *buff);
void display_pkg(byte *pkg);

#endif /* ssi_utils_h */
