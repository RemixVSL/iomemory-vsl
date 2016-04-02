//-----------------------------------------------------------------------------
// Copyright (c) 2006-2014, Fusion-io, Inc.(acquired by SanDisk Corp. 2014)
// Copyright (c) 2014 SanDisk Corp. and/or all its affiliates. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the SanDisk Corp. nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#define IODRIVE4_PCI_DEVICE                 0x2001 ///< Generation 2 Hardware

#define GEN3_DEVICEID                       0x3000

#define FUSION_PCI_SUBSYSTEM_VENDOR_ID      PCI_VENDOR_ID_FUSION
#define FUSION_PCI_SUBSYSTEM_DEVICE_ID      0x1010

#define HP_PCI_SUBSYSTEM_VENDOR_ID          0x103c
#define HP_PCI_MEZZ_SUBSYSTEM_DEVICE_ID     0x324D
#define HP_PCI_IODRIVE_SUBSYSTEM_DEVICE_ID  0x324E

#define DELL_PCI_SUBSYSTEM_VENDOR_ID        0x1028
#define IBM_PCI_SUBSYSTEM_VENDOR_ID         0x1014

#define OLDEST_FW_MAJ_VER_OF_THIS_GEN       6

#endif /* __FIO_PORT_PCI_H__ */
