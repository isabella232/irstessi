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

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <features.h>

#include <ssi.h>

#include <engine/exception.h>
#include <engine/list.h>
#include <engine/container.h>
#include <engine/string.h>
#include <engine/object.h>
#include <engine/cache.h>
#include <engine/storage_device.h>
#include <engine/end_device.h>
#include <engine/raid_device.h>
#include <engine/volume.h>
#include <engine/session.h>
#include <engine/context_manager.h>
#include <engine/array.h>
#include <engine/volume.h>

/* */
SSI_Status SsiGetVolumeHandles(SSI_Handle session, SSI_ScopeType scopeType,
    SSI_Handle scopeHandle, SSI_Handle *handleList, SSI_Uint32 *handleCount)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(session);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusInvalidSession;
    }
    ScopeObject *pScopeObject = pSession->getObject(scopeHandle);
    if (pScopeObject == 0) {
        return SSI_StatusInvalidScope;
    }
    if (*pScopeObject != scopeType) {
        return SSI_StatusInvalidScope;
    }
    Container container;
    try {
        pScopeObject->getVolumes(container);
    } catch (...) {
        return SSI_StatusInvalidScope;
    }
    return container.getHandles(handleList, handleCount);
}

/* */
SSI_Status SsiGetVolumeInfo(SSI_Handle session, SSI_Handle volumeHandle,
    SSI_VolumeInfo *volumeInfo)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(session);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusInvalidSession;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->getInfo(volumeInfo);
}

/* */
SSI_Status SsiVolumeMarkAsNormal(SSI_Handle volumeHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->markAsNormal();
}

/* */
SSI_Status SsiVolumeRebuild(SSI_Handle volumeHandle, SSI_Handle diskHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    EndDevice *pEndDevice = pSession->getEndDevice(diskHandle);
    if (pEndDevice == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->rebuild(pEndDevice);
}

/* */
SSI_Status SsiVolumeDelete(SSI_Handle volumeHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->remove();
}

/* */
SSI_Status SsiVolumeCreateFromDisks(SSI_CreateFromDisksParams params, SSI_Handle *volumeHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    if (volumeHandle == 0) {
        return SSI_StatusInvalidParameter;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume;
    Array *pArray;
    try {
        EndDevice *pEndDevice;
        Container container;
        for (unsigned int i = 0; i < params.numDisks; ++i) {
            pEndDevice = pSession->getEndDevice(params.disks[i]);
            if (pEndDevice == 0) {
                return SSI_StatusInvalidHandle;
            }
            container.add(pEndDevice);
        }
        pEndDevice = pSession->getEndDevice(params.sourceDisk);
        try {
            pArray = new Array();
        } catch (...) {
            return SSI_StatusInsufficientResources;
        }
        pArray->setEndDevices(container);
        pArray->create();
        try {
            pVolume = new Volume(pArray);
        } catch (...) {
            delete pArray;
            return SSI_StatusInsufficientResources;
        }
        pVolume->setSourceDisk(pEndDevice);
        pVolume->setEndDevices(container);
        pVolume->setComponentSize(params.sizeInBytes / container);
        pVolume->setName(params.volumeName);
        pVolume->setStripSize(params.stripSize);
        pVolume->setRaidLevel(params.raidLevel);
        pVolume->create();
        pArray->update();
        pVolume->update();
        pSession->addVolume(pVolume);
        pSession->addArray(pArray);
        *volumeHandle = pVolume->getId();
        return SSI_StatusOk;
    } catch (Exception ex) {
        delete pVolume;
        delete pArray;
        switch (ex) {
        case E_INVALID_STRIP_SIZE:
            return SSI_StatusInvalidStripSize;
        case E_INVALID_NAME:
            return SSI_StatusInvalidString;
        case E_INVALID_RAID_LEVEL:
            return SSI_StatusInvalidRaidLevel;
        default:
            return SSI_StatusFailed;
        }
    }
}

/* */
SSI_Status SsiVolumeCreate(SSI_CreateFromArrayParams params)
{
    Volume *pVolume;

    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Array *pArray = pSession->getArray(params.arrayHandle);
    if (pArray == 0) {
        return SSI_StatusInvalidHandle;
    }
    Container container;
    pArray->getEndDevices(container, true);
    try {
        try {
            pVolume = new Volume(pArray);
        } catch (...) {
            return SSI_StatusInsufficientResources;
        }
        pVolume->setEndDevices(container);
        pVolume->setComponentSize(params.sizeInBytes / container);
        pVolume->setName(params.volumeName);
        pVolume->setStripSize(params.stripSize);
        pVolume->setRaidLevel(params.raidLevel);
        pVolume->create();
        pVolume->update();
        pSession->addVolume(pVolume);
        return SSI_StatusOk;
    } catch (Exception ex) {
        delete pVolume;
        switch (ex) {
        case E_INVALID_STRIP_SIZE:
            return SSI_StatusInvalidStripSize;
        case E_INVALID_NAME:
            return SSI_StatusInvalidString;
        case E_INVALID_RAID_LEVEL:
            return SSI_StatusInvalidRaidLevel;
        default:
            return SSI_StatusFailed;
        }
    } catch (...) {
        return SSI_StatusInsufficientResources;
    }
}

/* */
SSI_Status SsiVolumeRename(SSI_Handle volumeHandle,
    const SSI_Char volumeName[SSI_VOLUME_NAME_LENGTH])
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->rename(volumeName);
}

/* */
SSI_Status SsiExpandVolume(SSI_Handle volumeHandle, SSI_Uint64 newSizeMB)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->expand(newSizeMB * 1024ULL * 1024ULL);
}

/* */
SSI_Status SsiVolumeSetCachePolicy(SSI_Handle volumeHandle,
    SSI_VolumeCachePolicy policy)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->setCachePolicy(policy == SSI_VolumeCachePolicyOff);
}

/* */
SSI_Status SsiVolumeInitialize(SSI_Handle volumeHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->initialize();
}

/* */
SSI_Status SsiVolumeVerify(SSI_Handle volumeHandle, SSI_Bool repair)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->verify(repair == SSI_TRUE);
}

/* */
SSI_Status SsiVolumeCancelVerify(SSI_Handle volumeHandle)
{
    if (pContextMgr == 0) {
        return SSI_StatusNotInitialized;
    }
    Session *pSession;
    try {
        pSession = pContextMgr->getSession(SSI_NULL_HANDLE);
    } catch (...) {
        return SSI_StatusFailed;
    }
    if (pSession == 0) {
        return SSI_StatusFailed;
    }
    Volume *pVolume = pSession->getVolume(volumeHandle);
    if (pVolume == 0) {
        return SSI_StatusInvalidHandle;
    }
    return pVolume->cancelVerify();
}

/* ex: set tabstop=4 softtabstop=4 shiftwidth=4 textwidth=80 expandtab: */
