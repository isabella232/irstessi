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

#ifndef __ISCI_EXPANDER_H__INCLUDED__
#define __ISCI_EXPANDER_H__INCLUDED__

#include <typeinfo>

#include "routing_device.h"

#ifdef SSI_HAS_PRAGMA_ONCE
#pragma once
#endif

/* */
class ISCI_Expander : public RoutingDevice {
public:
    ISCI_Expander(const String &path);

    // Object
public:
    virtual bool operator ==(const Object &object) const {
        return typeid(*this) == typeid(object) &&
               static_cast<const RoutingDevice *>(&object)->getRoutingDeviceType() == getRoutingDeviceType() &&
               static_cast<const ISCI_Expander *>(&object)->getSasAddress() == m_SASAddress;
    }

    // StorageObject

public:
    virtual void discover();
    virtual boost::shared_ptr<Port> getPortByPath(const String &) const;
    virtual void getAddress(SSI_Address &) const;

    // RoutingDevice

public:
    virtual SSI_RoutingDeviceType getRoutingDeviceType() const {
        return SSI_RoutingDeviceTypeExpander;
    }

    // ISCI_Expander

protected:
    unsigned long long m_SASAddress;

public:

    unsigned long long getSasAddress() const {
        return m_SASAddress;
    }
};

#endif /* __ISCI_EXPANDER_H__INCLUDED__ */

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
