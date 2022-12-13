/*
 * Copyright (c) 2022 Winsider Seminars & Solutions, Inc.  All rights reserved.
 *
 * This file is part of System Informer.
 *
 * Authors:
 *
 *     jxy-s   2022
 *
 */

#pragma once

#ifdef EXT
#undef EXT
#endif

#ifdef _DYNDATA_PRIVATE
#define EXT
#define OFFDEFAULT = ULONG_MAX
#define DYNIMPORTDEFAULT = NULL
#define DYNPTRDEFAULT = NULL
#else
#define EXT extern
#define OFFDEFAULT
#define DYNIMPORTDEFAULT
#define DYNPTRDEFAULT
#endif

EXT ULONG KphDynNtVersion;

EXT ULONG KphDynEgeGuid OFFDEFAULT;
EXT ULONG KphDynEpObjectTable OFFDEFAULT;
EXT ULONG KphDynEreGuidEntry OFFDEFAULT;
EXT ULONG KphDynHtHandleContentionEvent OFFDEFAULT;
EXT ULONG KphDynOtName OFFDEFAULT;
EXT ULONG KphDynOtIndex OFFDEFAULT;
EXT ULONG KphDynObDecodeShift OFFDEFAULT;
EXT ULONG KphDynObAttributesShift OFFDEFAULT;
EXT ULONG KphDynAlpcCommunicationInfo OFFDEFAULT;
EXT ULONG KphDynAlpcOwnerProcess OFFDEFAULT;
EXT ULONG KphDynAlpcConnectionPort OFFDEFAULT;
EXT ULONG KphDynAlpcServerCommunicationPort OFFDEFAULT;
EXT ULONG KphDynAlpcClientCommunicationPort OFFDEFAULT;
EXT ULONG KphDynAlpcHandleTable OFFDEFAULT;
EXT ULONG KphDynAlpcHandleTableLock OFFDEFAULT;
EXT ULONG KphDynAlpcAttributes OFFDEFAULT;
EXT ULONG KphDynAlpcAttributesFlags OFFDEFAULT;
EXT ULONG KphDynAlpcPortContext OFFDEFAULT;
EXT ULONG KphDynAlpcSequenceNo OFFDEFAULT;
EXT ULONG KphDynAlpcState OFFDEFAULT;

EXT PLIST_ENTRY KphDynPsLoadedModuleList DYNIMPORTDEFAULT;
EXT PERESOURCE KphDynPsLoadedModuleResource DYNIMPORTDEFAULT;
EXT PRTL_FIND_EXPORTED_ROUTINE_BY_NAME KphDynRtlFindExportedRoutineByName DYNIMPORTDEFAULT;
EXT PPS_GET_PROCESS_PROTECTION KphDynPsGetProcessProtection DYNIMPORTDEFAULT;
EXT PRTL_IMAGE_NT_HEADER_EX KphDynRtlImageNtHeaderEx DYNIMPORTDEFAULT;
EXT PKE_REMOVE_QUEUE_DPC_EX KphDynKeRemoveQueueDpcEx DYNIMPORTDEFAULT;
EXT PPS_SET_LOAD_IMAGE_NOTIFY_ROUTINE_EX KphDynPsSetLoadImageNotifyRoutineEx DYNIMPORTDEFAULT;
EXT PPS_SET_CREATE_PROCESS_NOTIFY_ROUTINE_EX2 KphDynPsSetCreateProcessNotifyRoutineEx2 DYNIMPORTDEFAULT;
EXT PPS_SET_CREATE_THREAD_NOTIFY_ROUTINE_EX KphDynPsSetCreateThreadNotifyRoutineEx DYNIMPORTDEFAULT;
EXT PPS_GET_THREAD_EXIT_STATUS KphDynPsGetThreadExitStatus DYNIMPORTDEFAULT;
EXT PCI_FREE_POLICY_INFO KphDynCiFreePolicyInfo DYNIMPORTDEFAULT;
EXT PCI_VERIFY_HASH_IN_CATALOG KphDynCiVerifyHashInCatalog DYNIMPORTDEFAULT;
EXT PCI_CHECK_SIGNED_FILE KphDynCiCheckSignedFile DYNIMPORTDEFAULT;
EXT PCI_VERIFY_HASH_IN_CATALOG_EX KphDynCiVerifyHashInCatalogEx DYNIMPORTDEFAULT;
EXT PCI_CHECK_SIGNED_FILE_EX KphDynCiCheckSignedFileEx DYNIMPORTDEFAULT;

EXT PUNICODE_STRING KphDynPortName DYNPTRDEFAULT;
EXT PUNICODE_STRING KphDynAltitude DYNPTRDEFAULT;

EXT ULONG KphDynDisableImageLoadProtection OFFDEFAULT;

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS KphDynamicDataInitialization(
    _In_ PUNICODE_STRING RegistryPath
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID KphDynamicDataCleanup(
    VOID
    );
