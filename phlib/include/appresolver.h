/*
 * Copyright (c) 2022 Winsider Seminars & Solutions, Inc.  All rights reserved.
 *
 * This file is part of System Informer.
 *
 * Authors:
 *
 *     dmex    2017-2018
 *
 */

#ifndef _PH_APPRESOLVER_H
#define _PH_APPRESOLVER_H

#ifdef __cplusplus
extern "C" {
#endif

_Success_(return)
BOOLEAN PhAppResolverGetAppIdForProcess(
    _In_ HANDLE ProcessId,
    _Out_ PPH_STRING *ApplicationUserModelId
    );

_Success_(return)
BOOLEAN PhAppResolverGetAppIdForWindow(
    _In_ HWND WindowHandle,
    _Out_ PPH_STRING *ApplicationUserModelId
    );

HRESULT PhAppResolverActivateAppId(
    _In_ PPH_STRING AppUserModelId,
    _In_opt_ PWSTR CommandLine,
    _Out_opt_ HANDLE *ProcessId
    );

typedef struct _PH_PACKAGE_TASK_ENTRY
{
    PPH_STRING TaskName;
    GUID TaskGuid;
} PH_PACKAGE_TASK_ENTRY, *PPH_PACKAGE_TASK_ENTRY;

PPH_LIST PhAppResolverEnumeratePackageBackgroundTasks(
    _In_ PPH_STRING PackageFullName
    );

HRESULT PhAppResolverPackageStopSessionRedirection(
    _In_ PPH_STRING PackageFullName
    );

PPH_STRING PhGetAppContainerName(
    _In_ PSID AppContainerSid
    );

PPH_STRING PhGetAppContainerSidFromName(
    _In_ PWSTR AppContainerName
    );

PPH_STRING PhGetAppContainerPackageName(
    _In_ PSID Sid
    );

PPH_STRING PhGetProcessPackageFullName(
    _In_ HANDLE ProcessHandle
    );

BOOLEAN PhIsTokenFullTrustAppPackage(
    _In_ HANDLE TokenHandle
    );

BOOLEAN PhIsPackageCapabilitySid(
    _In_ PSID AppContainerSid,
    _In_ PSID Sid
    );

PPH_STRING PhGetPackagePath(
    _In_ PPH_STRING PackageFullName
    );

PPH_LIST PhGetPackageAssetsFromResourceFile(
    _In_ PWSTR FilePath
    );

// Immersive PLM task support

HRESULT PhAppResolverBeginCrashDumpTask(
    _In_ HANDLE ProcessId,
    _Out_ HANDLE* TaskHandle
    );

HRESULT PhAppResolverBeginCrashDumpTaskByHandle(
    _In_ HANDLE ProcessHandle,
    _Out_ HANDLE* TaskHandle
    );

HRESULT PhAppResolverEndCrashDumpTask(
    _In_ HANDLE TaskHandle
    );

#ifdef __cplusplus
}
#endif

#endif
