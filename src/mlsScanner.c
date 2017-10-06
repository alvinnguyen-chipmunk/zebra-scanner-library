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
 * \brief mlsBarcodeReader_GetVersion provide software version
 * \return string of software version
 * -
 */
//const char *mlsBarcodeReader_GetVersion(void)
char * GetVersion(void)
{
    return VERSION;
}

/*!
 * \brief mlsBarcodeReader_GetDevice - Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void)
{
    const char * temp = mlsScannerDevice_GetNode(SCANNER_SUBSYSTEM, SCANNER_VENDOR_ID, SCANNER_PRODUCT_ID);
    return temp;
}

/*!
 * \brief mlsBarcodeReader_Open: Open Reader descriptor file for read write automatic.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
//int mlsBarcodeReader_Open(const char *name)
char mlsBarcodeReader_Open(char *name)
{
    gchar    *deviceNode = NULL;
    gboolean scannerReady = FALSE;
    gint     retValue = EXIT_SUCCESS;

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

    STYL_INFO(" ** Scanner port: %s", deviceNode);

    #if 0
    gint     tryNumber = 1;
    do
    {
        STYL_WARNING("Try more time ..............");
        tryNumber--;
        /* ***************** Open device node ********************* */
        gStylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);
        if (gStylScannerFD != -1)
        {
            /* ***************** Configure TTY port ******************* */
            if(mlsScannerConfig_ConfigTTY(gStylScannerFD) == EXIT_SUCCESS)
            {
                if(mlsScannerConfig_ConfigSSI(gStylScannerFD, SCANNING_TRIGGER_AUTO) == EXIT_SUCCESS)
                {
                    scannerReady = TRUE;
                    break;
                }
                else
                {
                    STYL_ERROR("Can not set configure for SSI protocol.");
                    mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
                }
            }
        }
        sleep(3);
    }
    while(tryNumber > 0);
    #else
    ///////////////////////////////////////////////////////////////////
    STYL_ERROR("Configure first time.");
    gStylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);
    if(mlsScannerConfig_ConfigTTY(gStylScannerFD) == EXIT_SUCCESS)
    {
        if(mlsScannerConfig_ConfigSSI(gStylScannerFD, SCANNING_TRIGGER_AUTO)==EXIT_FAILURE)
            sleep(10);
    }
    mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
    gStylScannerFD = -1;

    sleep(2);
    ///////////////////////////////////////////////////////////////////
    gint tryNumber = 1;
    do
    {
        tryNumber--;
        STYL_ERROR("Reopen for working section.");
        gStylScannerFD = mlsScannerConfig_OpenTTY(deviceNode);

        g_free(deviceNode);

        if (gStylScannerFD != -1)
        {
            /* ***************** Configure TTY port ******************* */
            if(mlsScannerConfig_ConfigTTY(gStylScannerFD) == EXIT_SUCCESS)
            {
                retValue = mlsScannerConfig_ConfigSSI(gStylScannerFD, SCANNING_TRIGGER_AUTO);
                #if 0
                switch(retValue)
                {
                case EXIT_SUCCESS:
                    scannerReady = TRUE;
                    break;
                case EXIT_FAILURE:
                    STYL_ERROR("Can not set configure for SSI protocol.");
                    mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
                    goto __error;
                    break;
                case EXIT_WARNING:
                    scannerReady = FALSE;
                    break;
                }
                #else
                if(mlsScannerConfig_ConfigSSI(gStylScannerFD, SCANNING_TRIGGER_AUTO) == EXIT_SUCCESS)
                {
                    scannerReady = TRUE;
                    break;
                }
                else
                {
                    STYL_ERROR("Can not set configure for SSI protocol.");
                    mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
                }
                #endif
            }
        }
    }
    while(tryNumber > 0);
    #endif

    if(scannerReady==FALSE)
        goto __error;

    /* Flush buffer of device*/
    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
        goto __error;
    }
    /* Enable device to scanning */
    if(mlsScannerSSI_SendCommand(gStylScannerFD,SSI_CMD_SCAN_ENABLE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not enable device to scanning");
        goto __error;
    }

__exit:
    return EXIT_SUCCESS;

__error:
    /* ***************** Close TTY port ******************* */
    //mlsScannerConfig_CloseTTY_Only(gStylScannerFD);
    gStylScannerFD = -1;
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
//int mlsBarcodeReader_Close()
char mlsBarcodeReader_Close()
{
    STYL_INFO("Close scanner device.");
    return (char) mlsScannerConfig_CloseTTY(gStylScannerFD);
}

/*!
 * \brief mlsBarcodeReader_Reopen closes then opens device
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
//int mlsBarcodeReader_Reopen(const char *name)
char mlsBarcodeReader_Reopen(char *name)
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
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buffer, const int bufferLength, const int timeout)
{
    gint retValue = 0;

    byte  recvBuff[bufferLength];
    gchar symbolBuff[DATA_SYMBOL_LEN_MAXIMUM];

    memset(recvBuff,   0, bufferLength           );
    memset(symbolBuff, 0, DATA_SYMBOL_LEN_MAXIMUM);

    STYL_INFO("Invoke mlsScannerSSI_Read");

    /* Convert 1/10 seconds to mili-seconds */
    guint timeout_ms = (timeout*1000)/10;

    retValue = mlsScannerSSI_Read(gStylScannerFD, recvBuff, bufferLength, timeout_ms);

    if ( (retValue > 0) && (SSI_CMD_DECODE_DATA == recvBuff[PKG_INDEX_OPCODE]) )
    {
        STYL_INFO("READ DATA SIZE: %d", retValue);
        mlsScannerPackage_Display(recvBuff, retValue);
        retValue = mlsScannerPackage_Extract((gchar *)buffer, symbolBuff, recvBuff, (const gint)bufferLength);
        STYL_INFO("Code Type: %s", symbolBuff);
//        STYL_DEBUG("Send to stop section after receive a valid decode data.");
//        if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_SESSION_STOP) != EXIT_SUCCESS)
//        {
//            STYL_ERROR("Stop section request was fail.");
//        }
    }
    else
    {
        if(retValue <=0 )
        {
            STYL_WARNING("Nothing is received.");
        }
        else
        {
            STYL_ERROR("Received data is invalid.");
        }
    }

    return retValue;
}

/*!
 * \brief mlsBarcodeReader_ManualMode: Set configure for scanner via manual scanning.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_ManualMode()
{
    if(mlsScannerConfig_ConfigSSI(gStylScannerFD, SCANNING_TRIGGER_MANUAL) != EXIT_SUCCESS )
    {
        STYL_ERROR("Can not configure SSI for device");
    }
}

/*!
 * \brief mlsBarcodeReader_ReadData_Manual: Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData_Manual(char *buffer, const int bufferLength, const int timeout)
{
    gint retValue = 0;

    byte  recvBuff[bufferLength];
    gchar symbolBuff[DATA_SYMBOL_LEN_MAXIMUM];

    memset(recvBuff,   0, bufferLength           );
    memset(symbolBuff, 0, DATA_SYMBOL_LEN_MAXIMUM);


    STYL_INFO("Send START SECTION");
    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_SESSION_START) != EXIT_SUCCESS)
    {
        STYL_ERROR("Start section request was fail.");
        return 0;
    }

    /* Convert 1/10 seconds to mili-seconds */
    guint timeout_ms = (timeout*1000)/10;

    STYL_INFO("************************************** Invoke mlsScannerSSI_Read");
    retValue = mlsScannerSSI_Read(gStylScannerFD, recvBuff, bufferLength, timeout_ms);

    if ( (retValue > 0) && (SSI_CMD_DECODE_DATA == recvBuff[PKG_INDEX_OPCODE]) )
    {
        STYL_INFO("READ DATA SIZE: %d", retValue);
        mlsScannerPackage_Display(recvBuff, retValue);
        retValue = mlsScannerPackage_Extract((gchar *)buffer, symbolBuff, recvBuff, (const gint)bufferLength);
        STYL_INFO("Code Type: %s", symbolBuff);
    }
    else
    {
        if(retValue <= 0 )
        {
            STYL_WARNING("Nothing was received.");
        }
        else
        {
            STYL_ERROR("Received data was invalid.");
        }
        retValue = 0;
    }

    STYL_INFO("Send STOP SECTION");
    if(mlsScannerSSI_SendCommand(gStylScannerFD, SSI_CMD_SESSION_STOP) != EXIT_SUCCESS)
    {
        STYL_ERROR("Stop section request was fail.");
    }

    return retValue;
}

/*@}*/
























