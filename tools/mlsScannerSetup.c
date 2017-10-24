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
 * @file    mlsScannerSetup.c
 * @brief   C program help to setup parameter for SSI protocol that will use to communicate with decoder
 *
 * Long description.
 * @date    13/10/2017
 * @author  alvin.nguyen
 */

/********** Include section ***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <error.h>
#include <errno.h>
#include <glib.h>

#include "mlsBarcode.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define STYL_INFO(format, ...)  \
    mlsScannerSetup_Print(0, "%s[STYL INFO]: %s():%d " format "%s\n", ANSI_COLOR_BLUE, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_WARNING(format, ...)  \
    mlsScannerSetup_Print(0, "%s[STYL WARNING]: %s():%d " format "%s\n", ANSI_COLOR_YELLOW, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_SHOW(format, ...)  \
    mlsScannerSetup_Print(1, "%s[STYL INFO]: %s():%d " format "%s\n", ANSI_COLOR_GREEN, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_ERROR(format, ...) \
    mlsScannerSetup_Print(1, "%s[STYL ERROR]: %s():%d " format "%s\n", ANSI_COLOR_RED, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define HELP_STRING                                                                         \
        "============================================================================="     \
        "\n\tSTYL program - configure scanning mode for SSI Scanner device.\n"              \
        "\n\n\tUsage:\n\t     StylScannerSetup"                                             \
        "\n\n\tUsage:\n\t     StylScannerSetup manual|auto"                                 \
        "\n\n\tUsage:\n\t     StylScannerSetup /dev/ttyxxx manual|auto"                     \
        "\n\n\tUsage:\n\t     StylScannerSetup -h|--help"                                   \
        "\n============================================================================="

/********** Local (static) variable declaration section ***********************/
static int isRunning = FALSE;

/********** Local (static) function declaration section ***********************/
static void     mlsScannerSetup_Help        ();
static void     mlsScannerSetup_Print       (int isForce, char *format, ...);
static void     HandleSignal                (int sig);

/********** Local (static) function definition section ************************/
static void HandleSignal(int sig)
{
    if (SIGINT == sig)
    {
        psignal(sig, "Received");
        isRunning = FALSE;
    }
}

static void mlsScannerSetup_Help ()
{
    printf("%s\n%s\n%s\n", ANSI_COLOR_YELLOW, HELP_STRING, ANSI_COLOR_RESET);
}

static void mlsScannerSetup_Print(int isForce, char *format, ...)
{
    if (isForce == 0)
        if(getenv("STYL_DEBUG")==NULL)
            return;

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}



/********** Global function declaration section *******************************/
/********** Global function definition section ********************************/

int main(int argc, const char * argv[])
{
    gchar *scannerPort = NULL;
    gint   scannerMode = STYL_SCANNER_NONEMODE;

    switch(argc)
    {
    case 1:
        scannerPort = g_strdup(mlsBarcodeReader_GetDevice());
        scannerMode = STYL_SCANNER_AUTOMODE;
        break;
    case 2:
        if (g_strcmp0("-h", argv[1])==0 || g_strcmp0("--help", argv[1])==0)
        {
            mlsScannerSetup_Help();
            return 0;
        }
        scannerPort = g_strdup(mlsBarcodeReader_GetDevice());
        if (g_strcmp0("auto", argv[1])==0)
            scannerMode = STYL_SCANNER_AUTOMODE;
        else if (g_strcmp0("manual", argv[1])==0)
            scannerMode = STYL_SCANNER_MANUALMODE;
        break;
    case 3:
        scannerPort = g_strdup(argv[1]);
        if (g_strcmp0("auto", argv[2])==0)
            scannerMode = STYL_SCANNER_AUTOMODE;
        else if (g_strcmp0("manual", argv[2])==0)
            scannerMode = STYL_SCANNER_MANUALMODE;
        break;
    default:
        mlsScannerSetup_Help();
        return 1;
    }

    STYL_INFO("SSI Scanner Port %s", scannerPort);
    STYL_INFO("SSI Scanner Mode %d", scannerMode);

    if(scannerMode==STYL_SCANNER_NONEMODE || scannerPort==NULL)
    {
        mlsScannerSetup_Help();
        goto __exit__;
    }

    isRunning = TRUE;
    signal(SIGINT, HandleSignal);

//    gint countLoop = 3;
//    do
//    {
//        countLoop--;
        if(mlsBarcodeReader_Setup(scannerPort, scannerMode)==EXIT_FAILURE)
        {
            STYL_ERROR("******************* Unsuccessful ******************* \n");

        }
        else
        {
            STYL_SHOW("******************* Successful ********************* \n");
        }
//        else
//        {
//            break;
//        }
//    }
//    while(countLoop>0 && isRunning==TRUE);
__exit__:
    g_free(scannerPort);
    return 0;
}

/**@}*/
