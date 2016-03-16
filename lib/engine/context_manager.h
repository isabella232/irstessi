
/*
Copyright (c) 2011 - 2016, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/




#if __GNUC_PREREQ(3,4)
#pragma once
#endif /* __GNUC_PREREQ */

#ifndef __CONTEXT_H__INCLUDED__
#define __CONTEXT_H__INCLUDED__

// Forward declarations
class SessionManager;
class EventManager;
class UniqueIdManager;

// Forward declarations
class Object;
class Event;
class Session;

/* */
class ContextManager {
public:
    ContextManager();
    ~ContextManager();

public:
    SSI_Status closeSession(unsigned int id);
    unsigned int openSession();
    Session * getSession(unsigned int id) const;

    SSI_Status unregisterEvent(unsigned int id);
    unsigned int registerEvent();
    Event * getEvent(unsigned int id) const;

    void add(Object *pObejct);
    void remove(Object *pObject);
    void refresh();

    SSI_Status getSystemInfo(SSI_SystemInfo *) const;

private:
    SessionManager *m_pSessionMgr;
    EventManager *m_pEventMgr;
    UniqueIdManager *m_pUniqueIdMgr;
};

/* */
extern ContextManager *pContextMgr;

#endif /* __CONTEXT_H__INCLUDED__ */

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
