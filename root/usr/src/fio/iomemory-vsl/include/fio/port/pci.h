//-----------------------------------------------------------------------------
// Copyright 2006-2012 Fusion-io, Inc. All rights reserved.
//
// No use, or distribution, of this source code is permitted in any form or
// means without a valid, written license agreement with Fusion-io. Please
// refer to the included "License" or "License.txt" file for terms and
// conditions regarding the use and redistribution of this software.
//-----------------------------------------------------------------------------

#ifndef __FIO_PORT_PCI_H__
#define __FIO_PORT_PCI_H__

#define PCI_VENDOR_ID_NWLOGIC               0xFAFA
#define PCI_VENDOR_ID_PLX                   0x10b5
#define PCI_VENDOR_ID_FUSION                0x1AED

#define IODRIVE_OLD_PCI_VENDOR              PCI_VENDOR_ID_NWLOGIC
#define IODRIVE_OLD_PCI_DEVICE              0xE003

#define IODRIVE_PCI_DEVICE                  0x1000 ///< Old busted devices only
#define IODRIVE1_PCI_DEVICE                 0x1001 ///< All ioDimm1 firmware releases
#define IODRIVE3_OLD_PCI_DEVICE             0x1003 ///< All ioDimm3 firmware released with drivers < 1.2.5
#define IODRIVE3_BOOT_PCI_DEVICE            0x1004 ///< For future use (Boot 6 BAR design)
#define IODRIVE3_PCI_DEVICE                 0x1005 ///< All ioDimm3 firmware released with drivers >= 1.2.5
#define IOXTREME_PCI_DEVICE                 0x1006 ///< All ioXtreme firmware
#define IOXTREMEPRO_PCI_DEVICE              0x1007 ///< All ioXtreme Pro firmware
#define IOXTREME2_PCI_DEVICE                0x1008 ///< All ioXtreme 2 (180/360)

#define FUSION_PCI_SUBSYSTEM_VENDOR_ID      PCI_VENDOR_ID_FUSION
#define FUSION_PCI_SUBSYSTEM_DEVICE_ID      0x1010

#define HP_PCI_SUBSYSTEM_VENDOR_ID          0x103c
#define HP_PCI_MEZZ_SUBSYSTEM_DEVICE_ID     0x324D
#define HP_PCI_IODRIVE_SUBSYSTEM_DEVICE_ID  0x324E

#endif /* __FIO_PORT_PCI_H__ */
