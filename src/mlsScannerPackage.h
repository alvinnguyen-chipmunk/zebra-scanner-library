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
 * @file    mlsScannerPackage.h
 * @brief   C code - Implement some method for maintain package.
 *
 * Long description.
 * @date    24/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSSCANNERPACKAGE_H_INCLUDED
#define MLSSCANNERPACKAGE_H_INCLUDED
#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ****************************************************/
/********** Constant  and compile switch definition section ********************/
/********** Type declaration section *******************************************/
/********** Macro declaration section ******************************************/
/********** Function declaration section ***************************************/

/*!
 * \brief mlsScannerPackage_Display: print out package content.
 * \param
 * - package: pointer to package content.
 * - length: length of package in byte.
 */
void mlsScannerPackage_Display(byte *package, gint length);

/*!
 * \brief mlsScannerPackage_Extract: extract barcode from formatted package
 * \return
 * - barcode length: Success
 * - 0             : Fail
 */
gint mlsScannerPackage_Extract(gchar *buffer, gchar * symbolBuffer, byte *package, const gint buffLength);

/*!
 * \brief mlsScannerPackage_Dump: Dump content of message.
 */
void mlsScannerPackage_Dump (byte *buffer, gint length, gboolean isRead);

#ifdef __cplusplus
}
#endif
#endif // MLSSCANNERPACKAGE_H_INCLUDED
/**@}*/
