//
//  ssi_utils.c
//  zebra_scanner_C
//
//  Created by Hoàng Trung Huy on 8/22/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ssi_utils.h"
#include "ssi.h"

//  Calculate the 2's Complement checksum of the data packet
int add_checksum(byte *pkg) {
	int error = EXIT_SUCCESS;
	unsigned int cksumLSB = pkg[SSI_PKG_LENGTH] + 1;
	unsigned int cksumMSB = pkg[SSI_PKG_LENGTH];

	pkg[cksumMSB] = 0;
	pkg[cksumLSB] = 0;

	for (unsigned int i = 0; i < pkg[SSI_PKG_LENGTH]; i++)
	{
		if (pkg[cksumLSB] + pkg[i] > 0xFF)  // If adding the next byte will cause the digits to roll to the next byte, increment it manually
		{
			pkg[cksumMSB]++;
		}
		pkg[cksumLSB] += pkg[i];
	}

	// Get 1's complement
	pkg[cksumMSB] ^= 0xFF;
	pkg[cksumLSB] ^= 0xFF;

	// Get 2's complement
	if (pkg[cksumLSB] == 0xFF) {
		pkg[cksumMSB] += 1;
		pkg[cksumLSB] = 0x00;
	}
	else {
		pkg[cksumLSB] += 1;
	}

	return error;
}

int prepare_pkg(byte *pkg, byte opcode) {
	int error = EXIT_SUCCESS;

	pkg[SSI_PKG_LENGTH] = SSI_DEFAULT_LENGTH;
	pkg[SSI_PKG_OPCODE] = opcode;
	pkg[SSI_PKG_SOURCE] = SSI_HOST;
	pkg[SSI_PKG_STATUS] = SSI_DEFAULT_STATUS;

	add_checksum(pkg);

	return error;
}

int wakeup_scanner(int fd) {
	int wakeup_cmd[2] = {0x00, 0x00};

	write(fd, wakeup_cmd, 2);

	return 0;
}

void display_pkg(byte *pkg) {
	for (int i = 0; i < (pkg[SSI_PKG_LENGTH] + 2); i++) {
		printf("0x%x ", pkg[i]);
	}
	printf("\n");
}

int ssi_read(int fd, byte *buff) {
	int readByte = 0;
	int recvPkgLen = 0;
	struct termios dev_conf;

	tcgetattr(fd, &dev_conf);

	// Get package length first
	dev_conf.c_cc[VTIME] = 0;
	dev_conf.c_cc[VMIN] = 1;
	tcsetattr(fd, TCSANOW, &dev_conf);
	do {
		read(fd, &recvPkgLen, 1);
	} while (0 == recvPkgLen);
	readByte += recvPkgLen + 2;

	dev_conf.c_cc[VMIN] = readByte - 1;
	tcsetattr(fd, TCSANOW, &dev_conf);

	buff[SSI_PKG_LENGTH] = recvPkgLen;
	read(fd, &buff[1], readByte - 1);

	return readByte;
}
