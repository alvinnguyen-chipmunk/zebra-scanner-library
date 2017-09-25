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
#include "mlsBarcode.h"
#include "CStylScannerUtils.h"
#include "CStylScannerConfig.h"
#include "CStylScannerDevice.h"
#include "CStylScannerSSI.h"

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
        STYL_WARNING("Device path is not give. Auto-detect enable.");
        deviceNode = g_strdup(StylScannerDevice_GetNode(SCANNER_SUBSYSTEM,
                                               SCANNER_VENDOR_ID,
                                               SCANNER_PRODUCT_ID));
    }
    else
    {
        deviceNode = g_strdup(name);
    }

    STYL_INFO(" ** Scanner port: %s", deviceNode);

    /* ***************** Open device node ********************* */
    StylScanner_FD = StylScannerConfig_OpenTTY(deviceNode);
    g_free(deviceNode);

    if (StylScanner_FD <= 0)
    {
        StylScanner_FD = -1;
        ret = EXIT_FAILURE;
        goto EXIT;
    }

    /* ***************** Configure TTY port ******************* */
    if(StylScannerConfig_ConfigTTY(StylScanner_FD) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not configure TTY for device");
        goto ERROR;
    }

    if(StylScannerConfig_ConfigSSI(StylScanner_FD) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not configure SSI for device");
        goto ERROR;
    }

        /** ////////////////////////////////////// **/

    /* Flush buffer of device*/
//    if(mlsBarcodeReader_Flush() != EXIT_SUCCESS)
//    {
//        STYL_ERROR("Can not flush buffer of device");
//        goto ERROR;
//    }

    /* Enable device to scanning */
//    if(mlsBarcodeReader_Enable() != EXIT_SUCCESS)
//    {
//        STYL_ERROR("Can not enable device to scanning");
//        goto ERROR;
//    }


EXIT:
    return ret;
ERROR:
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
    return (int) StylScannerConfig_CloseTTY(StylScanner_FD);
}

/**@}*/

