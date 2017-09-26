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
 * @file    CStylScannerPackage.h
 * @brief   C code - Implement some method for maintain package.
 *
 * Long description.
 * @date    24/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef CSTYLSCANNERPACKAGE_H_INCLUDED
#define CSTYLSCANNERPACKAGE_H_INCLUDED
#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ****************************************************/
#include "CStylScannerUtils.h"

/********** Constant  and compile switch definition section ********************/
/********** Type declaration section *******************************************/
/********** Macro declaration section ******************************************/
/********** Function declaration section ***************************************/

/*!
 * \brief StylScannerPackage_Display: print out package content.
 * \param
 * - package: pointer to package content.
 * - length: length of package in byte.
 */
void StylScannerPackage_Display(byte *package, gint length);

/*!
 * \brief StylScannerPackage_Extract: extract barcode from formatted package
 * \return
 * - barcode length: Success
 * - 0             : Fail
 */
gint StylScannerPackage_Extract(char *buffer, byte *package, const int buffLength);

#ifdef __cplusplus
}
#endif
#endif // CSTYLSCANNERPACKAGE_H_INCLUDED
/**@}*/
