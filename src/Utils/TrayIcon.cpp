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

#include "../StdAfx.h"
#include "TrayIcon.h"

namespace DuiLib {
const int TOOLTIPTEXTSIZE = 128;
CTrayIcon::CTrayIcon(void)
{
    SecureZeroMemory(&m_trayData, sizeof(m_trayData));
    m_bEnabled = false;
    m_bVisible = false;
    m_hWnd = nullptr;
    m_hIcon = nullptr;
    m_uMessage = UIMSG_TRAYICON;
}

CTrayIcon::~CTrayIcon(void)
{
    DeleteTrayIcon();
}

void CTrayIcon::CreateTrayIcon(HWND _RecvHwnd, UINT _IconIDResource, LPCTSTR _ToolTipText, UINT _Message)
{
    if (!_RecvHwnd || _IconIDResource <= 0) {
        return;
    }
    if (_Message != 0) {
        m_uMessage = _Message;
    }
    m_hIcon = LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(_IconIDResource));
    m_trayData.cbSize = sizeof(NOTIFYICONDATA);
    m_trayData.hWnd = _RecvHwnd;
    m_trayData.uID = _IconIDResource;
    m_trayData.hIcon = m_hIcon;
    m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_trayData.uCallbackMessage = m_uMessage;
    if (_ToolTipText) {
        if (_tcslen(_ToolTipText) < TOOLTIPTEXTSIZE) {
            _tcscpy_s(m_trayData.szTip, _ToolTipText);
        }
    }

    // Error handling issues, Unchecked return value
    (void)Shell_NotifyIcon(NIM_ADD, &m_trayData);

    m_bEnabled = true;
}

void CTrayIcon::DeleteTrayIcon()
{
    // Error handling issues, Unchecked return value
    (void)Shell_NotifyIcon(NIM_DELETE, &m_trayData);

    m_bEnabled = false;
    m_bVisible = false;
    m_hWnd = nullptr;
    m_uMessage = UIMSG_TRAYICON;
}

bool CTrayIcon::SetTooltipText(LPCTSTR _ToolTipText)
{
    if (_ToolTipText) {
        if (_tcslen(_ToolTipText) < TOOLTIPTEXTSIZE) {
            _tcscpy_s(m_trayData.szTip, _ToolTipText);
        }
    }
    if (!m_bEnabled) {
        return FALSE;
    }
    m_trayData.uFlags = NIF_TIP;
    return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
}

bool CTrayIcon::SetTooltipText(UINT _IDResource)
{
    const int tooltipTextLen = 256;
    TCHAR mbuf[tooltipTextLen] = {0};
    LoadString(CPaintManagerUI::GetInstance(), _IDResource, mbuf, tooltipTextLen);
    return SetTooltipText(mbuf);
}

DuiLib::CDuiString CTrayIcon::GetTooltipText() const
{
    return m_trayData.szTip;
}

bool CTrayIcon::SetIcon(HICON _Hicon)
{
    if (_Hicon) {
        m_hIcon = _Hicon;
    }
    m_trayData.uFlags = NIF_ICON;
    m_trayData.hIcon = _Hicon;

    if (!m_bEnabled) {
        return FALSE;
    }
    return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;

    return false;
}

bool CTrayIcon::SetIcon(LPCTSTR _IconFile)
{
    HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(), _IconFile);
    return SetIcon(hIcon);
}

bool CTrayIcon::SetIcon(UINT _IDResource)
{
    HICON hIcon = LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(_IDResource));
    return SetIcon(hIcon);
}

HICON CTrayIcon::GetIcon() const
{
    HICON hIcon = m_trayData.hIcon;
    return hIcon;
}

void CTrayIcon::SetHideIcon()
{
    if (IsVisible()) {
        SetIcon((HICON)nullptr);
        m_bVisible = TRUE;
    }
}

void CTrayIcon::SetShowIcon()
{
    if (!IsVisible()) {
        SetIcon(m_hIcon);
        m_bVisible = FALSE;
    }
}

void CTrayIcon::RemoveIcon()
{
    m_trayData.uFlags = 0;

    // Error handling issues, Unchecked return value
    (void)Shell_NotifyIcon(NIM_DELETE, &m_trayData);

    m_bEnabled = FALSE;
}
}
