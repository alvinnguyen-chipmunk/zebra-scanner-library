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
#define SSI_CUSTOM_DEFAULTS					0x12
#define SSI_DECODE_DATA						0xF3
#define SSI_FLUSH_QUEUE						0xD2
#define SSI_PARAM_SEND						0xC6

// Devices ID
#define SSI_HOST							0x04

#define SSI_DEFAULT_LEN						0x04
#define SSI_DEFAULT_STATUS					0x00
#define SSI_CKSUM_LEN						0x02

// Param
#define SSI_USB_DEV_TYPE					0x
#define SSI_DEC_FORMAT						0xEE
#define SSI_DEC_RAW							0x00
#define SSI_DEC_PACKED						0x01
#define SSI_BEEP_NONE						0xFF

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
#define SSI_STAT_RETRANS					0x01
#define SSI_STAT_CONTINUATION				0x02
#define SSI_STAT_CHANGETYPE					0x08

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
