/*
 * IMPORTANT - READ BEFORE COPYING, INSTALLING OR USING.
 * BY LOADING OR USING THIS SOFTWARE, YOU AGREE TO THE TERMS OF THIS INTEL
 * SOFTWARE LICENSE AGREEMENT.  IF YOU DO NOT WISH TO SO AGREE, DO NOT COPY,
 * INSTALL OR USE THIS SOFTWARE.
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright (C) 2010 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#if __GNUC_PREREQ(3, 4)
#pragma once
#endif /* __GNUC_PREREQ */

#ifndef __EXCEPTION_H__INCLUDED__
#define __EXCEPTION_H__INCLUDED__

/* */
enum Exception {
    E_FIRST = 0,
    E_SUCCESS = E_FIRST,
    E_GENERIC,
    E_INVALID_SCOPE,
    E_FILE_EMPTY,
    E_PERMISSION_DENIED,
    E_NOT_FOUND,
    E_BUFFER_TOO_SMALL,
    E_OUT_OF_RESOURCES,
    E_NULL_POINTER,
    E_INVALID_SEQUENCE,
    E_INVALID_OFFSET,
    E_INVALID_OPERATION,
    E_OVERFLOW,
    E_INVALID_OBJECT,
    E_INVALID_HANDLE,
    E_OUT_OF_RANGE,
    E_INVALID_DIRECTORY,
    E_NOT_AVAILABLE,
    E_INVALID_USAGE,
    E_ARRAY_CREATE_FAILED,
    E_VOLUME_CREATE_FAILED,
    E_SYSTEM_DEVICE,
    E_INVALID_STRIP_SIZE,
    E_INVALID_NAME,
    E_INVALID_RAID_LEVEL,
    E_LAST
};

int errno_to_exception_code(int err);

#endif /* __EXCEPTION_H__INCLUDED__ */

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
