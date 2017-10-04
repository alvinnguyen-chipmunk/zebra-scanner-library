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
 * @file    mlsScannerSSI.h
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

#include "mlsScannerUtils.h"
#include "mlsScannerSSI.h"
#include "mlsScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static void     mlsScannerSSI_PreparePackage           (byte *package, byte opcode, byte *param, byte paramLen);
static uint16_t mlsScannerSSI_CalculateChecksum        (byte *package, gint length);
static gint     mlsScannerSSI_IsChecksumOK             (byte *package);
static gint     mlsScannerSSI_SerialRead               (gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms);
static gint     mlsScannerSSI_SerialWrite              (gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms);
/********** Local (static) function definition section ************************/

/*!
 * \brief mlsScannerSSI_SerialRead: read data from serial port
 * \return
 * - number of readed bytes or O if no byte was readed
 */
static gint mlsScannerSSI_SerialRead(gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms)
{
    /*  Initialize the file descriptor set.*/
    fd_set  fds;
    FD_ZERO (&fds);
    FD_SET  (pFile, &fds);

    /* Initialize the timeout. */
    struct timeval tv;

    guint min_timeout_ms = TIMEOUT_BYTE_MS * sizeBuffer;

    STYL_DEBUG("timeout_ms is: %d", timeout_ms);
    STYL_DEBUG("min_timeout_ms is: %d", min_timeout_ms);

    if(timeout_ms > min_timeout_ms)
    {
        tv.tv_sec  = (guint)(timeout_ms / 1000);
        tv.tv_usec = (guint)((timeout_ms % 1000) * 1000);
    }
    else
    {
        STYL_WARNING("Timeout value is invalid. Using timeout value default.");
        tv.tv_sec  = (guint)(min_timeout_ms / 1000);
        tv.tv_usec = (guint)((min_timeout_ms % 1000) * 1000);
    }


    gint nbytes = 0;
    while (nbytes < sizeBuffer)
    {
        /*See if there is data available. */
        STYL_DEBUG("Timeout is: %d", tv.tv_sec);
        STYL_DEBUG("Timeout u is: %d", tv.tv_usec);
        gint rc = select (pFile + 1, &fds, NULL, NULL, &tv);
        /* TODO: Recalculate timeout here! */
        if (rc < 0)
        {
            /* ********* Error during select call **************** */
            STYL_ERROR("select: %d - %s", errno, strerror(errno));
            return 0;
        }
        else if (rc == 0)
        {
            /* ********* Timeout ********************************* */
            STYL_DEBUG("Timeout over");
            break;
        }

        /* ********* Read the available data. ******************** */
        gint n = read (pFile, buffer + nbytes, sizeBuffer - nbytes);
        if (n < 0)
        {
            /* ********* Error during select call **************** */
            STYL_ERROR("read: %d - %s", errno, strerror(errno));
            return 0;
        }
        else if (n == 0)
            break; /* EOF reached. */

        /* Increase the number of bytes read. */
        nbytes += n;
    }
    /* ********* Return the number of bytes read. ***************** */
    return nbytes;
}

/*!
 * \brief mlsScannerSSI_SerialWrite: write data to serial port
 * \return
 * - number of readed bytes or O if no byte was readed
 */
 //unsigned int PM_UART::sendBuff(unsigned char* buff, unsigned int len, unsigned int timeout_ms)
static gint mlsScannerSSI_SerialWrite(gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms)
{
    guint   sizeSent    = 0;
    guint   n           = 0;
    struct  timeval     tv ;
    fd_set  sset;

    FD_ZERO(&sset);
    FD_SET(pFile, &sset);

    guint min_timeout_ms = TIMEOUT_BYTE_MS * sizeBuffer;

    if(timeout_ms > min_timeout_ms)
    {
        tv.tv_sec  = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
    }
    else
    {
        STYL_WARNING("Timeout value is invalid. Using timeout value default.");
        tv.tv_sec  = min_timeout_ms / 1000;
        tv.tv_usec = (min_timeout_ms % 1000) * 1000;
    }

    while (sizeSent < sizeBuffer)
    {
        switch (select(pFile + 1, NULL, &sset, NULL, &tv))
        {
        case -1:
            STYL_ERROR("select: %d - %s", errno, strerror(errno));
            return 0;
        case 0:
            STYL_ERROR("select: %d - %s", errno, strerror(errno));
            return -1;
        default:
            if (FD_ISSET(pFile, &sset) != 0)
            {
                n = write(pFile, buffer + sizeSent, sizeBuffer - sizeSent);
                if (n <= 0)
                {
                    STYL_ERROR("write: %d - %s", errno, strerror(errno));
                    return 0;
                }
            }
            else
            {
                return 0;
            }

            break;
        }

        sizeSent += n;
    }

    STYL_INFO("Serial sent %d bytes", sizeSent);

    return sizeSent;
}

/*!
 * \brief mlsScannerSSI_IsChecksumOK: check 2 last bytes for checksum
 * \return
 * - TRUE : checksum is correct
 * - FALSE: checksum is incorrect
 */
static gint mlsScannerSSI_IsChecksumOK(byte *package)
{
    uint16_t checksum = 0;

    checksum += package[PACKAGE_LEN(package) + 1];
    checksum += package[PACKAGE_LEN(package)] << 8;
    STYL_INFO("checksum receive: %x", checksum);
    STYL_INFO("checksum calculate: %x", mlsScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package)));
    return (checksum == mlsScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package)));
}

/*!
 * \brief mlsScannerSSI_IsContinue: check package's type
 * \return
 * - 0  : Is last package in multiple packages stream
 * - 1 : Is intermediate package in multiple packages stream
 */
gint mlsScannerSSI_IsContinue(byte *package)
{
    return (SSI_PARAM_STATUS_CONTINUATION & package[PKG_INDEX_STAT]);
}

/*!
 * \brief mlsScannerSSI_CorrectPackage: check package's is correct
 * \return
 * - 1 : Receive package is correct
 * - 0 : Receive package is incorrect
 */
gint mlsScannerSSI_CorrectPackage(byte *package)
{
    return !(SSI_ID_DECODER | package[PKG_INDEX_SRC]);
}


/*!
 * \brief PreparePkg: generate package from input opcode and params
 * \param
 */
static void mlsScannerSSI_PreparePackage(byte *package, byte opcode, byte *param, byte paramLen)
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
    checksum = mlsScannerSSI_CalculateChecksum(package, PACKAGE_LEN(package));
    package[PACKAGE_LEN(package)]     = checksum >> 8;
    package[PACKAGE_LEN(package) + 1] = checksum & 0xFF;
}

/*!
 * \brief mlsScannerSSI_CalculateChecksum: calculate 2's complement of package
 * \param
 * - package: pointer of package content.
 * - length: Length of package in byte.
 * \return 16 bits checksum (no 2'complement) value
 */
static uint16_t mlsScannerSSI_CalculateChecksum(byte *package, gint length)
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
 * \brief mlsScannerSSI_Read: read formatted package and response ACK from/to scanner via file descriptor
 * \return number of read bytes
 */
gint mlsScannerSSI_Read(gint pFile, byte *buffer, gint sizeBuffer, const gint timeout_ms)
{
    gint retValue = 0;
    gint sizeReceived = 0;
    gint readRequest;
    byte recvBuff[PACKAGE_LEN_MAXIMUM];
    gint lastIndex = 0;

    memset(&recvBuff, 0, PACKAGE_LEN_MAXIMUM);

    do
    {
        /* Read 1 first byte for length */
        readRequest = 1;
        retValue += mlsScannerSSI_SerialRead(pFile, &recvBuff[PKG_INDEX_LEN], readRequest, timeout_ms);
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

            sizeReceived = mlsScannerSSI_SerialRead(pFile, &recvBuff[PKG_INDEX_LEN + 1], readRequest, timeout_ms);
            STYL_INFO("");
            mlsScannerPackage_Display(recvBuff, NO_GIVEN);
            STYL_INFO("sizeReceived: %d", sizeReceived);
            if(sizeReceived != readRequest)
            {
                retValue = 0;
                goto __error;
            }
            else
            {
                retValue += sizeReceived;
                STYL_INFO("retValue: %d", retValue);

                if(!mlsScannerSSI_CorrectPackage(recvBuff))
                {
                    STYL_ERROR("Receive a invalid package");
                    retValue = 0;
                    goto __error;
                }

                if (mlsScannerSSI_IsChecksumOK(recvBuff))
                {
                    /* Send ACK for buffer received */
                    STYL_INFO("Checksum OK! Send ACK for checksum.");
                    if(mlsScannerSSI_Write(pFile, SSI_CMD_ACK, NULL, 0) != EXIT_SUCCESS)
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
                        gint sizeMemcpy = PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM;
                        memcpy(&buffer[lastIndex], recvBuff, sizeMemcpy);
                        lastIndex += sizeMemcpy;
                    }
                }
                else
                {
                    STYL_ERROR("*********** Checksum fail!");
                    retValue = 0;
                    goto __error;
                }
            }
        }
    }
    while(mlsScannerSSI_IsContinue(recvBuff));

    return retValue;

__error:
    #if 0
    STYL_INFO("Error! Send NAK to scanner");
    if(mlsScannerSSI_Write(pFile, SSI_CMD_NAK, NULL, 0) != EXIT_SUCCESS)
    {
        STYL_ERROR("Send NAK to scanner got problem.");
    }
    #else
    STYL_ERROR("Error! Send END SECTION to scanner.");
    if(mlsScannerSSI_SendCommand(pFile, SSI_CMD_SESSION_STOP) != EXIT_SUCCESS)
    {
        STYL_ERROR("Stop section request was fail.");
    }
    #endif
    return retValue;
}

/*!
 * \brief mlsScannerSSI_GetACK: get ACK raw package
 * \return number of read bytes
 */
gint mlsScannerSSI_GetACK(gint pFile, byte *buffer, gint sizeBuffer, const gint timeout_ms)
{
    gint retValue = 0;
    gint sizeReceived = 0;
    gint readRequest;
    byte recvBuff[PACKAGE_LEN_MAXIMUM];
    gint lastIndex = 0;

    memset(&recvBuff, 0, PACKAGE_LEN_MAXIMUM);

    /* Read 1 first byte for length */
    readRequest = 1;
    retValue = mlsScannerSSI_SerialRead(pFile, &recvBuff[PKG_INDEX_LEN], readRequest, timeout_ms);
    STYL_INFO("Fisrt byte size: %d", retValue);
    if(retValue <= 0)
    {
        goto __error;
    }
    else
    {
        STYL_INFO("Length is: %x", recvBuff[PKG_INDEX_LEN]);
        if(recvBuff[PKG_INDEX_LEN] >= PACKAGE_LEN_MAXIMUM)
        {
            STYL_ERROR("Receive value of length package is invalid.");
            goto __error;
        }
        /* Read rest of byte of package */
        readRequest = recvBuff[PKG_INDEX_LEN] + SSI_LEN_CHECKSUM - 1; /* 1 is byte read before */
        STYL_INFO("Rest byte is: %d", readRequest);

        sizeReceived = mlsScannerSSI_SerialRead(pFile, &recvBuff[PKG_INDEX_LEN + 1], readRequest, timeout_ms);
        STYL_INFO("");
        mlsScannerPackage_Display(recvBuff, NO_GIVEN);
        STYL_INFO("sizeReceived: %d", sizeReceived);

        if(sizeReceived != readRequest)
        {
            goto __error;
        }
        else
        {
            retValue += sizeReceived;
            STYL_INFO("Total size received: %d", retValue);

            if(!mlsScannerSSI_CorrectPackage(recvBuff))
            {
                STYL_ERROR("********** Receive a invalid package");
                goto __error;
            }

            if (mlsScannerSSI_IsChecksumOK(recvBuff))
            {
                /* Copy current buffer to finally buffer */
                memcpy(buffer, recvBuff, PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM);
            }
            else
            {
                STYL_ERROR("*********** Checksum fail!");
                goto __error;
            }
        }
    }

    STYL_INFO("ACK buffer size: %d", retValue);
    mlsScannerPackage_Display(buffer, NO_GIVEN);
    return retValue;

__error:
    retValue = 0;
    return retValue;

}

/*!
 * \brief mlsScannerSSI_Write: write formatted package and check ACK to/from scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerSSI_Write(gint pFile, byte opcode, byte *param, byte paramLen)
{
    gint retValue = EXIT_SUCCESS;
    gint sizeSend = 0;

    gint  bufferSize    = (SSI_LEN_HEADER + SSI_LEN_CHECKSUM + paramLen) * sizeof(byte);
    byte *bufferContent = malloc(bufferSize);

    /* ***************** Flush old input queue *************** */
    tcflush(pFile, TCIFLUSH);

    mlsScannerSSI_PreparePackage(bufferContent, opcode, param, paramLen);

    STYL_WARNING("bufferSize: %d", bufferSize);
    STYL_WARNING("Send data: ");
    mlsScannerPackage_Display(bufferContent, bufferSize);

    STYL_WARNING("PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM: %d", PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM);
    sizeSend = PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM;
    if(mlsScannerSSI_SerialWrite(pFile, bufferContent, sizeSend, TTY_TIMEOUT) != sizeSend)
    {
        STYL_ERROR("Send data to scanner got problem.");
        retValue = EXIT_FAILURE;
    }
    free(bufferContent);

    return retValue;
}

/*!
 * \brief mlsScannerSSI_CheckACK: receive ACK package after mlsScannerSSI_Write() and check for ACK
 * \return
 * - EXIT_SUCCESS: Success	ACK
 * - EXIT_FAILURE: Fail		Unknown cause
 */
gint mlsScannerSSI_CheckACK(gint pFile)
{
    gint retValue = EXIT_SUCCESS;
    byte recvBuff[PACKAGE_LEN_ACK_MAXIMUM];

    memset(recvBuff, 0, PACKAGE_LEN_ACK_MAXIMUM);
    STYL_INFO("Invoke mlsScannerSSI_GetACK");
    retValue = mlsScannerSSI_GetACK(pFile, recvBuff, PACKAGE_LEN_ACK_MAXIMUM, TTY_TIMEOUT);
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


/*!
 * \brief mlsScannerSSI_SendCommand: Send a command then check ACK
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerSSI_SendCommand(gint pFile, byte opCode)
{
    gint retValue = EXIT_SUCCESS;
    retValue = mlsScannerSSI_Write(pFile, opCode, NULL, 0);
    if(retValue==EXIT_SUCCESS)
        retValue = mlsScannerSSI_CheckACK(pFile);

    return retValue;
}

/**@}*/

