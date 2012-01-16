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
#include <stddef.h>

#include <ssi.h>
#include <log/log.h>

#include "exception.h"
#include "string.h"
#include "list.h"
#include "container.h"
#include "filesystem.h"
#include "utils.h"
#include "mdadm_config.h"


#define MDADM_CONFIG_PATH "/etc/mdadm.conf"
static String stdConfig = "# This config has been automatically generated by SSI API library\n"
                          "# for use with CIM Provider. Please do not edit\n"
                          "\n"
                          "MAILADDR root\n"
                          "AUTO +imsm +1.x -all\n"
                          "policy domain=global metadata=imsm path=* action=spare-same-slot\n";

/* */
static bool correct_config(String &config) {
    return config + "\n" == stdConfig;
}

static int write_config(String file, String &config)
{
    SysfsAttr attr = file;
    try {
        attr << config;
    } catch (...) {
        dlog("Warning: file %s could not be written", file.get());
        return 1;
    }
    return 0;
}

static void backup_config(String &config)
{
    write_config("/etc/mdadm.conf.original", config);
}

static int restart_monitor()
{
    SysfsAttr attr = String("/var/run/mdadm/autorebuild.pid");
    String pid, s = "ssimsg";
    try {
        attr >> pid;
        if (shell("kill -n 15 " + pid) == 0)
            dlog("killed Monitor: " + pid);
    } catch (...) {
        /*ok new monitor will be started anyway*/
    }
    try {
        if (shell_cap("whereis -b ssimsg", s) == 0)
            s = s.reverse_after(" ");
    } catch (...) {
    }
    dlog("starting Monitor...");
    return shell("mdadm --monitor -y --scan --daemonise -p " + s);
}

static bool monitor_running()
{
    SysfsAttr attr = String("/var/run/mdadm/autorebuild.pid");
    String pid, buffer;
    try {
        attr >> pid;
    } catch (...) {
        return false;
    }
    if (shell_cap("ps " + pid, buffer) == 0) {
        try {
            buffer.find("mdadm");
        } catch (...) {
            return false;
        }
        try {
            buffer.find("ssimsg");
        } catch (...) {
            return false;
        }
        return true;
    }
    return false;
}

/* */
void check_configuration()
{
    SysfsAttr attr = String(MDADM_CONFIG_PATH);
    String config;
    bool configOk = false;
    bool backup = true;

    /*check mdadm.conf*/
    try {
        attr >> config;
        configOk = correct_config(config);
    } catch (Exception ex) {
        if (ex != E_NOT_FOUND)
            dlog("Warning: mdadm config file cannot be read, new one will be written");
            backup = false;
    }

    if (configOk && monitor_running()) {
        dlog("Configuration correct");
        return;
    }
    if (!configOk) {
        if (backup)
            backup_config(config);
        if (write_config(MDADM_CONFIG_PATH, stdConfig) != 0)
            dlog("Warning: failed to update mdadm.conf");
    }
    if (restart_monitor() == 0)
        dlog("Monitor restarted successfully")
    else
        dlog("Error starting Monitor");
}

void get_mdadm_version(char *buffer, size_t size)
{
    String ver;
    if (!buffer || size == 0)
        return;
    if (shell_cap("mdadm --version 2>&1", ver) == 0) {
        ver = ver.between("- v", " ");
        check_dots(ver);
        ver.get(buffer, size);
    }
}
/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=98 expandtab: */
