/*
 * Copyright 2011 - 2017 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <cstddef>
#include <log/log.h>

#include "isci_expander.h"
#include "filesystem.h"
#include "port.h"
#include "isci_expander_phy.h"

using boost::shared_ptr;

/* */
ISCI_Expander::ISCI_Expander(const String &path)
    : RoutingDevice(path),
      m_SASAddress(0)
{
    Directory dir(m_Path + "/sas_device");
    File attr;
    std::list<Directory *> dirs = dir.dirs();
    foreach (i, dirs) {
        try {
            attr = *(*i) + "sas_address";
            attr >> m_SASAddress;
        } catch (...) {
            /* TODO: report read failure of attribtue. */
        }
    }
    dlog(" sas adress " + path + "\n" + String(m_SASAddress) + "\n");
    dir = m_Path + "/sas_expander";
    dirs = dir.dirs();
    foreach (i, dirs) {
        try {
            attr = *(*i) + "product_id";
            attr >> m_ProductId;
        } catch (...) {
        }
        try {
            attr = *(*i) + "vendor_id";
            attr >> m_Vendor;
        } catch (...) {
        }
        try {
            attr = *(*i) + "product_rev";
            attr >> m_ProductRev;
        } catch (...) {
        }
        try {
            attr = *(*i) + "component_vendor_id";
            attr >> m_ComponentVendorId;
        } catch (...) {
        }
        try {
            attr = *(*i) + "component_id";
            attr >> m_ComponentId;
        } catch (...) {
        }
        try {
            attr = *(*i) + "component_revision_id";
            attr >> m_ComponentRev;
        } catch (...) {
        }
    }
}

/* */
shared_ptr<Port> ISCI_Expander::getPortByPath(const String &path) const
{
    foreach (i, m_Ports) {
        if ((*i)->getPath() == path) {
            return (*i);
        }
    }

    if (m_pSubtractivePort) {
        if (m_pSubtractivePort->getPath() == path) {
            return m_pSubtractivePort;
        }
    }

    return shared_ptr<Port>();
}

/* */
void ISCI_Expander::getAddress(SSI_Address &address) const
{
    address.scsiAddress.host = 0;
    address.scsiAddress.bus = 0;
    address.scsiAddress.target = 0;
    address.scsiAddress.lun = 0;
    address.sasAddressPresent = m_SASAddress ? SSI_TRUE : SSI_FALSE;
    address.sasAddress = m_SASAddress;
}

/* */
void ISCI_Expander::discover()
{
    Directory dir(m_Path, "phy");
    unsigned int number = 0;
    std::list<Directory *> dirs = dir.dirs();
    foreach (i, dirs) {
        attachPhy(shared_ptr<Phy>(new ISCI_Expander_Phy(*(*i), number++, shared_from_this())));
    }

    foreach (i, m_Phys) {
        (*i)->discover();
    }

    foreach (i, m_Ports) {
        (*i)->discover();
    }
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
