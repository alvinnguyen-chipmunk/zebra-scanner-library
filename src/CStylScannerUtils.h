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
#ifdef __RELEASE__
#define ERROR(format, args...) __ERROR__("%s [STYLSSI-ERROR]: " format "%s \n",ANSI_COLOR_RED, ##args, ANSI_COLOR_RED)
#else
#define ERROR(format, args...) __ERROR__("%s [STYLSSI-ERROR]: %s():[%d] " format "%s \n",ANSI_COLOR_RED, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_RED)
#endif // __RELEASE__
#define STYL_ERROR(format, ...) ERROR(format, ##__VA_ARGS__)
#define STYL_ERROR_INLINE(format, ...) __ERROR__("%s" format "%s", ANSI_COLOR_RED, ##__VA_ARGS__, ANSI_COLOR_RED)

#define __WARNING__(format, ...) fprintf (stderr, format, ## __VA_ARGS__); fprintf(stderr, "%s", ANSI_COLOR_RESET)
#ifdef __RELEASE__
#define WARNING(format, args...) __WARNING__("%s [STYLSSI-WARNING]: " format "%s \n",ANSI_COLOR_YELLOW, ##args, ANSI_COLOR_YELLOW)
#else
#define WARNING(format, args...) __WARNING__("%s [STYLSSI-WARNING]: %s():[%d] " format "%s \n",ANSI_COLOR_YELLOW, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_YELLOW)
#endif // __RELEASE__
#define STYL_WARNING(format, ...) WARNING(format, ##__VA_ARGS__)
#define STYL_WARNING_INLINE(format, ...) __WARNING__("%s" format "%s", ANSI_COLOR_YELLOW, ##__VA_ARGS__, ANSI_COLOR_YELLOW)

#define __INFO__(format, ...) fprintf (stdout, format, ## __VA_ARGS__); fprintf(stdout, "%s", ANSI_COLOR_RESET)
#ifdef __RELEASE__
#define INFO(format, args...) __INFO__("%s [STYLSSI-INFO]: " format "%s \n",ANSI_COLOR_GREEN, ##args, ANSI_COLOR_GREEN)
#else
#define INFO(format, args...) __INFO__("%s [STYLSSI-INFO]: %s():[%d] " format "%s \n",ANSI_COLOR_GREEN, __FUNCTION__, __LINE__, ##args, ANSI_COLOR_GREEN)
#endif // __RELEASE__
#define STYL_INFO(format, ...) INFO(format, ##__VA_ARGS__)
#define STYL_INFO_INLINE(format, ...) __INFO__("%s" format "%s", ANSI_COLOR_GREEN, ##__VA_ARGS__, ANSI_COLOR_GREEN)
#define STYL_INFO_OTHER(format, ...) __INFO__("%s" format "%s", ANSI_COLOR_MAGENTA, ##__VA_ARGS__, ANSI_COLOR_MAGENTA)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BAUDRATE		B9600

/* ************** SSI Commands opcodes **************/
#define SSI_CMD_PARAM                       0xC6
#define SSI_CMD_ACK							0xD0
#define SSI_CMD_NAK							0xD1
#define SSI_CMD_DECODE_DATA                 0xF3
#define SSI_CMD_FLUSH_QUEUE                 0xD2
#define SSI_CMD_SCAN_ENABLE                 0xE9
#define SSI_CMD_SCAN_DISABLE                0xEA

/* ************** Devices ID **************************/
#define SSI_ID_HOST                         0x04
#define SSI_ID_DECODER                      0x00

/* ************** Field length ************************/
#define SSI_LEN_HEADER						0x04
#define SSI_LEN_CHECKSUM					0x02
#define SSI_LEN_DECODE_TYPE					0x01

/* ************** Parameter Type **********************/
#define SSI_PARAM_TYPE_PARAM_PREFIX			0xFF
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

/* ************** Parameter value **********************/
#define SSI_PARAM_VALUE_TRIGGER_PRESENT     0x07
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
#define PACKAGE_LEN_ACK_MAXIMUM            8

#define DATA_SYMBOL_LEN_MAXIMUM            32

/* ************** TTY configure **************************/
#define TTY_BUFF_MAXSIZE                    0xFF
#define TTY_TIMEOUT                         300 /* mili-seconds */
/********** Function declaration section **************************************/

#ifdef __cplusplus
}
#endif

#endif // CSTYLSCANNERUTILS_H_INCLUDED
/**@}*/
