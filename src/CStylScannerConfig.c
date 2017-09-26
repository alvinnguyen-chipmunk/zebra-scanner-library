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
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include "CStylScannerUtils.h"
#include "CStylScannerConfig.h"
#include "CStylScannerSSI.h"
#include "CStylScannerPackage.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
static gint StylScannerLockfile_FD  = -1;

/********** Local Macro definition section ************************************/
#define TIMEOUT_MSEC		        50
#define LOCK_SCANNER_PATH           "/tmp/lock_scanner"

/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static gint         StylScannerConfig_LockDevice            (gint pFile, gboolean isLock);

/********** Local (static) function definition section ************************/
/*!
 * \brief StylScannerConfig_LockDevice: Lock or unlock device
 * \return
 * - EXIT_SUCCESS: if executing success
 * - EXIT_FAILURE: if executing fail
 */
static gint StylScannerConfig_LockDevice (gint pFile, gboolean isLock)
{
    if(TRUE==isLock)
    {
        if(flock(pFile, LOCK_EX | LOCK_NB) < 0)
        {
            STYL_ERROR("flock: %d - %s", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    else
    {
        if(flock(pFile, LOCK_UN | LOCK_NB) < 0)
        {
            STYL_ERROR("flock: %d - %s", errno, strerror(errno));
            return EXIT_FAILURE;
        }
        unlink(LOCK_SCANNER_PATH);
        return EXIT_SUCCESS;
    }
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
    StylScannerLockfile_FD = open(LOCK_SCANNER_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if(StylScannerLockfile_FD < 0)
    {
        STYL_ERROR("Lock file: open: %d - %s",errno, strerror(errno));
    }
    if(StylScannerConfig_LockDevice(StylScannerLockfile_FD, TRUE) !=  EXIT_SUCCESS)
    {
        STYL_ERROR("Device %s is busy.\n", deviceNode);
    }
    else
    {
        pFile = open(deviceNode, O_RDWR);
        if (pFile <= 0)
        {
            STYL_ERROR("Open Scanner device %s: open: %d - %s\n", deviceNode, errno, strerror(errno));
        }
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
        /* Enable device to scanning */
    if(StylScannerConfig_Disable(pFile) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not disable scanner device");
    }

    if(StylScannerConfig_Flush(pFile) != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
    }

    if (close(pFile) < 0)
    {
        STYL_ERROR("close: %d - %s", errno, strerror(errno));
    }

    return StylScannerConfig_LockDevice(StylScannerLockfile_FD, FALSE);
}

/*!
 * \brief StylScannerConfig_ConfigTTY: Do configure for SSI port type.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigTTY(gint pFile)
{
    gint    ret             = EXIT_SUCCESS;
    gint    flags           = 0;
    struct  termios         devConf;

    /* ********** Set flags for blocking mode and sync for writing ******** */
    flags = fcntl(pFile, F_GETFL);
    if (0 > flags)
    {
        STYL_ERROR("fcntl for 'F_GETFL': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto __exit;
    }
    flags |= (O_FSYNC);
    flags &= ~(O_NDELAY | O_ASYNC);

    if (fcntl(pFile, F_SETFL, flags))
    {
        STYL_ERROR("fcntl for 'F_SETFL': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto __exit;
    }

    /* ********** Configure TTY device ************************************ */
    devConf.c_cflag = (CS8 | CLOCAL | CREAD);
    devConf.c_iflag = 0;
    devConf.c_oflag = 0;
    devConf.c_lflag = 0;
    devConf.c_cc[VMIN] = 0;
    devConf.c_cc[VTIME] = TIMEOUT_MSEC;	// read non-blocking flush dump data. See blocking read timeout later

    /* ********** Portability ************************************ */
    /* Use cfsetspeed instead of CBAUD since c_cflag/CBAUD is not in POSIX */
    if (cfsetspeed(&devConf, BAUDRATE))
    {
        STYL_ERROR("cfsetspeed set 'BAUDRATE': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto __exit;
    }

    /* ********** ... ************************************ */
    if (tcsetattr(pFile, TCSANOW, &devConf))
    {
        STYL_ERROR("tcsetattr 'TCSANOW': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto __exit;
    }

__exit:
    return ret;
}

/*!
 * \brief StylScannerConfig_ConfigSSI: Send parameters to configure scanner as SSI interface.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigSSI(gint pFile)
{
    /*
        #define SSI_PARAM_TYPE_PARAM_PREFIX			0xFF
        #define SSI_PARAM_TYPE_TEMPORARY	    	0x00
        #define SSI_PARAM_TYPE_PERMANENT 			0x08

        #define SSI_PARAM_INDEX_TRIGGER             0x8A
        #define SSI_PARAM_DEF_FORMAT_B              0xEE
        #define SSI_PARAM_B_DEF_SW_ACK              0x9F
        #define SSI_PARAM_B_DEF_SCAN                0xEC

        #define SSI_PARAM_VALUE_TRIGGER_PRESENT     0x07
        #define SSI_PARAM_VALUE_ENABLE              0x01
        #define SSI_PARAM_VALUE_DISABLE             0x00
    */
    gint retValue = EXIT_SUCCESS;
    byte paramContent[9] = {  SSI_PARAM_TYPE_PARAM_PREFIX
                      ,SSI_PARAM_DEF_FORMAT_B,      SSI_PARAM_VALUE_ENABLE
                      ,SSI_PARAM_B_DEF_SW_ACK,      SSI_PARAM_VALUE_ENABLE
                      ,SSI_PARAM_B_DEF_SCAN,        SSI_PARAM_VALUE_ENABLE
                      ,SSI_PARAM_INDEX_TRIGGER,     SSI_PARAM_VALUE_TRIGGER_PRESENT
                     };
    gint paramSize = sizeof(paramContent) / sizeof(*paramContent);

    STYL_INFO("");
    StylScannerPackage_Display(paramContent, paramSize);

    retValue = StylScannerSSI_Write(pFile, SSI_CMD_PARAM, paramContent, paramSize);

    if(retValue==EXIT_SUCCESS)
        retValue = StylScannerSSI_CheckACK(pFile);

    return retValue;
}

/*!
 * \brief StylScannerConfig_Flush: Flush buffer of scanner
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_Flush(gint pFile)
{
    gint retValue = EXIT_SUCCESS;
    retValue = StylScannerSSI_Write(pFile, SSI_CMD_FLUSH_QUEUE, NULL, 0);
    if(retValue==EXIT_SUCCESS)
        retValue = StylScannerSSI_CheckACK(pFile);

    return retValue;
}

/*!
 * \brief StylScannerConfig__Enable: Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_Enable(gint pFile)
{
    int retValue = EXIT_SUCCESS;
    retValue = StylScannerSSI_Write(pFile, SSI_CMD_SCAN_ENABLE, NULL, 0);
    if(retValue==EXIT_SUCCESS)
        retValue = StylScannerSSI_CheckACK(pFile);

    return retValue;
}

/*!
 * \brief StylScannerConfig__Disable: Disable reader, Reader can't scan any QR code/bar code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_Disable(gint pFile)
{
    int retValue = EXIT_SUCCESS;
    retValue = StylScannerSSI_Write(pFile, SSI_CMD_SCAN_DISABLE, NULL, 0);
    if(retValue==EXIT_SUCCESS)
        retValue = StylScannerSSI_CheckACK(pFile);

    return retValue;
}
/**@}*/
