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
#include "UIFadeButton.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CFadeButtonUI)

CFadeButtonUI::CFadeButtonUI() : m_bMouseHove(FALSE), m_bFadeAlpha(0), m_bMouseLeave(FALSE) {}

CFadeButtonUI::~CFadeButtonUI()
{
    CUIAnimation::StopAnimation();
}

LPCTSTR CFadeButtonUI::GetClass() const
{
    return _T("FadeButtonUI");
}

LPVOID CFadeButtonUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, _T("FadeButton")) == 0)
        return static_cast<CFadeButtonUI *>(this);
    return CButtonUI::GetInterface(pstrName);
}

void CFadeButtonUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    m_sLastImage = m_sNormalImage;
}

void CFadeButtonUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_MOUSEENTER && !CUIAnimation::IsAnimationRunning(FADE_IN_ID)) {
        m_bFadeAlpha = 0;
        m_bMouseHove = TRUE;
        CUIAnimation::StopAnimation(FADE_OUT_ID);
        CUIAnimation::StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_IN_ID);
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_MOUSELEAVE && !CUIAnimation::IsAnimationRunning(FADE_OUT_ID)) {
        m_bFadeAlpha = 0;
        m_bMouseLeave = TRUE;
        CUIAnimation::StopAnimation(FADE_IN_ID);
        CUIAnimation::StartAnimation(FADE_ELLAPSE, FADE_FRAME_COUNT, FADE_OUT_ID);
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_TIMER) {
        OnTimer(static_cast<int>(event.wParam));
    }
    CButtonUI::DoEvent(event);
}

void CFadeButtonUI::OnTimer(int nTimerID)
{
    CUIAnimation::OnAnimationElapse(nTimerID);
}

void CFadeButtonUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused())
        m_uButtonState |= UISTATE_FOCUSED;
    else
        m_uButtonState &= ~UISTATE_FOCUSED;
    if (!IsEnabled())
        m_uButtonState |= UISTATE_DISABLED;
    else
        m_uButtonState &= ~UISTATE_DISABLED;

    if ((m_uButtonState & UISTATE_DISABLED) != 0) {
        if (!m_sDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) {
            } else
                return;
        }
    } else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
        if (!m_sPushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sPushedImage)) {
            } else
                return;
        }
    } else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
        if (!m_sFocusedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) {
            } else
                return;
        }
    }

    if (!m_sNormalImage.IsEmpty()) {
        if (CUIAnimation::IsAnimationRunning(FADE_IN_ID) ||
            CUIAnimation::IsAnimationRunning(FADE_OUT_ID)) {
            if (m_bMouseHove) {
                m_bMouseHove = FALSE;
                m_sLastImage = m_sHotImage;
                if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) {
                }
                return;
            }

            if (m_bMouseLeave) {
                m_bMouseLeave = FALSE;
                m_sLastImage = m_sNormalImage;
                if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) {
                }
                return;
            }

            m_sOldImage = m_sNormalImage;
            m_sNewImage = m_sHotImage;
            if (CUIAnimation::IsAnimationRunning(FADE_OUT_ID)) {
                m_sOldImage = m_sHotImage;
                m_sNewImage = m_sNormalImage;
            }
            CDuiString sFadeOut;
            CDuiString sFadeIn;
            const int fadeMaxValue = 255;
            sFadeOut.Format(_T("fade='%d'"), fadeMaxValue - m_bFadeAlpha);
            sFadeIn.Format(_T("fade='%d'"), m_bFadeAlpha);
            if (!DrawImage(hDC, (LPCTSTR)m_sOldImage, sFadeOut)) {
            }
            if (!DrawImage(hDC, (LPCTSTR)m_sNewImage, sFadeIn)) {
            }
            return;
        } else {
            if (m_sLastImage.IsEmpty())
                m_sLastImage = m_sNormalImage;
            if (!DrawImage(hDC, (LPCTSTR)m_sLastImage)) {
            }
            return;
        }
    }
}

void CFadeButtonUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
{
    const int fadeMaxValue = 255;
    m_bFadeAlpha = (BYTE)((nCurFrame / static_cast<double>(nTotalFrame)) * fadeMaxValue);
    // 动画结束或继续放映
    m_bFadeAlpha = m_bFadeAlpha == 0 ? FADE_ELLAPSE : m_bFadeAlpha;
    Invalidate();
}
} // namespace DuiLib