#include "onlnchk.h"

static TASKDIALOG_BUTTON TaskDialogButtonArray[] =
{
    { IDYES, L"View last analysis\nOpen the last MetaDefenderCloud analysis page" },
    { IDOK, L"Upload file\nUpload fresh sample to MetaDefenderCloud for analysis" }
};

HRESULT CALLBACK TaskDialogHashCallbackProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ LONG_PTR dwRefData
)
{
    PUPLOAD_CONTEXT context = (PUPLOAD_CONTEXT)dwRefData;

    switch (uMsg)
    {
    case TDN_NAVIGATED:
    {
        SendMessage(hwndDlg, TDM_SET_MARQUEE_PROGRESS_BAR, TRUE, 0);
        SendMessage(hwndDlg, TDM_SET_PROGRESS_BAR_MARQUEE, TRUE, 1);

        if (context->TaskbarListClass)
            ITaskbarList3_SetProgressState(context->TaskbarListClass, PhMainWndHandle, TBPF_INDETERMINATE);

        PhReferenceObject(context);
        PhQueueItemWorkQueue(PhGetGlobalWorkQueue(), UploadFileThreadStart, context);
    }
    break;
    case TDN_BUTTON_CLICKED:
    {
        if ((INT)wParam == IDCANCEL)
        {
            context->Cancel = TRUE;
            return S_FALSE;
        }
    }
    break;
    }

    return S_OK;
}

HRESULT CALLBACK TaskDialogViewHashProgressCallbackProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ LONG_PTR dwRefData
)
{
    PUPLOAD_CONTEXT context = (PUPLOAD_CONTEXT)dwRefData;

    switch (uMsg)
    {
    case TDN_NAVIGATED:
    {
        SendMessage(hwndDlg, TDM_SET_MARQUEE_PROGRESS_BAR, TRUE, 0);
        SendMessage(hwndDlg, TDM_SET_PROGRESS_BAR_MARQUEE, TRUE, 1);

        PhReferenceObject(context);
        PhQueueItemWorkQueue(PhGetGlobalWorkQueue(), ViewReportThreadStart, context);
    }
    break;
    case TDN_BUTTON_CLICKED:
    {
        if ((INT)wParam == IDCANCEL)
        {
            context->Cancel = TRUE;
            return S_FALSE;
        }
    }
    break;
    }

    return S_OK;
}

VOID ShowMetaDefenderProgressDialog(
    _In_ PUPLOAD_CONTEXT Context
)
{
    PPROCESS_DB_OBJECT object;
    //Showing user we are updating result in column
    
    object = FindMDCProcessDbObject(&Context->FileName->sr);

    object->Result = PhCreateString(L"Uploading...");

    TASKDIALOGCONFIG config;

    memset(&config, 0, sizeof(TASKDIALOGCONFIG));
    config.cbSize = sizeof(TASKDIALOGCONFIG);
    config.dwFlags = TDF_USE_HICON_MAIN | TDF_ALLOW_DIALOG_CANCELLATION | TDF_CAN_BE_MINIMIZED | TDF_EXPAND_FOOTER_AREA | TDF_ENABLE_HYPERLINKS | TDF_SHOW_PROGRESS_BAR;
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.hMainIcon = PhGetApplicationIcon(FALSE);

    config.pszWindowTitle = PhaFormatString(L"Uploading %s...", PhGetStringOrEmpty(Context->BaseFileName))->Buffer;
    config.pszMainInstruction = PhaFormatString(L"Uploading %s...", PhGetStringOrEmpty(Context->BaseFileName))->Buffer;
    config.pszContent = L"Uploaded: ~ of ~ (0%)\r\nSpeed: ~ KB/s";

    config.cxWidth = 200;
    config.lpCallbackData = (LONG_PTR)Context;
    config.pfCallback = TaskDialogHashCallbackProc;

    SendMessage(Context->DialogHandle, TDM_NAVIGATE_PAGE, 0, (LPARAM)&config);
}

VOID ShowMetaDefenderViewReportProgressDialog(
    _In_ PUPLOAD_CONTEXT Context
)
{
    TASKDIALOGCONFIG config;

    memset(&config, 0, sizeof(TASKDIALOGCONFIG));
    config.cbSize = sizeof(TASKDIALOGCONFIG);
    config.dwFlags = TDF_USE_HICON_MAIN | TDF_ALLOW_DIALOG_CANCELLATION | TDF_CAN_BE_MINIMIZED | TDF_EXPAND_FOOTER_AREA | TDF_ENABLE_HYPERLINKS | TDF_SHOW_PROGRESS_BAR;
    config.dwCommonButtons = TDCBF_CANCEL_BUTTON;
    config.hMainIcon = PhGetApplicationIcon(FALSE);

    config.pszWindowTitle = PhaFormatString(L"Locating analysis for %s...", PhGetStringOrEmpty(Context->BaseFileName))->Buffer;
    config.pszMainInstruction = PhaFormatString(L"Locating analysis for %s...", PhGetStringOrEmpty(Context->BaseFileName))->Buffer;

    config.cxWidth = 200;
    config.lpCallbackData = (LONG_PTR)Context;
    config.pfCallback = TaskDialogViewHashProgressCallbackProc;

    SendMessage(Context->DialogHandle, TDM_NAVIGATE_PAGE, 0, (LPARAM)&config);
}

HRESULT CALLBACK TaskDialogHashFoundProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    _In_ LONG_PTR dwRefData
)
{
    PUPLOAD_CONTEXT context = (PUPLOAD_CONTEXT)dwRefData;

    switch (uMsg)
    {
    case TDN_NAVIGATED:
        {
            if (context->TaskbarListClass)
            {
                ITaskbarList3_SetProgressState(context->TaskbarListClass, PhMainWndHandle, TBPF_NOPROGRESS);
            }
        }
        break;
    case TDN_BUTTON_CLICKED:
        {
            INT buttonID = (INT)wParam;

            if (buttonID == IDOK)
            {
                ShowMetaDefenderProgressDialog(context);
                return S_FALSE;
            }
            else if (buttonID == IDYES)
            {
                ShowMetaDefenderViewReportProgressDialog(context);
                return S_FALSE;
            }
        }
        break;
    case TDN_VERIFICATION_CLICKED:
        {
            BOOL verification = (BOOL)wParam;
        }
        break;
    }

    return S_OK;
}

VOID ShowHashFoundDialog(
    _In_ PUPLOAD_CONTEXT Context
)
{
    PPH_STRING mainInstruction = NULL;
    TASKDIALOGCONFIG config;
    if (PhIsNullOrEmptyString(Context->LastAnalysisAgo))
    {
        mainInstruction = PhFormatString(L"Missing Information for Item");
        Context->LastAnalysisAgo = PhFormatString(L"%s", L"Unknown");
        Context->LastAnalysisDate = PhFormatString(L"%s", L"Unknown");
    }
    else
    {
        mainInstruction = PhFormatString(
            L"%s was last analyzed %s day(s) ago",
            PhGetStringOrEmpty(Context->BaseFileName),
            PhGetStringOrEmpty(Context->LastAnalysisAgo)
        );
    }
    memset(&config, 0, sizeof(TASKDIALOGCONFIG));
    config.cbSize = sizeof(TASKDIALOGCONFIG);
    config.dwFlags = TDF_USE_HICON_MAIN | TDF_ALLOW_DIALOG_CANCELLATION | TDF_CAN_BE_MINIMIZED | TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS;
    config.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    config.hMainIcon = PhGetApplicationIcon(FALSE);
    config.pszMainInstruction = mainInstruction->Buffer;
    // was last analyzed by VirusTotal on 2016-12-28 05:26:50 UTC (1 hour ago) it was first analyzed by VirusTotal on 2016-12-12 17:08:19 UTC.
    config.pszContent = PhaFormatString(
        L"Detection ratio: %s/%s\r\nLast analyzed: %s\r\nUpload size: %s\r\n\r\nYou can take a look at the last analysis or upload it again now.",
        PhGetStringOrEmpty(Context->Detected),
        PhGetStringOrEmpty(Context->MaxDetected),
        PhGetStringOrEmpty(Context->LastAnalysisDate),
        PhGetStringOrEmpty(Context->FileSize)
    )->Buffer;
    config.pszVerificationText = L"Remember this selection...";
    config.pButtons = TaskDialogButtonArray;
    config.cButtons = ARRAYSIZE(TaskDialogButtonArray);
    config.lpCallbackData = (LONG_PTR)Context;
    config.pfCallback = TaskDialogHashFoundProc;

    PhDereferenceObject(mainInstruction);

    SendMessage(Context->DialogHandle, TDM_NAVIGATE_PAGE, 0, (LPARAM)&config);
}

