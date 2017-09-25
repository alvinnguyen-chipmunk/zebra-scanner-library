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
 * @file    CStylScannerDevice.c
 * @brief   C code - check tty port is exist and return its node
 *
 * Long description.
 * @date    22/09/2017
 * @author  Alvin Nguyen - alvin.nguyen@styl.solutions
 */

/********** Include section ***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>

#include "CStylScannerDevice.h"
#include "CStylScannerUtils.h"

/********** Local Type definition section *************************************/
/********** Local Constant and compile switch definition section **************/
/********** Local Macro definition section ************************************/
#define VID     "idVendor"
#define PID     "idProduct"

/********** Local (static) variable declaration section ***********************/
/********** Local (static) function declaration section ***********************/
/********** Local (static) function definition section ************************/

/********** Global function definition section ********************************/

/*!
 * \brief StylScannerDevice_GetNode - Get node string for device with vendor id and product id and subsystem
 * \param
 * - devSubsystem : String for subsystem that is where to list all devices
 * - devVendorID  : String for vendor ID that will let filter for specific device
 * - devProductID : String for product ID that will let filter for specific device
 * \return 	the device node file name of the udev device, or NULL if no device node exists
 */
const char * StylScannerDevice_GetNode(const char *devSubsystem, const char *devVendorID, const char *devProductID)
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    const char *deviceNode;
    /* Create the udev object */
    udev = udev_new();
    if (!udev)
    {
        STYL_ERROR("Can't create udev object to find device.");
        goto __exit;
    }

    /* Create a list of the devices in the "devSubsystem" subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, devSubsystem);
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    /* For each item enumerated, found out device with "devVendorID" and "devProductID"
       udev_list_entry_foreach is a macro which expands to a loop.
       The loop will be executed for each member in devices,
       setting dev_list_entry to a list entry which contains the device's path in /sys. */
    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path;

        const char *vendor;
        const char *product;
        const char *node;

        deviceNode = NULL;
        /* Get the filename of the /sys entry for the device
           and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        node = udev_device_get_devnode(dev);

        dev = udev_device_get_parent_with_subsystem_devtype (dev, "usb", "usb_device");
        if (!dev)
        {
            continue;
        }

        vendor = udev_device_get_sysattr_value(dev,"idVendor");
        product = udev_device_get_sysattr_value(dev,"idProduct");

        if(vendor && product &&
                (strcmp (vendor , devVendorID ) == 0) &&
                (strcmp (product, devProductID) == 0))
        {
            deviceNode = node;
            udev_device_unref(dev);
            break;
        }
        udev_device_unref(dev);
    }

__exit:
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return deviceNode;
}

/**@}*/

