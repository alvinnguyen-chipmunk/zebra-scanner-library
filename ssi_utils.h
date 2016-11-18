/*******************************************************************************
     (C) Copyright 2009 Styl Solutions Co., Ltd. , All rights reserved *
     *
     This source code and any compilation or derivative thereof is the sole *
     property of Styl Solutions Co., Ltd. and is provided pursuant to a *
     Software License Agreement. This code is the proprietary information *
     of Styl Solutions Co., Ltd and is confidential in nature. Its use and *
     dissemination by any party other than Styl Solutions Co., Ltd is *
     strictly limited by the confidential information provisions of the *
     Agreement referenced above. *
     ******************************************************************************/

#ifndef ssi_utils_h
#define ssi_utils_h

#include <stdint.h>
#include <termios.h>
#include "ssi.h"

int OpenTTY(char *name);
int ConfigTTY(int fd);
int ConfigSSI(int fd);
int WriteSSI(int fd, byte opcode, byte *param, byte paramLen);
int ReadSSI(int fd, byte *buff, const int timeout);
int CheckACK(int fd);
void PrintError(int ret);
int ExtractBarcode(char *buff, byte *pkg, const int buffLength);
void DisplayPkg(byte *pkg);

#endif /* ssi_utils_h */
