/*******************************************************************************
 *  (C) Copyright 2009 STYL Solutions Co., Ltd. , All rights reserved          *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of STYL Solutions Co., Ltd. and is provided pursuant to a         *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of STYL Solutions Co., Ltd and is confidential in nature.  Its use and     *
 *  dissemination by any party other than STYL Solutions Co., Ltd is           *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @file    mlsScanner_Demo_2.c
 * @brief   Simple test application use C library to get data from qrcode/barcode scanner
 *
 * Long description.
 * @date    28/09/2017
 * @author  alvin.nguyen
 */

/********** Include section ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "mlsBarcode.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/

#define BUFFER_LEN	4096
#define TRUE		1
#define FALSE		0

/********** Local (static) variable declaration section ***********************/

static int isRunning = FALSE;

static void mlsHandleSignal(int sig);

/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/
/********** Global function declaration section *******************************/
/********** Global function definition section ********************************/

int main(int argc, const char * argv[])
{
    char        buffer[BUFFER_LEN];
    const char *deviceName              = argv[1];
    int         retValue                = EXIT_FAILURE;
    int         decodeLength            = 0;
    const int   timeout                 = 5;	/* 1/10 second */

    printf("Version: \x1b[35m%s\e[0m\n", mlsBarcodeReader_GetVersion());

    printf("Device: \x1b[35m%s\e[0m\n", mlsBarcodeReader_GetDevice());

    if(mlsBarcodeReader_Open(deviceName) != EXIT_SUCCESS)
        goto __exit;

    if(mlsBarcodeReader_ManualMode() != EXIT_SUCCESS)
        goto __exit;

    retValue = EXIT_SUCCESS;

    isRunning = TRUE;
    signal(SIGINT, mlsHandleSignal);

    while (isRunning)
    {
        printf("\n=============================================\n");

        memset(buffer, 0, BUFFER_LEN);
        decodeLength = mlsBarcodeReader_ReadData_Manual(buffer, BUFFER_LEN, timeout);
        if (decodeLength > 0)
        {
            printf("Barcode(%d):\e[36m\n%s\e[0m\n\n", decodeLength, buffer);
        }
        sleep(2);
    }

    mlsBarcodeReader_Close();

__exit:
    printf("Finished!\n");
    return retValue;
}

static void mlsHandleSignal(int sig)
{
    if (SIGINT == sig)
    {
        psignal(sig, "Received");
        isRunning = FALSE;
    }
}

/**@}*/
