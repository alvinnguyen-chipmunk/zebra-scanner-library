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
 * @file    CStylScannerPackage.c
 * @brief   C code - Implement some method for maintain package.
 *
 * Long description.
 * @date    24/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"

#include "CStylScannerUtils.h"
#include "CStylScannerPackage.h"
#include "CStylScannerSSI.h"
/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/
/********** Global function definition section ********************************/

/*!
 * \brief StylScannerPackage_Display: print out package content.
 * \param
 * - package: pointer to package content.
 * - length: length of package in byte.
 */
void StylScannerPackage_Display(byte *package, gint length)
{
    for (gint i = 0; i < length; i++)
    {
        STYL_INFO_OTHER(" 0x%02x", package[i]);
    }

    STYL_INFO_OTHER("\n");
}

/*!
 * \brief StylScannerPackage_Extract: extract barcode from formatted package
 * \return
 * - barcode length: Success
 * - 0             : Fail
 */
gint StylScannerPackage_Extract(char *buffer, byte *package, const int buffLength)
{
    char *pBuffer  = buffer;
    byte *pPackage = package;

    int decodeLength = 0;
    int partLength = 0;

    if (NULL != pPackage)
    {
        while (StylScannerSSI_IsContinue(pPackage))
        {

            partLength = PACKAGE_LEN(pPackage) - SSI_LEN_HEADER - SSI_LEN_DECODE_TYPE;
            if(buffLength <= (decodeLength + partLength))
            {
                STYL_ERROR("Cannot extract large receive buffer.");
                decodeLength = 0;
                break;
            }
            if(partLength >= 0)
                memcpy(pBuffer, &pPackage[PKG_INDEX_DECODE_TYPE + 1], partLength);
            decodeLength += partLength;

            /* Point to next pkg */
            pPackage += PACKAGE_LEN(pPackage) + SSI_LEN_CHECKSUM;
            pBuffer += decodeLength;
        }

        /* Get the last part of barcode */
        partLength = PACKAGE_LEN(pPackage) - SSI_LEN_HEADER - SSI_LEN_DECODE_TYPE;
        if(partLength >= 0)
            memcpy(pBuffer, &pPackage[PKG_INDEX_DECODE_TYPE + 1], partLength);
        decodeLength += partLength;
    }

    return decodeLength;
}


/**@}*/
