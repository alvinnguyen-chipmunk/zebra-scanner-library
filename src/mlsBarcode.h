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
 * @file    mlsBarcode.h
 * @brief   C library communicate Qrcode/Barcode decoder.
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSBARCODE_H_INCLUDED
#define MLSBARCODE_H_INCLUDED
#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
/********** Constant and compile switch definition section ********************/
/********** Type definition section *******************************************/
/********** Macro definition section*******************************************/
/* ************** Scanning mode **************************/
#define STYL_SCANNER_NONEMODE       0
#define STYL_SCANNER_AUTOMODE       1
#define STYL_SCANNER_MANUALMODE     2
#define STYL_SCANNER_CONTINUOUSMODE 3

/********** Function declaration section **************************************/
/*!
 * \brief mlsBarcodeReader_Open: Open serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open(const char *name);

/*!
 * \brief mlsBarcodeReader_Reopen: Close then re-open serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Reopen(const char *name);

/*!
 * \brief mlsBarcodeReader_Open_ManualMode: Open serial port then set configure for scanner is manual-trigger mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open_ManualMode(const char *name);

/*!
 * \brief mlsBarcodeReader_Open_AutoMode: Open serial port then set configure for scanner is auto-trigger mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open_AutoMode(const char *name);

/*!
 * \brief mlsBarcodeReader_ReadData: Read decode data from already serial port opened before via auto-trigger mode.
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buffer, const int bufferLength, const char deciTimeout);

/*!
 * \brief mlsBarcodeReader_ReadData_Manual: Read decode data from already serial port opened before via manual-trigger mode.
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData_Manual(char *buffer, const int bufferLength, const char deciTimeout);

/*!
 * \brief mlsBarcodeReader_close: Close serial device port of scanner camera.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Close();

/*!
 * \brief mlsBarcodeReader_Setup: Open serial port then set configure for scanner is "scannerMode" mode.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Setup(const char *scannerPort, const int scannerMode);

/*!
 * \brief mlsBarcodeReader_GetRevision: Get revision number of decoder.
 * \return
 * - length of revision string: success
 * - 0: failure.
 */
unsigned int mlsBarcodeReader_GetRevision(char *buffer, int bufferLength, char deciTimeout);

/*!
 * \brief mlsBarcodeReader_GetDevice: Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void);

/*!
 * \brief mlsBarcodeReader_GetMode: Get the current mode of scanner
 * \return
 * - 0: scanner is not setup before
 * - 1: presentation/auto-trigger mode
 * - 2: host/manual-trigger mode
 */
unsigned int mlsBarcodeReader_GetMode(void);

/*!
 * \brief GetVersion: provide software version
 * \return string of software version
 */
char * GetVersion(void);

/*!
 * \brief mlsBarcodeReader_GetVersion: provide software version
 * \return string of software version
 */
char * mlsBarcodeReader_GetVersion(void);

#if 0
/*!
 * \brief mlsBarcodeReader_LedAlert: trigger aim led 2 time for alert
 */
 void mlsBarcodeReader_LedAlert(void);
#endif

#ifdef __cplusplus
}
#endif
#endif // MLSBARCODE_H_INCLUDED
/*@}*/
