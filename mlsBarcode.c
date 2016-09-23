//
//  mlsBarcode.c
//  zebra_scanner_C
//
//  Created by Hoàng Trung Huy on 8/22/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#ifndef MLSQRREADER_H
#define MLSQRREADER_H
//#ifdef __cplusplus
//extern "C"
//{
//#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <assert.h>

#include "ssi.h"
#include "ssi_utils.h"
#include "mlsBarcode.h"

typedef enum _state {START, STOP, FLUSH_QUEUE, REPLY_ACK, REPLY_NAK, GET_BARCODE, WAIT_DEC_EVENT} ssiState;
typedef enum _bool {FALSE, TRUE} bool;

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
unsigned int mlsBarcodeReader_ReadData(char *buff, const int buffLength, const int timeout) {
	int barcodeLen = 0;
	int ret = 0;
	ssiState currentState = WAIT_DEC_EVENT;
	ssiState nextState = WAIT_DEC_EVENT;
	ssiState previousState = WAIT_DEC_EVENT;
	bool isInSession = TRUE;
	byte recvBuff[4000] = {0};
	assert( (timeout >= 0) && (timeout <= 25) );

	while (isInSession)
	{
		switch (currentState) {
			case START:
				printf("Send Start session cmd...");
				ret = WriteSSI(scanner, SSI_START_SESSION, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
				}
				else
				{
					printf("OK\n");
				}

				break;

			case STOP:
				isInSession = FALSE;
//				printf("Send Stop session cmd...");
//				ret = WriteSSI(scanner, SSI_STOP_SESSION, NULL, 0);
//				usleep(1000);
//				if ( (ret) || (! CheckACK(scanner) ) )
//				{
//					PrintError(ret);
//				}
//				else
//				{
				/* TODO: need to research if auto detect accept STOP_SESSION */
					ret = barcodeLen;
					printf("OK\n");
//				}
				break;

			case WAIT_DEC_EVENT:
				printf("Wait for decode event...");
				ret = ReadSSI(scanner, recvBuff, timeout);
				if (ret <= 0)
				{
					PrintError(ret);
					nextState = STOP;
				}
				else
				{
					printf("OK\n");
					nextState = REPLY_ACK;
					previousState = currentState;
				}
				break;

			case REPLY_ACK:
				ret = WriteSSI(scanner, SSI_CMD_ACK, NULL, 0);
				if (ret)
				{
					PrintError(ret);
					nextState = STOP;
				}
				else
				{
					printf("OK\n");
					switch (previousState)
					{
						case WAIT_DEC_EVENT:
							nextState = GET_BARCODE;
							break;
						case GET_BARCODE:
							nextState = STOP;
						default:
							break;
					}
				}
				previousState = currentState;
				break;

			case REPLY_NAK:
				break;

			case GET_BARCODE:
				// Receive barcode in formatted package
				printf("Receive data: \n");
				ret = ReadSSI(scanner, recvBuff, timeout);
				if (ret <= 0)
				{
					buff = NULL;
					nextState = STOP;
				}
				else
				{
					// Extract barcode to buffer
					assert(NULL != recvBuff);
					barcodeLen = ExtractBarcode(buff, recvBuff, buffLength);
					DisplayPkg(recvBuff);
					nextState = REPLY_ACK;
					previousState = currentState;
				}
				break;

			case FLUSH_QUEUE:
				printf("Send Scan disable cmd...");
				ret = WriteSSI(scanner, SSI_SCAN_DISABLE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					printf("OK\n");
				}

				printf("Send flush queue cmd...");
				ret = WriteSSI(scanner, SSI_FLUSH_QUEUE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					printf("OK\n");
				}

				printf("Send Scan enable cmd...");
				ret = WriteSSI(scanner, SSI_SCAN_ENABLE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					printf("OK\n");
				}

				break;

			default:
				break;
		}
		currentState = nextState;
	}

	printf("Barcode Len = %d\n", ret);
	return ret;
}

/*!
 * \brief mlsBarcodeReader_Enable Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_SUCCESS: Fail
 */
char mlsBarcodeReader_Enable()
{
	char ret = EXIT_SUCCESS;

	printf("Enable scanner...");
	ret = WriteSSI(scanner, SSI_SCAN_ENABLE, NULL, 0);
	if ( (ret) || (! CheckACK(scanner) ) )
	{
		PrintError(ret);
		ret = EXIT_FAILURE;
	}
	else
	{
		printf("OK\n");
	}

	return ret;
}

/*!
 * \brief mlsBarcodeReader_Disable Disable reader, Reader can't scan any QR code/bar code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_SUCCESS: Fail
 */
char mlsBarcodeReader_Disable()
{
	char ret = EXIT_SUCCESS;

	printf("Disable scanner...");
	ret = WriteSSI(scanner, SSI_SCAN_DISABLE, NULL, 0);
	if ( (ret) || (! CheckACK(scanner) ) )
	{
		PrintError(ret);
		ret = EXIT_FAILURE;
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

//#ifdef __cplusplus
//}
//#endif
#endif // MLSQRREADER_H
