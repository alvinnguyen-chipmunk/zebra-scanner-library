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
 * @file    CStylScannerSSI.h
 * @brief   C code - Implement SSI Communication.
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#include "CStylScannerUtils.h"
#include "CStylScannerSSI.h"
#include "CStylScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static void     StylScannerSSI_PreparePackage           (byte *package, byte opcode, byte *param, byte paramLen);
static uint16_t StylScannerSSI_CalculateChecksum        (byte *package, gint length);
static gint     StylScannerSSI_IsChecksumOK             (byte *package);

/********** Local (static) function definition section ************************/

/*!
 * \brief StylScannerSSI_IsChecksumOK: check 2 last bytes for checksum
 * \return
 * - TRUE : checksum is correct
 * - FALSE: checksum is incorrect
 */
static gint StylScannerSSI_IsChecksumOK(byte *package)
{
    uint16_t checksum = 0;

    checksum += package[PACKAGE_LEN(package) + 1];
    checksum += package[PACKAGE_LEN(package)] << 8;
    STYL_INFO("checksum receive: %x", checksum);
    STYL_INFO("checksum calculate: %x", StylScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package)));
    return (checksum == StylScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package)));
}

/*!
 * \brief StylScannerSSI_IsContinue: check package's type
 * \return
 * - TRUE  : Is last package in multiple packages stream
 * - FALSE : Is intermediate package in multiple packages stream
 */
gint StylScannerSSI_IsContinue(byte *package)
{
    return (SSI_PARAM_STATUS_CONTINUATION & package[PKG_INDEX_STAT]);
}

/*!
 * \brief PreparePkg: generate package from input opcode and params
 * \param
 */
static void StylScannerSSI_PreparePackage(byte *package, byte opcode, byte *param, byte paramLen)
{
    uint16_t checksum = 0;
    /*
        #define PKG_INDEX_LEN						0
        #define PKG_INDEX_OPCODE					1
        #define PKG_INDEX_SRC						2
        #define PKG_INDEX_STAT						3
    */
    package[PKG_INDEX_LEN]      = SSI_LEN_HEADER;
    package[PKG_INDEX_OPCODE]   = opcode        ;
    package[PKG_INDEX_SRC]      = SSI_ID_HOST   ;

    if(opcode == SSI_CMD_PARAM          ||
       opcode == SSI_CMD_SCAN_DISABLE   ||
       opcode == SSI_CMD_SCAN_ENABLE     )
        package[PKG_INDEX_STAT]     = SSI_PARAM_TYPE_PERMANENT;
    else
        package[PKG_INDEX_STAT]     = SSI_PARAM_TYPE_TEMPORARY; // fix here again

    if ( (NULL != param) && (0 != paramLen) )
    {
        package[PKG_INDEX_LEN] += paramLen;
        memcpy(&package[PKG_INDEX_STAT + 1], param, paramLen);
    }

    /* ***************** Add checksum ************************* */
    checksum = StylScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package));
    package[PACKAGE_LEN(package)]     = checksum >> 8;
    package[PACKAGE_LEN(package) + 1] = checksum & 0xFF;
}

/*!
 * \brief StylScannerSSI_CalculateChecksum: calculate 2's complement of package
 * \param
 * - package: pointer of package content.
 * - length: Length of package in byte.
 * \return 16 bits checksum (no 2'complement) value
 */
static uint16_t StylScannerSSI_CalculateChecksum(byte *package, gint length)
{
    uint16_t checksum = 0;
    for (guint i = 0; i < length; i++)
    {
        checksum += package[i];
    }
    /* ***************** 1's complement *********************** */
    checksum ^= 0xFFFF;	// flip all 16 bits
    /* ***************** 2's complement *********************** */
    checksum += 1;
    return checksum;
}

/********** Global function definition section ********************************/

/*!
 * \brief StylScannerSSI_Read: read formatted package and response ACK from/to scanner via file descriptor
 * \return number of read bytes
 */
gint StylScannerSSI_Read(gint pFile, byte *buffer, gint sizeBuffer, const gint timeout)
{
    gint retValue = 0;
    gint temp = 0;
    gint readRequest;
    unsigned char MAX_VMIN = 255;
    byte recvBuff[PACKAGE_LEN_MAXIMUM];
    struct termios devConf;

    gint oldVTIME = 0;
    gint oldVMIN  = 0;

    gint lastIndex = 0;

    memset(&recvBuff, 0, PACKAGE_LEN_MAXIMUM);

    STYL_INFO("MAX_VMIN: %x", MAX_VMIN);

    /* Backup for configure of termios */
    tcgetattr(pFile, &devConf);
    oldVTIME = devConf.c_cc[VTIME];
    oldVMIN  = devConf.c_cc[VMIN];

    do
    {
        /* Change reading condition for only 1 byte */
        devConf.c_cc[VTIME] = timeout;
        devConf.c_cc[VMIN] = 0;
        tcsetattr(pFile, TCSANOW, &devConf);

        /* Read 1 first byte for length */
        readRequest = 1;
        retValue += (gint)read(pFile, &recvBuff[PKG_INDEX_LEN], readRequest);
        STYL_INFO("retValue: %d", retValue);
        if(retValue <= 0)
        {
            break;
        }
        else
        {
            STYL_INFO("Length is: %x", recvBuff[PKG_INDEX_LEN]);
            if(recvBuff[PKG_INDEX_LEN] >= PACKAGE_LEN_MAXIMUM)
            {
                STYL_ERROR("Receive value of length package is invalid.");
                retValue = 0;
                break;
            }
            /* Read rest of byte of package */
            readRequest = recvBuff[PKG_INDEX_LEN] + SSI_LEN_CHECKSUM - 1; /* 1 is byte read before */
            STYL_INFO("Rest byte is: %d", readRequest);

            /* Change reading condition for ensure read enough bytes */
            /* However only except 127 */
            STYL_ERROR("oldVTIME: %d", oldVTIME);
            devConf.c_cc[VTIME] = oldVTIME;
            devConf.c_cc[VMIN] = MAX_VMIN;
            tcsetattr(pFile, TCSANOW, &devConf);

            temp = (gint) read(pFile, &recvBuff[PKG_INDEX_LEN + 1], readRequest);
            STYL_INFO("temp: %d", temp);
            if(temp != readRequest)
            {
                STYL_ERROR("read: %d - %s", errno, strerror(errno));
                retValue = 0;
                goto __error;
            }
            else
            {
                retValue += temp;
                STYL_INFO("retValue: %d", retValue);
                STYL_INFO("");
                StylScannerPackage_Display(recvBuff, PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM);
                if (StylScannerSSI_IsChecksumOK(recvBuff))
                {
                    /* Send ACK for buffer received */
                    STYL_INFO("Checksum OK! Send ACK for checksum.");
                    if(StylScannerSSI_Write(pFile, SSI_CMD_ACK, NULL, 0) != EXIT_SUCCESS)
                    {
                        STYL_ERROR("Send ACK for checksum fail.");
                    }
                    else
                    {
                        if(sizeBuffer <= (lastIndex + PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM))
                        {
                            STYL_ERROR("Receive buffer too large.");
                            retValue = 0;
                            break;
                        }
                        /* Copy current buffer to finally buffer */
                        memcpy(&buffer[lastIndex], recvBuff, PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM);
                        lastIndex += PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM;
                    }
                }
                else
                {
                    STYL_ERROR("Checksum fail!");
                    retValue = 0;
                    goto __error;
                }
            }
        }
    }
    while(StylScannerSSI_IsContinue(recvBuff));

//    STYL_INFO("");
//    StylScannerPackage_Display(buffer, retValue);

    /* Restore value configure of file descriptor */
    devConf.c_cc[VTIME] = oldVTIME;
    devConf.c_cc[VMIN]  = oldVMIN;
    tcsetattr(pFile, TCSANOW, &devConf);

    return retValue;

__error:
    /* Restore value configure of file descriptor */
    devConf.c_cc[VTIME] = oldVTIME;
    devConf.c_cc[VMIN]  = oldVMIN;
    tcsetattr(pFile, TCSANOW, &devConf);

    STYL_INFO("Error! Send NAK to scanner");
    if(StylScannerSSI_Write(pFile, SSI_CMD_NAK, NULL, 0) != EXIT_SUCCESS)
    {
        STYL_ERROR("Send NAK to scanner");
    }
    return retValue;
}

/*!
 * \brief StylScannerSSI_Write: write formatted package and check ACK to/from scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerSSI_Write(gint pFile, byte opcode, byte *param, byte paramLen)
{
    gint retValue = EXIT_SUCCESS;

    gint  bufferSize    = (SSI_LEN_HEADER + SSI_LEN_CHECKSUM + paramLen) * sizeof(byte);
    byte *bufferContent = malloc(bufferSize);

    /* ***************** Flush old input queue *************** */
    tcflush(pFile, TCIFLUSH);

    StylScannerSSI_PreparePackage(bufferContent, opcode, param, paramLen);

    STYL_WARNING("bufferSize: %d", bufferSize);
    STYL_WARNING("Send data: ");
    StylScannerPackage_Display(bufferContent, bufferSize);

    STYL_WARNING("PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM: %d", PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM);
    if ( write(pFile, bufferContent, PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM) <= 0)
    {
        STYL_ERROR("write: %d - %s", errno, strerror(errno));
        retValue = EXIT_FAILURE;
    }
    free(bufferContent);

    return retValue;
}

/*!
 * \brief StylScannerSSI_CheckACK: receive ACK package after StylScannerSSI_Write() and check for ACK
 * \return
 * - EXIT_SUCCESS: Success	ACK
 * - EXIT_FAILURE: Fail		Unknown cause
 */
gint StylScannerSSI_CheckACK(gint pFile)
{
    gint retValue = EXIT_SUCCESS;
    byte recvBuff[PACKAGE_LEN_ACK_MAXIMUM];

    STYL_INFO("Invoke StylScannerSSI_Read");
    retValue = StylScannerSSI_Read(pFile, recvBuff, PACKAGE_LEN_ACK_MAXIMUM, 1);
    if ( (retValue > 0) && (SSI_CMD_ACK == recvBuff[PKG_INDEX_OPCODE]) )
    {
        retValue = EXIT_SUCCESS;
    }
    else
    {
        retValue = EXIT_FAILURE;
    }

    return retValue;
}
/**@}*/
