/*
 * IMPORTANT - READ BEFORE COPYING, INSTALLING OR USING.
 * BY LOADING OR USING THIS SOFTWARE, YOU AGREE TO THE TERMS OF THIS INTEL
 * SOFTWARE LICENSE AGREEMENT.  IF YOU DO NOT WISH TO SO AGREE, DO NOT COPY,
 * INSTALL OR USE THIS SOFTWARE.
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2010 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <features.h>

#include <ssi.h>

#include "exception.h"
#include "list.h"
#include "container.h"
#include "string.h"
#include "filesystem.h"
#include "object.h"
#include "storage_device.h"
#include "phy.h"
#include "port.h"
#include "end_device.h"
#include "nondisk_device.h"
#include "routing_device.h"
#include "session.h"
#include "block_device.h"
#include "multimedia_device.h"
#include "stream_device.h"
#include "ahci_disk.h"
#include "ahci_cdrom.h"
#include "ahci_tape.h"
#include "ahci_multiplier.h"
#include "remote_port.h"

/* */
AHCI_Multiplier::AHCI_Multiplier(const String &path, Directory &dir)
    : RoutingDevice(path), m_pPhy(0)
{
    m_pPhy = new Phy(path, 0);
    m_pPhy->setParent(this);
    m_pSubtractivePort = new RemotePort(path);
    m_pSubtractivePort->setParent(this);
    m_pSubtractivePort->attachPhy(m_pPhy);

    unsigned int number = 0;
    for (Iterator<Directory *> i = dir; *i != 0; ++i) {
        for (Iterator<Directory *> j = *(*i); *j != 0; ++j) {
            if (__internal_attach_end_device(*(*j), ++number)) {
                break;
            }
        }
    }
}

/* */
bool AHCI_Multiplier::__internal_attach_end_device(const Path &path, unsigned int number)
{
    CanonicalPath temp = path + "driver";
    EndDevice *pEndDevice = 0;
    if (temp == "/sys/bus/scsi/drivers/sd") {
        pEndDevice = new AHCI_Disk(path);
    } else
    if (temp == "/sys/bus/scsi/drivers/sr") {
        pEndDevice = new AHCI_CDROM(path);
    } else
    if (temp == "/sys/bus/scsi/drivers/st") {
        pEndDevice = new AHCI_Tape(path);
    }
    if (pEndDevice != 0) {
        Phy *pPhy = new Phy(path, number);
        attachPhy(pPhy);
        pPhy->setParent(this);
        Port *pPort = new Port(path);
        attachPort(pPort);
        pPort->setParent(this);
        pPort->attachPhy(pPhy);
        pPort->attachPort(pEndDevice->getPort());
    }
    return (pEndDevice != 0);
}

/* */
void AHCI_Multiplier::acquireId(Session *pSession)
{
    pSession->addPhy(m_pPhy);
    RoutingDevice::acquireId(pSession);
}

/* */
void AHCI_Multiplier::getAddress(SSI_Address &address) const
{
    address.scsiAddress.host = 0;
    address.scsiAddress.bus = 0;
    address.scsiAddress.target = 0;
    address.scsiAddress.lun = 0;
    address.sasAddressPresent = SSI_FALSE;
    address.sasAddress = 0ULL;
}

/* */
void AHCI_Multiplier::getPhys(Container<Phy> &container) const
{
    RoutingDevice::getPhys(container);
    container.add(m_pPhy);
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
