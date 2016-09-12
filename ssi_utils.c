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

#define TIMEOUT_DEFAULT		5;

static int is_timeout=FALSE;
static int is_read=FALSE;
static int ssiDev = 0;

static byte pkg[MAX_PKG_LEN] = {0};
static int byteRead = 0;

static void HandleSignal(int sig);
static int CalculateChecksum(byte *pkg);
static int PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen);

int ConfigSSI(int fd)
{
	int ret = EXIT_SUCCESS;
	const int paramLen = 3;
	byte param[3] = {SSI_BEEP_NONE, SSI_DEC_FORMAT, SSI_DEC_PACKED};

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

	return ret;
}

//  Calculate the 2's Complement checksum of the data packet
static int CalculateChecksum(byte *pkg) {
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

static int PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen) {
	int error = EXIT_SUCCESS;
	int checksum = 0;

	pkg[INDEX_LEN] = SSI_DEFAULT_LEN + paramLen;
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

int wakeup_scanner(int fd) {
	int wakeup_cmd[2] = {0x00, 0x00};

	write(fd, wakeup_cmd, 2);

	return 0;
}

void DisplayPkg(byte *pkg) {
	if (NULL != pkg)
	{
		for (int i = 0; i < (PKG_LEN(pkg) + 2); i++) {
			printf("0x%x ", pkg[i]);
		}
		printf("\n");
	}
}

int WriteSSI(int fd, byte opcode, byte *param, byte paramLen)
{
	int ret = EXIT_SUCCESS;
	byte *sendBuff = malloc( (SSI_DEFAULT_LEN + paramLen) * sizeof(byte) );
	byte *recvBuff = malloc(MAX_PKG_LEN * sizeof(byte));

	PreparePkg(sendBuff, opcode, param, paramLen);
	write(fd, sendBuff, sendBuff[INDEX_LEN]);
	DisplayPkg(sendBuff);

	if (SSI_CMD_ACK != opcode)
	{
		// Check ACK
		ret = ReadSSI(fd, recvBuff);
		if (SSI_CMD_ACK != recvBuff[INDEX_OPCODE])
		{
			ret = -1;
			DisplayPkg(recvBuff);
		}
	}

	free(sendBuff);
	free(recvBuff);
	return ret;
}

int ReadSSI(int fd, byte *buff) {
	int timeoutValue = 0;
	int ret = 0;
	struct itimerval timeoutTimer;
	ssiDev = fd;
	is_timeout = FALSE;
	is_read = FALSE;

	if (NULL == getenv("ZEBRA_TIMEOUT"))
	{
		timeoutValue = TIMEOUT_DEFAULT;
	}
	else
	{
		timeoutValue = atoi(getenv("ZEBRA_TIMEOUT"));
	}

	// Setup one shot timeout timer
	timeoutTimer.it_interval.tv_sec = 0;
	timeoutTimer.it_interval.tv_usec = 0;
	timeoutTimer.it_value.tv_sec = timeoutValue;
	timeoutTimer.it_value.tv_usec = 0;

	// Start timer
	signal(SIGALRM, HandleSignal);
	setitimer(ITIMER_REAL, &timeoutTimer, NULL);

	// Enable interrupt for receiver
	signal(SIGIO, HandleSignal);

	while (! is_timeout)
	{
		if (is_read)
		{
			int checksum = 0;
			checksum = CalculateChecksum(pkg);

			if ( (MSB_16(checksum) == pkg[PKG_LEN(pkg)]) && (LSB_16(checksum) == pkg[PKG_LEN(pkg) + 1]) )
			{
				ret = byteRead;
				memcpy(buff, pkg, (PKG_LEN(pkg) + 2) );
				break;
			}
			else
			{
				ret = -ECKSUM;
				buff = NULL;
				break;
			}
		}
	}
	signal(SIGIO, SIG_IGN);
	signal(SIGALRM, SIG_IGN);

	if (is_timeout)
	{
		ret = -ETIME;
		buff = NULL;
	}

	return ret;
}

static void HandleSignal(int sig)
{
	if (SIGALRM == sig)
	{
		printf("TIMEOUT\t");
		is_timeout = TRUE;
	}
	else if (SIGIO == sig)
	{
		int pkgLen = 0;

		read(ssiDev, &pkgLen, 1);
		if (pkgLen > 0)
		{
			pkg[0] = pkgLen;
			byteRead++;
			byteRead += read(ssiDev, &pkg[1], pkgLen+1);
			is_read = TRUE;
		}

		signal(sig, HandleSignal);
	}
}

int ConfigTTY(int fd)
{
	int ret = EXIT_SUCCESS;
	int flags = 0;
	struct termios devConf;

	// Set flags for blocking mode and sync for writing
	flags = fcntl(fd, F_GETFL);
	if (0 > flags) {
		perror("F_GETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}
	flags |= (O_NDELAY | O_ASYNC);

	ret = fcntl(fd, F_SETFL, flags);
	if (ret) {
		perror("F_SETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	// Configure tty dev
	devConf.c_cflag = (CRTSCTS | CS8 | CLOCAL | CREAD);
	devConf.c_iflag = 0;
	devConf.c_oflag = 0;
	devConf.c_lflag = 0;
	devConf.c_cc[VMIN] = 0;
	devConf.c_cc[VTIME] = 30;	// 3s timeout

	ret = cfsetspeed(&devConf, BAUDRATE);
	if (ret) {
		perror("Set speed");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	ret = tcsetattr(fd, TCSANOW, &devConf);
	if (ret) {
		perror("Set attribute");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	ret = fcntl(fd, F_SETOWN, getpid());
	if (ret) {
		perror("F_SETOWN");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

EXIT:
	return ret;
}

int OpenTTY(void)
{
	int fd = 0;
	char *dev_name = getenv("ZEBRA_SCANNER");

	fd = open(dev_name, O_RDWR);
	if (fd < 0) {
		perror(__func__);
	}

	return fd;
}
