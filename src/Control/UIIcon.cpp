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
#include "UIIcon.h"

namespace DuiLib {
CIconUI::CIconUI(void)
{
    SecureZeroMemory(&m_rcInset, sizeof(m_rcInset));
    SetAttribute(_T("bkcolor"), _T("#00000000"));
}

CIconUI::~CIconUI(void) {}

CControlUI *CIconUI::CreateControl(void)
{
    return new CIconUI();
}

LPCTSTR CIconUI::GetClass() const
{
    return _T("IconUI");
}

LPVOID CIconUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_ICON) == 0) {
        return static_cast<CIconUI *>(this);
    }
    return __super::GetInterface(pstrName);
}

void CIconUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    const int radix = 10;
    if (_tcscmp(pstrName, _T("icon")) == 0) {
        m_sIcoImage = pstrValue;
    }
    if (_tcscmp(pstrName, _T("defaulticon")) == 0) {
        m_sDefaultIcon = pstrValue;
    }
    if (_tcscmp(pstrName, _T("inset")) == 0) {
        RECT rcInset = { 0 };
        LPTSTR pstr = nullptr;
        rcInset.left = _tcstol(pstrValue, &pstr, radix);
        ASSERT(pstr);
        rcInset.top = _tcstol(pstr + 1, &pstr, radix);
        ASSERT(pstr);
        rcInset.right = _tcstol(pstr + 1, &pstr, radix);
        ASSERT(pstr);
        rcInset.bottom = _tcstol(pstr + 1, &pstr, radix);
        ASSERT(pstr);
        SetInset(rcInset);
    }
    Invalidate();
    __super::SetAttribute(pstrName, pstrValue);
}

void CIconUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);
}

void CIconUI::SetInset(RECT rc)
{
    m_rcInset = rc;
}

void CIconUI::DoEvent(TEventUI &event)
{
    CControlUI::DoEvent(event);
}

void CIconUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
}

void CIconUI::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);
}

void CIconUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = nullptr;
    HICON hIcon = nullptr;
    RECT rcTemp = { 0 };
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) {
        return;
    }

    SHFILEINFO FileInfo;
    SecureZeroMemory(&FileInfo, sizeof(FileInfo));
    (void)::SHGetFileInfo(m_sIcoImage, 0, &FileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
    hIcon = FileInfo.hIcon;
    if (hIcon == nullptr) {
        m_sIcoImage.Replace(L".exe", L".ico");
        hFind = FindFirstFile(m_sIcoImage, &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE) {
            hIcon = (HICON)::LoadImage(m_pManager->GetInstance(), m_sDefaultIcon, IMAGE_ICON, 0, 0,
                LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
        } else {
            if (!FindClose(hFind)) {
                ASSERT(FALSE);
            }
            hIcon = (HICON)::LoadImage(m_pManager->GetInstance(), m_sIcoImage, IMAGE_ICON, 0, 0,
                LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
        }
    }
    if (hIcon != nullptr) {
        RECT rect;
        RECT m_rcInset = CIconUI::m_rcInset;
        CResourceManager::GetInstance()->Scale(&m_rcInset);
        rect.left = m_rcItem.left + m_rcInset.left;
        rect.right = m_rcItem.right - m_rcInset.right;
        rect.top = m_rcItem.top + m_rcInset.top;
        rect.bottom = m_rcItem.bottom - m_rcInset.bottom;
        ::DrawIconEx(hDC, rect.left, rect.top, hIcon, rect.right - rect.left, rect.bottom - rect.top, 0, nullptr,
            DI_NORMAL);
        ::DeleteObject(hIcon);
    }
}
}