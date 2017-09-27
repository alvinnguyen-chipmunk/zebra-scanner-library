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
 * @file    mlsDevice.h
 * @brief   C code - check tty port is exist and return its node
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

/********** Constant  and compile switch definition section *******************/

/********** Type definition section *******************************************/

/********** Macro definition section ******************************************/

/********** Function declaration section **************************************/

/*!
 * \brief mlsBarcodeReader_GetDevice - Get the device node file name of the udev device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char *mlsBarcodeReader_GetDevice(void);

/*!
 * \brief mlsBarcodeReader_GetVersion provide software version
 * \return string of software version
 * -
 */
const char *mlsBarcodeReader_GetVersion(void);

/*!
 * \brief mlsBarcodeReader_Open: Open Reader descriptor file for read write automatic.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Open(const char *name);

///*!
// * \brief mlsBarcodeReader_Manual_Open: Open Reader descriptor file for read write manually.
// * \return
// * - EXIT_SUCCESS: Success
// * - EXIT_FAILURE: Fail
// */
//int mlsBarcodeReader_Manual_Open(const char *name);

/*!
 * \brief mlsBarcodeReader_reopen() closes and re-opens scanner device file
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Reopen(const char *name);

/*!
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buffer, const int buffLength, const int timeout);

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
int mlsBarcodeReader_Close();

#ifdef __cplusplus
}
#endif
#endif // MLSBARCODE_H_INCLUDED
/**@}*/
