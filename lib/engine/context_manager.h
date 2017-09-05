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

#ifndef __CONTEXT_H__INCLUDED__
#define __CONTEXT_H__INCLUDED__

#include "handle_manager.h"
#include "event_manager.h"
#include "session_manager.h"

#ifdef SSI_HAS_PRAGMA_ONCE
#pragma once
#endif

/* */
class ContextManager {
public:
    typedef HandleManager<Object>::object_ptr object_ptr;

    ContextManager();
    ~ContextManager();

public:
    SSI_Status closeSession(SSI_Handle handle);
    SSI_Handle openSession();
    boost::shared_ptr<Session> getSession(SSI_Handle handle) const;

    SSI_Status unregisterEvent(SSI_Handle handle);
    SSI_Handle registerEvent();
    boost::shared_ptr<Event> getEvent(SSI_Handle handle) const;

    bool add(const object_ptr& pObejct);
    object_ptr remove(const object_ptr& pObject);
    object_ptr remove(Object *pObject);

    SSI_Status getSystemInfo(SSI_SystemInfo *) const;

private:
    ContextManager(const ContextManager&) { /* do not create copies */ }
    SessionManager m_SessionMgr;
    EventManager m_EventMgr;
    HandleManager<Object> m_HandleMgr;
    void operator = (const ContextManager&) {}
};

/* */
extern ContextManager *pContextMgr;

#endif /* __CONTEXT_H__INCLUDED__ */

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
