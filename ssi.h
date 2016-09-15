//
//  ssi_command.h
//  zebra_scanner
//
//  Created by Hoàng Trung Huy on 8/19/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

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
// PARAM_*: param with none-bool values
#define PARAM_BEEP_NONE						0xFF
#define PARAM_TRIGGER_MODE					0x8A
#define PARAM_TRIGGER_HOST					0x08
#define PARAM_DEC_TIMEOUT					0x88

// Package index
#define INDEX_LEN							0
#define INDEX_OPCODE						1
#define INDEX_SRC							2
#define INDEX_STAT							3
#define INDEX_BARCODETYPE					4

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
#define ECKSUM								1

#endif /* ssi_command_h */
