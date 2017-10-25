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
 * @file    mlsScannerExample.c
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
/********** Local (static) variable declaration section ***********************/
static int isRunning = FALSE;

/********** Local (static) function declaration section ***********************/
static void HandleSignal(int sig);

/********** Local (static) function definition section ************************/
static void HandleSignal(int sig)
{
    if (SIGINT == sig)
    {
        psignal(sig, "Received");
        isRunning = FALSE;
    }
}

/********** Global function declaration section *******************************/

/********** Global function definition section ********************************/
int main(int argc, const char * argv[])
{
    const char      *deviceName     = argv[1];
    int             chooseNumber    = -1;
    char            chooseString[256];
    const char      deciTimeout     = 5;	// decisecond (1/10 second)
    char            buffer[STYL_STRING_MAXLEN];
    int             decodeLength    = 0;

    do
    {
        STYL_INFO_0("\n\n******************************************************\n");
        STYL_INFO_0("\nSTYL Barcode/QrCode Scanner Demo Application.\nPlease choose:\n");
        STYL_INFO_0("  1. Connect scanner\n");
        STYL_INFO_0("  2. Disconnect scanner\n");
        STYL_INFO_0("  3. Get device port\n");
        STYL_INFO_0("  4. Get library version\n");
        STYL_INFO_0("  5. Get firmware revision (scanner connected)\n");
        STYL_INFO_0("  6. Get current scanning mode (scanner connected)\n");
        STYL_INFO_0("  7. Execute scanning automatic mode (scanner connected)\n");
        STYL_INFO_0("  8. Execute scanning manual mode (scanner connected)\n");
        STYL_INFO_0("  0. Exit\n");
        STYL_INFO_0("\n===> Your choose: ");

        memset(chooseString, 0, 256);
        chooseNumber = scanf("%s", chooseString); printf("\n");
        sscanf(chooseString, "%[0-9]d", chooseString);
        chooseNumber = atoi(chooseString);

        switch(chooseNumber)
        {
            case 1:
            {
                if(mlsBarcodeReader_Open(deviceName)==EXIT_SUCCESS)
                {
                    STYL_INFO_1("**** Scanner connect successful.");
                }
                else
                {
                    STYL_ERROR("**** Scanner connect unsuccessful.");
                }
            }
                break;

            case 2:
            {
                if(mlsBarcodeReader_Close()==EXIT_SUCCESS)
                {
                    STYL_INFO_1("**** Scanner disconnect successful.");
                }
                else
                {
                    STYL_ERROR("**** Scanner disconnect unsuccessful.");
                }
            }
                break;

            case 3:
            {
                STYL_INFO_1("**** Scanner serial device port name: %s.", mlsBarcodeReader_GetDevice());
            }
                break;

            case 4:
            {
                STYL_INFO_1("**** Scanner SSI library version: %s.", GetVersion());
            }
                break;

            case 5:
            {
                memset(buffer, 0, STYL_STRING_MAXLEN);
                decodeLength = mlsBarcodeReader_GetRevision(buffer, STYL_STRING_MAXLEN, deciTimeout);
                if (decodeLength != 0)
                {
                    STYL_INFO_1("**** Decoder firmware revision number: %s.", buffer);
                }
                else
                {
                    STYL_ERROR("**** Get Decoder firmware revision failure: %s.", buffer);
                }
            }
                break;

            case 6:
            {
                switch(mlsBarcodeReader_GetMode())
                {
                    case 1:
                        STYL_INFO_1("**** Scanner mode is: auto-trigger scanning.");
                        break;
                    case 2:
                        STYL_INFO_1("**** Scanner mode is: manual-trigger scanning.");
                        break;
                    default:
                        STYL_ERROR("**** Get scanner current mode scanning failure.");
                        break;
                }
            }
                break;

            case 7:
            {
                isRunning = TRUE;
                signal(SIGINT, HandleSignal);

                while (isRunning==TRUE)
                {
                    memset(buffer, 0, STYL_STRING_MAXLEN);
                    decodeLength = mlsBarcodeReader_ReadData(buffer, STYL_STRING_MAXLEN, deciTimeout);
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
            }
                break;

            case 8:
            {
                isRunning = TRUE;
                signal(SIGINT, HandleSignal);

                while (isRunning==TRUE)
                {
                    memset(buffer, 0, STYL_STRING_MAXLEN);
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
            }
                break;

            case 0:
            default:
                break;
        };
    }
    while(chooseNumber!=0);

    return 0;
}

/*@}*/
