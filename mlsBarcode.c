//
//  mlsBarcode.c
//  zebra_scanner_C
//
//  Created by Hoàng Trung Huy on 8/22/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#ifndef MLSQRREADER_H
#define MLSQRREADER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <locale.h>

#include "ssi.h"
#include "ssi_utils.h"
#include "mlsBarcode.h"

static int scanner = 0;

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open() {
	char ret = EXIT_SUCCESS;
	struct termios dev_conf;
	int flags = 0;
	char *dev_name = getenv("ZEBRA_SCANNER");

	scanner = open(dev_name, O_RDWR | O_NOCTTY);
	if (scanner < 0) {
		perror(__func__);
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	// Set flags for blocking mode and sync for writing
	flags = fcntl(scanner, F_GETFL);
	if (0 > flags) {
		perror("F_GETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}
	flags |= (O_NDELAY | O_ASYNC);

	ret = fcntl(scanner, F_SETFL, flags);
	if (ret) {
		perror("F_SETFL");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	// Configure tty dev
	dev_conf.c_cflag = (CRTSCTS | CS8 | HUPCL);
	dev_conf.c_iflag = 0;
	dev_conf.c_oflag = 0;
	dev_conf.c_lflag = 0;
	dev_conf.c_lflag = (ISIG);

	ret = cfsetspeed(&dev_conf, BAUDRATE);
	if (ret) {
		perror("Set speed");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

	ret = tcsetattr(scanner, TCSANOW, &dev_conf);
	if (ret) {
		perror("Set attribute");
		ret = EXIT_FAILURE;
		goto EXIT;
	}

//	error = fcntl(scanner, F_SETOWN, getpid());
//	if (error) {
//		perror("F_SETOWN");
//		error = EXIT_FAILURE;
//		goto EXIT;
//	}

EXIT:
	return ret;
}

/*!
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buff) {
	int barcodeLen = 0;
	int ret = 0;

	byte sendBuff[MAX_PKG_LEN];
	byte recvBuff[MAX_PKG_LEN] = {0};

	printf("Wipe out input buffer...");
	tcflush(scanner, TCIFLUSH);
	printf("OK\n");

	printf("Send Start session cmd...");
	ssi_write(scanner, SSI_START_SESSION, NULL, 0);

	// Receive barcode in formatted package
	printf("Received data: ");
	ret = ssi_read(scanner, recvBuff);
	if (ret <= 0)
	{
		printf("ERROR\n");
		buff = NULL;
		goto EXIT;
	}
	else
	{
		display_pkg(recvBuff);
	}

	printf("Send ACK to scanner...");
	prepare_pkg(sendBuff, SSI_CMD_ACK, NULL, 0);
	write(scanner, sendBuff, SSI_DEFAULT_LEN + 2);
	printf("OK\n");

	// Extract barcode to buffer
	barcodeLen = recvBuff[INDEX_LEN] - INDEX_BARCODETYPE - 1;
	memcpy(buff, &recvBuff[INDEX_BARCODETYPE + 1], barcodeLen);

EXIT:
	printf("Send Stop session cmd...");
	prepare_pkg(sendBuff, SSI_STOP_SESSION, NULL, 0);
	write(scanner, sendBuff, SSI_DEFAULT_LEN + 2);
	printf("OK\n");

	return ret;
}

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Close() {
	char error = EXIT_SUCCESS;

	error = close(scanner);
	if (error) {
		perror(__func__);
	}
	
	return error;
}

#ifdef __cplusplus
}
#endif
#endif // MLSQRREADER_H
