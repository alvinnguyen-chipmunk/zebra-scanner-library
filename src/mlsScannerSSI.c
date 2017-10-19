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
#include <unistd.h>
#include <sys/select.h>

#include "mlsScannerUtils.h"
#include "mlsScannerSSI.h"
#include "mlsScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static void     mlsScannerSSI_PreparePackage            (byte *package, byte opcode, byte *param, byte paramLen, gboolean isPermanent);
static uint16_t mlsScannerSSI_CalculateChecksum        (byte *package, gint length);
static gint     mlsScannerSSI_IsChecksumOK             (byte *package);
//static gint     mlsScannerSSI_SerialRead               (gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms);
//static gint     mlsScannerSSI_SerialWrite              (gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms);

/********** Local (static) function definition section ************************/

#if 0
/*!
 * \brief mlsScannerSSI_SerialRead: read data from serial port
 * \return
 * - number of readed bytes or O if no byte was readed
 */
static gint mlsScannerSSI_SerialRead(gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms)
{
    /*  Initialize the file descriptor set.*/
    fd_set      fds;
    FD_ZERO     (&fds);
    FD_SET      (pFile, &fds);
    gint        readBytes = 0;

    /* Initialize the timeout. */
    struct timeval tv;

    guint min_timeout_ms = TIMEOUT_BYTE_MS * sizeBuffer;

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

    while (readBytes < sizeBuffer)
    {
        /*See if there is data available. */
        STYL_INFO("\n ==== Size buffer will be read %d", sizeBuffer);
        STYL_INFO("Read Serial: Timeout sec  is: %d", tv.tv_sec);
        STYL_INFO("Read Serial: Timeout usec is: %d", tv.tv_usec);

        STYL_INFO("Scanner file descriptor: %d", pFile);

        gint rc = select (pFile + 1, &fds, NULL, NULL, &tv);
        /* TODO: Recalculate timeout here! */
        if (rc == -1)
        {
            /* ********* Error during select call **************** */
            STYL_ERROR("select: %d - %s", errno, strerror(errno));
            break;
        }
        else if (rc == 0)
        {
            /* ********* Timeout ********************************* */
            STYL_WARNING("Read serial get timeout over.");
            break;
        }
        else
        {
            /* ********* Read the available data. ******************** */
            if (FD_ISSET(pFile, &fds))
            {
                gint n = read (pFile, buffer + readBytes, sizeBuffer - readBytes);
                STYL_INFO("Now, number of byte received is: %d", n);
                if (n == -1)
                {
                    /* ********* Error during select call **************** */
                    STYL_ERROR("read: %d - %s", errno, strerror(errno));
                    return 0;
                }
                else if (n == 0)
                {
                    break; /* EOF reached. */
                }
                else
                {
                    /* Increase the number of bytes read. */
                    readBytes += n;
                }
            }

        }
    }
    /* ********* Return the number of bytes read. ***************** */
    STYL_INFO("Total byte received is: %d", readBytes);
    return readBytes;
}

/*!
 * \brief mlsScannerSSI_SerialWrite: write data to serial port
 * \return
 * - number of readed bytes or O if no byte was readed
 */
static gint mlsScannerSSI_SerialWrite(gint pFile, byte* buffer, guint sizeBuffer, guint timeout_ms)
{
    guint   sizeSent    = 0;
    guint   n           = 0;
    struct  timeval     tv ;
    fd_set  fds;

    FD_ZERO(&fds);
    FD_SET(pFile, &fds);

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
        gint rc = select(pFile + 1, NULL, &fds, NULL, &tv);
        if(rc == -1)
        {
            STYL_ERROR("select: %d - %s", errno, strerror(errno));
            break;
        }
        else if (rc == 0)
        {
            STYL_WARNING("Write serial get timeout over.");
            break;
        }
        else
        {
            if (FD_ISSET(pFile, &fds))
            {
                n = write(pFile, buffer + sizeSent, sizeBuffer - sizeSent);
                if(n==0)
                {
                    break; /* Send all bytes done*/
                }
                else if(n==-1)
                {
                    STYL_ERROR("write: %d - %s", errno, strerror(errno));
                    break;
                }
                else
                {
                    /* Increase the number of bytes read. */
                    sizeSent += n;
                }

            }
        }
    }

    STYL_INFO("Serial sent %d bytes", sizeSent);
    return sizeSent;
}
#endif // 0


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
 * \brief mlsScannerSSI_PreparePackage: generate package from input opcode and parameters
 * \param
 */
static void mlsScannerSSI_PreparePackage(byte *package, byte opcode, byte *param, byte paramLen, gboolean isPermanent)
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

    if(isPermanent==TRUE)
    {
        package[PKG_INDEX_STAT]     = SSI_PARAM_TYPE_PERMANENT;
    }
    else
    {
        package[PKG_INDEX_STAT]     = SSI_PARAM_TYPE_TEMPORARY;
    }

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
 * \brief mlsScannerSSI_Read: Only read raw data form serial port.
 * \return number of read bytes
 */
gint mlsScannerSSI_Read(gint pFile, byte *buffer, gint sizeBuffer, const gchar deciTimeout, gboolean sendACK)
{
    byte recvBuff[PACKAGE_LEN_MAXIMUM];
    gint lastIndex = 0;

    gint totalReceived = 0;
    gint sizeReceived  = 0;
    gint sizeRequest   = 0;

    gint isError      = FALSE;

    struct termios serial_opt;

    // Backup old value
    tcgetattr(pFile, &serial_opt);

    do
    {
        memset(&recvBuff, 0, PACKAGE_LEN_MAXIMUM);
        /* Read 1 first byte for length */
        sizeRequest = 1;
        /* Setup timeout read 1 byte for length of package */
        serial_opt.c_cc[VTIME] = deciTimeout;
        serial_opt.c_cc[VMIN] = 0;
        STYL_INFO("Set value of VTIME is: %d", serial_opt.c_cc[VTIME]);
        STYL_INFO("Set value of VMIN is : %d", serial_opt.c_cc[VMIN]);
        tcsetattr(pFile, TCSANOW, &serial_opt);

        /* Get again for debug */
        tcgetattr(pFile, &serial_opt);
        STYL_INFO("Current value of VTIME is: %d", serial_opt.c_cc[VTIME]);
        STYL_INFO("Current value of VMIN is : %d", serial_opt.c_cc[VMIN]);

        /* Read */
        sizeReceived = (int) read(pFile, &recvBuff[PKG_INDEX_LEN], sizeRequest);
        STYL_INFO("Fisrt byte size: %d", sizeReceived);

        if(sizeReceived == sizeRequest)
        {
            STYL_INFO("Size of package is: 0x%x  (hex) ; %d  (dec)", recvBuff[PKG_INDEX_LEN], recvBuff[PKG_INDEX_LEN]);
            mlsScannerPackage_Dump(recvBuff, sizeReceived, TRUE);
            if(recvBuff[PKG_INDEX_LEN] <= PACKAGE_LEN_MAXIMUM-2) /* length maximum is 255 */
            {
                /* Re-calculate for size for rest of package */
                sizeRequest = recvBuff[PKG_INDEX_LEN] + SSI_LEN_CHECKSUM - 1; /* 1 is byte read before */
                STYL_INFO("Rest byte is: %d", sizeRequest);

                /* Setup timeout enough to read all of rest bytes */
                serial_opt.c_cc[VTIME] = 0;
                serial_opt.c_cc[VMIN]  = sizeRequest;
                STYL_INFO("Set value of VTIME is: %d", serial_opt.c_cc[VTIME]);
                STYL_INFO("Set value of VMIN is : %d", serial_opt.c_cc[VMIN]);
                tcsetattr(pFile, TCSANOW, &serial_opt);

                /* Get again for debug */
                tcgetattr(pFile, &serial_opt);
                STYL_INFO("Current value of VTIME is: %d", serial_opt.c_cc[VTIME]);
                STYL_INFO("Current value of VMIN is : %d", serial_opt.c_cc[VMIN]);

                sizeReceived = (int) read(pFile, &recvBuff[PKG_INDEX_LEN + 1], sizeRequest);
                STYL_INFO("Received byte is: %d", sizeReceived);
                mlsScannerPackage_Dump(recvBuff, sizeReceived, TRUE);

                if(sizeReceived == sizeRequest)
                {
                    sizeReceived += 1;
                    totalReceived += sizeReceived;
                    STYL_INFO("Current total byte received is: %d", totalReceived);
                    mlsScannerPackage_Dump(recvBuff, sizeReceived, TRUE);

                    STYL_INFO("Check package is correct from decoder");
                    if(mlsScannerSSI_CorrectPackage(recvBuff))
                    {
                        STYL_INFO("Check package is correct from decoder success.");
                        STYL_INFO("Check checksum for received package");
                        if (mlsScannerSSI_IsChecksumOK(recvBuff))
                        {
                            STYL_INFO("Check checksum for received package success");
                            gboolean doneACK = TRUE;
                            if(sendACK == TRUE)
                            {
                                doneACK = FALSE;
                                /* Send ACK for buffer received */
                                STYL_INFO("Send ACK for received package");
                                if(mlsScannerSSI_Write(pFile, SSI_CMD_ACK, NULL, 0, FALSE, FALSE) == EXIT_SUCCESS)
                                {
                                    STYL_INFO("Send ACK for received package success");
                                    doneACK = TRUE;
                                }
                            }
                            if(doneACK==TRUE)
                            {
                                if(sizeBuffer > (lastIndex + PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM))
                                {
                                    /* Copy current buffer to finally buffer */
                                    gint sizeMemcpy = PACKAGE_LEN(recvBuff) + SSI_LEN_CHECKSUM;
                                    memcpy(&buffer[lastIndex], recvBuff, sizeMemcpy);
                                    lastIndex += sizeMemcpy;
                                }
                                else
                                {
                                    STYL_ERROR("Length of all package of data is large.");
                                    isError=TRUE;
                                    break;
                                }
                            }
                            else
                            {
                                STYL_INFO("Send ACK for received package fail");
                                isError=TRUE;
                                break;
                            }
                        }
                        else
                        {
                            STYL_ERROR("Check checksum for received package fail");
                            isError=TRUE;
                            break;
                        }
                    }
                    else
                    {
                        STYL_ERROR("Check package is correct from decoder fail.");
                        isError=TRUE;
                        break;
                    }
                }
                else
                {
                    STYL_ERROR("Package invalid. Only read %d bytes of %d bytes", sizeReceived, sizeRequest);
                    isError=TRUE;
                    break;
                }
            }
            else
            {
                STYL_ERROR("Length of package over 255 bytes.");
                isError=TRUE;
                break;
            }
        }
        else
        {
            STYL_ERROR("Read first byte of package: TIMEOUT");
            isError=TRUE;
            break;
        }
    }
    while(mlsScannerSSI_IsContinue(recvBuff));

    if(isError==TRUE)
        return 0;

    STYL_INFO("Total length of received data is: %d", totalReceived);
    mlsScannerPackage_Dump(buffer, totalReceived, TRUE);
    return totalReceived;
}

/*!
 * \brief mlsScannerSSI_Write: write formatted package and check ACK to/from scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerSSI_Write(gint pFile, byte opcode, byte *param, byte paramLen, gboolean sendWakeup, gboolean isPermanent)
{
    gint retValue = EXIT_SUCCESS;
    gint sizeSend = 0;
    gint  bufferSize      = 0;
    byte *bufferContent   = NULL;

    byte  bufferWakeup[]  = { 0x00, 0x00 };

    /* ***************** Flush old input queue ***************************** */
    tcflush(pFile, TCIFLUSH);

    /* ***************** If the scanner is in Low Power mode, this will wake it up. *************** */
    /* ***************** Else the NULLs are ignored. *************** */
    if(sendWakeup==TRUE)
    {
        mlsScannerPackage_Dump(bufferWakeup, SSI_LEN_WAKEUP, FALSE);
        #if 1
        if ( write(pFile, bufferWakeup, SSI_LEN_WAKEUP) == -1)
        {
            STYL_ERROR("Send wakeup data to scanner got problem.");
            STYL_ERROR("write: %d - %s", errno, strerror(errno));
            retValue = EXIT_FAILURE;
        }
        #else
        if(mlsScannerSSI_SerialWrite(pFile, bufferWakeup, SSI_LEN_WAKEUP, WRITE_TTY_TIMEOUT) != SSI_LEN_WAKEUP)
        {
            STYL_ERROR("Send wakeup data to scanner got problem.");
            return EXIT_FAILURE;
        }
        #endif // 1
        /* ***************** Sleep waiting for scanner wakeup done ************* */
        usleep(50000);
    }

    /* ***************** Setup then send parameter to scanner *************** */
    bufferSize = (SSI_LEN_HEADER + SSI_LEN_CHECKSUM + paramLen) * sizeof(byte);
    bufferContent = malloc(bufferSize);
    mlsScannerSSI_PreparePackage(bufferContent, opcode, param, paramLen, isPermanent);
    mlsScannerPackage_Dump(bufferContent, bufferSize, FALSE);
    sizeSend = PACKAGE_LEN(bufferContent) + SSI_LEN_CHECKSUM;
    #if 1
    if ( write(pFile, bufferContent, sizeSend) == -1)
    {
        STYL_ERROR("Send data to scanner got problem.");
        STYL_ERROR("write: %d - %s", errno, strerror(errno));
        retValue = EXIT_FAILURE;
    }

    #else
    if(mlsScannerSSI_SerialWrite(pFile, bufferContent, sizeSend, WRITE_TTY_TIMEOUT) != sizeSend)
    {
        STYL_ERROR("Send data to scanner got problem.");
        retValue = EXIT_FAILURE;
    }
    #endif // 1
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
    retValue = mlsScannerSSI_Read(pFile, recvBuff, PACKAGE_LEN_ACK_MAXIMUM, ACK_TIMEOUT, FALSE);
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
    gint tryCount = 3;

    while(tryCount > 0)
    {
        tryCount--;
        retValue = mlsScannerSSI_Write(pFile, opCode, NULL, 0, TRUE, FALSE);
        if(retValue==EXIT_SUCCESS)
        {
            retValue = mlsScannerSSI_CheckACK(pFile);
            if(retValue == EXIT_SUCCESS)
                break;
        }
        sleep(1);
    }

    return retValue;
}

/*@}*/

