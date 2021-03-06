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

#if (HAVE_CONFIG_H == 1)
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include "session_manager.h"

using std::nothrow;
using boost::shared_ptr;

/* */
SessionManager::SessionManager()
{
}

/* */
shared_ptr<Session> SessionManager::getSession(SSI_Handle handle) const
{
    return m_sessions.at(handle);
}

/* */
SSI_Handle SessionManager::openSession()
{
    shared_ptr<Session> pSession = shared_ptr<Session>(new(nothrow) Session());

    if (!pSession) {
        return SSI_NULL_HANDLE;
    }

    if (!m_sessions.insert(pSession).second) { /* Out of resources */
        return SSI_NULL_HANDLE;
    }

    try {
        pSession->initialize();
    } catch (...) {
        m_sessions.remove(pSession->getHandle());
        return SSI_NULL_HANDLE;
    }

    return pSession->getHandle();
}

/* */
SSI_Status SessionManager::closeSession(SSI_Handle handle)
{
    if (handle == SSI_NULL_HANDLE) {
        return SSI_StatusInvalidParameter;
    }

    shared_ptr<Session> pSession = m_sessions.remove(handle);

    if (pSession == NULL) {
        return SSI_StatusInvalidHandle;
    }

    return SSI_StatusOk;
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
