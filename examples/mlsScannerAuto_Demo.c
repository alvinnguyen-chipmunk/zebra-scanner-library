/*******************************************************************************
(C) Copyright 2009 Styl Solutions Co., Ltd. , All rights reserved              *
                                                                               *
This source code and any compilation or derivative thereof is the sole         *
property of Styl Solutions Co., Ltd. and is provided pursuant to a             *
Software License Agreement. This code is the proprietary information           *
of Styl Solutions Co., Ltd and is confidential in nature. Its use and          *
dissemination by any party other than Styl Solutions Co., Ltd is               *
strictly limited by the confidential information provisions of the             *
Agreement referenced above.                                                    *
*******************************************************************************/

/**
 * @file    mlsScanner_Demo_1.c
 * @brief   Simple example use C library to get data from qrcode/barcode scanner
 *
 * Long description.
 * @date    28/09/2017
 * @author  alvin.nguyen
 */

/********** Include section ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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

static void HandleSignal(int sig);

/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/
/********** Global function declaration section *******************************/
/********** Global function definition section ********************************/

int main(int argc, const char * argv[])
{
    char buff[BUFFER_LEN];
    const char *deviceName = argv[1];
    int ret = EXIT_SUCCESS;
    int barcodeLen = 0;
    const int timeout = 10;	// 1/10 sec
    memset(buff, 0, BUFFER_LEN);

    printf("Version: %s\n", mlsBarcodeReader_GetVersion());

    printf("Device: %s\n", mlsBarcodeReader_GetDevice());

    ret = mlsBarcodeReader_Open(deviceName);
    if (ret!=EXIT_SUCCESS)
    {
        goto __exit;
    }

    /* ************This is to test Reopen API only, not required *************** */
    mlsBarcodeReader_Reopen(deviceName);

    isRunning = TRUE;
    signal(SIGINT, HandleSignal);

    while (isRunning)
    {
        ret = mlsBarcodeReader_ReadData(buff, BUFFER_LEN, timeout);
        if (ret > 0)
        {
            barcodeLen = ret;
            printf("\e[36mBarcode(%d):\n%s\e[0m\n\n", barcodeLen, buff);
            memset(buff, 0, BUFFER_LEN);
        }
    }

    mlsBarcodeReader_Close();

__exit:
    printf("Finished!\n");
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

/**@}*/
