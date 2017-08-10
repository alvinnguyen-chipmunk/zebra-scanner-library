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
 * @file    mlsBarcode.c
 * @brief   C library - get data from qrcode/barcode scanner
 *
 * Long description.
 * @date    13/07/2017
 * @author  luck.hoang alvin.nguyen
 */

/********** Include section ***************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <assert.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "ssi.h"
#include "mlsBarcode.h"

/********** Local Type definition section *************************************/

typedef enum _state
{
    START,
    STOP,
    FLUSH_QUEUE,
    REPLY_ACK,
    REPLY_NAK,
    GET_BARCODE,
    WAIT_DEC_EVENT
} ssiState;

/********** Local Constant and compile switch definition section **************/

static int scanner = 0;

static int styl_scanner_lockfile_fd = -1;

/********** Local Macro definition section ************************************/

#define MSB_16(x)		    (x >> 8)
#define LSB_16(x)		    (x & UINT8_MAX)

#define TRUE				1
#define FALSE				0

#define LOCK_SCANNER_PATH	"/tmp/lock_scanner"

#define TIMEOUT_MSEC		50

#define LOCK_FAIL           0
#define LOCK_SUCCESS        1

/********** Local (static) variable declaration section ***********************/

static uint16_t     CalculateChecksum       (byte *pkg);
static void         PreparePkg              (byte *pkg, byte opcode, byte *param, byte paramLen);
static int          IsChecksumOK            (byte *pkg);
static int          IsContinue              (byte *pkg);
static char         *strNAK                 (int code);
static int          LockScanner             (int fd);
static int          UnlockScanner           (void);
static int          OpenTTY                 (const char *name);
static int          CloseTTY                ();
static int          ConfigTTY               (int fd);
static int          ConfigSSI               (int fd);
static int          WriteSSI                (int fd, byte opcode, byte *param, byte paramLen);
static int          ReadSSI                 (int fd, byte *buff, const int timeout);
static int          CheckACK                (int fd);
static void         PrintError              (int ret);
static int          ExtractBarcode          (char *buff, byte *pkg, const int buffLength);
static void         DisplayPkg              (byte *pkg);

/********** Local (static) function definition section ************************/

/*!
 * \brief strNAK generate NAK message based on code
 * \return NAK message string
 */
static char *strNAK(int code)
{
    char *msg = NULL;

    switch (code)
    {
    case NAK_RESEND:
        msg = "RESEND";
        break;

    case NAK_CANCEL:
        msg = "CANCEL";
        break;

    case NAK_DENIED:
        msg = "DENIED";
        break;

    case NAK_BAD_CONTEXT:
        msg = "NAK_BAD_CONTEXT";
        break;

    default:
        break;
    }

    return msg;
}

/*!
 * \brief CalculateChecksum calculate 2's complement of pkg
 * \return 16 bits checksum (2' complement) value
 */
static uint16_t CalculateChecksum(byte *pkg)
{
    int checksum = 0;

    for (unsigned int i = 0; i < PKG_LEN(pkg); i++)
    {
        checksum += pkg[i];
    }

    // 1's complement
    checksum ^= 0xFFFF;	// flip all 16 bits

    // 2's complement
    checksum += 1;

    return checksum;
}

/*!
 * \brief PreparePkg generate package from input opcode and params
 */
static void PreparePkg(byte *pkg, byte opcode, byte *param, byte paramLen)
{
    uint16_t checksum = 0;

    pkg[INDEX_LEN] = SSI_HEADER_LEN;
    pkg[INDEX_OPCODE] = opcode;
    pkg[INDEX_SRC] = SSI_HOST;
    pkg[INDEX_STAT] = SSI_DEFAULT_STATUS;

    if ( (NULL != param) && (0 != paramLen) )
    {
        pkg[INDEX_LEN] += paramLen;
        memcpy(&pkg[INDEX_STAT + 1], param, paramLen);
    }

    // add checksum
    checksum = CalculateChecksum(pkg);
    pkg[PKG_LEN(pkg)] = checksum >> 8;
    pkg[PKG_LEN(pkg) + 1] = checksum & 0xFF;
}

/*!
 * \brief IsChecksumOK check 2 last bytes for checksum
 * \return
 * - TRUE: checksum is correct
 * - FALSE: checksum is incorrect
 */
static int IsChecksumOK(byte *pkg)
{
    uint16_t cksum = 0;

    cksum += pkg[PKG_LEN(pkg) + 1];
    cksum += pkg[PKG_LEN(pkg)] << 8;

    return (cksum == CalculateChecksum(pkg));
}

/*!
 * \brief IsLastPackage check package's type
 * \return
 * - TRUE: Is last package in multiple packages stream
 * - FALSE: Is intermediate package in multiple packages stream
 */
static int IsContinue(byte *pkg)
{
    return (STAT_CONTINUATION & pkg[INDEX_STAT]);
}

/*!
 * \brief DisplayPkg print out package.
 * If PKG_LEN(pkg) > MAX_PKG_LEN, only first MAX_PKG_LEN bytes are displayed
 */
static void DisplayPkg(byte *pkg)
{
    if (NULL != pkg)
    {
        for (int i = 0; i < PKG_LEN(pkg) + 2; i++)
        {
            STYL_INFO_OTHER("0x%x ", pkg[i]);
        }
        STYL_INFO_OTHER("\n");
    }
}

/*!
 * \brief ConfigSSI send parameters to configure scanner
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
static int ConfigSSI(int fd)
{
    int ret = EXIT_SUCCESS;
    byte param[12] = { 0x01,
                       PARAM_B_DEC_FORMAT, ENABLE,
                       PARAM_B_SW_ACK, ENABLE,
                       PARAM_B_SCAN_PARAM, DISABLE,	// Disable to avoid accidental changes param from scanning
                       PARAM_TRIGGER_MODE, PARAM_TRIGGER_PRESENT,
                       PARAM_INDEX_F0,	PARAM_B_DEC_EVENT, ENABLE
                     };

    ret = WriteSSI(fd, SSI_PARAM_SEND, param, ( sizeof(param) / sizeof(*param) ) );
    return ret;
}

/*!
 * \brief WriteSSI write formatted package and check ACK to/from scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
static int WriteSSI(int fd, byte opcode, byte *param, byte paramLen)
{
    int ret = EXIT_SUCCESS;
    byte *sendBuff = malloc( (SSI_HEADER_LEN + paramLen) * sizeof(byte) );

    // Flush old input queue
    tcflush(fd, TCIFLUSH);

    PreparePkg(sendBuff, opcode, param, paramLen);

    if ( write(fd, sendBuff, PKG_LEN(sendBuff) + 2) <= 0)
    {
        STYL_ERROR("write: %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
    }
    free(sendBuff);
    return ret;
}

/*!
 * \brief ReadSSI read formatted package and response ACK from/to scanner via file descriptor
 * \return number of read bytes
 */
static int ReadSSI(int fd, byte *buff, const int timeout)
{
    int ret = 0;
    int lastIndex = 0;
    int oldVTIME = 0;
    int oldVMIN = 0;
    int readRequest = 0;
    struct termios devConf;
    byte recvBuff[MAX_PKG_LEN];

    // Backup old value
    tcgetattr(fd, &devConf);
    oldVTIME = devConf.c_cc[VTIME];
    oldVMIN = devConf.c_cc[VMIN];

    do
    {
        // Read 1 first byte for length
        // Setup timeout read
        readRequest = 1;
        devConf.c_cc[VTIME] = timeout;
        devConf.c_cc[VMIN] = 0;
        tcsetattr(fd, TCSANOW, &devConf);
        ret += (int) read(fd, &recvBuff[INDEX_LEN], readRequest);
        if (ret <= 0)
        {
            break;
        }
        // read package + cksum - first_byte
        readRequest = recvBuff[INDEX_LEN] + 2 - 1;
        // Change reading condition to ensure read enough bytes
        devConf.c_cc[VTIME] = 0;
        devConf.c_cc[VMIN] = readRequest;
        tcsetattr(fd, TCSANOW, &devConf);
        // Get n next bytes (n = length + 2 - 1), 2 last bytes are cksum
        ret += (int) read(fd, &recvBuff[INDEX_LEN + 1], readRequest);

        if ( (ret > 0) || (IsChecksumOK(recvBuff)) )
        {
            WriteSSI(fd, SSI_CMD_ACK, NULL, 0);
            memcpy(&buff[lastIndex], recvBuff, PKG_LEN(recvBuff) + 2);
            lastIndex += PKG_LEN(recvBuff) + 2;
        }
        else
        {
            ret = -1;
            break;
        }
    }
    while (IsContinue(recvBuff));

EXIT:
    // Return old value
    devConf.c_cc[VTIME] = oldVTIME;
    devConf.c_cc[VMIN] = oldVMIN;
    tcsetattr(fd, TCSANOW, &devConf);
    return ret;
}

/*!
 * \brief Open TTY port of device
 * \return
 * - file descriptor: Success
 * - -1: Fail
 */
static int OpenTTY(const char *name)
{
    int fd = 0;

    styl_scanner_lockfile_fd = open(LOCK_SCANNER_PATH, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if(styl_scanner_lockfile_fd < 0)
    {
        STYL_ERROR("Scanner lockfile: open: %d - %s",errno, strerror(errno));
    }

    if(LockScanner(styl_scanner_lockfile_fd) !=  LOCK_SUCCESS)
    {
        STYL_ERROR("Device %s is busy.\n", name);
        return -1;
    }

    fd = open(name, O_RDWR);
    if (fd <= 0)
    {
        STYL_ERROR("Cannot open Scanner device: %s\n", name);
    }

    return fd;
}

/*!
 * \brief Close TTY port of device
 * \return EXIT_SUCCESS
 */
static int CloseTTY()
{
    char error = -1;
    error = close(scanner);
    if (error<0)
    {
        STYL_ERROR("close %d - %s", errno, strerror(errno));
    }
    UnlockScanner();
    return EXIT_SUCCESS;
}

static int LockScanner(int fd)
{
    if(flock(fd, LOCK_EX | LOCK_NB) < 0)
    {
        return LOCK_FAIL;
    }
    return LOCK_SUCCESS;
}

static int UnlockScanner(void)
{
    int ret = LOCK_SUCCESS;
    if(flock(styl_scanner_lockfile_fd, LOCK_UN | LOCK_NB) < 0)
    {
        ret = LOCK_FAIL;
        STYL_ERROR("Unlock Scanner: flock %d - %s", errno, strerror(errno));
    }
    unlink(LOCK_SCANNER_PATH);
    return ret;
}

/*!
 * \brief WriteSSI write formatted package to scanner via file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
static int ConfigTTY(int fd)
{
    int ret = EXIT_SUCCESS;
    int flags = 0;
    struct termios devConf;

    // Set flags for blocking mode and sync for writing
    flags = fcntl(fd, F_GETFL);
    if (0 > flags)
    {
        STYL_ERROR("fcntl 'F_GETFL': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto EXIT;
    }
    flags |= (O_FSYNC);
    flags &= ~(O_NDELAY | O_ASYNC);

    ret = fcntl(fd, F_SETFL, flags);
    if (ret)
    {
        STYL_ERROR("fcntl 'F_SETFL': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto EXIT;
    }

    // Configure tty dev
    devConf.c_cflag = (CS8 | CLOCAL | CREAD);
    devConf.c_iflag = 0;
    devConf.c_oflag = 0;
    devConf.c_lflag = 0;
    devConf.c_cc[VMIN] = 0;
    devConf.c_cc[VTIME] = TIMEOUT_MSEC;	// read non-blocking flush dump data. See blocking read timeout later

    // Portability: Use cfsetspeed instead of CBAUD since c_cflag/CBAUD is not in POSIX
    ret = cfsetspeed(&devConf, BAUDRATE);
    if (ret)
    {
        STYL_ERROR("cfsetspeed 'BAUDRATE': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto EXIT;
    }

    ret = tcsetattr(fd, TCSANOW, &devConf);
    if (ret)
    {
        STYL_ERROR("tcsetattr 'TCSANOW': %d - %s", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto EXIT;
    }

EXIT:
    return ret;
}

/*!
 * \brief ExtractBarcode extract barcode from formatted package
 * \return
 * - barcode length: Success
 * - 0: Fail
 */
static int ExtractBarcode(char *buff, byte *pkg, const int buffLength)
{
    char *barcodePtr = buff;
    byte *pkgPtr = pkg;
    int barcodeLength = 0;
    int barcodePartLength = 0;

    if (NULL != pkgPtr)
    {
        while (IsContinue(pkgPtr))
        {
            barcodePartLength = PKG_LEN(pkgPtr) - SSI_HEADER_LEN - 1;
            memcpy(barcodePtr, &pkgPtr[INDEX_BARCODETYPE + 1], barcodePartLength);
            barcodeLength += barcodePartLength;	// 1 byte of barcode type
            // Point to next pkg
            barcodePtr += PKG_LEN(pkgPtr) + 2;
            pkgPtr += PKG_LEN(pkgPtr) + 2;
        }

        // Get the last part of barcode
        barcodePartLength = PKG_LEN(pkgPtr) - SSI_HEADER_LEN - 1;
        if(barcodePartLength >= 0)
            memcpy(barcodePtr, &pkgPtr[INDEX_BARCODETYPE + 1], barcodePartLength);
        barcodeLength += barcodePartLength;	// 1 byte of barcode type
    }
    return barcodeLength;
}

/*!
 * \brief CheckACK receive ACK package after WriteSSI() and check for ACK
 * \return
 * - EXIT_SUCCESS: Success	ACK
 * - EXIT_FAILURE: Fail		Unknown cause
 * - ENAK(3)	 : Fail		NAK
 */
static int CheckACK(int fd)
{
    int ret = EXIT_SUCCESS;
    byte recvBuff[MAX_PKG_LEN];
    ret = ReadSSI(fd, recvBuff, 1);
    if ( (ret > 0) && (SSI_CMD_ACK == recvBuff[INDEX_OPCODE]) )
    {
        ret = EXIT_SUCCESS;
    }
    else if (SSI_CMD_NAK == recvBuff[INDEX_OPCODE])
    {
        ret = ENAK;
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    return ret;
}

/*!
 * \brief HandleError prints error message and indicate next step
 */
static void PrintError(int ret)
{
    printf("ERROR:");
    switch (ret)
    {
    case ENAK:
        printf(" NAK\n");
        break;
    case ENODEC:
        printf(" no decode event\n");
        break;

    default:
        printf("\n");
        break;
    }
}

/********** Global function definition section ********************************/

/*!
 * \brief mlsBarcodeReader_Open Open Reader descritptor file for read write
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Open(const char *name)
{
    char ret = EXIT_SUCCESS;

    assert(name != NULL);

    STYL_INFO("Scanner port: %s", name);

    ret = OpenTTY(name);
    if (ret <= 0)
    {
        ret = EXIT_FAILURE;
        goto EXIT;
    }
    else
    {
        scanner = ret;
    }

    ret = (char) ConfigTTY(scanner);
    if (ret)
    {
        STYL_ERROR("Can not configure TTY for device");
        goto ERROR;
    }

    ret = (char) ConfigSSI(scanner);
    if (ret)
    {
        STYL_ERROR("Can not configure SSI for device");
        goto ERROR;
    }

    /* Flush buffer of device*/
    if(mlsBarcodeReader_Flush() != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not flush buffer of device");
        goto ERROR;
    }
    STYL_INFO("Flushed buffer of device");

    /* Enable device to scanning */
    if(mlsBarcodeReader_Enable() != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not enable device to scanning");
        goto ERROR;
    }
    STYL_INFO("Enabled device to scanning");

EXIT:
    return ret;
ERROR:
    CloseTTY();
    scanner = -1;
    return EXIT_FAILURE;
}

/*!
 * \brief mlsBarcodeReader_close close Reader file descriptor
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Close()
{
    char error = EXIT_SUCCESS;

    /* Enable device to scanning */
    if(mlsBarcodeReader_Disable() != EXIT_SUCCESS)
    {
        STYL_ERROR("Can not disable scanner device");
    }
    STYL_INFO("Disabled scanner device");

    error = CloseTTY();
    return error;
}

/*!
 * \brief mlsBarcodeReader_ReadData Reader data from descriptor file (blocking read)
 * \param buff point to buffer which store data.
 * \return number of byte(s) read.
 */
unsigned int mlsBarcodeReader_ReadData(char *buff, const int buffLength, const int timeout)
{
    int barcodeLen = 0;
    int ret = 0;
    ssiState currentState = WAIT_DEC_EVENT;
    ssiState nextState = WAIT_DEC_EVENT;
    ssiState previousState = WAIT_DEC_EVENT;
    int isInSession = TRUE;
    byte recvBuff[4000] = {0};

    assert( (timeout >= 0) && (timeout <= 25) );

    while (isInSession)
    {
        switch (currentState)
        {
        #if 0
        case START:
            ret = WriteSSI(scanner, SSI_START_SESSION, NULL, 0);
            if ( (EXIT_SUCCESS!=ret) || (EXIT_SUCCESS!=CheckACK(scanner)) )
            {
                if (NULL != debugLevel)
                {
                    PrintError(ret);
                }
                nextState = STOP;
            }
            break;
        #endif // 0
        case STOP:
            isInSession = FALSE;
            #if 0
			printf("Send Stop session cmd...");
			ret = WriteSSI(scanner, SSI_STOP_SESSION, NULL, 0);
			usleep(1000);
			if ( (ret) || (! CheckACK(scanner) ) )
			{
				PrintError(ret);
			}
			else
			{
                ret = barcodeLen;
			}
			#else
			/* TODO: need to research if auto detect accept STOP_SESSION */
            ret = barcodeLen;
			#endif // 0
            break;

        case WAIT_DEC_EVENT:
            ret = ReadSSI(scanner, recvBuff, timeout);
            if (ret <= 0)
            {
                nextState = STOP;
            }
            else
            {
                nextState = REPLY_ACK;
                previousState = currentState;
            }
            break;

        case REPLY_ACK:
            ret = WriteSSI(scanner, SSI_CMD_ACK, NULL, 0);
            if (EXIT_SUCCESS!=ret)
            {
                PrintError(ret);
                nextState = STOP;
            }
            else
            {
                switch (previousState)
                {
                case WAIT_DEC_EVENT:
                    nextState = GET_BARCODE;
                    break;
                case GET_BARCODE:
                    nextState = STOP;
                default:
                    break;
                }
            }
            previousState = currentState;
            break;
        #if 0
        case REPLY_NAK:
            break;
        #endif // 0
        case GET_BARCODE:
            // Receive barcode in formatted package
            ret = ReadSSI(scanner, recvBuff, timeout);
            if (ret <= 0)
            {
                buff = NULL;
                nextState = STOP;
            }
            else
            {
                // Extract barcode to buffer
                assert(NULL != recvBuff);
                barcodeLen = ExtractBarcode(buff, recvBuff, buffLength);
                DisplayPkg(recvBuff);
                nextState = REPLY_ACK;
                previousState = currentState;
            }
            break;
        #if 0
        case FLUSH_QUEUE:
            /* TODO: need to research why disable - flush - enable don't work */
            break;
        #endif // 0
        default:
            break;
        }
        currentState = nextState;
    }

    STYL_WARNING("Barcode Len = %d\n", ret);

    return ret;
}

/*!
 * \brief mlsBarcodeReader_Reopen closes then opens device
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Reopen(const char *name)
{
    char error = EXIT_SUCCESS;
    error = mlsBarcodeReader_Close();
    if(EXIT_SUCCESS==error)
    {
        error = mlsBarcodeReader_Open(name);
    }
    return error;
}

/*!
 * \brief mlsBarcodeReader_Enable Enable Reader for scaning QR code/Bar Code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Enable()
{
    char ret = EXIT_SUCCESS;
    ret = WriteSSI(scanner, SSI_SCAN_ENABLE, NULL, 0);
    if ( (EXIT_SUCCESS!=ret) || (EXIT_SUCCESS!=CheckACK(scanner)) )
    {
        PrintError(ret);
        ret = EXIT_FAILURE;
    }
    return ret;
}

/*!
 * \brief mlsBarcodeReader_Disable Disable reader, Reader can't scan any QR code/bar code
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Disable()
{
    char ret = EXIT_SUCCESS;
    ret = WriteSSI(scanner, SSI_SCAN_DISABLE, NULL, 0);
    if ( (EXIT_SUCCESS!=ret) || (EXIT_SUCCESS!=CheckACK(scanner)) )
    {
        PrintError(ret);
        ret = EXIT_FAILURE;
    }
    return ret;
}

/*!
 * \brief mlsBarcodeReader_Flush Flush buffer of scanner
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
char mlsBarcodeReader_Flush()
{
    char ret = EXIT_SUCCESS;
    ret = WriteSSI(scanner, SSI_FLUSH_QUEUE, NULL, 0);
    if ( (EXIT_SUCCESS!=ret) || (EXIT_SUCCESS!=CheckACK(scanner)) )
    {
        PrintError(ret);
        ret = EXIT_FAILURE;
    }
    return ret;
}

/*!
 * \brief GetVersion provide software version
 * \return string of software version
 * -
 */
char *GetVersion(void)
{
    return VERSION;
}

/**@}*/
