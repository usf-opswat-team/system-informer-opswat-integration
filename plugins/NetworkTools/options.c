/*
 * Copyright (c) 2022 Winsider Seminars & Solutions, Inc.  All rights reserved.
 *
 * This file is part of System Informer.
 *
 * Authors:
 *
 *     wj32    2010-2013
 *     dmex    2012-2022
 *
 */

#include "nettools.h"

INT_PTR CALLBACK OptionsDlgProc(
    _In_ HWND hwndDlg,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    )
{
    static PH_LAYOUT_MANAGER LayoutManager;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        {
            PhSetDialogItemValue(hwndDlg, IDC_PINGPACKETLENGTH, PhGetIntegerSetting(SETTING_NAME_PING_SIZE), FALSE);
            PhSetDialogItemValue(hwndDlg, IDC_MAXHOPS, PhGetIntegerSetting(SETTING_NAME_TRACERT_MAX_HOPS), FALSE);
            Button_SetCheck(GetDlgItem(hwndDlg, IDC_ENABLE_EXTENDED_TCP), PhGetIntegerSetting(SETTING_NAME_EXTENDED_TCP_STATS) ? BST_CHECKED : BST_UNCHECKED);

            PhInitializeLayoutManager(&LayoutManager, hwndDlg);
            PhAddLayoutItem(&LayoutManager, GetDlgItem(hwndDlg, IDC_DATABASE), NULL, PH_ANCHOR_TOP | PH_ANCHOR_LEFT | PH_ANCHOR_RIGHT);
            PhAddLayoutItem(&LayoutManager, GetDlgItem(hwndDlg, IDC_BROWSE), NULL, PH_ANCHOR_TOP | PH_ANCHOR_RIGHT);
        }
        break;
    case WM_DESTROY:
        {
            PhSetIntegerSetting(SETTING_NAME_PING_SIZE, PhGetDialogItemValue(hwndDlg, IDC_PINGPACKETLENGTH));
            PhSetIntegerSetting(SETTING_NAME_TRACERT_MAX_HOPS, PhGetDialogItemValue(hwndDlg, IDC_MAXHOPS));
            PhSetIntegerSetting(SETTING_NAME_EXTENDED_TCP_STATS, Button_GetCheck(GetDlgItem(hwndDlg, IDC_ENABLE_EXTENDED_TCP)) == BST_CHECKED);

            PhDeleteLayoutManager(&LayoutManager);
        }
        break;
    case WM_SIZE:
        {
            PhLayoutManagerLayout(&LayoutManager);
        }
        break;
    case WM_CTLCOLORBTN:
        return HANDLE_WM_CTLCOLORBTN(hwndDlg, wParam, lParam, PhWindowThemeControlColor);
    case WM_CTLCOLORDLG:
        return HANDLE_WM_CTLCOLORDLG(hwndDlg, wParam, lParam, PhWindowThemeControlColor);
    case WM_CTLCOLORSTATIC:
        return HANDLE_WM_CTLCOLORSTATIC(hwndDlg, wParam, lParam, PhWindowThemeControlColor);
    }

    return FALSE;
}
