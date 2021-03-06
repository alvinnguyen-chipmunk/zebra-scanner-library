//
//  main.c
//  zebra_scanner_C
//
//  Created by Hoàng Trung Huy on 8/22/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "mlsBarcode.h"

#define BUFFER_LEN	4000
#define TRUE		1
#define FALSE		0

static int isRunning = FALSE;

static void HandleSignal(int sig);

int main(int argc, const char * argv[])
{
	char buff[BUFFER_LEN];
	char *deviceName = argv[1];
	int ret = EXIT_SUCCESS;
	int barcodeLen = 0;
	const int timeout = 10;	// 1/10 sec
	memset(buff, 0, BUFFER_LEN);

	printf("Version: %s\n", GetVersion());

	ret = mlsBarcodeReader_Open(deviceName);
	if (ret)
	{
		goto EXIT;
	}

	// This is to test Reopen API only, not required
	mlsBarcodeReader_Reopen(deviceName);

	isRunning = TRUE;
	signal(SIGINT, HandleSignal);
	while (isRunning)
	{
		ret = mlsBarcodeReader_ReadData(buff, BUFFER_LEN, timeout);
		if (ret > 0)
		{
			barcodeLen = ret;
			printf("\e[36mBarcode(%d):\n%s\e[0m\n", barcodeLen, buff);
			memset(buff, 0, BUFFER_LEN);
		}
	}

	printf("Finished!\n");
	mlsBarcodeReader_Close();

EXIT:
	return ret;
}

static void HandleSignal(int sig)
{
	if (SIGINT == sig)
	{
		psignal(sig, "Received");
		isRunning = FALSE;
	}
}
