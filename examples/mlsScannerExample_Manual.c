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
 * @file    mlsScannerExample_Manual.c
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
#include <unistd.h>

#include "mlsBarcode.h"
#include "mlsScannerExample_Common.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
#define HELP_STRING                                                                          \
        "============================================================================="      \
        "\n\tSTYL SS Scanner example application - Auto trigger mode demo."                  \
        "\n\n\tUsage:\n\t     StylScannerExample_Maual"                                      \
        "\n\n\tUsage:\n\t     StylScannerExample_Maual /dev/ttyxxx"                          \
        "\n\n\tUsage:\n\t     StylScannerExample_Maual reconfig"                             \
        "\n\n\tUsage:\n\t     StylScannerExample_Maual /dev/ttyxxx reconfig"                 \
        "\n\n\tUsage:\n\t     StylScannerExample_Maual -h|--help"                            \
        "\n============================================================================="

/********** Local (static) variable declaration section ***********************/
static int isRunning = FALSE;

/********** Local (static) function declaration section ***********************/
static void HandleSignal(int sig);
static void mlsScannerExample_Help();
/********** Local (static) function definition section ************************/
static void HandleSignal(int sig)
{
    if (SIGINT == sig)
    {
        psignal(sig, "Received");
        isRunning = FALSE;
    }
}

static void mlsScannerExample_Help ()
{
    printf("%s\n%s\n%s\n", ANSI_COLOR_YELLOW, HELP_STRING, ANSI_COLOR_RESET);
}

/********** Global function declaration section *******************************/
/********** Global function definition section ********************************/

int main(int argc, const char * argv[])
{
    const char      *deviceName     = argv[1];
    const char       deciTimeout     = 5;	// decisecond (1/10 second)
    char            buffer[STYL_STRING_MAXLEN];
    int             decodeLength    = 0;
    char            retValue        = EXIT_FAILURE;

    char            doConfig        = FALSE;
    char            doScanPort      = FALSE;

    char            isOpened        = FALSE;

    switch(argc)
    {
    case 1:
        break;
    case 2:
        {
            if (strcmp("-h", argv[1])==0 || strcmp("--help", argv[1])==0)
            {
                mlsScannerExample_Help();
                return 0;
            }
            else if(strcmp("reconfig", argv[1])==0)
            {
                doConfig   = TRUE;
                doScanPort = TRUE;
            }
        }
        break;
    case 3:
        {
            if(strcmp("reconfig", argv[2])==0)
            {
                doConfig = TRUE;
                doScanPort = FALSE;
            }
            else
            {
                STYL_ERROR("Parameter is invalid.");
                printf("%s\n%s\n%s\n", ANSI_COLOR_YELLOW, HELP_STRING, ANSI_COLOR_RESET);
                return 1;
            }
        }
        break;
    default:
        {
            STYL_ERROR("Parameter is invalid.");
            printf("%s\n%s\n%s\n", ANSI_COLOR_YELLOW, HELP_STRING, ANSI_COLOR_RESET);
            return 1;
        }
    }

    STYL_INFO("Option parsed: reconfig: %d (TRUE: 1), scannerPort: %s", doConfig, deviceName);

    if(doConfig==TRUE)
    {
        if (doScanPort)
        {
            if (mlsBarcodeReader_Open_ManualMode(NULL) == EXIT_SUCCESS)
                isOpened = TRUE;
        }
        else
        {
            if (mlsBarcodeReader_Open_ManualMode(deviceName) == EXIT_SUCCESS)
                isOpened = TRUE;
        }
    }
    else
    {
        if (doScanPort)
        {
            if (mlsBarcodeReader_Open(NULL) == EXIT_SUCCESS)
                isOpened = TRUE;
        }
        else
        {
            if (mlsBarcodeReader_Open(deviceName) == EXIT_SUCCESS)
                isOpened = TRUE;
        }
    }

    if(isOpened)
    {
        isRunning = TRUE;
        signal(SIGINT, HandleSignal);

        while (isRunning)
        {
            memset(buffer, 0, STYL_STRING_MAXLEN);
            printf("\n=============================================\n");
            decodeLength = mlsBarcodeReader_ReadData_Manual(buffer, STYL_STRING_MAXLEN, deciTimeout);
            if (decodeLength > 0)
            {
                STYL_INFO_1("**** Decode data (%d):%s\n%s", decodeLength, ANSI_COLOR_YELLOW, buffer);
            }
            else
            {
                STYL_INFO_2("**** No decode data");
            }
            sleep(2);
        }
        mlsBarcodeReader_Close();
    }
    return 0;
}

/*@}*/
