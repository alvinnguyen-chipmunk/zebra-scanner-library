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

#define MSB_16(x)		(x >> 8)
#define LSB_16(x)		(x & UINT8_MAX)

#define TRUE				1
#define FALSE				0


#define TIMEOUT_MSEC		50

static uint16_t CalculateChecksum(byte *pkg);
static void PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen);
static int IsChecksumOK(byte *pkg);

/*!
 * \brief CalculateChecksum calculate 2's complement of pkg
 * \return 16 bits checksum (2' complement) value
 */
static uint16_t CalculateChecksum(byte *pkg)
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

/*!
 * \brief PreparePkg generate package from input opcode and params
 */
static void PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen)
{
	uint16_t checksum = 0;

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
}

/*!
 * \brief IsChecksumOK check 2 last bytes for checksum
 * \return
 */
static int IsChecksumOK(byte *pkg)
{
	int ret = 0;
	uint16_t cksum = 0;

	cksum += pkg[PKG_LEN(pkg) + 1];
	cksum += pkg[PKG_LEN(pkg)] << 8;

	if (cksum == CalculateChecksum(pkg))
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}

	return ret;
}

/*!
 * \brief DisplayPkg print out package
 */
void DisplayPkg(byte *pkg)
{
	static char *debug = NULL;
	debug = getenv("SSI_DEBUG");
	if ( (NULL != debug) && (NULL != pkg) )
	{
		for (int i = 0; i < (PKG_LEN(pkg) + 2); i++)
		{
			printf("0x%x ", pkg[i]);
		}
		printf("\n");
	}
}

/*!
 * \brief ConfigSSI send parameters to configure scanner
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int ConfigSSI(int fd)
{
	int ret = EXIT_SUCCESS;
	byte param[9] =	{
		PARAM_BEEP_NONE,
		PARAM_B_DEC_FORMAT	, ENABLE,
		PARAM_B_SW_ACK		, ENABLE,
		PARAM_B_SCAN_PARAM	, DISABLE,	// Disable to avoid accidental changes param from scanning
		PARAM_TRIGGER_MODE	, PARAM_TRIGGER_HOST,
		};

	printf("Configure SSI parameters...");
	ret = WriteSSI(fd, SSI_PARAM_SEND, param, ( sizeof(param) / sizeof(*param) ) );

	if (ret)
	{
		printf("ERROR: %s\n", __func__);
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

EXIT:
	return ret;
}

/*!
 * \brief WriteSSI write formatted package and check ACK to/from scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int WriteSSI(int fd, byte opcode, byte *param, byte paramLen)
{
	int ret = EXIT_SUCCESS;
	byte *sendBuff = malloc( (SSI_DEFAULT_LEN + paramLen) * sizeof(byte) );
	byte recvBuff[MAX_PKG_LEN];

	PreparePkg(sendBuff, opcode, param, paramLen);
	if ( write(fd, sendBuff, sendBuff[INDEX_LEN] + 2) <= 0)
	{
		perror("write");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	// Check ACK
	if (SSI_CMD_ACK != opcode)
	{
		if (ReadSSI(fd, recvBuff) <= 0)
		{
			printf("%s: no ACK\n", __func__);
			ret = EXIT_FAILURE;
		}
		else if (SSI_CMD_ACK != recvBuff[INDEX_OPCODE])
		{
			printf("%s: ACK failure\n", __func__);
			ret = EXIT_FAILURE;
		}
	}

EXIT:
	free(sendBuff);
	return ret;
}

/*!
 * \brief ReadSSI read formatted package and response ACK from/to scanner via file descriptor
 * \return number of read bytes
 */
int ReadSSI(int fd, byte *buff)
{
	int ret = 0;

	// Get first byte which indicate package lenght
	ret = (int) read(fd, buff, MAX_PKG_LEN);
	if ( (ret > 0) || (IsChecksumOK(buff)) )
	{
		DisplayPkg(buff);
		WriteSSI(fd, SSI_CMD_ACK, NULL, 0);
	}
	else
	{
		printf("%s: ERROR", __func__);
		goto EXIT;
	}

EXIT:
	return ret;
}

/*!
 * \brief WriteSSI write formatted package to scanner via file descriptor
 * \return file descriptor
 */
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

/*!
 * \brief WriteSSI write formatted package to scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
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
	devConf.c_cc[VTIME] = TIMEOUT_MSEC;	// ms timeout

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
