
/*
Copyright (c) 2011, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/




#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <features.h>

#include <unistd.h>
#include <asm/types.h>

#include <ssi.h>
#include <orom/orom.h>

#include "exception.h"
#include "list.h"
#include "container.h"
#include "string.h"
#include "filesystem.h"
#include "object.h"
#include "cache.h"
#include "controller.h"
#include "isci.h"
#include "raid_info.h"
#include "phy.h"
#include "session.h"
#include "enclosure.h"
#include "isci_phy.h"
#include "port.h"
#include "pci_header.h"
#include "isci_raid_info.h"

/* */
ISCI::ISCI(const String &path)
    : Controller(path)
{
    m_Name = "ISCI at " + m_Path.reverse_right("0000:");

    struct orom_info *pInfo = orom_get(m_PciDeviceId);
    if (pInfo != 0) {
        m_PrebootMgrVersion =
            String(pInfo->prod_ver.major) + String(".") +
            String(pInfo->prod_ver.minor) + String(".") +
            String(pInfo->prod_ver.hotfix) + String(".") +
            String(pInfo->prod_ver.build);
        m_twoTbVolumePrebootSupported = pInfo->a_2tb_vol;
        m_twoTbDiskPrebootSupported = pInfo->a_2tb_disk;
        m_ESATASpanning = pInfo->c_esata;
        m_NVSRAMSupported = pInfo->a_nvm;
        m_HWXORSupported = pInfo->f_hardware_xor;
        m_PhyLocate = pInfo->f_led_locate;
        m_DiskUnlock = pInfo->c_hdd_passwd;
        m_PatrolReadSupport = pInfo->f_read_patrol;

        m_pRaidInfo = new ISCI_RaidInfo(this, pInfo->dpa, pInfo->tds,
                                        pInfo->vpa, pInfo->vphba, pInfo->chk);
    }
}

/* */
void ISCI::discover()
{
    Directory dir(m_Path, "host");
    SysfsAttr attr;
    for (Iterator<Directory *> i = dir; *i != 0; ++i) {
        unsigned int number = 0;
        try {
            attr = *(*i) + "scsi_host" + (*i)->reverse_after("/") + "isci_id";
            attr >> number;
        } catch (Exception) {
            /* TODO: report read failure of attribtue. */
        }
        Directory phys(*(*i), "phy");
        number *= 4;
        for (Iterator<Directory *> j = phys; *j != 0; ++j, ++number) {
            Phy *pPhy = new ISCI_Phy(*(*j), number, this);
            attachPhy(pPhy);
        }
    }
    for (Iterator<Phy *> i = m_Phys; *i != 0; ++i) {
        (*i)->discover();
    }
    for (Iterator<Port *> i = m_Ports; *i != 0; ++i) {
        (*i)->discover();
    }
}

/* */
Port * ISCI::getPortByPath(const String &path) const
{
    for (Iterator<Port *> i = m_Ports; *i != 0; ++i) {
        if ((*i)->getPath() == path) {
            return (*i);
        }
    }
    return 0;
}

/* */
void ISCI::getAddress(SSI_Address &address) const
{
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
