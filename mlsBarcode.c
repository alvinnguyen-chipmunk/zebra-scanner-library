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

#ifndef STYL_SW_VERSION
#define STYL_SW_VERSION     "1.0"
#endif

typedef enum _state {START, STOP, FLUSH_QUEUE, REPLY_ACK, REPLY_NAK, GET_BARCODE, WAIT_DEC_EVENT} ssiState;
typedef enum _bool {FALSE, TRUE} bool;

static int scanner = 0;

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open(char *name) {
	char ret = EXIT_SUCCESS;
	const char *debugLevel = getenv("STYL_DEBUG");

	if (NULL != debugLevel) {
		printf("DEBUG: %s\n", name);
	}

	ret = OpenTTY(name);
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
	const char *debugLevel = getenv("STYL_DEBUG");

	assert( (timeout >= 0) && (timeout <= 25) );

	while (isInSession)
	{
		switch (currentState) {
			case START:
				if (NULL != debugLevel)
				{
					printf("Send Start session cmd...");
				}

				ret = WriteSSI(scanner, SSI_START_SESSION, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					if (NULL != debugLevel)
					{
						PrintError(ret);
					}
					nextState = STOP;
				}
				else
				{
					if (NULL != debugLevel)
					{
						printf("OK\n");
					}
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
				if (NULL != debugLevel)
				{
					printf("OK\n");
				}
//				}
				break;

			case WAIT_DEC_EVENT:

				if (NULL != debugLevel)
				{
					printf("Wait for decode event...");
				}
				ret = ReadSSI(scanner, recvBuff, timeout);
				if (ret <= 0)
				{
					if (NULL != debugLevel)
					{
						PrintError(ret);
					}
					nextState = STOP;
				}
				else
				{
					if (NULL != debugLevel)
					{
						printf("OK\n");
					}
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

					if (NULL != debugLevel)
					{
						printf("OK\n");
					}
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
				if (NULL != debugLevel)
				{
					printf("Receive data: \n");
				}
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
					if (NULL != debugLevel) {
						DisplayPkg(recvBuff);
					}
					nextState = REPLY_ACK;
					previousState = currentState;
				}
				break;

			case FLUSH_QUEUE:
				if (NULL != debugLevel) {
					printf("Send Scan disable cmd...");
				}

				ret = WriteSSI(scanner, SSI_SCAN_DISABLE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					if (NULL != debugLevel)
					{
						printf("OK\n");
					}
				}

				if (NULL != debugLevel) {
					printf("Send flush queue cmd...");
				}

				ret = WriteSSI(scanner, SSI_FLUSH_QUEUE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					if (NULL != debugLevel)
					{
						printf("OK\n");
					}
				}

				if (NULL != debugLevel) {
					printf("Send Scan enable cmd...");
				}

				ret = WriteSSI(scanner, SSI_SCAN_ENABLE, NULL, 0);
				if ( (ret) || (! CheckACK(scanner) ) )
				{
					PrintError(ret);
					nextState = STOP;
					break;
				}
				else
				{
					if (NULL != debugLevel) {
						printf("OK\n");
					}
				}

				break;

			default:
				break;
		}
		currentState = nextState;
	}

	if (NULL != debugLevel)
	{
		printf("Barcode Len = %d\n", ret);
	}
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
	const char *debugLevel = getenv("STYL_DEBUG");
	
	if (NULL != debugLevel) {
		printf("Enable scanner...");
	}

	ret = WriteSSI(scanner, SSI_SCAN_ENABLE, NULL, 0);
	if ( (ret) || (! CheckACK(scanner) ) )
	{
		PrintError(ret);
		ret = EXIT_FAILURE;
	}
	else
	{
		if (NULL != debugLevel) {
			printf("OK\n");
		}
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
	const char *debugLevel = getenv("STYL_DEBUG");

	if (NULL != debugLevel) {
		printf("Disable scanner...");
	}

	ret = WriteSSI(scanner, SSI_SCAN_DISABLE, NULL, 0);
	if ( (ret) || (! CheckACK(scanner) ) )
	{
		PrintError(ret);
		ret = EXIT_FAILURE;
	}
	else
	{
		if (NULL != debugLevel) {
			printf("OK\n");
		}
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

/*!
 * \brief GetVersion provide software version
 * \return string of software version
 * - 
 */
char *GetVersion(void)
{
	return STYL_SW_VERSION;
}

/*!
 * \brief mlsBarcodeReader_Reopen closes then opens device
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Reopen(char *name) {
	char error = EXIT_SUCCESS;

	error = mlsBarcodeReader_Close();
	
	if(!error) {
		error = mlsBarcodeReader_Open(name);
	}

	return error;
}
//#ifdef __cplusplus
//}
//#endif
#endif // MLSQRREADER_H
