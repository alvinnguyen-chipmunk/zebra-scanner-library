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
 * @file    mlsScannerConfig.h
 * @brief   C code - Configure for TTY port and SSI protocol.
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSSCANNERCONFIG_H_INCLUDED
#define MLSSCANNERCONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

/********** Constant  and compile switch definition section *******************/
/********** Type definition section *******************************************/
/********** Macro definition section ******************************************/
/********** Function declaration section **************************************/

/*!
 * \brief StylScannerConfig_OpenTTY: Open TTY port of device
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_OpenTTY(gchar *deviceNode);

/*!
 * \brief StylScannerConfig_CloseTTY: Close TTY port of device.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_CloseTTY(gint pFile);

/*!
 * \brief StylScannerConfig_ConfigTTY: Do configure for SSI port.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigTTY(gint pFile);

/*!
 * \brief StylScannerConfig_ConfigSSI: Send parameters to configure scanner as SSI interface.
 * \return
 * - EXIT_SUCCESS: Success
 * - EXIT_FAILURE: Fail
 */
gint StylScannerConfig_ConfigSSI(gint pFile, byte triggerMode);


#ifdef __cplusplus
}
#endif

#endif // MLSSCANNERCONFIG_H_INCLUDED
/**@}*/
