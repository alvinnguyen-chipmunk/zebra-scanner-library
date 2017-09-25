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
 * @file    mlsUtils.h
 * @brief   C code - define some utils
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef CSTYLSCANNERUTILS_H_INCLUDED
#define CSTYLSCANNERUTILS_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
/********** Constant  and compile switch definition section *******************/
/********** Type definition section *******************************************/
typedef unsigned char byte;

/********** Macro definition section ******************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
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
#define STYL_ERROR_INLINE(format, ...) __ERROR__("%s" format "%s", ANSI_COLOR_RED, ##__VA_ARGS__, ANSI_COLOR_RED)

#define __WARNING__(format, ...) fprintf (stderr, format, ## __VA_ARGS__); fprintf(stderr, "%s", ANSI_COLOR_RESET)
#define WARNING(format, args...) __WARNING__("%s [STYLSSI-WARNING]: %s():[%d] " format "%s \n",ANSI_COLOR_YELLOW, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_YELLOW)
#define STYL_WARNING(format, ...) WARNING(format, ##__VA_ARGS__)
#define STYL_WARNING_INLINE(format, ...) __WARNING__("%s" format "%s", ANSI_COLOR_YELLOW, ##__VA_ARGS__, ANSI_COLOR_YELLOW)

#define __INFO__(format, ...) fprintf (stdout, format, ## __VA_ARGS__); fprintf(stdout, "%s", ANSI_COLOR_RESET)
#define INFO(format, args...) __INFO__("%s [STYLSSI-INFO]: %s():[%d] " format "%s \n",ANSI_COLOR_GREEN, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_GREEN)
#define STYL_INFO(format, ...) INFO(format, ##__VA_ARGS__)
#define STYL_INFO_INLINE(format, ...) __INFO__("%s" format "%s", ANSI_COLOR_GREEN, ##__VA_ARGS__, ANSI_COLOR_GREEN)
#define STYL_INFO_OTHER(format, ...) __INFO__("%s" format "%s", ANSI_COLOR_MAGENTA, ##__VA_ARGS__, ANSI_COLOR_MAGENTA)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BAUDRATE		B9600

/* ************** SSI Commands opcodes **************/
#define SSI_START_SESSION					0xE4
#define SSI_STOP_SESSION					0xE5
#define SSI_REQ_REVISION					0xA3
#define SSI_PARAM_SEND						0xC6
#define SSI_CMD_ACK							0xD0
#define SSI_CMD_NAK							0xD1
#define SSI_CUSTOM_DEFAULTS					0x12
#define SSI_DEC_DATA						0xF3
#define SSI_FLUSH_QUEUE						0xD2
#define SSI_PARAM_SEND						0xC6
#define SSI_SCAN_ENABLE						0xE9
#define SSI_SCAN_DISABLE					0xEA

#define SSI_LED_OFF                         0xE8
#define SSI_LED_ON                          0xE7
#define SSI_START_DECODE                    0xE4
#define SSI_STOP_DECODE                     0xE5

/* ************** NAK Codes **************************/
#define NAK_RESEND							0x01
#define NAK_BAD_CONTEXT						0x02
#define NAK_DENIED							0x06
#define NAK_CANCEL							0x0A

/* ************** Devices ID **************************/
#define SSI_HOST							0x04
#define SSI_HEADER_LEN						0x04
#define SSI_DEFAULT_STATUS					0x00
#define SSI_CKSUM_LEN						0x02

/* ************** Parameter B *************************/
/* PARAM_B_*: bool value parameters                   */
#define ENABLE								0x01
#define DISABLE							    0x00
#define PARAM_B_SCAN_PARAM					0xEC
#define PARAM_B_DEC_FORMAT					0xEE
#define PARAM_B_SW_ACK						0x9F
#define PARAM_B_DEC_EVENT					0x00

/* PARAM_*: param with none-bool values               */
#define PARAM_BEEP_NONE						0xFF
#define PARAM_TRIGGER_MODE					0x8A
#define PARAM_TRIGGER_PRESENT				0x07
#define PARAM_TRIGGER_HOST					0x08
#define PARAM_TRIGGER_AUTO					0x09
#define PARAM_TRIGGER_AUTO_WITHLED			0x0A
#define PARAM_DEC_TIMEOUT					0x88

/* PARAM_INDEX_Fx (x = 0, 1, 2)                       */
#define PARAM_INDEX_F0						0xF0
#define PARAM_INDEX_F1						0xF1
#define PARAM_INDEX_F2						0xF2

/* ************** Package index *************************/
#define INDEX_LEN							0
#define INDEX_OPCODE						1
#define INDEX_SRC							2
#define INDEX_STAT							3
#define INDEX_BARCODETYPE					4
#define INDEX_CAUSE							4

/* ************** Actions ********************************/
#define SSI_CUSTOM_DEFAULTS_ACT_WR			0x00
#define SSI_CUSTOM_DEFAULTS_ACT_RESTORE		0x01

/* ************** Message length Limit *******************/
#define MAX_PKG_LEN	                        257

/* ************** Error codes ****************************/
#define ECKSUM								2
#define ENAK								0xA0
#define ENODEC								4

/* ************** Scanner devices information ************/
#define SCANNER_SUBSYSTEM                  "tty"

#if 1
#define SCANNER_VENDOR_ID                  "05e0"
#define SCANNER_PRODUCT_ID                 "1701"
#else
#define SCANNER_VENDOR_ID                  "1d6b"
#define SCANNER_PRODUCT_ID                 "0002"
#endif // 1

/* ************** Other utils ****************************/
#define PKG_LEN(x)		                   (x[INDEX_LEN])

/********** Function declaration section **************************************/

#ifdef __cplusplus
}
#endif

#endif // CSTYLSCANNERUTILS_H_INCLUDED
/**@}*/
