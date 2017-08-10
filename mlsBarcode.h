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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifdef __RELEASE__
#define __DEBUG__(format, ...)
#else
#define __DEBUG__(format, ...) fprintf(stderr, "%s", ANSI_COLOR_RESET); fprintf (stderr, format, ## __VA_ARGS__)
#endif // __RELEASE__
#define DEBUG_BEGIN(format, args...) __DEBUG__("[STYLSSI-DEBUG]: %s||%s():[%d] " format ": ",__FILE__,__FUNCTION__, __LINE__, ##args)
#define DEBUG_END(format, args...) __DEBUG__(format "\n", ##args)
#define DEBUG(format, args...) __DEBUG__("[STYLSSI-DEBUG]: %s||%s():[%d] " format "\n",__FILE__,__FUNCTION__, __LINE__, ##args)
#define DEBUG_1(format, ...) DEBUG("\n"      format, ##__VA_ARGS__)
#define DEBUG_0() DEBUG("\n")
#define STYL_DEBUG(format, ...) DEBUG(format, ##__VA_ARGS__)

#define STYL_DEBUG_BEGIN(format, ...) DEBUG_BEGIN(format, ##__VA_ARGS__)
#define STYL_DEBUG_END(format, ...) DEBUG_END(format, ##__VA_ARGS__)


#define __ERROR__(format, ...) fprintf (stderr, format, ## __VA_ARGS__) ; fprintf(stderr, "%s", ANSI_COLOR_RESET)
#define ERROR(format, args...) __ERROR__("%s [STYLSSI-ERROR]: %s():[%d] " format "%s \n",ANSI_COLOR_RED, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_RED)
#define STYL_ERROR(format, ...) ERROR(format, ##__VA_ARGS__)

#ifndef __CONSOLE__
#define __WARNING__(format, ...)
#else
#define __WARNING__(format, ...) fprintf (stderr, format, ## __VA_ARGS__); fprintf(stderr, "%s", ANSI_COLOR_RESET)
#endif // __CONSOLE__
#define WARNING(format, args...) __WARNING__("%s [STYLSSI-WARNING]: %s():[%d] " format "%s \n",ANSI_COLOR_YELLOW, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_YELLOW)
#define STYL_WARNING(format, ...) WARNING(format, ##__VA_ARGS__)

#ifndef __CONSOLE__
#define __INFO__(format, ...)
#else
#define __INFO__(format, ...) fprintf (stdout, format, ## __VA_ARGS__); fprintf(stdout, "%s", ANSI_COLOR_RESET)
#endif // __CONSOLE__
#define INFO(format, args...) __INFO__("%s [STYLSSI-INFO]: %s():[%d] " format "%s \n",ANSI_COLOR_GREEN, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_GREEN)
#define STYL_INFO(format, ...) INFO(format, ##__VA_ARGS__)

/********** Function declaration section **************************************/

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open(const char *name);

/*!
 * \brief mlsBarcodeReader_Enable Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Enable();

/*!
 * \brief mlsBarcodeReader_Disable Disable reader, Reader can't scan any QR code/bar code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Disable();

/*!
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buff, const int buffLength, const int timeout);

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Close();

/*!
 * \brief GetVersion provide software version
 * \return string of software version
 * -
 */
char *GetVersion(void);

/*!
 * \brief mlsBarcodeReader_reopen() closes and re-opens scanner device file
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Reopen(const char *name);

/*!
 * \brief mlsBarcodeReader_Flush Flush buffer of scanner
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Flush();

#ifdef __cplusplus
}
#endif
#endif // MLSQRREADER_H
/**@}*/
