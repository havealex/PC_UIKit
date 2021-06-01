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
#include "UIAnimationLayout.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CAnimationLayoutUI)

CAnimationLayoutUI::CAnimationLayoutUI()
    : CUIAnimation(this),
      m_iCurSel(-1),
      m_bIsVerticalDirection(false),
      m_nPositiveDirection(-1),
      m_pCurrentControl(nullptr),
      m_pLastControl(nullptr),
      m_bControlVisibleFlag(false),
      m_bFirstSetPosFlag(true)
{}

LPCTSTR CAnimationLayoutUI::GetClass() const
{
    return _T("AnimationLayoutUI");
}

LPVOID CAnimationLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("AnimationLayout")) == 0) {
        return static_cast<CAnimationLayoutUI *>(this);
    }
    return CContainerUI::GetInterface(pstrName);
}

int CAnimationLayoutUI::SetDirection(int nDirection)
{
    if (nDirection == 1 || nDirection == -1) {
        m_nPositiveDirection = nDirection;
    }

    return m_nPositiveDirection;
}

bool CAnimationLayoutUI::SelectItem(int iIndex)
{
    if (iIndex < 0 || iIndex >= m_items.GetSize()) {
        return false;
    }
    if (iIndex == m_iCurSel) {
        return true;
    }

    m_bFirstSetPosFlag = false;

    m_pLastControl = m_pCurrentControl;

    int iOldSel = m_iCurSel;
    m_iCurSel = iIndex;
    for (int it = 0; it < m_items.GetSize(); it++) {
        if (GetItemAt(it) == nullptr) {
            continue;
        }
        if (it == iIndex) {
            GetItemAt(it)->SetVisible(false);
            m_bControlVisibleFlag = false;
            m_pCurrentControl = static_cast<CControlUI *>(m_items.GetAt(it));
        } else {
            // Null pointer dereferences, Dereference null return (stat)
            CControlUI *item1 = GetItemAt(it);
            if (item1 == nullptr) {
                continue;
            }

            if (m_pLastControl == nullptr) {
                if ((it + 1) == iIndex) {
                    m_pLastControl = item1;
                } else {
                    if (it == (m_items.GetSize() - 1)) {
                        m_pLastControl = item1;
                    }
                }
            }

            if (item1 != m_pLastControl) {
                item1->SetVisible(false);
            }
        }
    }

    NeedParentUpdate();
    AnimationSwitch();

    return true;
}

bool CAnimationLayoutUI::SelectItem(CControlUI *pControl)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) {
        return false;
    } else {
        return SelectItem(iIndex);
    }
}

void CAnimationLayoutUI::AnimationSwitch()
{
    m_rcLastControlPos = m_rcItem;
    m_rcItemOld = m_rcItem;
    if (!m_bIsVerticalDirection) {
        m_rcCurPos.top = m_rcItem.top;
        m_rcCurPos.bottom = m_rcItem.bottom;
        m_rcCurPos.left = m_rcItem.left - (m_rcItem.right - m_rcItem.left) * m_nPositiveDirection;
        m_rcCurPos.right = m_rcItem.right - (m_rcItem.right - m_rcItem.left) * m_nPositiveDirection;
    } else {
        m_rcCurPos.left = m_rcItem.left;
        m_rcCurPos.right = m_rcItem.right;
        m_rcCurPos.top = m_rcItem.top - (m_rcItem.bottom - m_rcItem.top) * m_nPositiveDirection;
        m_rcCurPos.bottom = m_rcItem.bottom - (m_rcItem.bottom - m_rcItem.top) * m_nPositiveDirection;
    }

    StopAnimation(LAYOUT_ANIMATION_ID);
    StartAnimation(LAYOUT_ANIMATION_ELLAPSE, LAYOUT_ANIMATION_FRAME_COUNT, LAYOUT_ANIMATION_ID);
}

void CAnimationLayoutUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER) {
        OnTimer(static_cast<int>(event.wParam));
    }
    __super::DoEvent(event);
}

void CAnimationLayoutUI::OnTimer(int nTimerID)
{
    OnAnimationElapse(nTimerID);
}

void CAnimationLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
{
    if (!m_bControlVisibleFlag && m_pCurrentControl != nullptr) {
        m_bControlVisibleFlag = true;
        m_pCurrentControl->SetVisible(true);

        if (m_pLastControl != nullptr) {
            m_pLastControl->SetVisible(true);
        }
    }

    int iStepLen = 0;
    if (!m_bIsVerticalDirection) {
        if (nTotalFrame == 0) {
            return;
        }
        iStepLen = (m_rcItemOld.right - m_rcItemOld.left) * m_nPositiveDirection / nTotalFrame;
        if (nCurFrame != nTotalFrame) {
            m_rcCurPos.left = m_rcCurPos.left + iStepLen;
            m_rcCurPos.right = m_rcCurPos.right + iStepLen;
        } else {
            m_rcItem = m_rcCurPos = m_rcItemOld;
        }

        m_rcLastControlPos.left += iStepLen;
        m_rcLastControlPos.right += iStepLen;

        m_rcCurrentControlPos = m_rcCurPos;
    } else {
        iStepLen = (m_rcItemOld.bottom - m_rcItemOld.top) * m_nPositiveDirection / nTotalFrame;
        if (nCurFrame != nTotalFrame) {
            m_rcCurPos.top = m_rcCurPos.top + iStepLen;
            m_rcCurPos.bottom = m_rcCurPos.bottom + iStepLen;
        } else {
            m_rcItem = m_rcCurPos = m_rcItemOld;
        }

        m_rcLastControlPos.top += iStepLen;
        m_rcLastControlPos.bottom += iStepLen;

        m_rcCurrentControlPos = m_rcCurPos;
    }

    if (m_pCurrentControl != nullptr) {
        m_pCurrentControl->SetPos(m_rcCurrentControlPos);
    }

    if (m_pLastControl != nullptr) {
        m_pLastControl->SetPos(m_rcLastControlPos);
    }

    NeedParentUpdate();
}

void CAnimationLayoutUI::OnAnimationStop(INT nAnimationID)
{
    if (m_pLastControl != nullptr) {
        m_pLastControl->SetVisible(false);
        m_pLastControl->SetPos(m_rcItemOld);
        m_pLastControl = nullptr;
    }

    if (m_pCurrentControl != nullptr) {
        m_pCurrentControl->SetPos(m_rcItemOld);
    }

    NeedParentUpdate();
}

void CAnimationLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("animation_direction")) == 0 && _tcsicmp(pstrValue, _T("vertical")) == 0) {
        m_bIsVerticalDirection = true; // pstrValue = "vertical" or "horizontal"
    }
    return CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CAnimationLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
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
        if (!CUIAnimation::IsAnimationRunning(LAYOUT_ANIMATION_ID)) {
            pControl->SetPos(rc);
        }
    }
}

bool CAnimationLayoutUI::Add(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    bool ret = CContainerUI::Add(pControl);
    if (!ret) {
        return ret;
    }

    bool bVisible = pControl->IsVisible();
    if (m_iCurSel == -1 && bVisible) {
        m_iCurSel = GetItemIndex(pControl);
    } else {
        pControl->SetVisible(false);
    }

    return ret;
}

bool CAnimationLayoutUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (pControl == nullptr) {
        return false;
    }
    bool ret = CContainerUI::AddAt(pControl, iIndex);
    if (!ret) {
        return ret;
    }

    if (m_iCurSel == -1 && pControl->IsVisible()) {
        m_iCurSel = GetItemIndex(pControl);
    } else if (m_iCurSel != -1 && iIndex <= m_iCurSel) {
        m_iCurSel += 1;
    } else {
        pControl->SetVisible(false);
    }

    return ret;
}

bool CAnimationLayoutUI::Remove(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }

    int index = GetItemIndex(pControl);
    bool ret = CContainerUI::Remove(pControl);
    if (!ret) {
        return false;
    }

    if (m_iCurSel == index) {
        if (GetCount() > 0) {
            m_iCurSel = 0;

            // Null pointer dereferences, Dereference null return (stat)
            CControlUI *item1 = GetItemAt(m_iCurSel);

            if (item1 != nullptr) {
                item1->SetVisible(true);
            }
        } else
            m_iCurSel = -1;
        NeedParentUpdate();
    } else if (m_iCurSel > index) {
        m_iCurSel -= 1;
    }

    return ret;
}

void CAnimationLayoutUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
    NeedParentUpdate();
}

int CAnimationLayoutUI::GetCurSel()
{
    return m_iCurSel;
}

void CAnimationLayoutUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it));
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }
        pControl->DoPaint(hDC, rcPaint);
    }
}
} // namespace DuiLib