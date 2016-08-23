#ifndef MLSQRREADER_H
#define MLSQRREADER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
//#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <locale.h>

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open();

/*!
 * \brief mlsBarcodeReader_Enable Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_SUCCESS: Fail
 */
char mlsBarcodeReader_Enable();

/*!
 * \brief mlsBarcodeReader_Disable Disable reader, Reader can't scan any QR code/bar code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_SUCCESS: Fail
 */
char mlsBarcodeReader_Disable();

/*!
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buff);

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_SUCCESS: Fail
 */
char mlsBarcodeReader_Close();

#ifdef __cplusplus
}
#endif
#endif // MLSQRREADER_H