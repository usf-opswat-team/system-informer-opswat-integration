#include "onlnchk.h"
#include "metadefender.h"

typedef struct Date {
    int d, m, y;
}date;

const int numDayPerMonth[12]
= { 31, 28, 31, 30, 31, 30,
   31, 31, 30, 31, 30, 31 };

//HANDLE VirusTotalHandle = NULL;
HANDLE MDCHandle = NULL;
//PPH_LIST VirusTotalList = NULL;
PPH_LIST MDCList = NULL;
PH_QUEUED_LOCK MDCProcessListLock = PH_QUEUED_LOCK_INIT;

PPH_STRING MetaDefenderStringToTime(
    _In_ PPH_STRING Time
)
{
    PPH_STRING result = NULL;
    SYSTEMTIME time = { 0 };
    SYSTEMTIME localTime = { 0 };
    INT count;

    count = swscanf(
        PhGetString(Time),
        L"%hu-%hu-%huT%hu:%hu:%hu",
        &time.wYear,
        &time.wMonth,
        &time.wDay,
        &time.wHour,
        &time.wMinute,
        &time.wSecond
    );

    if (count == 6)
    {
        if (SystemTimeToTzSpecificLocalTime(NULL, &time, &localTime))
        {
            result = PhFormatDateTime(&localTime);
        }
    }

    return result;
}

INT countLeapYears(
    _In_ date d
)
{
    INT years = d.y;

    if (d.m <= 2)
        years--;

    return years / 4
        - years / 100
        + years / 400;
}

PPH_STRING getDifference(
    _In_ date dt1,
    _In_ date dt2
)
{
    PPH_STRING difference;
    ULONG n1 = dt1.y * 365 + dt1.d;

    for (INT i = 0; i < dt1.m - 1; i++)
        n1 += numDayPerMonth[i];

    n1 += countLeapYears(dt1);

    ULONG n2 = dt2.y * 365 + dt2.d;
    for (INT i = 0; i < dt2.m - 1; i++)
        n2 += numDayPerMonth[i];
    n2 += countLeapYears(dt2);

    difference = PhFormatString(
        L"%d",
        (n2 - n1)
    );

    return difference;
}

PPH_STRING MetaDefenderNumDaysSince(
    _In_ PPH_STRING Time
)
{
    date prev;
    INT count;
    count = swscanf(
        PhGetString(Time),
        L"%d-%d-%dT",
        &prev.y,
        &prev.m,
        &prev.d
    );

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    date now = { tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900 };
    return getDifference(prev, now);
}

PMDC_FILE_HASH_ENTRY MDCAddCacheResult(
    _In_ PPH_STRING FileName,
    _In_ PPROCESS_EXTENSION Extension
)
{
    PMDC_FILE_HASH_ENTRY result;

    result = PhAllocateZero(sizeof(MDC_FILE_HASH_ENTRY));
    result->FileName = PhReferenceObject(FileName);
    result->FileNameAnsi = PhConvertUtf16ToMultiByte(PhGetString(FileName));
    result->Extension = Extension;

    PhAcquireQueuedLockExclusive(&MDCProcessListLock);
    PhAddItemList(MDCList, result);
    PhReleaseQueuedLockExclusive(&MDCProcessListLock);

    return result;
}

VOID MDCRemoveCacheResult(
    _In_ PPROCESS_EXTENSION Extension
)
{
    PhAcquireQueuedLockExclusive(&MDCProcessListLock);

    for (ULONG i = 0; i < MDCList->Count; i++)
    {
        PMDC_FILE_HASH_ENTRY extension = MDCList->Items[i];

        PhRemoveItemList(MDCList, i);

        PhClearReference(&extension->FileName);
        PhClearReference(&extension->FileHash);
        PhClearReference(&extension->FileNameAnsi);
        PhClearReference(&extension->FileHashAnsi);
        PhClearReference(&extension->CreationTime);

        PhFree(extension);
    }

    //PhClearList(VirusTotalList);
    //PhDereferenceObject(VirusTotalList);

    PhReleaseQueuedLockExclusive(&MDCProcessListLock);
}

PMDC_FILE_HASH_ENTRY MDCGetCachedResult(
    _In_ PPH_STRING FileName
)
{
    ULONG i;
    BOOLEAN found = FALSE;

    PhAcquireQueuedLockExclusive(&MDCProcessListLock);

    for (i = 0; i < MDCList->Count; i++)
    {
        PMDC_FILE_HASH_ENTRY extension = MDCList->Items[i];

        if (PhEqualString(extension->FileName, FileName, TRUE))
        {
            PhReleaseQueuedLockExclusive(&MDCProcessListLock);
            return extension;
        }
    }

    PhReleaseQueuedLockExclusive(&MDCProcessListLock);
    return NULL;
}

PPH_BYTES MDCTimeString(
    _In_ PLARGE_INTEGER LargeInteger
)
{
    SYSTEMTIME systemTime;
    PPH_STRING dateString;
    PPH_STRING timeString;
    PPH_BYTES result;

    PhLargeIntegerToLocalSystemTime(&systemTime, LargeInteger);
    dateString = PhFormatDate(&systemTime, L"yyyy-MM-dd");
    timeString = PhFormatTime(&systemTime, L"HH:mm:ss");

    result = PhFormatBytes("%S %S", dateString->Buffer, timeString->Buffer);

    PhDereferenceObject(timeString);
    PhDereferenceObject(dateString);

    return result;
}

PPH_STRING MDCBuildJsonArray(
    _In_ PMDC_FILE_HASH_ENTRY Entry,
    _Inout_ PPH_STRING jsonString
)
{
    HANDLE fileHandle;
    FILE_NETWORK_OPEN_INFORMATION fileAttributeInfo;
    PPH_STRING hashString = NULL;
    
    
    

    if (NT_SUCCESS(PhQueryFullAttributesFileWin32(
        Entry->FileName->Buffer,
        &fileAttributeInfo
    )))
    {
        Entry->CreationTime = MDCTimeString(&fileAttributeInfo.CreationTime);
    }

    if (NT_SUCCESS(PhCreateFileWin32(
        &fileHandle,
        Entry->FileName->Buffer,
        FILE_GENERIC_READ,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT // FILE_OPEN_FOR_BACKUP_INTENT
    )))
    {
        if (NT_SUCCESS(HashFileAndResetPosition(
            fileHandle,
            &fileAttributeInfo.EndOfFile,
            Sha256HashAlgorithm,
            &hashString
        )))
        {
            Entry->FileHash = hashString;
            Entry->FileHashAnsi = PhConvertUtf16ToMultiByte(hashString->Buffer);
            jsonString = PhConcatStrings2(jsonString->Buffer, hashString->Buffer);   
            
        }

        NtClose(fileHandle);
    }

    return jsonString;
}

PPH_LIST MDCJsonToResultList(
    _In_ PVOID JsonObject
)
{
    ULONG i;
    ULONG arrayLength;
    PPH_LIST results;

    if (!(arrayLength = PhGetJsonArrayLength(JsonObject)))
        return NULL;

    results = PhCreateList(arrayLength);

    for (i = 0; i < arrayLength; i++)
    {
        PMDC_API_RESULT result;
        PVOID jsonArrayObject;

        if (!(jsonArrayObject = PhGetJsonArrayIndexObject(JsonObject, i)))
            continue;

        PVOID jsonScanDetailsObject;
        INT64 avs_engines;
        if (!(jsonScanDetailsObject = PhGetJsonObject(jsonArrayObject, "scan_details")))
            continue;

        if (!(avs_engines = PhGetJsonObjectLength(jsonScanDetailsObject)))
            avs_engines = 0;
        
        
        result = PhAllocateZero(sizeof(MDC_API_RESULT));
        result->FileHash = PhGetJsonValueAsString(jsonArrayObject, "hash");
        result->Positives = PhGetJsonValueAsUInt64(jsonArrayObject, "total_detected_avs");
        result->Total = avs_engines;

        PhAddItemList(results, result);
    }

    return results;
}

PPH_BYTES MDCGetCachedDbHash(
    _In_ PPH_STRINGREF CachedHash
)
{
    ULONG length;
    PUCHAR buffer;
    PPH_BYTES string;

    length = (ULONG)CachedHash->Length / sizeof(WCHAR) / 2;

    buffer = PhAllocate(length + 1);
    memset(buffer, 0, length + 1);

    PhHexStringToBuffer(CachedHash, buffer);

    string = PhCreateBytes(buffer);

    for (SIZE_T i = 0; i < string->Length; i++)
        string->Buffer[i] = string->Buffer[i] ^ 0x0D06F00D;

    PhFree(buffer);
    return string;
}

PPH_BYTES MDCSendHttpRequest(
    _In_ PPH_BYTES jsonString
)
{
    PPH_BYTES subRequestBuffer = NULL;
    PPH_HTTP_CONTEXT httpContext = NULL;
    PPH_STRING versionString = NULL;
    PPH_STRING userAgentString = NULL;
    PPH_STRING urlPathString = NULL;
    ULONG totalBytes = 0;
    PH_STRING_BUILDER httpRequestHeaders = { 0 };

    versionString = PhGetPhVersion();
    userAgentString = PhConcatStrings2(L"ProcessHacker_", versionString->Buffer);

    if (!PhHttpSocketCreate(&httpContext, PhGetString(userAgentString)))
        goto CleanupExit;

    if (!PhHttpSocketConnect(httpContext, L"api.metadefender.com", PH_HTTP_DEFAULT_HTTPS_PORT))
        goto CleanupExit;

    {
        PPH_BYTES resourceString = MDCGetCachedDbHash(&ProcessObjectDbHash); //resource string is hash


        //make Post/hash for batch hashes 
        urlPathString = PhFormatString(
            L"%s%s",
            L"/v4",
            L"/hash"
        );

        PhClearReference(&resourceString);
    }

    if (!PhHttpSocketBeginRequest(
        httpContext,
        L"POST",
        urlPathString->Buffer,
        PH_HTTP_FLAG_REFRESH | PH_HTTP_FLAG_SECURE
    ))
    {
        goto CleanupExit;
    }

    PPH_BYTES serviceHash;

    //Get MetaDefender API key from setting or user
    serviceHash = MetaDefenderGetApiKey(PhMainWindowHandle);

    PhInitializeStringBuilder(&httpRequestHeaders, DOS_MAX_PATH_LENGTH);

    PhAppendFormatStringBuilder(
        &httpRequestHeaders,
        L"\x0061\x0070\x0069\x006B\x0065\x0079:%hs\r\n",
        serviceHash->Buffer
    );

    if (!PhHttpSocketAddRequestHeaders(httpContext, httpRequestHeaders.String->Buffer, (ULONG)httpRequestHeaders.String->Length / sizeof(WCHAR)))
        goto CleanupExit;

    if (!PhHttpSocketAddRequestHeaders(httpContext, L"Content-Type: application/json", 0))
        goto CleanupExit;

    if (!PhHttpSocketAddRequestHeaders(httpContext, L"includescandetails: 1", 0))
        goto CleanupExit;

    if (!PhHttpSocketSendRequest(httpContext, jsonString->Buffer, (ULONG)jsonString->Length))
        goto CleanupExit;

    if (!PhHttpSocketEndRequest(httpContext))
        goto CleanupExit;

    if (!(subRequestBuffer = PhHttpSocketDownloadString(httpContext, FALSE)))
        goto CleanupExit;

CleanupExit:

    if (httpContext)
        PhHttpSocketDestroy(httpContext);

    PhClearReference(&urlPathString);
    PhClearReference(&versionString);
    PhClearReference(&userAgentString);

    if (jsonString)
        PhDereferenceObject(jsonString);

    return subRequestBuffer;
}

PMDC_FILE_HASH_ENTRY MDCGetCachedResultFromHash(
    _In_ PPH_STRING FileHash
)
{
    ULONG i;
    BOOLEAN found = FALSE;

    PhAcquireQueuedLockExclusive(&MDCProcessListLock);

    for (i = 0; i < MDCList->Count; i++)
    {
        PMDC_FILE_HASH_ENTRY extension = MDCList->Items[i];
        

        if (PhIsNullOrEmptyString(extension->FileHash))
            continue;

        if (PhEqualString(extension->FileHash, FileHash, TRUE)) //we are passing in a null FileHash casuing error
        {
            PhReleaseQueuedLockExclusive(&MDCProcessListLock);
            return extension;
        }
    }

    PhReleaseQueuedLockExclusive(&MDCProcessListLock);
    return NULL;
}



//Thread function that performs all scanning
NTSTATUS NTAPI MDCProcessApiThread(
    _In_ PVOID Parameter
)
{
    // TODO: Workqueue support.
    PhSetThreadBasePriority(NtCurrentThread(), THREAD_PRIORITY_LOWEST);
    PhSetThreadIoPriority(NtCurrentThread(), IoPriorityVeryLow);

    PhDelayExecution(10 * 1000); //delay

    do
    {
        ULONG i;
        //INT64 resultLength;
        PPH_BYTES jsonApiResult = NULL;
        //PPH_STRING jsonString = NULL;
        PWSTR empty = L"{\"hash\": [";
        PVOID rootJsonObject = NULL;
        PVOID dataJsonObject; 
        PPH_STRING jsonArrayToSendString = NULL;
        PPH_LIST resultTempList = NULL;
        PPH_LIST MDCResults = NULL;

        //jsonArray = PhCreateJsonArray();
        PPH_STRING jsonString = PhCreateString(empty);
        resultTempList = PhCreateList(30);

        PhAcquireQueuedLockExclusive(&MDCProcessListLock); //aquire lock to add process to list

        //Where does VirusTotalList get initialized????
        //I think VirustTotalList holds the # of processes?
        for (i = 0; i < MDCList->Count; i++) //VirusTotalList is a PPH_LIST which holds ULONG count
        {
            PMDC_FILE_HASH_ENTRY extension = MDCList->Items[i]; //Items is a PVOID*

            //resultTempList gets value from function PhAddItemList at end of loop iteration
            if (resultTempList->Count >= 30)
                break;

            if (!extension->Stage1)
            {
                extension->Stage1 = TRUE;

                PhAddItemList(resultTempList, extension); //add item to list?
            }
            
        }

        PhReleaseQueuedLockExclusive(&MDCProcessListLock); //release lock, process added to list

        if (resultTempList->Count == 0) //list is empty no processes added...or no new process addedd
        {
            PhDelayExecution(30 * 1000); // Wait 30 seconds
            goto CleanupExit;
        }

        for (i = 0; i < resultTempList->Count; i++) //for the new processes found 
        {
            jsonString = PhConcatStrings2(jsonString->Buffer, L"\"");
            jsonString = MDCBuildJsonArray(resultTempList->Items[i], jsonString); //get the process and make a string of hashes
            if (i == resultTempList->Count - 1) {
                break;
            }
            jsonString = PhConcatStrings2(jsonString->Buffer, L"\",");
        }
        jsonString = PhConcatStrings2(jsonString->Buffer, L"\"]}");


        if (!(jsonApiResult = MDCSendHttpRequest(PhConvertUtf16ToUtf8(jsonString->Buffer))))
            goto CleanupExit;

        if (!(rootJsonObject = PhCreateJsonParser(jsonApiResult->Buffer)))
            goto CleanupExit;

        if (!(dataJsonObject = PhGetJsonObject(rootJsonObject, "data"))) //at the end they get key data
            goto CleanupExit;

        //if (!(resultLength = PhGetJsonValueAsUInt64(rootJsonObject, "result"))) //and they get key result
          //  goto CleanupExit;
        ////////////////////// Done Trying to get rootJsonObject ////////////////////////////////////////

        if (MDCResults = MDCJsonToResultList(dataJsonObject)) //JSON to virusTotalResults
        {
            for (i = 0; i < MDCResults->Count; i++)
            {
                PMDC_API_RESULT result = MDCResults->Items[i];

                if (!PhIsNullOrEmptyString(result->FileHash))
                {
                    PMDC_FILE_HASH_ENTRY entry = MDCGetCachedResultFromHash(result->FileHash); //get fileHash

                    if (entry && !entry->Processed) //if entry is not null and not processed
                    {
                        entry->Processed = TRUE;  //mark entry as processed
                        entry->Positives = result->Positives;
                        entry->Total = result->Total;
                        
                        if (!FindMDCProcessDbObject(&entry->FileName->sr))
                        {
                            //we would use MDC DbObject 
                            CreateMDCProcessDbObject( //call that thing christopher was working on
                                entry->FileName,
                                entry->Positives,
                                entry->Total,
                                result->FileHash
                            );
                        }
                    }
                }
                
            }
        }

    CleanupExit:

        if (MDCResults)
        {
            for (i = 0; i < MDCResults->Count; i++)
            {
                PMDC_API_RESULT result = MDCResults->Items[i];

                //    PhClearReference(&result->Permalink);
                //    PhClearReference(&result->FileHash);
                //    PhClearReference(&result->DetectionRatio);

                PhFree(result);
            }

            PhDereferenceObject(MDCResults);
        }

        if (rootJsonObject)
        {
            PhFreeJsonObject(rootJsonObject);
        }

        if (jsonArrayToSendString)
            PhDereferenceObject(jsonArrayToSendString);

      //  if (jsonString)
//{
     //       PhDereferenceObject(jsonString);
      //  }

        if (jsonApiResult)
        {
            PhDereferenceObject(jsonApiResult);
        }

        if (resultTempList)
        {
            // Re-queue items without any results from VirusTotal.
            //for (i = 0; i < resultTempList->Count; i++)
            //{
            //    PVIRUSTOTAL_FILE_HASH_ENTRY result = resultTempList->Items[i];
            //    PPROCESS_EXTENSION extension = result->Extension;
            //
            //    if (extension->Retries > 3)
            //        continue;
            //
            //    if (PhIsNullOrEmptyString(result->FileResult))
            //    {
            //        extension->Stage1 = FALSE;
            //    }
            //
            //    extension->Retries++;
            //}

            PhDereferenceObject(resultTempList);
        }

        PhDelayExecution(5 * 1000); // Wait 5 seconds

    } while (MDCHandle);

    return STATUS_SUCCESS;
}


//This initializes the procoess monitoring of the process list
VOID InitializeMDCProcessMonitor(
    VOID
)
{
    MDCList = PhCreateList(100);

    PhCreateThreadEx(&MDCHandle, MDCProcessApiThread, NULL); //ApiThread is our work horse for scanning
}

VOID CleanupMDCProcessMonitor(
    VOID
)
{
    if (MDCHandle)
    {
        NtClose(MDCHandle);
        MDCHandle = NULL;
    }

    if (MDCList)
    {
        PhDereferenceObject(MDCList);
    }
}

//Prompt the user for an MetaDefender Cloud API key
PPH_BYTES MetaDefenderGetApiKey(
    _In_ HWND hDlg
)
{
    PPH_BYTES string;
    PPH_STRING key = NULL;

    key = PhGetStringSetting(SETTING_NAME_METADEFENDER_API_KEY);

    if (PhIsNullOrEmptyString(key))
    {
        //TODO: User input validation
        if (PhaChoiceDialog(
            hDlg,
            L"MetaDefender API Key",
            L"MetaDefender API Key:",
            NULL,
            0,
            NULL,
            PH_CHOICE_DIALOG_USER_CHOICE,
            &key,
            NULL,
            NULL
        ))
        {
            PhSetStringSetting(SETTING_NAME_METADEFENDER_API_KEY, PhGetString(key));
        }
        else
        {
            PhSetStringSetting(SETTING_NAME_METADEFENDER_API_KEY, L"");
        }
    }

    string = PhConvertUtf16ToMultiByte(PhGetStringOrEmpty(key));
    return string;
}
