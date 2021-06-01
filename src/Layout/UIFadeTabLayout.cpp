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
#include "UIFadeTabLayout.h"

namespace DuiLib {
const int FADE_ALPHA = 123; // alpha通道透明度设置为FADE_ALPHA
const int FADE_MAX_VALUE = 100; // fade属性范围为0-FADE_MAX_VALUE
const int TWO_UNIT = 2; // 2个单位
const int BIT_COUNT = 32; // 32Bit
void SetChildOpacity(CContainerUI *pContainer, int n)
{
    if (pContainer == nullptr) {
        return;
    }
    for (int it = 0; it < pContainer->GetCount(); it++) {
        auto pControl = static_cast<CControlUI *>(pContainer->GetItemAt(it));
        if (pControl == nullptr) {
            continue;
        }

        auto pChildContainer = dynamic_cast<CContainerUI *>(pControl);
        if (pChildContainer == nullptr) {
            pControl->SetOpacity(n);
        } else {
            SetChildOpacity(pChildContainer, n);
        }
    }
}

static HBITMAP GenerateBitmap(HDC hDC, RECT rc)
{
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = BIT_COUNT;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
    LPDWORD pDest = nullptr;
    HDC hCloneDC = ::CreateCompatibleDC(hDC);
    HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS,
        reinterpret_cast<LPVOID *>(&pDest), nullptr, 0);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);
    if (hBitmap != nullptr) {
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hDC, rc.left, rc.top, SRCCOPY);
        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
        ::GdiFlush();
    } else {
        ::DeleteDC(hCloneDC);
    }

    return hBitmap;
}

IMPLEMENT_DUICONTROL(CFadeTabLayoutUI)

CFadeTabLayoutUI::CFadeTabLayoutUI() : CUIAnimation(this), m_iLastSel(-1)
{
    m_bFadeAlpha = 0;
}

CFadeTabLayoutUI::~CFadeTabLayoutUI()
{
    StopAnimation();
}

LPCTSTR CFadeTabLayoutUI::GetClass() const
{
    return _T("FadeTabLayoutUI");
}

LPVOID CFadeTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("FadeTabLayout")) == 0)
        return static_cast<CFadeTabLayoutUI *>(this);
    return CTabLayoutUI::GetInterface(pstrName);
}

bool CFadeTabLayoutUI::SelectItem(int iIndex)
{
    if (iIndex < 0 || iIndex >= m_items.GetSize())
        return false;
    if (iIndex == m_iCurSel) {
        return true;
    }

    int iOldSel = m_iCurSel;
    m_iLastSel = m_iCurSel;
    m_iCurSel = iIndex;
    for (int it = 0; it < m_items.GetSize(); it++) {
        // Null pointer dereferences, Dereference null return (stat)
        CControlUI *item1 = GetItemAt(it);

        if (item1 == nullptr) {
            continue;
        }

        if (it == iIndex) {
            item1->SetVisible(false);
        } else {
            if (it != iOldSel) {
                item1->SetVisible(false);
            } else {
            }
        }
    }
    NeedParentUpdate();

    if (m_pManager != nullptr) {
        m_pManager->SetNextTabControl();
        m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, iOldSel);

        CControlUI *pItemIn = GetItemAt(m_iCurSel);
        if (pItemIn) {
            m_pManager->SendNotify(pItemIn, DUI_MSGTYPE_TABSELECT_IN, m_iCurSel, 0);
        }
        CControlUI *pItemOut = GetItemAt(iOldSel);
        if (pItemOut) {
            m_pManager->SendNotify(pItemOut, DUI_MSGTYPE_TABSELECT_OUT, iOldSel, 0);
        }
    }

    AnimationSwitch();

    return true;
}

void CFadeTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
}

void CFadeTabLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it));
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsFloat()) {
            SetFloatPos(it);
            continue;
        }
        RECT ori_rc = pControl->GetPos();
        {
            pControl->SetPos(rc);
        }
    }
}

void CFadeTabLayoutUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    m_rcPaint = rcPaint;

    if (1) {
        CControlUI *pCurItem = GetItemAt(m_iCurSel);
        if (pCurItem != nullptr) {
            CControlUI *pLastItem = GetItemAt(m_iLastSel);
            if (pLastItem != nullptr) {
                if (m_bFadeAlpha < FADE_ALPHA) {
                    pLastItem->DoPaint(hDC, rcPaint);
                } else {
                    pCurItem->DoPaint(hDC, rcPaint);
                }
            } else {
                pCurItem->DoPaint(hDC, rcPaint);
            }
        } else {
            __super::DoPaint(hDC, rcPaint);
        }
        return;
    }

    using LPALPHABLEND = BOOL(WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend =
        (LPALPHABLEND)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
    if (lpAlphaBlend == nullptr) {
        CControlUI *pCurItem = GetItemAt(m_iCurSel);
        if (pCurItem != nullptr) {
            pCurItem->DoPaint(hDC, rcPaint);
        } else {
            __super::DoPaint(hDC, rcPaint);
        }
        return;
    }

    CControlUI *pCurItem = GetItemAt(m_iCurSel);
    if (pCurItem != nullptr) {
        CControlUI *pLastItem = GetItemAt(m_iLastSel);
        if (pLastItem != nullptr) {
            if (1) {
                HDC hCloneDC = ::CreateCompatibleDC(hDC);
                BYTE *pbytes = nullptr;
                RECT rcCopy = m_rcItem;
                rcCopy.left = rcCopy.top = 0;
                HBITMAP hBitmap = GenerateBitmap(hDC, rcCopy);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCloneDC, hBitmap);

                RECT rcDest = m_rcItem;
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;

                if (m_bFadeAlpha < FADE_ALPHA) {
                    pLastItem->DoPaint(hCloneDC, rcPaint);

                    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (FADE_ALPHA - m_bFadeAlpha) * TWO_UNIT - 1,
                        AC_SRC_ALPHA};
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, bf);
                } else {
                    pCurItem->DoPaint(hCloneDC, rcPaint);

                    BLENDFUNCTION bf = {AC_SRC_OVER, 0, (m_bFadeAlpha - FADE_ALPHA) * TWO_UNIT,
                        AC_SRC_ALPHA};
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, bf);
                }

                ::GdiFlush();

                SelectObject(hCloneDC, hOldBitmap);
                ::DeleteObject(hBitmap);
                ::DeleteDC(hCloneDC);
            } else {
                pLastItem->DoPaint(hDC, rcPaint);

                HDC hCloneDC = ::CreateCompatibleDC(hDC);
                BYTE *pbytes = nullptr;
                HBITMAP hBitmap =
                    CRenderEngine::CreateARGB32Bitmap(hCloneDC, m_rcItem.right, m_rcItem.bottom, &pbytes);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hCloneDC, hBitmap);

                PaintBkColor(hCloneDC);

                pCurItem->DoPaint(hCloneDC, rcPaint);

                RECT rcDest = m_rcItem;
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;

                BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_bFadeAlpha, AC_SRC_ALPHA };
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcDest.left,
                    rcDest.top, rcDest.right, rcDest.bottom, bf);

                SelectObject(hCloneDC, hOldBitmap);
                ::DeleteObject(hBitmap);
                ::DeleteDC(hCloneDC);
            }
        } else {
            pCurItem->DoPaint(hDC, rcPaint);
        }
    }

    return;

    RECT rcDest = m_rcItem;
    rcDest.right -= rcDest.left;
    rcDest.bottom -= rcDest.top;

    CControlUI *pLastItem = nullptr;
    if (m_bFadeAlpha < FADE_MAX_VALUE && m_iLastSel != -1 && m_iLastSel != m_iCurSel &&
        (pLastItem = GetItemAt(m_iLastSel)) != nullptr) {
        if (lpAlphaBlend != nullptr) {
            RECT rcTemp = { 0 };
            if (::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) {
                HDC hClone2DC = ::CreateCompatibleDC(hDC);
                int required2Width = m_rcItem.right;
                int required2Height = m_rcItem.bottom;
                BYTE *pbytes2 = nullptr;
                HBITMAP hBitmap2 =
                    CRenderEngine::CreateARGB32Bitmap(hClone2DC, required2Width, required2Height, &pbytes2);
                HBITMAP hOldBitmap2 = (HBITMAP)SelectObject(hClone2DC, hBitmap2);

                pLastItem->DoPaint(hClone2DC, rcPaint);

                BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_bFadeAlpha, AC_SRC_ALPHA };
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hClone2DC, rcDest.left,
                    rcDest.top, rcDest.right, rcDest.bottom, bf);

                SelectObject(hClone2DC, hOldBitmap2);
                ::DeleteObject(hBitmap2);
                ::DeleteDC(hClone2DC);
            }
        }
    }
}

void CFadeTabLayoutUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER) {
        OnTimer(static_cast<int>(event.wParam));
    }
    __super::DoEvent(event);
}

void CFadeTabLayoutUI::OnTimer(int nTimerID)
{
    OnAnimationElapse(nTimerID);
}

void CFadeTabLayoutUI::OnAnimationStart(INT nAnimationID, BOOL bFirstLoop)
{
    CControlUI *pCurItem = GetItemAt(m_iCurSel);
    if (pCurItem != nullptr) {
        pCurItem->SetVisible(true);
    }

    const int opacityValue = 100; // 不透明度值设置为100
    m_bFadeAlpha = 0;
    SetChildOpacity(this, opacityValue);
    Invalidate();
}

void CFadeTabLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
{
    m_bFadeAlpha = static_cast<BYTE>((nCurFrame / static_cast<double>(nTotalFrame)) * FADE_MAX_VALUE);
    // m_bFadeAlpha为0时动画消逝（FADETAB_ANIMATION_ELLAPSE），非0时继续播
    m_bFadeAlpha = m_bFadeAlpha == 0 ? FADETAB_ANIMATION_ELLAPSE : m_bFadeAlpha;

    const float transformToOpacity = 2.55; // fade转换为不透明度需要除以2.55
    if (m_bFadeAlpha < FADE_ALPHA) {
        int fade = (FADE_ALPHA - m_bFadeAlpha) * TWO_UNIT - 1;
        SetChildOpacity(this, static_cast<int>(fade / transformToOpacity));
    } else {
        int fade = (m_bFadeAlpha - FADE_ALPHA) * TWO_UNIT;
        if (fade == 0) {
            fade = 1;
        }
        SetChildOpacity(this, static_cast<int>(fade / transformToOpacity));
    }
    Invalidate();
}

void CFadeTabLayoutUI::OnAnimationStop(INT nAnimationID)
{
    CControlUI *pLastItem = GetItemAt(m_iLastSel);
    if (pLastItem != nullptr) {
        pLastItem->SetVisible(false);
    }
    m_iLastSel = -1;
    const int opacityValue = 100; // 不透明度值设置为100
    m_bFadeAlpha = FADE_MAX_VALUE;
    SetChildOpacity(this, opacityValue);
    Invalidate();
}

void CFadeTabLayoutUI::AnimationSwitch()
{
    const int animationID = 50; // 通过animationID来获取动画数据
    StopAnimation(FADETAB_ANIMATION_ID);
    StartAnimation(FADETAB_ANIMATION_ELLAPSE, animationID, FADETAB_ANIMATION_ID);
}
}
