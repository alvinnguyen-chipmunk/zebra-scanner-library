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
 * @file    mlsScannerConfig.c
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

#include "mlsScannerUtils.h"
#include "mlsScannerConfig.h"
#include "mlsScannerSSI.h"
#include "mlsScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
#define TIMEOUT_MSEC		        50
#define LOCK_SCANNER_PATH           "/var/log/styl_scanner"
#define CONFIG_SCANNER_PATH         "/etc/stylssi/stylssi.conf"

/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static gint         mlsScannerConfig_LockDevice            (gboolean isLock);

/********** Local (static) function definition section ************************/

/*!
 * \brief mlsScannerConfig_LockDevice: Lock or unlock device
 * \return
 * - EXIT_SUCCESS: if executing success
 * - EXIT_FAILURE: if executing fail
 */
static gint mlsScannerConfig_LockDevice (gboolean isLock)
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
            guint oldPID = (guint)g_ascii_strtoull(buffer, NULL, 10);
            if(kill((pid_t)oldPID, 0)==0)
            {
                STYL_ERROR("Scanner device port was used by process %d", oldPID);
                getError = TRUE;
            }
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
 * \brief mlsScannerConfig_OpenTTY: Open TTY port of device
 * \return
 * - EXIT_SUCCESS: if executing success
 * - EXIT_SUCCESS: if executing success
 */
gint mlsScannerConfig_OpenTTY(const gchar *deviceNode)
{
    gint pFile = -1;

    pFile = open(deviceNode, O_RDWR | O_NOCTTY);
    if (pFile == -1)
    {
        STYL_ERROR("Open Scanner device %s: open: %d - %s\n", deviceNode, errno, strerror(errno));
    }
    else if(mlsScannerConfig_LockDevice(TRUE) !=  EXIT_SUCCESS)
    {
        if (close(pFile) == -1)
        {
            STYL_ERROR("close: %d - %s", errno, strerror(errno));
        }
        STYL_ERROR("Device %s is busy.\n", deviceNode);
        pFile = -1;
    }
    return pFile;
}

/*!
 * \brief mlsScannerConfig_CloseTTY: Close TTY port of device.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerConfig_CloseTTY(gint pFile)
{
    /* Disable device for scanning */
    if(mlsScannerSSI_SendCommand(pFile, SSI_CMD_SCAN_DISABLE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not disable scanner device");
    }

    if(mlsScannerSSI_SendCommand(pFile, SSI_CMD_FLUSH_QUEUE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
    }

    if(mlsScannerConfig_LockDevice(FALSE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Unlock for device fail.");
    }

    if (close(pFile) == -1)
    {
        STYL_ERROR("close: %d - %s", errno, strerror(errno));
    }

    return EXIT_SUCCESS;
}

/*!
 * \brief mlsScannerConfig_CloseTTY_Only: Close TTY port of device.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerConfig_CloseTTY_Only(gint pFile)
{
    if(mlsScannerConfig_LockDevice(FALSE) != EXIT_SUCCESS)
    {
        STYL_ERROR("Unlock for scanner device port fail.");
    }

    if (close(pFile) == -1)
    {
        STYL_ERROR("close: %d - %s", errno, strerror(errno));
    }

    return EXIT_SUCCESS;
}

/*!
 * \brief mlsScannerConfig_ConfigTTY: Do configure for SSI port.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerConfig_ConfigTTY(gint pFile)
{
    // fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    /*
    struct termios {
            tcflag_t c_iflag;       // input mode flags   //
            tcflag_t c_oflag;       // output mode flags  //
            tcflag_t c_cflag;       // control mode flags //
            tcflag_t c_lflag;       // local mode flags   //
            cc_t c_line;            // line discipline    //
            cc_t c_cc[NCCS];        // control characters //
        };
    */
    gint retValue = EXIT_FAILURE;
    struct termios serial_opt;
    speed_t br_speed = BAUDRATE;

    STYL_INFO("serialPort: %d\n", pFile);

     /* ********** Clear struct for new port settings **************** */
    bzero(&serial_opt, sizeof(serial_opt));
#if 1
    /* ********** IGNPAR  : ignore bytes with parity errors ********** */
    serial_opt.c_iflag = IGNPAR;
#else
    serial_opt.c_iflag &= ~(BRKINT|PARMRK|IGNPAR|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IXOFF|IXANY|INPCK);
#endif
    /* Raw output */
    serial_opt.c_oflag = 0;

#if 1
    /*
      BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
      CRTSCTS : output hardware flow control (only used if the cable has
                all necessary lines. See sect. 7 of Serial-HOWTO)
      CS8     : 8n1 (8bit,no parity,1 stopbit)
      CLOCAL  : local connection, no modem contol
      CREAD   : enable receiving characters
    */
    serial_opt.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
#else
    /* ********** Set baudrate ************************ */
    cfsetispeed(&serial_opt, br_speed);
    /* ********** Set parity: yes ********************* */
    serial_opt.c_cflag |= PARENB;
    /* ********** Set parity value: no **************** */
    serial_opt.c_cflag &= ~PARENB;
    /* ********** Set parity value: odd *************** */
    //serial_opt.c_cflag |= PARODD;
    /* ********** Set parity value: even ************** */
    //serial_opt.c_cflag &= ~PARODD;
    /* ********** Set stop bit is: 1 ****************** */
    serial_opt.c_cflag &= ~CSTOPB;
    /* ********** Set stop bit is: 2 ****************** */
    //serial_opt.c_cflag |= CSTOPB;
#endif

#if 1
    /* ********** Set input mode (non-canonical, no echo,...) ********** */
    serial_opt.c_lflag = 0;
#else
    /* ICANON  : enable canonical input disable all echo functionality, and don't send signals to calling program */
    serial_opt.c_lflag = ICANON;
#endif

    serial_opt.c_cc[VTIME]    = 0;      /* inter-character timer unused */
    serial_opt.c_cc[VMIN]     = 0;      /* read will be satisfied immediately. */
                                        /* The number of characters currently available,
                                        /* or the number of characters requested will be returned */

    /* ********** Flush file descriptor ******** */
    tcflush(pFile, TCIFLUSH);

    /* ********** Set configure attribute for file descriptor ******** */
    if(tcsetattr(pFile, TCSANOW, &serial_opt)==0)
    {
        retValue = EXIT_SUCCESS;
    }
    return retValue;
}

/*!
 * \brief mlsScannerConfig_ConfigSSI: Send parameters to configure scanner as SSI interface.
 * \param
 * - File descriptor of scanner device
 * - triggerMode: SCANNING_TRIGGER_AUTO or SCANNING_TRIGGER_MANUAL
 * - isPermanent: Write SSI parameter to flash of decoder.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint mlsScannerConfig_ConfigSSI(gint pFile, byte triggerMode, gboolean isPermanent)
{
    gint retValue = EXIT_FAILURE;

    byte paramContent[12] = {     SSI_PARAM_VALUE_BEEP         // Beep Code - 0xFF is NO BEEP
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

    //mlsScannerPackage_Dump(paramContent, paramSize, FALSE);

    if (mlsScannerSSI_Write(pFile, SSI_CMD_PARAM, paramContent, paramSize, TRUE, isPermanent)==EXIT_SUCCESS)
    {
        if(mlsScannerSSI_CheckACK(pFile)==EXIT_SUCCESS)
        {
            retValue = EXIT_SUCCESS;

            gint pConfigFile = -1;
            gchar *buffer = NULL;
            /* Write configure number to file. */
            pConfigFile = g_open (CONFIG_SCANNER_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
            if(pConfigFile==-1)
            {
                STYL_ERROR("Open configure file of scanner SSI library fail.");
                STYL_ERROR("g_open: %d - %s", errno, strerror(errno));
            }
            else
            {
                buffer = g_strdup_printf("%d", triggerMode);
                if(write(pConfigFile, buffer, (guint)strlen(buffer))<=0)
                {
                    STYL_ERROR("Save configure SSI fail.");
                    STYL_ERROR("write: %d - %s", errno, strerror(errno));
                }
                g_free(buffer);
                g_close(pConfigFile, NULL);
            }
        }
    }

    return retValue;
}

/*!
 * \brief mlsScannerConfig_CheckRevision: Request revision number of decoder
 * \param
 * - File descriptor of scanner device
 * \return
 * - length of revision string: success
 * - 0: failure.
 */
guint mlsScannerConfig_CheckRevision(gint pFile, gchar *buffer, gint bufferLength, gchar deciTimeout)
{
    byte recvBuff[PACKAGE_LEN_MAXIMUM];
    gint retValue = EXIT_FAILURE;
    gint revisionLength = 0;

    memset(recvBuff, 0, PACKAGE_LEN_MAXIMUM);
    retValue = mlsScannerSSI_Write(pFile, SSI_CMD_REVISION_REQUEST, NULL, 0, TRUE, FALSE);
    if(retValue==EXIT_SUCCESS)
    {
        retValue = mlsScannerSSI_Read(pFile, recvBuff, PACKAGE_LEN_MAXIMUM, deciTimeout, FALSE);
        if(retValue>0)
        {
            mlsScannerPackage_Dump(recvBuff, NO_GIVEN, TRUE);
            if(SSI_CMD_REVISION_REPLY == recvBuff[PKG_INDEX_OPCODE])
            {
                if(retValue >= bufferLength)
                {
                    STYL_ERROR("String buffer is not enough for received data.");
                    return EXIT_FAILURE;
                }
                else
                {
                    revisionLength = mlsScannerPackage_Extract(buffer, NULL, recvBuff, (const gint)bufferLength, FALSE);
                    return revisionLength;
                }
            }
        }
    }

    return 0;
}


/*!
 * \brief mlsScannerConfig_GetMode: Get current mode of scanner
 * \return
 * - 0: scanner is not setup before
 * - 1: presentation/auto-trigger mode
 * - 2: host/manual-trigger mode
 */
guint mlsScannerConfig_GetMode (void)
{
    gint    pModeFile   = -1;
    gchar   modeString  = '0';

    /*
    #define SCANNING_TRIGGER_NONE              0x00
    #define SCANNING_TRIGGER_AUTO              0x01
    #define SCANNING_TRIGGER_MANUAL            0x02
    */

    if (g_file_test (CONFIG_SCANNER_PATH, G_FILE_TEST_EXISTS))
    {
        pModeFile = g_open (CONFIG_SCANNER_PATH, O_RDWR);
        if(pModeFile==-1)
        {
            STYL_ERROR("g_open: %d - %s", errno, strerror(errno));
            return SCANNING_TRIGGER_NONE;
        }
        else
        {
            if(read(pModeFile, &modeString, 1)<=0)
            {
                STYL_ERROR("read: %d - %s", errno, strerror(errno));
            }

            STYL_INFO("Value read from configure file of scanner is: %c", modeString);

            g_close(pModeFile, NULL);

            switch(modeString)
            {
            case '0':
                return SCANNING_TRIGGER_NONE;
            case '1':
                return SCANNING_TRIGGER_AUTO;
            case '2':
                return SCANNING_TRIGGER_MANUAL;
            default:
                return SCANNING_TRIGGER_NONE;
            }
        }
    }
    return SCANNING_TRIGGER_NONE;
}

/*@}*/
