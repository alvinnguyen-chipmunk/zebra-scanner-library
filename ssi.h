//
//  ssi_command.h
//  zebra_scanner
//
//  Created by Hoàng Trung Huy on 8/19/16.
//  Copyright © 2016 Hoàng Trung Huy. All rights reserved.
//

#ifndef ssi_h
#define ssi_h

#define ZEBRA_DEVICE		"/dev/ttyACM0"
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

// Devices ID
#define SSI_HOST							0x04

#define SSI_DEFAULT_LENGTH					0x04
#define SSI_DEFAULT_STATUS					0x00
#define SSI_CKSUM_LENGTH					0x02

// Parameters
#define SSI_DEC								0xEE
#define SSI_DEC_RAW							0x00
#define SSI_DEC_PACKED						0x01
#define SSI_BEEP_NONE						0xFF

// Package index
#define SSI_PKG_LENGTH						0
#define SSI_PKG_OPCODE						1
#define SSI_PKG_SOURCE						2
#define SSI_PKG_STATUS						3
#define SSI_PKG_BARCODETYPE					4

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

#endif /* ssi_command_h */
