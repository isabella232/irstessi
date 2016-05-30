
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
#include <cstddef>
#include <typeinfo>

#include <ssi.h>

#include "exception.h"
#include "container.h"
#include "string.h"
#include "filesystem.h"
#include "object.h"
#include "routing_device.h"
#include "enclosure.h"
#include "session.h"
#include "utils.h"
#include "storage_device.h"
#include "end_device.h"
#include "controller.h"

/* */
Enclosure::Enclosure(const String &path)
    : StorageObject(path),
      m_LogicalId(""),
      m_SlotCount(0),
      m_SubenclosureCount(0)
{
    CanonicalPath temp;
    File attr;
    temp = m_Path + "/generic";
    m_SgName = temp.reverse_after("/");

    try {
        attr = m_Path + "/vendor";
        attr >> m_VendorId;
    } catch (...) {
    }
    try {
        attr = m_Path + "/model";
        attr >> m_ProductId;
    } catch (...) {
    }
    try {
        attr = m_Path + "/rev";
        attr >> m_Rev;
    } catch (...) {
    }
    Directory dir = m_Path + "/enclosure";
    std::list<Directory *> dirs = dir.dirs();
    foreach (i, dirs) {
        try {
            attr =  *(*i) + "components";
            attr >> m_SlotCount;
        } catch (...) {
        }
    }

    String sbuffer;
    const String command1 = "sg_ses -p 0x1 /dev/" + m_SgName;
    if (shell_cap(command1, sbuffer) == 0) {
        m_LogicalId = sbuffer.between("logical identifier (hex): ", "\n");
        m_LogicalId.trim();
        m_SubenclosureCount = sbuffer.between("number of secondary subenclosures:", "\n");
    }
    const String command2 = "sg_ses -p 0xa /dev/" + m_SgName;
    shell_cap(command2, sbuffer);
    if (sbuffer)
        __get_slot_info(sbuffer);

}

/* */
Enclosure::~Enclosure()
{
    foreach (i, m_Slots)
        delete *i;
}

/* */
void Enclosure::getEndDevices(Container<EndDevice> &container, bool) const
{
    container = m_EndDevices;
}

/* */
SSI_Status Enclosure::getInfo(SSI_EnclosureInfo *pInfo) const
{
    if (pInfo == NULL) {
        return SSI_StatusInvalidParameter;
    }
    pInfo->enclosureHandle = pInfo->uniqueId = getId();
    pInfo->enclosureKey = (getId() & 0x0fffffff);
    foreach (i, m_RoutingDevices) {
        StorageObject *parent = (*i)->getParent();
        if (parent && dynamic_cast<Controller *>(parent)) {
            pInfo->enclosureKey |= 0x10000000;
            break;
        }
    }
    m_VendorId.get(pInfo->vendorInfo, sizeof(pInfo->vendorInfo));
    m_Rev.get(pInfo->productRev, sizeof(pInfo->productRev));
    m_ProductId.get(pInfo->productId, sizeof(pInfo->productId));
    m_LogicalId.get(pInfo->logicalId, sizeof(pInfo->logicalId));
    pInfo->processorCount = 0;
    pInfo->subenclosureCount = m_SubenclosureCount;
    pInfo->elementCount = m_Slots.size();
    pInfo->numberOfSlots = m_SlotCount;
    return SSI_StatusOk;
}

/* */
void Enclosure::getRoutingDevices(Container<RoutingDevice> &container, bool) const
{
    container = m_RoutingDevices;
}

/* */
String Enclosure::getLogicalId() const
{
    return m_LogicalId;
}

/* */
bool Enclosure::operator ==(const Object &object) const
{
    return typeid(*this) == typeid(object) &&
           m_LogicalId == static_cast<const Enclosure *>(&object)->getLogicalId();
}

/* */
String Enclosure::getKey() const
{
    return m_LogicalId;
}

/* */
void Enclosure::attachEndDevice(EndDevice *pEndDevice)
{
    m_EndDevices.add(pEndDevice);
}

/* */
void Enclosure::attachEndDevices(Container<EndDevice> &EndDevices)
{
    foreach (i, EndDevices) {
        (*i)->setEnclosure(this);
        attachEndDevice(*i);
    }
}

/* */
void Enclosure::attachRoutingDevice(RoutingDevice *pRoutingDevice)
{
    m_RoutingDevices.add(pRoutingDevice);
}

/* */
void Enclosure::addToSession(Session *pSession)
{
    Container<EndDevice> container;
    foreach (i, m_RoutingDevices) {
        (*i)->getEndDevices(container, false);
        attachEndDevices(container);
    }
    pSession->addEnclosure(this);
}

/* */
unsigned int Enclosure::getSlotNumber(unsigned long long sasAddress) const
{
    foreach (i, m_Slots)
        if ((*i)->sasAddress == sasAddress)
            return (*i)->slotNumber;
    return -1U;
}

/* */
void Enclosure::getSlotAddress(SSI_Address &address, unsigned int number)
{
    (void)address;
    (void)number;
}

/* */
void Enclosure::__get_slot_info(String &buffer)
{
    String info, right;
    unsigned int offset;
    right = buffer.after("element index");
    while (right) {
        try {
            /* find start of next info block */
            offset = right.find("element index");
        }
        catch (...) {
            offset = 0;
        }
        info = offset?right.left(offset):right;
        /* get info */
        String tmp = info.between("bay number: ", "\n");
        if (tmp) {
            Slot *pSlot = new Slot;
            pSlot->slotNumber = tmp;
            tmp = info.reverse_after("SAS address:");
            tmp = tmp.left("\n");
            pSlot->sasAddress = tmp;
            if (pSlot->sasAddress != 0)
                m_Slots.add(pSlot);
            else
                delete pSlot;
        }
        right = offset?right.get(offset+13):"";
    }
}

/* */
bool Enclosure::attachedTo(StorageObject *pObject) const
{
    foreach (i, m_RoutingDevices)
        if ((*i)->getParent() == pObject)
            return true;
    return false;
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
