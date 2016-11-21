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
	int ret = EXIT_SUCCESS;
	int barcodeLen = 0;
	const int timeout = 10;	// 1/10 sec
	char buff[BUFFER_LEN];
	memset(buff, 0, BUFFER_LEN);

	printf("DEBUG: %s\n", argv[1]);
	ret = mlsBarcodeReader_Open(argv[1]);
	if (ret)
	{
		goto EXIT;
	}

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
