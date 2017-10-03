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
 * @file    mlsScannerPackage.c
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

#include "mlsScannerUtils.h"
#include "mlsScannerPackage.h"
#include "mlsScannerSSI.h"
/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
static const gchar * mlsScannerPackage_GetSymbology (byte symbol);

/********** Local (static) function definition section ************************/

/*!
 * \brief mlsScannerPackage_GetSymbology: find out symbol string for code type
 * \return Constant string for code type
 */
static const gchar * mlsScannerPackage_GetSymbology(byte symbol)
{
    switch (symbol)
    {
    case 0x16 :
        return "Bookland";
    case 0x02 :
        return "Codabar";
    case 0x0C :
        return "Code 11";
    case 0x03 :
        return "Code 128";
    case 0x20 :
        return "Code 32";
    case 0x01 :
        return "Code 39";
    case 0x13 :
        return "Code 39 Full ASCII";
    case 0x07 :
        return "Code 93";
    case 0x51 :
        return "Composite (CC-A + EAN-128)";
    case 0x52 :
        return "Composite (CC-A + EAN-13)";
    case 0x53 :
        return "Composite (CC-A + EAN-8)";
    case 0x54 :
        return "Composite (CC-A + RSS Expanded)";
    case 0x55 :
        return "Composite (CC-A + RSS Limited)";
    case 0x56 :
        return "Composite (CC-A + RSS-14)";
    case 0x57 :
        return "Composite (CC-A + UPC-A)";
    case 0x58 :
        return "Composite (CC-A + UPC-E)";
    case 0x61 :
        return "Composite (CC-B + EAN-128)";
    case 0x62 :
        return "Composite (CC-B + EAN-13)";
    case 0x63 :
        return "Composite (CC-B + EAN-8)";
    case 0x64 :
        return "Composite (CC-B + RSS Expanded)";
    case 0x65 :
        return "Composite (CC-B + RSS Limited)";
    case 0x66 :
        return "Composite (CC-B + RSS-14)";
    case 0x67 :
        return "Composite (CC-B + UPC-A)";
    case 0x68 :
        return "Composite (CC-B + UPC-E)";
    case 0x59 :
        return "Composite (CC-C + EAN-128)";
    case 0x17 :
        return "Coupon Code";
    case 0x38 :
        return "Cue CAT Code";
    case 0x04 :
        return "Discrete 2 of 5";
    case 0x1B :
        return "Data Matrix";
    case 0x0F :
        return "EAN-128";
    case 0x0B :
        return "EAN-13";
    case 0x4B :
        return "EAN-13 + 2";
    case 0x8B :
        return "EAN-13 + 5";
    case 0x0A :
        return "EAN-8";
    case 0x4A :
        return "EAN-8 + 2";
    case 0x8A :
        return "EAN-8 + 5";
    case 0x05 :
        return "IATA";
    case 0x19 :
        return "ISBT-128";
    case 0x21 :
        return "ISBT Concatenated";
    case 0x06 :
        return "ITF";
    case 0x9A :
        return "Macro Micro PDF";
    case 0x28 :
        return "Macro PDF";
    case 0x25 :
        return "Maxicode";
    case 0x1A :
        return "Micro PDF";
    case 0x0E :
        return "MSI";
    case 0x99 :
        return "MultiPacket";
    case 0x11 :
        return "PDF-417";
    case 0x1F :
        return "Planet (US)";
    case 0x23 :
        return "Postal Australia";
    case 0x24 :
        return "Dutch Postal";
    case 0x22 :
        return "Japan Postal";
    case 0x27 :
        return "UK Postal";
    case 0x26 :
        return "Postbar";
    case 0x1E :
        return "Postnet (US)";
    case 0x1C :
        return "QR Code";
    case 0x32 :
        return "RSS Expanded";
    case 0x31 :
        return "RSS Limited";
    case 0x30 :
        return "RSS-14";
    case 0x37 :
        return "Scanlet Webcode";
    case 0x5A :
        return "TLC-39";
    case 0x15 :
        return "Trioptic";
    case 0x08 :
        return "UPC-A";
    case 0x48 :
        return "UPC-A + 2";
    case 0x88 :
        return "UPC-A + 5";
    case 0x09 :
        return "UPC-E";
    case 0x49 :
        return "UPC-E + 2";
    case 0x89 :
        return "UPC-E + 5";
    case 0x10:
        return "UPC-E1";
    case 0x50 :
        return "UPC-E1 + 2";
    case 0x90 :
        return "UPC-E1 + 5";
    default :
        return "Unknown symbol";
    }
}

/********** Global function definition section ********************************/

/*!
 * \brief mlsScannerPackage_Display: print out package content.
 * \param
 * - package: pointer to package content.
 * - length: length of package in byte.
 */
void mlsScannerPackage_Display(byte *package, gint length)
{
    gint sizeBuffer = 0;
    if (length==NO_GIVEN)
        sizeBuffer = PACKAGE_LEN(package)+SSI_LEN_CHECKSUM;
    else
        sizeBuffer = length;

    for (gint i = 0; i < sizeBuffer; i++)
    {
        STYL_INFO_OTHER(" 0x%02x", package[i]);
    }
    STYL_INFO_OTHER("\n");
}

/*!
 * \brief mlsScannerPackage_Extract: extract barcode from formatted package
 * \return
 * - barcode length: Success
 * - 0             : Fail
 */
gint mlsScannerPackage_Extract(gchar *buffer, gchar * symbolBuffer, byte *package, const gint buffLength)
{
    gchar *pBuffer  = buffer;
    byte *pPackage = package;

    gint decodeLength = 0;
    gint partLength = 0;

    const gchar * symbol = mlsScannerPackage_GetSymbology(package[PKG_INDEX_DECODE_TYPE]);
    memcpy(symbolBuffer, symbol, strlen(symbol));

    if (NULL != pPackage)
    {
        while (mlsScannerSSI_IsContinue(pPackage))
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
            pBuffer += partLength;
        }

        /* Get the last part of barcode */
        mlsScannerPackage_Display(pPackage, NO_GIVEN);
        partLength = PACKAGE_LEN(pPackage) - SSI_LEN_HEADER - SSI_LEN_DECODE_TYPE;
        STYL_WARNING("Last partLength: %d", partLength);
        if(partLength >= 0)
        {
            memcpy(pBuffer, &pPackage[PKG_INDEX_DECODE_TYPE + 1], partLength);
            decodeLength += partLength;
        }
        else
        {
            STYL_ERROR("Parsing last part of decode data got problem.");
        }
    }

    STYL_WARNING("Data extract size: %d", decodeLength);

    STYL_DEBUG("\e[36m pBuffer: \n%s\e[0m\n\n", pBuffer);

    STYL_DEBUG("\e[36m buffer: \n%s\e[0m\n\n", buffer);

    return decodeLength;
}


/**@}*/
