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

	ret = OpenTTY();
	if (ret <= 0)
	{
		ret = EXIT_FAILURE;
		goto EXIT;
	}
	else
	{
		scanner = ret;
	}

	ret = (char) ConfigTTY(scanner);
	if (ret)
	{
		printf("%s: ERROR\n", __func__);
		goto EXIT;
	}

	ret = (char) ConfigSSI(scanner);
	if (ret)
	{
		printf("%s: ERROR\n", __func__);
		goto EXIT;
	}

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
	byte recvBuff[MAX_PKG_LEN] = {0};

	printf("Wipe out input buffer...");
	tcflush(scanner, TCIFLUSH);
	printf("OK\n");

	printf("Send Start session cmd...");
	ret = WriteSSI(scanner, SSI_START_SESSION, NULL, 0);
	if (ret)
	{
		printf("ERROR\n");
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

	// Receive barcode in formatted package
	printf("Received data!\n");
	ret = ReadSSI(scanner, recvBuff);
	if (ret <= 0)
	{
		printf("ERROR\n");
		buff = NULL;
		goto EXIT;
	}
	else
	{
		printf("Send ACK to scanner!\n");
		WriteSSI(scanner, SSI_CMD_ACK, NULL, 0);

		// Extract barcode to buffer
		barcodeLen = recvBuff[INDEX_LEN] - INDEX_BARCODETYPE - 1;
		memcpy(buff, &recvBuff[INDEX_BARCODETYPE + 1], barcodeLen);
		DisplayPkg(recvBuff);
	}

EXIT:
	printf("Send Stop session cmd...");
	ret = WriteSSI(scanner, SSI_STOP_SESSION, NULL, 0);
	if (ret)
	{
		printf("ERROR\n");
		goto EXIT;
	}
	else
	{
		printf("OK\n");
	}

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
