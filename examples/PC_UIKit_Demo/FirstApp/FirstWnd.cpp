/*
 * Copyright (C) 2021. Huawei Device Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 */

#include "FirstWnd.h"

DUI_BEGIN_MESSAGE_MAP(FirstWnd, CNotifyPump)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, FirstWnd::OnClick)
    DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, FirstWnd::OnSelectMenuChanged)
DUI_END_MESSAGE_MAP()

CDuiString FirstWnd::GetSkinFile()
{
    return L"layout/FirstWnd.xml";
}

LPCTSTR FirstWnd::GetWindowClassName(void) const
{
    return L"FirstWnd";
}

void FirstWnd::OnClick(TNotifyUI &msg)
{
    CControlUI* sender = msg.pSender;
    if (sender == nullptr) {
        return;
    }

    const CDuiString name = sender->GetName();
    if (name == L"closebtn") {
        PostQuitMessage(0);
    }
}

void FirstWnd::OnSelectMenuChanged(TNotifyUI &msg)
{
    CControlUI* sender = msg.pSender;
    if (sender == nullptr) {
        return;
    }

    CTabLayoutUI* tabLayout = static_cast<CTabLayoutUI *>(m_pm.FindControl(L"tabContainer"));
    if (tabLayout == nullptr) {
        return;
    }

    const CDuiString name = sender->GetName();
    if (name == L"quickStartMenu") {
        tabLayout->SelectItem(0);
    } else if (name == L"AboutMenu") {
        tabLayout->SelectItem(1);
    }
}
