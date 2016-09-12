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
#include <errno.h>
#include <assert.h>

#include <sys/signal.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ssi_utils.h"
#include "ssi.h"

#define TRUE	1;
#define FALSE	0;

#define MSB_16(x)		(x >> 8)
#define LSB_16(x)		(x & UINT8_MAX)

#define TIMEOUT_SEC		5;

static int CalculateChecksum(byte *pkg);
static int PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen);

//  Calculate the 2's Complement checksum of the data packet
static int CalculateChecksum(byte *pkg)
{
	int checksum = 0;

	for (unsigned int i = 0; i < PKG_LEN(pkg); i++)
	{
		checksum += pkg[i];
	}

	// 1's complement
	checksum ^= 0xFFFF;	// flip all 16 bits

	// 2's complement
	checksum += 1;

	return checksum;
}

static int PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen)
{
	int error = EXIT_SUCCESS;
	int checksum = 0;

	pkg[INDEX_LEN] = SSI_DEFAULT_LEN;
	pkg[INDEX_OPCODE] = opcode;
	pkg[INDEX_SRC] = SSI_HOST;
	pkg[INDEX_STAT] = SSI_DEFAULT_STATUS;

	if ( (NULL != param) && (0 != paramLen) )
	{
		pkg[INDEX_LEN] += paramLen;
		memcpy(&pkg[INDEX_STAT + 1], param, paramLen);
	}

	// add checksum
	checksum = CalculateChecksum(pkg);
	pkg[PKG_LEN(pkg)] = checksum >> 8;
	pkg[PKG_LEN(pkg) + 1] = checksum & 0xFF;

	return error;
}

void DisplayPkg(byte *pkg)
{
	if (NULL != pkg)
	{
		for (int i = 0; i < (PKG_LEN(pkg) + 2); i++)
		{
			printf("0x%x ", pkg[i]);
		}
		printf("\n");
	}
}

int ConfigSSI(int fd)
{
	int ret = EXIT_SUCCESS;
	const int paramLen = 3;
	byte param[3] =	{0, 0, 0};

	param[0] = SSI_BEEP_NONE;
	param[1] = SSI_SW_HANDSHAKE;
	param[2] = SSI_EN_ACK;
	printf("Enable ACK/NAK handshaking...");
	ret = WriteSSI(fd, SSI_PARAM_SEND, param, paramLen);

	if (ret < 0)
	{
		printf("ERROR: %s\n", __func__);
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

	param[0] = SSI_BEEP_NONE;
	param[1] = SSI_DEC_FORMAT;
	param[2] = SSI_DEC_PACKED;
	printf("Configure SSI package format...");
	ret = WriteSSI(fd, SSI_PARAM_SEND, param, paramLen);

	if (ret < 0)
	{
		printf("ERROR: %s\n", __func__);
	}
	else
	{
		printf("OK\n");
	}

EXIT:
	return ret;
}

int WriteSSI(int fd, byte opcode, byte *param, byte paramLen)
{
	int ret = EXIT_SUCCESS;
	byte *sendBuff = malloc( (SSI_DEFAULT_LEN + paramLen) * sizeof(byte) );
	byte *recvBuff = malloc(MAX_PKG_LEN * sizeof(byte));

	PreparePkg(sendBuff, opcode, param, paramLen);
	ret = (int) write(fd, sendBuff, sendBuff[INDEX_LEN] + 2);
	if (ret <= 0)
	{
		perror("write");
		goto EXIT;
	}
	DisplayPkg(sendBuff);

	// Check ACK
	if (SSI_CMD_ACK != opcode)
	{
		ret = ReadSSI(fd, recvBuff);
		if (SSI_CMD_ACK != recvBuff[INDEX_OPCODE])
		{
			ret = -1;
		}
	}

EXIT:
	free(sendBuff);
	free(recvBuff);
	return ret;
}

int ReadSSI(int fd, byte *buff)
{

	int ret = 0;

	// Get first byte which indicate package lenght
	ret = (int) read(fd, buff, 1);
	if (ret)
	{
		perror("read");
		goto EXIT;
	}
	printf("%x\n", buff[0]);
//	DisplayPkg(buff);

EXIT:
	return ret;
}


int OpenTTY(void)
{
	int fd = 0;
	char *dev_name = getenv("ZEBRA_SCANNER");
//	char *dev_name = "/dev/tty.usbmodem1421";

	fd = open(dev_name, O_RDWR);
	if (fd < 0)
	{
		perror(__func__);
	}

	return fd;
}

int ConfigTTY(int fd)
{
	int ret = EXIT_SUCCESS;
	int flags = 0;
	struct termios devConf;

	// Set flags for blocking mode and sync for writing
	flags = fcntl(fd, F_GETFL);
	if (0 > flags)
	{
		perror("F_GETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}
	flags |= (O_FSYNC);
	flags &= ~(O_NDELAY | O_ASYNC);

	ret = fcntl(fd, F_SETFL, flags);
	if (ret)
	{
		perror("F_SETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	// Configure tty dev
	devConf.c_cflag = (CS8 | CLOCAL | CREAD);
	devConf.c_iflag = 0;
	devConf.c_oflag = 0;
	devConf.c_lflag = 0;
	devConf.c_cc[VMIN] = 0;
	devConf.c_cc[VTIME] = 30;	// 3s timeout

	// Portability: Use cfsetspeed instead of CBAUD since c_cflag/CBAUD is not in POSIX
	ret = cfsetspeed(&devConf, BAUDRATE);
	if (ret)
	{
		perror("Set speed");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	ret = tcsetattr(fd, TCSANOW, &devConf);
	if (ret)
	{
		perror("Set attribute");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

EXIT:
	return ret;
}
