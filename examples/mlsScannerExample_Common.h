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
 * @file    mlsScannerExample_Common.h
 * @brief   C code - define some utils for example applications
 *
 * Long description.
 * @date    16/10/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSSCANNEREXAMPLE_COMMON_H_INCLUDED
#define MLSSCANNEREXAMPLE_COMMON_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
/********** Constant  and compile switch definition section *******************/
/********** Type definition section *******************************************/
/********** Macro definition section ******************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define STYL_INFO(format, ...)  \
    mlsScannerExample_Print(0, "%s[STYL INFO]: %s():%d" format "%s\n", ANSI_COLOR_BLUE, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_INFO_1(format, ...)  \
    mlsScannerExample_Print(0, "%s[STYL INFO]: %s():%d" format "%s\n", ANSI_COLOR_GREEN, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_ERROR(format, ...) \
    mlsScannerExample_Print(0, "%s[STYL ERROR]: %s():%d" format "%s\n", ANSI_COLOR_RED, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_WARNING(format, ...)  \
    mlsScannerExample_Print(0, "%s[STYL WARNING]: %s():%d" format "%s\n", ANSI_COLOR_YELLOW, __FUNCTION__,__LINE__, ##__VA_ARGS__, ANSI_COLOR_RESET);

#define STYL_STRING_MAXLEN          4096

#define TRUE                        1
#define FALSE                       0
/********** Function declaration section **************************************/
/*!
 * \brief mlsScannerExample_Print: Print out some information.
 */
void mlsScannerExample_Print(int isError, char *format, ...);

#ifdef __cplusplus
}
#endif
#endif // MLSSCANNEREXAMPLE_COMMON_H_INCLUDED
/*@}*/
