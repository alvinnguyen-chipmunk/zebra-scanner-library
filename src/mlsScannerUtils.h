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
 * @file    mlsScannerUtils.h
 * @brief   C code - define some utils
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSSCANNERUTILS_H_INCLUDED
#define MLSSCANNERUTILS_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"

/********** Constant  and compile switch definition section *******************/
/********** Type definition section *******************************************/
typedef unsigned char byte;

/********** Macro definition section ******************************************/#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define STYL_INFO(format, ...)          printf(ANSI_COLOR_BLUE); \
                                        mlsScannerUtils_Print(0, "[STYL SSI SCANNER INFO]: " format "\n", ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);
#define STYL_INFO_INLINE(format, ...)   printf(ANSI_COLOR_BLUE); \
                                        mlsScannerUtils_Print(0, "[STYL SSI SCANNER INFO]: " format, ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);


#define STYL_ERROR(format, ...)         printf(ANSI_COLOR_RED); \
                                        mlsScannerUtils_Print(1, "[STYL SSI SCANNER ERROR]: " format "\n", ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);
#define STYL_ERROR_INLINE(format, ...)  printf(ANSI_COLOR_RED); \
                                        mlsScannerUtils_Print(1, "[STYL SSI SCANNER ERROR]: " format, ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);

#define STYL_WARNING(format, ...)       printf(ANSI_COLOR_YELLOW); \
                                        mlsScannerUtils_Print(0, "[STYL SSI SCANNER WARN]: " format "\n", ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);
#define STYL_WARNING_INLINE(format, ...) printf(ANSI_COLOR_YELLOW); \
                                        mlsScannerUtils_Print(0, "[STYL SSI SCANNER WARN]: " format, ##__VA_ARGS__); \
                                        printf(ANSI_COLOR_RESET);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BAUDRATE            B9600
#define TIMEOUT_BYTE_MS     2 /* (1/9600)*8*1000 ~= 0.8333 milisecond*/

/* ************** SSI Commands opcodes **************/
#define SSI_CMD_PARAM                       0xC6
#define SSI_CMD_ACK							0xD0
#define SSI_CMD_NAK							0xD1
#define SSI_CMD_DECODE_DATA                 0xF3
#define SSI_CMD_FLUSH_QUEUE                 0xD2
#define SSI_CMD_SCAN_ENABLE                 0xE9
#define SSI_CMD_SCAN_DISABLE                0xEA
#define SSI_CMD_SESSION_START               0xE4
#define SSI_CMD_SESSION_STOP                0xE5
#define SSI_CMD_REVISION_REQUEST            0xA3
#define SSI_CMD_REVISION_REPLY              0xA4


/* ************** Devices ID **************************/
#define SSI_ID_HOST                         0x04
#define SSI_ID_DECODER                      0x00

/* ************** Field length ************************/
#define SSI_LEN_HEADER						0x04
#define SSI_LEN_CHECKSUM					0x02
#define SSI_LEN_DECODE_TYPE					0x01
#define SSI_LEN_WAKEUP                      0x02

/* ************** Parameter Type **********************/
#define SSI_PARAM_TYPE_TEMPORARY	    	0x00
#define SSI_PARAM_TYPE_PERMANENT 			0x08

/* ************** Parameter Status *********************/
/*
    Bit 0: Retransmit (0 = First transmission | 1 = Subsequent transmission)
    Bit 1: Continuation (0 = Last packet of a multipacket message | 1 = Intermediate packet)
    Bit 2: recserved (0x00)
    Bit 3: Parameter Change Type (0 = Temporary change | 1 = Permanent change)
*/
#define SSI_PARAM_STATUS_RETRANS            0x01
#define SSI_PARAM_STATUS_CONTINUATION       0x02
#define SSI_PARAM_STATUS_CHANGE_TYPE        0x08

/* ************** Parameter index **********************/
#define SSI_PARAM_INDEX_TRIGGER             0x8A
#define SSI_PARAM_DEF_FORMAT_B              0xEE
#define SSI_PARAM_B_DEF_SW_ACK              0x9F
#define SSI_PARAM_B_DEF_SCAN                0xEC
#define SSI_PARAM_INDEX_EVENT               0xF0
#define SSI_PARAM_INDEX_EVENT_DECODE        0x00

/* ************** Parameter value **********************/
#define SSI_PARAM_VALUE_TRIGGER_PRESENT     0x07
#define SSI_PARAM_VALUE_TRIGGER_HOST        0x08
#define SSI_PARAM_VALUE_BEEP    			0xFF
#define SSI_PARAM_VALUE_ENABLE              0x01
#define SSI_PARAM_VALUE_DISABLE             0x00

/* ************** Package index *************************/
#define PKG_INDEX_LEN						0x00
#define PKG_INDEX_OPCODE					0x01
#define PKG_INDEX_SRC						0x02
#define PKG_INDEX_STAT						0x03
#define PKG_INDEX_DECODE_TYPE               0x04

/* ************** Scanner devices information ************/
#define SCANNER_SUBSYSTEM                  "tty"
#define SCANNER_VENDOR_ID                  "05e0"
#define SCANNER_PRODUCT_ID                 "1701"

/* ************** Other utils ****************************/
#define PACKAGE_LEN(x)		               (x[PKG_INDEX_LEN])
#define PACKAGE_LEN_MAXIMUM                257
#define PACKAGE_LEN_ACK_MAXIMUM            257

#define DATA_SYMBOL_LEN_MAXIMUM            32

#define NO_GIVEN                           -1

#define EXIT_WARNING                        2

/* ************** TTY configure **************************/
#define WRITE_TTY_TIMEOUT                  2000  /* mili-seconds */
#define ACK_TIMEOUT                        50 /* deci-seconds */
#define READ_TTY_TIMEOUT                   20 /* deci-seconds */

/* ************** Scanning mode **************************/
#define SCANNING_TRIGGER_NONE              0x00
#define SCANNING_TRIGGER_AUTO              0x01
#define SCANNING_TRIGGER_MANUAL            0x02

/********** Function declaration section **************************************/
/*!
 * \brief mlsScannerUtils_Print: Print out some information.
 */
void mlsScannerUtils_Print(gint isError, gchar *format, ...);

#ifdef __cplusplus
}
#endif
#endif // MLSSCANNERUTILS_H_INCLUDED
/*@}*/
