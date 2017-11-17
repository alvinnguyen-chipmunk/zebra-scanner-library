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
 * @file    mlsScanner.c
 * @brief   C library communicate Qrcode/Barcode decoder.
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>

#include "mlsBarcode.h"
#include "mlsScannerUtils.h"
#include "mlsScannerConfig.h"
#include "mlsScannerDevice.h"
#include "mlsScannerSSI.h"
#include "mlsScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
static gint gStylScannerFD          = -1;

/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/
/********** Global function definition section ********************************/
/*!
 * \brief GetVersion: provide software version
 * \return string of software version
 */
char * GetVersion(void)
{
    return VERSION;
}

/*!
 * \brief mlsBarcodeReader_GetVersion: provide software version
 * \return string of software version
 */
char * mlsBarcodeReader_GetVersion(void)
{
    return VERSION;
}


/*!
 * \brief mlsBarcodeReader_GetDevice - Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void)
{
    const char * temp = mlsScannerDevice_GetNode(SCANNER_SUBSYSTEM,
                                                 SCANNER_VENDOR_ID,
                                                 SCANNER_PRODUCT_ID);
    return temp;
}


/*!
 * \brief mlsBarcodeReader_Open: Open serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */

char mlsBarcodeReader_Open(const char *name)
{
    gchar    *deviceNode = NULL;
    gboolean scannerReady = FALSE;
    gint     retValue = EXIT_SUCCESS;
    gint     StylScannerFD = -1;

    if(name == NULL)
    {
        /* ***************** Auto-detect for device node string ********************* */
        STYL_WARNING("Device node string is not give. Auto-detect enabled.");
        deviceNode = g_strdup(mlsScannerDevice_GetNode(SCANNER_SUBSYSTEM,
                                               SCANNER_VENDOR_ID,
                                               SCANNER_PRODUCT_ID));
    }
    else
    {
        deviceNode = g_strdup(name);
    }

    if(!deviceNode)
    {
        STYL_ERROR("Device node path is invalid.");
        return EXIT_FAILURE;
    }

    STYL_INFO(" ** Scanner device port: %s **", deviceNode);

    StylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);

    STYL_INFO(" ** Scanner device file descriptor: %d **", StylScannerFD);

    g_free(deviceNode);

    if(StylScannerFD != -1)
    {
        if(mlsScannerConfig_ConfigTTY(StylScannerFD) == EXIT_SUCCESS)
        {
            gStylScannerFD = StylScannerFD;
            return EXIT_SUCCESS;
        }
        else
        {
            mlsScannerConfig_CloseTTY_Only(StylScannerFD);
        }
    }
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_Open_AutoMode: Open serial port then set configure for scanner is auto-trigger mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open_AutoMode(const char *name)
{
    gchar    *deviceNode = NULL;
    gint     StylScannerFD = -1;
    if(name == NULL)
    {
        /* ***************** Auto-detect for device node string ********************* */
        STYL_WARNING("Device node string is not give. Auto-detect enabled.");
        deviceNode = g_strdup(mlsScannerDevice_GetNode(SCANNER_SUBSYSTEM,
                                               SCANNER_VENDOR_ID,
                                               SCANNER_PRODUCT_ID));
    }
    else
    {
        deviceNode = g_strdup(name);
    }

    if(!deviceNode)
    {
        STYL_ERROR("Device node path is invalid.");
        return EXIT_FAILURE;
    }

    STYL_INFO(" ** Scanner device port: %s **", deviceNode);

    StylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);

    STYL_INFO(" ** Scanner device file descriptor: %d **", StylScannerFD);

    g_free(deviceNode);

    if (StylScannerFD != -1)
    {
            /* ***************** Configure TTY port ******************* */
            if(mlsScannerConfig_ConfigTTY(StylScannerFD) == EXIT_SUCCESS)
            {
                if(mlsScannerConfig_ConfigSSI(StylScannerFD, STYL_SCANNER_AUTOMODE, FALSE) == EXIT_SUCCESS)
                {
                    gStylScannerFD = StylScannerFD;
                    STYL_INFO("Setup scanner with SSI parameter of auto-trigger scanning mode success.");
                    return EXIT_SUCCESS;
                }
                else
                {
                    STYL_ERROR("Setup scanner with SSI parameter of auto-trigger scanning mode fail.");
                    mlsScannerConfig_CloseTTY_Only(StylScannerFD);
                }
            }
    }
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_Open_ManualMode: Open serial port then set configure for scanner is manual-trigger mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open_ManualMode(const char *name)
{
    gchar    *deviceNode = NULL;
    gint     StylScannerFD = -1;

    if(name == NULL)
    {
        /* ***************** Auto-detect for device node string ********************* */
        STYL_WARNING("Device node string is not give. Auto-detect enabled.");
        deviceNode = g_strdup(mlsScannerDevice_GetNode(SCANNER_SUBSYSTEM,
                                               SCANNER_VENDOR_ID,
                                               SCANNER_PRODUCT_ID));
    }
    else
    {
        deviceNode = g_strdup(name);
    }

    if(!deviceNode)
    {
        STYL_ERROR("Device node path is invalid.");
        return EXIT_FAILURE;
    }

    STYL_INFO(" ** Scanner device port: %s **", deviceNode);

    StylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);

    STYL_INFO(" ** Scanner device file descriptor: %d **", StylScannerFD);

    g_free(deviceNode);

    if (StylScannerFD != -1)
    {
            /* ***************** Configure TTY port ******************* */
            if(mlsScannerConfig_ConfigTTY(StylScannerFD) == EXIT_SUCCESS)
            {
                if(mlsScannerConfig_ConfigSSI(StylScannerFD, STYL_SCANNER_MANUALMODE, FALSE) == EXIT_SUCCESS)
                {
                    gStylScannerFD = StylScannerFD;
                    STYL_INFO("Setup scanner with SSI parameter of manual-trigger scanning mode.");
                    return EXIT_SUCCESS;
                }
                else
                {
                    STYL_ERROR("Setup scanner with SSI parameter of manual-trigger scanning mode fail.");
                    mlsScannerConfig_CloseTTY_Only(StylScannerFD);
                }
            }
    }
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_GetRevision: Get revision number of decoder.
 * \return
 * - length of revision string: success
 * - 0: failure.
 */
unsigned int mlsBarcodeReader_GetRevision(char *buffer, int bufferLength, char deciTimeout)
{
    gint retValue = 0;

    if(gStylScannerFD==-1)
    {
        STYL_ERROR("Scanner device port is invalid.");
        return EXIT_FAILURE;
    }

    retValue = mlsScannerConfig_CheckRevision(gStylScannerFD, buffer, bufferLength, deciTimeout);
    return retValue;
}

/*!
 * \brief mlsBarcodeReader_close: Close serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Close()
{
    char retValue = EXIT_FAILURE;
    if(gStylScannerFD==-1)
    {
        STYL_ERROR("Scanner device port is invalid.");
        return EXIT_FAILURE;
    }
    #if 0
    return (char) mlsScannerConfig_CloseTTY(gStylScannerFD);
    #else
    retValue = (char)mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
    gStylScannerFD = -1;
    return retValue;
    #endif // 0
}

/*!
 * \brief mlsBarcodeReader_Reopen: Close then re-open serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Reopen(const char *name)
{
    char retValue = EXIT_SUCCESS;
    retValue = mlsBarcodeReader_Close();
    if(EXIT_SUCCESS==retValue)
    {
        retValue = mlsBarcodeReader_Open(name);
    }
    return retValue;
}

/*!
 * \brief mlsBarcodeReader_ReadData: Read decode data from already serial port opened before via auto-trigger mode.
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buffer, const int bufferLength, const char deciTimeout)
{
    gint    retValue = 0;

    byte    recvBuff[bufferLength];
    gchar   symbolBuff[DATA_SYMBOL_LEN_MAXIMUM];

    if(gStylScannerFD==-1)
    {
        STYL_ERROR("Scanner device port is invalid.");
        return EXIT_FAILURE;
    }

    memset(recvBuff,   0, bufferLength           );
    memset(symbolBuff, 0, DATA_SYMBOL_LEN_MAXIMUM);

    retValue = mlsScannerSSI_Read(gStylScannerFD, recvBuff, bufferLength, deciTimeout, TRUE);

    if ( (retValue > 0) && (SSI_CMD_DECODE_DATA == recvBuff[PKG_INDEX_OPCODE]) )
    {
        retValue = mlsScannerPackage_Extract((gchar *)buffer, symbolBuff, recvBuff, (const gint)bufferLength, TRUE);
        STYL_INFO_1("Code Type: %s", symbolBuff);
    }
    else
    {
        if(retValue <= 0)
        {
            STYL_WARNING("No Data!");
        }
        else
        {
            STYL_ERROR("Received data is invalid.");
        }
        retValue = 0;
    }

//    /* Flush buffer of scanner one time for next read section */
//    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
//    {
//        STYL_ERROR("Can not flush buffer of device");
//    }
//    usleep(50000); /* waiting for scanner flush buffer */

    return retValue;
}

/*!
 * \brief mlsBarcodeReader_ReadData_Manual: Read decode data from already serial port opened before via manual-trigger mode.
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData_Manual(char *buffer, const int bufferLength, const char deciTimeout)
{
    gint    retValue = 0;

    byte    recvBuff[bufferLength];
    gchar   symbolBuff[DATA_SYMBOL_LEN_MAXIMUM];

    if(gStylScannerFD==-1)
    {
        STYL_ERROR("Scanner device port is invalid.");
        return EXIT_FAILURE;
    }

    memset(recvBuff,   0, bufferLength           );
    memset(symbolBuff, 0, DATA_SYMBOL_LEN_MAXIMUM);

    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_SESSION_START) != EXIT_SUCCESS)
    {
        STYL_ERROR("Start section request was fail.");
        return 0;
    }

    retValue = mlsScannerSSI_Read(gStylScannerFD, recvBuff, bufferLength, deciTimeout, TRUE);

    if ( (retValue > 0) && (SSI_CMD_DECODE_DATA == recvBuff[PKG_INDEX_OPCODE]) )
    {
        retValue = mlsScannerPackage_Extract((gchar *)buffer, symbolBuff, recvBuff, (const gint)bufferLength, TRUE);
        STYL_INFO_1("Code Type: %s", symbolBuff);
    }
    else
    {
        if(retValue <= 0)
        {
            STYL_WARNING("No Data!");
        }
        else
        {
            STYL_ERROR("Received data is invalid.");
        }
        retValue = 0;
    }

    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_SESSION_STOP) != EXIT_SUCCESS)
    {
        STYL_ERROR("Stop section request was fail.");
    }

    return retValue;
}

/*!
 * \brief mlsBarcodeReader_Setup: Open serial port then set configure for scanner is "scannerMode" mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Setup(const char *scannerPort, const int scannerMode)
{
    gint    scannerFD   = -1;
    char    retValue    = EXIT_FAILURE;


    STYL_INFO(" ** Scanner device port: %s **", scannerPort);

    scannerFD = mlsScannerConfig_OpenTTY(scannerPort);

    STYL_INFO(" ** Scanner device file descriptor: %d **", scannerFD);

    if(scannerFD != -1)
    {
        if(mlsScannerConfig_ConfigTTY(scannerFD) == EXIT_SUCCESS)
        {
            if (scannerMode == STYL_SCANNER_AUTOMODE)
            {
                if(mlsScannerConfig_ConfigSSI(scannerFD, STYL_SCANNER_AUTOMODE, TRUE) == EXIT_SUCCESS)
                {
                    STYL_INFO("Setup scanner with SSI parameter of auto-trigger scanning mode success.");
                    retValue = EXIT_SUCCESS;
                }
                else
                {
                    STYL_ERROR("Setup scanner with SSI parameter of auto-trigger scanning mode fail.");
                }
            }
            else if (scannerMode == STYL_SCANNER_MANUALMODE)
            {
                if(mlsScannerConfig_ConfigSSI(scannerFD, STYL_SCANNER_MANUALMODE, TRUE) == EXIT_SUCCESS)
                {
                    STYL_INFO("Setup scanner with SSI parameter of manual-trigger scanning mode success.");
                    retValue = EXIT_SUCCESS;

                        /* Flush buffer of device*/
                    if(mlsScannerSSI_SendCommand(scannerFD, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
                    {
                        STYL_ERROR("Flush buffer of decoder fail.");
                    }
                    else
                    {
                        STYL_INFO("Flush buffer of decoder success.");
                    }
                }
                else
                {
                    STYL_ERROR("Setup scanner with SSI parameter of manual-trigger scanning mode fail.");
                }
            }
        }
        mlsScannerConfig_CloseTTY_Only(scannerFD);
    }
    return retValue;
}

/*!
 * \brief mlsBarcodeReader_GetMode: Get the current mode of scanner
 * \return
 * - 0: scanner is not setup before
 * - 1: presentation/auto-trigger mode
 * - 2: host/manual-trigger mode
 */
unsigned int mlsBarcodeReader_GetMode(void)
{
    return (unsigned int)mlsScannerConfig_GetMode();
}

#if 0
/*!
 * \brief mlsBarcodeReader_LedAlert: trigger aim led 2 time for alert
 */
 void mlsBarcodeReader_LedAlert(void)
 {

 }
 #endif
/*@}*/

