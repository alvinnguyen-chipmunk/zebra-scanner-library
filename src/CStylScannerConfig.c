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
 * @file    CStylScannerConfig.c
 * @brief   C code - Configure for TTY port and SSI protocol.
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "CStylScannerUtils.h"
#include "CStylScannerConfig.h"
#include "CStylScannerSSI.h"
#include "CStylScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
static gint StylScannerLockfile_FD  = -1;

/********** Local Macro definition section ************************************/
#define TIMEOUT_MSEC		        50
#define LOCK_SCANNER_PATH           "/var/log/styl_scanner"

/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static gint         StylScannerConfig_LockDevice            (gboolean isLock);

/********** Local (static) function definition section ************************/

/*!
 * \brief StylScannerConfig_LockDevice: Lock or unlock device
 * \return
 * - EXIT_SUCCESS: if executing success
 * - EXIT_FAILURE: if executing fail
 */
static gint StylScannerConfig_LockDevice (gboolean isLock)
{
    if(TRUE==isLock)
    {
        gint pLockFile = -1;
        gchar *buffer =  NULL;
        gboolean getError = FALSE;
        if (g_file_test (LOCK_SCANNER_PATH, G_FILE_TEST_EXISTS))
        {
            pLockFile = g_open (LOCK_SCANNER_PATH, O_RDWR);
            if(pLockFile==-1)
            {
                STYL_ERROR("g_open: %d - %s", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            buffer = g_malloc0(32);
            if(read(pLockFile, buffer, 32)<=0)
            {
                STYL_ERROR("read: %d - %s", errno, strerror(errno));
                getError = TRUE;
            }
            STYL_ERROR("PID OLD IS: %s", buffer);
            guint oldPID = (guint)g_ascii_strtoull(buffer, NULL, 10);
            if(kill((pid_t)oldPID, 0)==0)
            {
                STYL_ERROR("Scanner device port was used by process %d", oldPID);
                getError = TRUE;
            }
            getError = TRUE;
        }
        else
        {
            pLockFile = g_open (LOCK_SCANNER_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
            if(pLockFile==-1)
            {
                STYL_ERROR("g_open: %d - %s", errno, strerror(errno));
                return EXIT_FAILURE;
            }
            buffer = g_strdup_printf("%d", getpid());
            if(write(pLockFile, buffer, (guint)strlen(buffer))<=0)
            {
                STYL_ERROR("write: %d - %s", errno, strerror(errno));
                getError = TRUE;
            }
        }
        g_free(buffer);
        g_close(pLockFile, NULL);

        if(getError==TRUE)
            return EXIT_FAILURE;
    }
    else
    {
        g_unlink(LOCK_SCANNER_PATH);
    }
    return EXIT_SUCCESS;
}

/********** Global function definition section ********************************/

/*!
 * \brief StylScannerConfig_OpenTTY: Open TTY port of device
 * \return
 * - EXIT_SUCCESS: if executing success
 * - EXIT_SUCCESS: if executing success
 */
gint StylScannerConfig_OpenTTY(gchar *deviceNode)
{
    gint pFile = -1;

    pFile = open(deviceNode, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
    if (pFile <= 0)
    {
        STYL_ERROR("Open Scanner device %s: open: %d - %s\n", deviceNode, errno, strerror(errno));
    }
    else if(StylScannerConfig_LockDevice(TRUE) !=  EXIT_SUCCESS)
    {
        if (close(pFile) < 0)
        {
            STYL_ERROR("close: %d - %s", errno, strerror(errno));
        }
        STYL_ERROR("Device %s is busy.\n", deviceNode);
        pFile = -1;
    }
    return pFile;
}

/*!
 * \brief StylScannerConfig_CloseTTY: Close TTY port of device.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_CloseTTY(gint pFile)
{
    /* Disable device for scanning */
    if(StylScannerSSI_SendCommand(pFile, SSI_CMD_SCAN_DISABLE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not disable scanner device");
    }

    if(StylScannerSSI_SendCommand(pFile, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
    }

    if(StylScannerConfig_LockDevice(FALSE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Unlock for device fail.");
    }

    if (close(pFile) < 0)
    {
        STYL_ERROR("close: %d - %s", errno, strerror(errno));
    }

    return EXIT_SUCCESS;
}

/*!
 * \brief StylScannerConfig_ConfigTTY: Do configure for SSI port.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigTTY(gint pFile)
{
    speed_t br_speed = BAUDRATE;
    gint mcs = 0;
    /* ********** Check serial port is valid ********** */
    STYL_INFO("serialPort: %d\n", pFile);
    if(pFile <= 0)
        return EXIT_FAILURE;
    /* ********** Check serial port is valid ********** */
    struct termios serial_opt;
    /* ********** Set file descriptor ***************** */
    tcgetattr(pFile, &serial_opt);
    /* ********** Set baudrate ************************ */
    cfsetispeed(&serial_opt, br_speed);

    /* ********** Set parity: yes ********************* */
    serial_opt.c_cflag |= PARENB;
    /* ********** Set parity value: no **************** */
    serial_opt.c_cflag &= ~PARENB;
    /* ********** Set parity value: odd **************** */
    //serial_opt.c_cflag |= PARODD;
    /* ********** Set parity value: even *************** */
    //serial_opt.c_cflag &= ~PARODD;
    /* ********** Set stop bit is: 1 ******************* */
    serial_opt.c_cflag &= ~CSTOPB;
    /* ********** Set stop bit is: 2 ******************* */
    //serial_opt.c_cflag |= CSTOPB;

    /** ********************************************** **/
    /* *********** Reset data bits ********************* */
    serial_opt.c_cflag &= ~CSIZE;
    /* *********** Disable hardware flow control ******* */
    serial_opt.c_cflag &=~CRTSCTS;
    /* *********** Set data bits is: 8 ***************** */
    serial_opt.c_cflag |= CS8;
    serial_opt.c_cflag |= (CLOCAL | CREAD);

    serial_opt.c_iflag &= ~(BRKINT|PARMRK|IGNPAR|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IXOFF|IXANY|INPCK);
    serial_opt.c_lflag=0;
    serial_opt.c_oflag=0;

    serial_opt.c_cc[VTIME] = 1;
    serial_opt.c_cc[VMIN] = TTY_BUFF_MAXSIZE;

    /* ********** Configure for file descriptor ******** */
    ioctl(pFile, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(pFile, TIOCMSET, &mcs);

    /* ********** Flush file descriptor ******** */
    tcflush(pFile, TCIFLUSH);
    if (tcsetattr(pFile, TCSANOW, &serial_opt)==-1)
    {
        close(pFile);
        return EXIT_FAILURE;
    }
    else
    {
        fcntl(pFile, F_SETFL, O_NONBLOCK);
        if(pFile > 0)
        {
            STYL_INFO("Setup for TTY port was success.");
            return EXIT_SUCCESS;
        }
        else
            return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}

/*!
 * \brief StylScannerConfig_ConfigSSI: Send parameters to configure scanner as SSI interface.
 * \param
 * - File descriptor of scanner device
 * - triggerMode: SCANNING_TRIGGER_AUTO or SCANNING_TRIGGER_MANUAL
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigSSI(gint pFile, byte triggerMode)
{
    /*
        #define SSI_PARAM_TYPE_PARAM_PREFIX			0xFF
        #define SSI_PARAM_TYPE_TEMPORARY	    	0x00
        #define SSI_PARAM_TYPE_PERMANENT 			0x08

        #define SSI_PARAM_INDEX_TRIGGER             0x8A
        #define SSI_PARAM_DEF_FORMAT_B              0xEE
        #define SSI_PARAM_B_DEF_SW_ACK              0x9F
        #define SSI_PARAM_B_DEF_SCAN                0xEC

        #define SSI_PARAM_INDEX_EVENT               0xF0
        #define SSI_PARAM_INDEX_EVENT_DECODE        0x00

        #define SSI_PARAM_VALUE_TRIGGER_PRESENT     0x07
        #define SSI_PARAM_VALUE_ENABLE              0x01
        #define SSI_PARAM_VALUE_DISABLE             0x00
    */
    gint retValue = EXIT_SUCCESS;
    byte paramContent[12] = {  SSI_PARAM_TYPE_PARAM_PREFIX
                                 ,SSI_PARAM_DEF_FORMAT_B,      SSI_PARAM_VALUE_ENABLE
                                 ,SSI_PARAM_B_DEF_SW_ACK,      SSI_PARAM_VALUE_ENABLE
                                 ,SSI_PARAM_B_DEF_SCAN,        SSI_PARAM_VALUE_ENABLE
                           };
    if (triggerMode == SCANNING_TRIGGER_MANUAL)
    {
        paramContent[7] = SSI_PARAM_INDEX_TRIGGER;
        paramContent[8] = SSI_PARAM_VALUE_TRIGGER_HOST;
    }
    else
    {
        paramContent[7] = SSI_PARAM_INDEX_TRIGGER;
        paramContent[8] = SSI_PARAM_VALUE_TRIGGER_PRESENT;
    }

    paramContent[9]  = SSI_PARAM_INDEX_EVENT;
    paramContent[10] = SSI_PARAM_INDEX_EVENT_DECODE;
    paramContent[11] = SSI_PARAM_VALUE_DISABLE;

    gint paramSize = sizeof(paramContent) / sizeof(*paramContent);

    STYL_INFO("");
    StylScannerPackage_Display(paramContent, paramSize);

    retValue = StylScannerSSI_Write(pFile, SSI_CMD_PARAM, paramContent, paramSize);

    if(retValue==EXIT_SUCCESS)
        retValue = StylScannerSSI_CheckACK(pFile);

    return retValue;
}

/**@}*/
