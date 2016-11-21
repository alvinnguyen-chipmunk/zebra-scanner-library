/*******************************************************************************
     (C) Copyright 2009 Styl Solutions Co., Ltd. , All rights reserved *
     *
     This source code and any compilation or derivative thereof is the sole *
     property of Styl Solutions Co., Ltd. and is provided pursuant to a *
     Software License Agreement. This code is the proprietary information *
     of Styl Solutions Co., Ltd and is confidential in nature. Its use and *
     dissemination by any party other than Styl Solutions Co., Ltd is *
     strictly limited by the confidential information provisions of the *
     Agreement referenced above. *
     ******************************************************************************/

#ifndef ssi_h
#define ssi_h

#define BAUDRATE		B9600

typedef unsigned char byte;

// Commands
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

// NAK Code
#define NAK_RESEND							0x01
#define NAK_BAD_CONTEXT						0x02
#define NAK_DENIED							0x06
#define NAK_CANCEL							0x0A

// Devices ID
#define SSI_HOST							0x04
#define SSI_HEADER_LEN						0x04
#define SSI_DEFAULT_STATUS					0x00
#define SSI_CKSUM_LEN						0x02

// Param:
// PARAM_B_*: bool value parameters
#define ENABLE								0x01
#define DISABLE								0x00
#define PARAM_B_SCAN_PARAM					0xEC
#define PARAM_B_DEC_FORMAT					0xEE
#define PARAM_B_SW_ACK						0x9F
#define PARAM_B_DEC_EVENT					0x00
// PARAM_*: param with none-bool values
#define PARAM_BEEP_NONE						0xFF
#define PARAM_TRIGGER_MODE					0x8A
#define PARAM_TRIGGER_PRESENT				0x07
#define PARAM_TRIGGER_HOST					0x08
#define PARAM_TRIGGER_AUTO					0x09
#define PARAM_TRIGGER_AUTO_WITHLED			0x0A
#define PARAM_DEC_TIMEOUT					0x88
// PARAM_INDEX_Fx (x = 0, 1, 2)
#define PARAM_INDEX_F0						0xF0
#define PARAM_INDEX_F1						0xF1
#define PARAM_INDEX_F2						0xF2

// Package index
#define INDEX_LEN							0
#define INDEX_OPCODE						1
#define INDEX_SRC							2
#define INDEX_STAT							3
#define INDEX_BARCODETYPE					4
#define INDEX_CAUSE							4

// Status bits
/*
 Bit 0: Retransmit (0 = First transmission | 1 = Subsequent transmission)
 Bit 1: Continuation (0 = Last packet of a multipacket message | 1 = Intermediate packet)
 Bit 2: recserved (0x00)
 Bit 3: Parameter Change Type (0 = Temporary change | 1 = Permanent change)
 */
#define STAT_RETRANS					0x01
#define STAT_CONTINUATION				0x02
#define STAT_CHANGETYPE					0x08

// Actions
#define SSI_CUSTOM_DEFAULTS_ACT_WR			0x00
#define SSI_CUSTOM_DEFAULTS_ACT_RESTORE		0x01

// Limit
#define MAX_PKG_LEN							257

// Macro
#define PKG_LEN(x)		(x[INDEX_LEN])

// Error
#define ECKSUM								2
#define ENAK								0xA0
#define ENODEC								4

#endif /* ssi_command_h */
