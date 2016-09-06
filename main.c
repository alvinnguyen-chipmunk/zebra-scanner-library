//
//  main.c
//  zebra_scanner_C
//
//  Created by Hoàng Trung Huy on 8/22/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "mlsBarcode.h"
#include "ssi.h"
#include "ssi_utils.h"

#define BUFFER_LEN	MAX_PKG_LEN

int main(int argc, const char * argv[])
{
	int ret = EXIT_SUCCESS;
	int barcodeLen = 0;
	char *buff = (char *) malloc(BUFFER_LEN * sizeof(char));

	ret = mlsBarcodeReader_Open();
	if (ret)
	{
		printf("ERROR: open device\n");
		goto EXIT;
	}

	ret = mlsBarcodeReader_ReadData(buff);
	if (ret <= 0)
	{
		printf("Error: read data\n");
		goto EXIT;
	}
	else
	{
		barcodeLen = ret;
		printf("\e[36mBarcode(%d): %s\e[0m\n", barcodeLen, buff);
	}

	ret = mlsBarcodeReader_Close();

EXIT:
	return ret;
}
