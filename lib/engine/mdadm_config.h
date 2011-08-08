/*
Copyright (c) 2011, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if __GNUC_PREREQ(3, 4)
#pragma once
#endif /* __GNUC_PREREQ */

#ifndef __MDADM_CONFIG_H__INCLUDED__
#define __MDADM_CONFIG_H__INCLUDED__


#define MDADM_CONFIG_PATH "/etc/mdadm.conf"
static String stdConfig = "# This config has been automatically generated by SSI API library\n"
                          "# for use with CIM Provider. Please do not edit\n"
                          "\n"
                          "MAILADDR root\n"
                          "AUTO +imsm +1.x -all\n"
                          "policy domain=global metadata=imsm path=* action=spare-same-slot\n";

bool correct_config(String &config);
void backup_config(String &config);
int write_config(String file, String &config);
int restart_monitor();
bool monitor_running();
void check_configuration();

#endif /* __MDADM_CONFIG_H__INCLUDED__ */
