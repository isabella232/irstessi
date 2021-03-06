#!/bin/bash
<<LICENSE_BSD

Copyright 2011 - 2017 Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

LICENSE_BSD

# The function returns the list of block devices attached to the given adapter.
# $1 the name of the adapter: ahci or isci
get_attached_devices() {
    local the_result=""
    local the_path="/sys/bus/pci/drivers/$1"
    [ -d $the_path ] && for th in `ls $the_path`; do
        if [ -h "$the_path/$th/driver" ]; then
            local the_real=( `cd -P "$the_path/$th"; pwd` )
            local the_back=( `cd -P "$the_path/$th/driver"; pwd` )
            if [ "$the_path" == "$the_back" ]; then
                for dev in `ls /sys/block`; do
                    local the_device=( `cd -P "/sys/block/$dev"; pwd` )
                    case "$the_device" in
                    "$the_real"*) the_result="/dev/$dev $the_result";;
                    esac
                done
            fi
        fi
    done
    echo "$the_result"
}
