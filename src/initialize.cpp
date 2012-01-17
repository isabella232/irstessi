
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
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <asm/types.h>

#include <ssi.h>

#include <engine/context_manager.h>
#include <log/log.h>
#include <orom/orom.h>
#include <efi/efi.h>
#include <engine/mdadm_config.h>

/* */
ContextManager *pContextMgr = 0;

/* */
SSI_Status SsiInitialize(void)
{
    if (getuid() != 0) {
        return SSI_StatusInsufficientPrivileges;
    }
    if (pContextMgr != 0) {
        return SSI_StatusDuplicate;
    }
    log_init(LOG_NORMAL, "/var/log/ssi.log");
    orom_init();
    check_configuration();
    try {
        pContextMgr = new ContextManager();
    } catch (...) {
        return SSI_StatusInsufficientResources;
    }
    return SSI_StatusOk;
}

/* */
SSI_Status SsiFinalize(void)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    try {
        delete pContextMgr;
    } catch (...) {
        // intentionally left blank
    }
    orom_fini();
    efi_fini();
    pContextMgr = 0;
    log_fini();
    return SSI_StatusOk;
}
