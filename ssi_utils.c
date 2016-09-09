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

#include <signal.h>
#include <sys/time.h>

#include "ssi_utils.h"
#include "ssi.h"

#define TRUE	1;
#define FALSE	0;

#define MSB_16(x)		(x >> 8)
#define LSB_16(x)		(x & UINT8_MAX)

#define TIMEOUT_DEFAULT		5;

static int is_timeout=FALSE;
static int is_read=FALSE;
static int scanner = 0;

static byte pkg[MAX_PKG_LEN] = {0};
static int byteRead = 0;

static void HandleSignal(int sig);
static int CalculateChecksum(byte *pkg);

int ssi_config(int fd)
{
	int ret = EXIT_SUCCESS;
	byte param[3] = {SSI_BEEP_NONE, SSI_DEC_FORMAT, SSI_DEC_PACKED};
	byte sendBuff[MAX_PKG_LEN];
	byte recvBuff[MAX_PKG_LEN];

	printf("Configure SSI package format...");
	prepare_pkg( sendBuff, SSI_PARAM_SEND, param, ( sizeof(param) / sizeof(*param) ) );
	write(scanner, sendBuff, PKG_LEN(sendBuff) + 2);
	printf("OK\n");

	printf("Receive ACK...");
	ret = ssi_read(scanner, recvBuff);
	if ( (ret <= 0) || (SSI_CMD_ACK != recvBuff[INDEX_OPCODE]) )
	{
		printf("ERROR\n");
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

EXIT:
	return ret;
}

//  Calculate the 2's Complement checksum of the data packet
int CalculateChecksum(byte *pkg) {
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

int prepare_pkg(byte *pkg, byte opcode, const byte *param, unsigned int param_len) {
	int error = EXIT_SUCCESS;
	int checksum = 0;

	pkg[INDEX_LEN] = SSI_DEFAULT_LEN + param_len;
	pkg[INDEX_OPCODE] = opcode;
	pkg[INDEX_SRC] = SSI_HOST;
	pkg[INDEX_STAT] = SSI_DEFAULT_STATUS;

	if (NULL != param)
	{
		memcpy(&pkg[INDEX_STAT + 1], param, param_len);
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

void display_pkg(byte *pkg) {
	for (int i = 0; i < (PKG_LEN(pkg) + 2); i++) {
		printf("0x%x ", pkg[i]);
	}
	printf("\n");
}

int ssi_write(int fd, byte opcode, const byte *param, unsigned int param_len)
{
	int ret = EXIT_SUCCESS;
	byte *sendBuff = (byte *)malloc( (SSI_DEFAULT_LEN + param_len) * sizeof(*sendBuff));
	byte *recvBuff = (byte *)malloc(MAX_PKG_LEN);
	if ( (NULL == sendBuff) || (NULL == recvBuff) )
	{
		printf("ERROR: %s: can not allocate buffer\n", __func__);
	}

	prepare_pkg(sendBuff, opcode, param, param_len);
	write(scanner, sendBuff, sendBuff[INDEX_LEN]);
	// Check ACK
	ret = ssi_read(scanner, recvBuff);
	if ( (ret <= 0) || (SSI_CMD_ACK != recvBuff[INDEX_OPCODE]) )
	{
		printf("ERROR\n");
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

EXIT:
	return ret;
}

int ssi_read(int fd, byte *buff) {
	int timeoutValue = 0;
	int ret = 0;
	struct itimerval timeoutTimer;

	scanner = fd;
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

void HandleSignal(int sig)
{
	if (SIGALRM == sig)
	{
		printf("TIMEOUT\t");
		is_timeout = TRUE;
		signal(SIGIO, SIG_IGN);
		signal(SIGALRM, SIG_IGN);
	}
	else if (SIGIO == sig)
	{
		int pkgLen = 0;

		read(scanner, &pkgLen, 1);
		if (pkgLen > 0)
		{
			pkg[0] = pkgLen;
			byteRead++;
			byteRead += read(scanner, &pkg[1], pkgLen+1);
			is_read = TRUE;
		}
	}
}
