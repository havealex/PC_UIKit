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

#ifndef __UITRAICON_H__
#define __UITRAICON_H__

#pragma once
#include <ShellAPI.h>

namespace DuiLib {
class UILIB_API CTrayIcon {
public:
    CTrayIcon(void);
    ~CTrayIcon(void);

public:
    void CreateTrayIcon(HWND _RecvHwnd, UINT _IconIDResource, LPCTSTR _ToolTipText = nullptr, UINT _Message = NULL);
    void DeleteTrayIcon();
    bool SetTooltipText(LPCTSTR _ToolTipText);
    bool SetTooltipText(UINT _IDResource);
    CDuiString GetTooltipText() const;

    bool SetIcon(HICON _Hicon);
    bool SetIcon(LPCTSTR _IconFile);
    bool SetIcon(UINT _IDResource);
    HICON GetIcon() const;
    void SetHideIcon();
    void SetShowIcon();
    void RemoveIcon();
    bool Enabled()
    {
        return m_bEnabled;
    };
    bool IsVisible()
    {
        return !m_bVisible;
    };

private:
    bool m_bEnabled;
    bool m_bVisible;
    HWND m_hWnd;
    UINT m_uMessage;
    HICON m_hIcon;
    NOTIFYICONDATA m_trayData;
};
}
#endif //
