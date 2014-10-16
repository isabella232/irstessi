
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

#include <dirent.h>
#include <sys/types.h>
#include <cstdlib>

#include <ssi.h>
#include <log/log.h>

#include "exception.h"
#include "container.h"
#include "string.h"
#include "filesystem.h"
#include "object.h"
#include "storage_device.h"
#include "raid_device.h"
#include "array.h"
#include "end_device.h"
#include "volume.h"
#include "controller.h"
#include "session.h"
#include "phy.h"
#include "enclosure.h"
#include "port.h"
#include "routing_device.h"
#include "raid_info.h"
#include "session_manager.h"
#include "ahci.h"
#include "isci.h"
#include "context_manager.h"
#include "array.h"
#include "volume.h"
#include "utils.h"
#include "mdadm_config.h"

/* */
Session::Session()
{
    Directory dir;
    std::list<Directory *> dirs;
    dlog("Open session");
    pContextMgr->refresh();
    dir = "/sys/bus/pci/drivers/ahci";
    dirs = dir.dirs();
    foreach (i, dirs) {
        CanonicalPath path = *(*i) + "driver";
        if (path == dir) {
            SysfsAttr attr;
            String vendor;
            attr = *(*i) + "vendor";
            try {
                attr >> vendor;
                if (vendor != "0x8086")
                    continue;
            } catch (...) {
                /* TODO log that vendor cannot be read from filesystem */
                continue;
            }
            AHCI *pAHCI = new AHCI(CanonicalPath(*(*i)));
            pAHCI->discover();
            pAHCI->acquireId(this);
        }
    }
    dir = "/sys/bus/pci/drivers/isci";
    dirs = dir.dirs();
    foreach (i, dirs) {
        CanonicalPath path = *(*i) + "driver";
        if (path == dir) {
            ISCI *pISCI = new ISCI(CanonicalPath(*(*i)));
            pISCI->discover();
            pISCI->acquireId(this);
        }
    }
    foreach (i, m_Controllers) {
        RaidInfo *pRaidInfo = (*i)->findRaidInfo(m_RaidInfo);
        if (pRaidInfo)
            pRaidInfo->acquireId(this);
    }
    if (m_EndDevices.size() > 0) {
        dir = "/sys/devices/virtual/block";
        dirs = dir.dirs();
        foreach (i, dirs) {
            __internal_attach_imsm_device(CanonicalPath(*(*i)));
        }
    }
    check_configuration();
}

/* */
Session::~Session()
{
    foreach (i, m_EndDevices)
        pContextMgr->releaseId(*i);
    foreach (i, m_Arrays)
        pContextMgr->releaseId(*i);
    foreach (i, m_Enclosures)
        pContextMgr->releaseId(*i);
    foreach (i, m_RaidInfo)
        pContextMgr->releaseId(*i);
    foreach (i, m_Phys)
        pContextMgr->releaseId(*i);
    foreach (i, m_Volumes)
        pContextMgr->releaseId(*i);
    foreach (i, m_Ports)
        pContextMgr->releaseId(*i);
    foreach (i, m_RoutingDevices)
        pContextMgr->releaseId(*i);
    foreach (i, m_Controllers)
        pContextMgr->releaseId(*i);
}
/* */
bool Session::equal(const Object *pObject) const
{
    return pObject == this;
}

/* */
String Session::getKey() const
{
    return (unsigned long long)(this);
}

/* */
ScopeObject * Session::getObject(unsigned int id)
{
    if (id == 0) {
        return this;
    }
    Object *pObject = NULL;
    try {
        switch (getTypeOfId(id)) {
        case ObjectType_EndDevice:
            pObject = m_EndDevices.find(id);
            break;
        case ObjectType_Array:
            pObject = m_Arrays.find(id);
            break;
        case ObjectType_Enclosure:
            pObject = m_Enclosures.find(id);
            break;
        case ObjectType_Phy:
            pObject = m_Phys.find(id);
            break;
        case ObjectType_Volume:
            pObject = m_Volumes.find(id);
            break;
        case ObjectType_Port:
            pObject = m_Ports.find(id);
            break;
        case ObjectType_RoutingDevice:
            pObject = m_RoutingDevices.find(id);
            break;
        case ObjectType_RaidInfo:
            pObject = m_RaidInfo.find(id);
            break;
        case ObjectType_Controller:
            pObject = m_Controllers.find(id);
            break;
        default:
            pObject = NULL;
        }
    } catch (...) {
        pObject = NULL;
    }
    return dynamic_cast<ScopeObject *>(pObject);
}

/* */
RaidInfo * Session::getRaidInfo(unsigned int id) const
{
    RaidInfo *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_RaidInfo) {
        pResult = dynamic_cast<RaidInfo *>(m_RaidInfo.find(id));
    }
    return pResult;
}

/* */
Array * Session::getArray(unsigned int id) const
{
    Array *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Array) {
        pResult = dynamic_cast<Array *>(m_Arrays.find(id));
    }
    return pResult;
}

/* */
Controller * Session::getController(unsigned int id) const
{
    Controller *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Controller) {
        pResult = dynamic_cast<Controller *>(m_Controllers.find(id));
    }
    return pResult;
}

/* */
StorageDevice * Session::getDevice(unsigned int id) const
{
    StorageDevice *pResult = NULL;
    switch (getTypeOfId(id)) {
    case ObjectType_Array:
        pResult = dynamic_cast<StorageDevice *>(m_Arrays.find(id));
        break;
    case ObjectType_EndDevice:
        pResult = dynamic_cast<StorageDevice *>(m_EndDevices.find(id));
        break;
    case ObjectType_Volume:
        pResult = dynamic_cast<StorageDevice *>(m_Volumes.find(id));
        break;
    default:
        pResult = NULL;
    }
    return pResult;
}

/* */
Phy * Session::getPhy(unsigned int id) const
{
    Phy *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Phy) {
        pResult = dynamic_cast<Phy *>(m_Phys.find(id));
    }
    return pResult;
}

/* */
Port * Session::getPort(unsigned int id) const
{
    Port *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Port) {
        pResult = dynamic_cast<Port *>(m_Ports.find(id));
    }
    return pResult;
}

/* */
RoutingDevice * Session::getRoutingDevice(unsigned int id) const
{
    RoutingDevice *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_RoutingDevice) {
        pResult = dynamic_cast<RoutingDevice *>(m_RoutingDevices.find(id));
    }
    return pResult;
}

/* */
Volume * Session::getVolume(unsigned int id) const
{
    Volume *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Volume) {
        pResult = dynamic_cast<Volume *>(m_Volumes.find(id));
    }
    return pResult;
}

/* */
Enclosure * Session::getEnclosure(unsigned int id) const
{
    Enclosure *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_Enclosure) {
        pResult = dynamic_cast<Enclosure *>(m_Enclosures.find(id));
    }
    return pResult;
}

/* */
EndDevice * Session::getEndDevice(unsigned int id) const
{
    EndDevice *pResult = NULL;
    if (getTypeOfId(id) == ObjectType_EndDevice) {
        pResult = dynamic_cast<EndDevice *>(m_EndDevices.find(id));
    }
    return pResult;
}

/* */
void Session::addEndDevice(EndDevice *pEndDevice)
{
    if (pEndDevice == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pEndDevice);
    m_EndDevices.add(pEndDevice);
}

/* */
void Session::addArray(Array *pArray)
{
    if (pArray == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pArray);
    m_Arrays.add(pArray);
}

/* */
void Session::addRoutingDevice(RoutingDevice *pRoutingDevice)
{
    if (pRoutingDevice == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pRoutingDevice);
    m_RoutingDevices.add(pRoutingDevice);
}

/* */
void Session::addEnclosure(Enclosure *pEnclosure)
{
    if (pEnclosure == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pEnclosure);
    m_Enclosures.add(pEnclosure);
}

/* */
void Session::addPhy(Phy *pPhy)
{
    if (pPhy == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pPhy);
    m_Phys.add(pPhy);
}

/* */
void Session::addVolume(Volume *pVolume)
{
    if (pVolume == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pVolume);
    m_Volumes.add(pVolume);
}

/* */
void Session::addPort(Port *pPort)
{
    if (pPort == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pPort);
    m_Ports.add(pPort);
}

/* */
void Session::addController(Controller *pController)
{
    if (pController == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pController);
    m_Controllers.add(pController);
}

/* */
void Session::addRaidInfo(RaidInfo *pRaidInfo)
{
    if (pRaidInfo == NULL) {
        throw E_NULL_POINTER;
    }
    pContextMgr->acquireId(pRaidInfo);
    m_RaidInfo.add(pRaidInfo);
}

/* */
void Session::__internal_attach_imsm_device(const String &path)
{
    CanonicalPath temp = path + "/md/metadata_version";
    if (temp) {
        String metadata;
        try {
            SysfsAttr attr = temp;
            attr >> metadata;
        } catch (...) {
        }
        if (metadata == "external:imsm") {
            __internal_attach_imsm_array(path);
        }
    }
}

/* */
void Session::__internal_attach_imsm_array(const String &path)
{
    Array *pArray;
    try {
        pArray = new Array(path);
        pArray->acquireId(this);
    } catch (Exception ex) {
        if (ex == E_INVALID_OBJECT) {
            delete pArray;
        }
    } catch (...) {
        /* TODO: log that there's not enough resources in the system. */
    }
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
