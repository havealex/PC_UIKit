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
#include "UIGroupBox.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CGroupBoxUI)

CGroupBoxUI::CGroupBoxUI()
    : m_uTextStyle(DT_SINGLELINE | DT_VCENTER | DT_CENTER), m_dwTextColor(0), m_dwDisabledTextColor(0), m_iFont(-1)
{
    const int iValue = 20;
    const int topValue = 25;
    SetInset(CDuiRect(iValue, topValue, iValue, iValue));
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
}

CGroupBoxUI::~CGroupBoxUI() {}

LPCTSTR CGroupBoxUI::GetClass() const
{
    return _T("GroupBoxUI");
}

LPVOID CGroupBoxUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("GroupBox")) == 0) {
        return static_cast<CGroupBoxUI *>(this);
    }
    return CVerticalLayoutUI::GetInterface(pstrName);
}
void CGroupBoxUI::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    Invalidate();
}

DWORD CGroupBoxUI::GetTextColor() const
{
    return m_dwTextColor;
}
void CGroupBoxUI::SetDisabledTextColor(DWORD dwTextColor)
{
    m_dwDisabledTextColor = dwTextColor;
    Invalidate();
}

DWORD CGroupBoxUI::GetDisabledTextColor() const
{
    return m_dwDisabledTextColor;
}
void CGroupBoxUI::SetFont(int index)
{
    m_iFont = index;
    Invalidate();
}

int CGroupBoxUI::GetFont() const
{
    return m_iFont;
}
void CGroupBoxUI::PaintText(HDC hDC)
{
    const int axesValue = 5;
    const int rcStep = 15;
    if (m_pManager == nullptr) {
        return;
    }
    CDuiString sText = GetText();
    if (sText.IsEmpty()) {
        return;
    }
    if (m_dwTextColor == 0) {
        m_dwTextColor = m_pManager->GetDefaultFontColor();
    }
    if (m_dwDisabledTextColor == 0) {
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    }
    if (sText.IsEmpty()) {
        return;
    }

    CDuiRect rcText = m_rcItem;
    rcText.Deflate(axesValue, axesValue);
    SIZE szAvailable = { rcText.right - rcText.left, rcText.bottom - rcText.top };
    SIZE sz = CalcrectSize(szAvailable);

    // ¼ÆËãÎÄ×ÖÇøÓò
    rcText.left = rcText.left + rcStep;
    rcText.top = rcText.top - axesValue;
    rcText.right = rcText.left + sz.cx;
    rcText.bottom = rcText.top + sz.cy;
    if (IsEnabled()) {
        CRenderEngine::DrawText(hDC, m_pManager, rcText, sText, m_dwTextColor, m_iFont, m_uTextStyle,
            GetAdjustColor(m_dwBackColor), FALSE);
    } else {
        CRenderEngine::DrawText(hDC, m_pManager, rcText, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle,
            GetAdjustColor(m_dwBackColor), FALSE);
    }
}
void CGroupBoxUI::PaintBorder(HDC hDC)
{
    const int axesValue = 5;
    if (m_nBorderSize > 0) {
        CDuiRect rcItem = m_rcItem;
        rcItem.Deflate(axesValue, axesValue);
        if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0) { // »­Ô²½Ç±ß¿ò
            if (IsFocused() && m_dwFocusBorderColor != 0) {
                CRenderEngine::DrawRoundRect(hDC, rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy,
                    GetAdjustColor(m_dwFocusBorderColor));
            } else {
                CRenderEngine::DrawRoundRect(hDC, rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy,
                    GetAdjustColor(m_dwBorderColor));
            }
        } else {
            if (IsFocused() && m_dwFocusBorderColor != 0) {
                CRenderEngine::DrawRect(hDC, rcItem, m_nBorderSize, GetAdjustColor(m_dwFocusBorderColor));
            } else {
                CRenderEngine::DrawRect(hDC, rcItem, m_nBorderSize, GetAdjustColor(m_dwBorderColor));
            }
        }
    }

    PaintText(hDC);
}
SIZE CGroupBoxUI::CalcrectSize(SIZE szAvailable)
{
    RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 20 };
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;

    CDuiString sText = GetText();

    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
        DT_CALCRECT | m_uTextStyle);
    SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
                 rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };

    return cXY;
}
void CGroupBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    const int radix = 16;
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("disabledtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetDisabledTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("font")) == 0) {
        SetFont(_ttoi(pstrValue));
    }

    CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
}
}
