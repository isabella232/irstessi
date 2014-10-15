
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

#include <ssi.h>

#include <engine/exception.h>
#include <engine/container.h>
#include <engine/string.h>
#include <engine/object.h>
#include <engine/context_manager.h>
#include <engine/event.h>

/* */
SSI_Status SsiGetEventHandle(SSI_Handle *eventHandle)
{
    if (pContextMgr == NULL) {
        return SSI_StatusNotInitialized;
    }
    if (eventHandle == NULL) {
        return SSI_StatusInvalidParameter;
    }
    try {
        *eventHandle = pContextMgr->registerEvent();
        if (*eventHandle) {
            return SSI_StatusOk;
        }
        return SSI_StatusInsufficientResources;
    } catch (...) {
        return SSI_StatusFailed;
    }
}

/* */
SSI_Status SsiFreeEventHandle(SSI_Handle eventHandle)
{
    if (pContextMgr == NULL) {
        return SSI_StatusNotInitialized;
    }
    try {
        return pContextMgr->unregisterEvent(eventHandle);
    } catch (...) {
        return SSI_StatusFailed;
    }
}

/* */
SSI_Status SsiEventWait(SSI_Uint32 timeout, SSI_Handle eventHandle)
{
    if (pContextMgr == NULL) {
        return SSI_StatusNotInitialized;
    }
    Event *pEvent;
    try {
        pEvent = pContextMgr->getEvent(eventHandle);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pEvent == NULL) {
        return SSI_StatusInvalidHandle;
    }
    return pEvent->wait(timeout);
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
