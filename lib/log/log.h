/*
Copyright (c) 2011 - 2016, Intel Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __LOG_H__INCLUDED__
#define __LOG_H__INCLUDED__

#include <features.h>

#if __GNUC_PREREQ(3, 4)
#pragma once
#endif /* __GNUC_PREREQ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/* */
enum log_level {
    LOG_FIRST = 0,
    LOG_NONE = LOG_FIRST,
    LOG_ALARM,
    LOG_NORMAL,
    LOG_DETAIL,
    LOG_DEBUG,
    LOG_ALL,
    LOG_LAST
};

/* */
#define log(__level, __message)                                 \
    do { if (log_get_level() >= (enum log_level)(__level))      \
        __log(__level, __message);                              \
    } while (0)

#define dlog(__message)                                 \
         log(LOG_DEBUG, __message);

/* */
void log_init(enum log_level level, const char *path);

/* */
void log_fini(void);

/* */
void __log(enum log_level level, const char *message);

/* */
enum log_level log_get_level(void);

/* */
void log_set_level(enum log_level level);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __LOG_H__INCLUDED__ */

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
