/*******************************************************************************
 *  (C) Copyright 2009 STYL Solutions Co., Ltd. , All rights reserved       *
 *                                                                             *
 *  This source code and any compilation or derivative thereof is the sole     *
 *  property of STYL Solutions Co., Ltd. and is provided pursuant to a      *
 *  Software License Agreement.  This code is the proprietary information      *
 *  of STYL Solutions Co., Ltd and is confidential in nature.  Its use and  *
 *  dissemination by any party other than STYL Solutions Co., Ltd is        *
 *  strictly limited by the confidential information provisions of the         *
 *  Agreement referenced above.                                                *
 ******************************************************************************/

/**
 * @file    barcode_test.c
 * @brief   Simple test application use C library to get data from qrcode/barcode scanner
 *
 * Long description.
 * @date    13/07/2017
 * @author  luck.hoang alvin.nguyen
 */

/********** Include section ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "mlsBarcode.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/

#define BUFFER_LEN	4000
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

    printf("Version: %s\n", GetVersion());

    ret = mlsBarcodeReader_Open(deviceName);
    if (ret!=EXIT_SUCCESS)
    {
        goto EXIT;
    }

    // This is to test Reopen API only, not required
    mlsBarcodeReader_Reopen(deviceName);

    isRunning = TRUE;
    signal(SIGINT, HandleSignal);
    while (isRunning)
    {
        ret = mlsBarcodeReader_Test();
        if (ret == EXIT_SUCCESS)
            printf("[BARCODE_TEST]: Testing OK !\n");
        else
            printf("[BARCODE_TEST]: Testing FAIL !\n");

        sleep(2);
    }

    mlsBarcodeReader_Close();

EXIT:
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
