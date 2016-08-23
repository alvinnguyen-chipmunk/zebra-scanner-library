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

#define BUFFER_LENGTH	50

int main(int argc, const char * argv[]) {
	int error = EXIT_SUCCESS;
	char *buff = (char *) malloc(BUFFER_LENGTH * sizeof(char));

	error = mlsBarcodeReader_Open();
	if (error) {
		perror("Open device");
		goto EXIT;
	}

	mlsBarcodeReader_ReadData(buff);
	if (error) {
		perror("Read data");
		goto EXIT;
	}
	printf("\e[36mBarcode: %s\e[0m\n", buff);

	error = mlsBarcodeReader_Close();

EXIT:
	return error;
}