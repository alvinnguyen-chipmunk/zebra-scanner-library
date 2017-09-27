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
 * @file    CStylScannerDevice.c
 * @brief   C code - check tty port is exist and return its node
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include "string.h"

#include "mlsBarcode.h"
#include "CStylScannerUtils.h"
#include "CStylScannerConfig.h"
#include "CStylScannerDevice.h"
#include "CStylScannerSSI.h"
#include "CStylScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
static gint StylScanner_FD          = -1;
/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/
/********** Global function definition section ********************************/

/*!
 * \brief mlsBarcodeReader_GetVersion provide software version
 * \return string of software version
 * -
 */
const char *mlsBarcodeReader_GetVersion(void)
{
    return VERSION;
}

/*!
 * \brief mlsBarcodeReader_GetDevice - Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void)
{
    const char * temp = StylScannerDevice_GetNode(SCANNER_SUBSYSTEM, SCANNER_VENDOR_ID, SCANNER_PRODUCT_ID);
    return temp;
}

/*!
 * \brief mlsBarcodeReader_Open: Open Reader descriptor file for read write automatic.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Open(const char *name)
{
    gchar *deviceNode = NULL;
    int ret = EXIT_SUCCESS;

    if(name == NULL)
    {
        /* ***************** Auto-detect for device node string ********************* */
        STYL_WARNING("Device node string is not give. Auto-detect enabled.");
        deviceNode = g_strdup(StylScannerDevice_GetNode(SCANNER_SUBSYSTEM,
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

    /* ***************** Open device node ********************* */
    StylScanner_FD = StylScannerConfig_OpenTTY(deviceNode);
    g_free(deviceNode);

    if (StylScanner_FD <= 0)
    {
        StylScanner_FD = -1;
        ret = EXIT_FAILURE;
        goto __exit;
    }

    /* ***************** Configure TTY port ******************* */
    if(StylScannerConfig_ConfigTTY(StylScanner_FD) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not configure TTY for device");
        goto __error;
    }

    if(StylScannerConfig_ConfigSSI(StylScanner_FD) != EXIT_SUCCESS )
    {
        STYL_ERROR("Can not configure SSI for device");
        goto __error;
    }


    /* Flush buffer of device*/
    if(StylScannerSSI_SendCommand(StylScanner_FD, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
        goto __error;
    }
    /* Enable device to scanning */
    if(StylScannerSSI_SendCommand(StylScanner_FD,SSI_CMD_SCAN_ENABLE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not enable device to scanning");
        goto __error;
    }

__exit:
    return ret;
__error:
    /* ***************** Close TTY port ******************* */
    StylScannerConfig_CloseTTY(StylScanner_FD);
    StylScanner_FD = -1;
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Close()
{
    STYL_INFO("Close scanner device.");
    return (int) StylScannerConfig_CloseTTY(StylScanner_FD);
}

/*!
 * \brief mlsBarcodeReader_Reopen closes then opens device
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Reopen(const char *name)
{
    STYL_WARNING("*****************************");
    STYL_WARNING("ENTER mlsBarcodeReader_Reopen");
    STYL_WARNING("*****************************");

    int retValue = EXIT_SUCCESS;
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
unsigned int mlsBarcodeReader_ReadData(char *buffer, const int buffLength, const int timeout)
{
    gint retValue = 0;

    if( (timeout <= 0) || (timeout >= 25) )
        return 0;

    byte  recvBuff[buffLength];
    gchar symbolBuff[DATA_SYMBOL_LEN_MAXIMUM];

    memset(recvBuff,   0, buffLength             );
    memset(symbolBuff, 0, DATA_SYMBOL_LEN_MAXIMUM);

    STYL_INFO("Invoke StylScannerSSI_Read");
    retValue = StylScannerSSI_Read(StylScanner_FD, recvBuff, buffLength, timeout);

    if ( (retValue > 0) && (SSI_CMD_DECODE_DATA == recvBuff[PKG_INDEX_OPCODE]) )
    {
        STYL_INFO("READ DATA SIZE: %d", retValue);
        StylScannerPackage_Display(recvBuff, retValue);
        retValue = StylScannerPackage_Extract((gchar *)buffer, symbolBuff, recvBuff, (const gint)buffLength);
        STYL_INFO("Code Type: %s", symbolBuff);
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


/**@}*/

