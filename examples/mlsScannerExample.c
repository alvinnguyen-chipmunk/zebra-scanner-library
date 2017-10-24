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
    char            retValue        = EXIT_FAILURE;

    do
    {
        printf("\n\n******************************************************\n%s", ANSI_COLOR_BLUE);
        printf("\nSTYL Barcode/QrCode Scanner Demo. Please choose:\n");
        printf("  1. Connect scanner\n");
        printf("  2. Disconnect scanner\n");
        printf("  3. Get device port\n");
        printf("  4. Get library version\n");
        printf("  5. Get firmware revision (scanner connected)\n");
        printf("  6. Get current scanning mode (scanner connected)\n");
        printf("  7. Execute scanning automatic mode\n");
        printf("  8. Execute scanning manual mode\n");
        printf("  0. Exit (also disconnect scanner)\n");
        printf("\n%sYour choose: ", ANSI_COLOR_RESET);

        int tmp = scanf("%s", chooseString); printf("\n");
        sscanf(chooseString, "%[0-9]d", chooseString);
        chooseNumber = atoi(chooseString);

        switch(chooseNumber)
        {
            case 1:
            {
                if(mlsBarcodeReader_Open(deviceName)==EXIT_SUCCESS)
                    printf("%s\nScanner device was opened.%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
                else
                    printf("%s\nScanner device cannot open.%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
            }
                break;

            case 2:
            {
                if(mlsBarcodeReader_Close()==EXIT_SUCCESS)
                    printf("%s\nScanner device was closed.%s", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
                else
                    printf("%s\nScanner device cannot close.%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
            }
                break;

            case 3:
            {
                printf("Version:%s %s%s\n", ANSI_COLOR_GREEN, mlsBarcodeReader_GetDevice(), ANSI_COLOR_RESET);
            }
                break;

            case 4:
            {
                printf("Version:%s %s%s\n", ANSI_COLOR_GREEN, GetVersion(), ANSI_COLOR_RESET);
            }
                break;

            case 5:
            {
                memset(buffer, 0, STYL_STRING_MAXLEN);
                decodeLength = mlsBarcodeReader_GetRevision(buffer, STYL_STRING_MAXLEN, deciTimeout);
                if (decodeLength != 0)
                {
                    printf("\nFirmware revision number: %s%s%s\n", ANSI_COLOR_GREEN, buffer, ANSI_COLOR_RESET);
                }
                else
                {
                    printf("%s\nGet firmware revision number fail.%s", ANSI_COLOR_RED, ANSI_COLOR_RESET);
                }
            }
                break;

            case 6:
            {
                unsigned int modeNumber = mlsBarcodeReader_GetMode();
                if(modeNumber == 1)
                {
                    printf("\nScanner mode is: %sauto-trigger%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
                }
                else if (modeNumber == 2)
                {
                    printf("\nScanner mode is: %smanual-trigger%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
                }
                else
                {
                    printf("\nGet mode scanner fail.%s\n", ANSI_COLOR_RED, ANSI_COLOR_RESET);
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
                        printf("\nBarcode(%d):%s\n%s%s\n", decodeLength, ANSI_COLOR_GREEN, buffer, ANSI_COLOR_RESET);
                    }
                    else
                    {
                        printf("\nNo data\n");
                    }
                    sleep(1);
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
                        printf("\nBarcode(%d):%s\n%s%s\n", decodeLength, ANSI_COLOR_GREEN, buffer, ANSI_COLOR_RESET);
                    }
                    else
                    {
                        printf("\nNo data\n");
                    }
                    sleep(1);
                }
            }
                break;

            case 0:
                break;

            default:
                break;
        };
    }
    while(chooseNumber!=0);

    return 0;
}

/*@}*/
