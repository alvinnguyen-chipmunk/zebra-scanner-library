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
 * @file    mlsScannerDevice.h
 * @brief   C code - check tty port is exist and return its node
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

#ifndef MLSSCANNERDEVICE_H_INCLUDED
#define MLSSCANNERDEVICE_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/********** Include section ***************************************************/
/********** Constant  and compile switch definition section *******************/
/********** Type definition section *******************************************/
/********** Macro definition section ******************************************/
/********** Function declaration section **************************************/

/*!
 * \brief StylScannerDevice_GetNode - Get node string for device with vendor id and product id and subsystem
 * \param
 * - devSubsystem : String for subsystem that is where to list all devices
 * - devVendorID  : String for vendor ID that will let filter for specific device
 * - devProductID : String for product ID that will let filter for specific device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char * StylScannerDevice_GetNode(const char *devSubsystem, const char *devVendorID, const char *devProductID);

#ifdef __cplusplus
}
#endif

#endif // MLSSCANNERDEVICE_H_INCLUDED
/**@}*/

