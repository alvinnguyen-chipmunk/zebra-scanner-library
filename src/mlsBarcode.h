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
 * @file    mlsBarcode.h
 * @brief   C library - get data from qrcode/barcode scanner
 *
 * Long description.
 * @date    13/07/2017
 * @author  luck.hoang alvin.nguyen
 */

#ifndef MLSQRREADER_H
#define MLSQRREADER_H
#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <locale.h>

/********** Constant  and compile switch definition section *******************/

/********** Type definition section *******************************************/

/********** Macro definition section ******************************************/
#define     SCANNER_SUBSYSTEM       "tty"
#define     SCANNER_VENDOR_ID       "05e0"
#define     SCANNER_PRODUCT_ID      "1701"

/********** Function declaration section **************************************/

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Open(const char *name);

/*!
 * \brief mlsBarcodeReader_Enable Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */

unsigned int mlsBarcodeReader_ReadData(char *buff, const int buffLength, const int timeout);

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Close();

/*!
 * \brief mlsBarcodeReader_GetVersion provide software version
 * \return string of software version
 * -
 */
const char *mlsBarcodeReader_GetVersion(void);

/*!
 * \brief mlsBarcodeReader_reopen() closes and re-opens scanner device file
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Reopen(const char *name);

/*!
 * \brief mlsBarcodeReader_Test Test subroutine
 * \param testString: A known barcode/qrcode
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Test(char * testString);

/*!
 * \brief mlsBarcodeReader_GetDevice - Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void);

#ifdef __cplusplus
}
#endif
#endif // MLSQRREADER_H
/**@}*/
