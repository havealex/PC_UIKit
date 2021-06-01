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

#include "../stdafx.h"
#include "UIAnimPanel.h"
#include "../Debug/LogOut.h"
#include <sstream>

namespace DuiLib {
int CAnimPanelUI::s_iAnimTimerID = PANEL_ANIMATION_ID;

CAnimPanelUI::CAnimPanelUI() : CUIAnimation(this)
{
    m_Step = 1;
}


CAnimPanelUI::~CAnimPanelUI()
{
    bool isMain = CPaintManagerUI::IsMainThread();
    if (isMain == false) {
        wstring str;
        if (GetName().IsEmpty() == false) {
            str = GetName();
        }
        LOG_ERROR((L"[DUILIB]: ~ CAnimPanelUI. outside ui thread , name:" + str).c_str());
    }

    if (m_iAnimTimerID != -1) {
        StopAnimation(m_iAnimTimerID);
    }
}

CControlUI *CAnimPanelUI::CreateControl(void)
{
    return new CAnimPanelUI();
}

LPCTSTR CAnimPanelUI::GetClass() const
{
    return _T("AnimPanelUI");
}

LPVOID CAnimPanelUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_ANIMPANEL) == 0) {
        return static_cast<CAnimPanelUI *>(this);
    }
    return CControlUI::GetInterface(pstrName);
}

void CAnimPanelUI::DoInit()
{
    if (m_bAutoPlay) {
        s_iAnimTimerID++;

        int total = 0;
        total = abs(m_iMaxFrame - m_iMinFrame) / m_Step + 1;
        if (abs(m_iMaxFrame - m_iMinFrame) % m_Step != 0) {
            ++total;
        }
        StartAnimation(m_uTimerEllapse, total, s_iAnimTimerID, m_bIsLoop);
        m_iAnimTimerID = s_iAnimTimerID;

        OnTimer(s_iAnimTimerID); // 解决第一个轮播周期内不加载图片的问题
    }
}

void CAnimPanelUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER) {
        OnTimer(static_cast<int>(event.wParam));
    }
    __super::DoEvent(event);
}

void CAnimPanelUI::OnTimer(int nTimerID)
{
    OnAnimationElapse(nTimerID);
}

void CAnimPanelUI::OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}

void CAnimPanelUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
{
    wostringstream streamImagePath;

    int left_size = m_sAnimImage.ReverseFind('.');
    int right_size = m_sAnimImage.GetLength() - left_size;
    int file_index = m_iMinFrame + nCurFrame * m_Step;
    if (m_iMinFrame <= m_iMaxFrame) {
        file_index = m_iMinFrame + nCurFrame * m_Step;
        if (file_index > m_iMaxFrame)
            file_index = m_iMaxFrame;
    } else if (m_iMinFrame > m_iMaxFrame) {
        file_index = m_iMinFrame - nCurFrame * m_Step;
        if (file_index < m_iMaxFrame) {
            file_index = m_iMaxFrame;
        }
    }

    streamImagePath << m_sAnimImage.Left(left_size).GetData() << file_index << m_sAnimImage.Right(right_size).GetData();

    m_sBkImage = CDuiString(streamImagePath.str().c_str());
    NeedUpdate();
}

void CAnimPanelUI::OnAnimationStop(INT nAnimationID)
{
    if (m_pManager == nullptr) {
        return;
    }
    m_pManager->SendNotify(this, DUI_MSGTYPE_ANIMATION_STOP);
}

void CAnimPanelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("MinFrame")) == 0) {
        SetMinFrame(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("MaxFrame")) == 0) {
        SetMaxFrame(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("IsLoop")) == 0) {
        SetIsLoop(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("AutoPlay")) == 0) {
        SetAutoPlay(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("AnimImage")) == 0) {
        SetAnimImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("TimerEllapse")) == 0) {
        SetTimerEllapse(_ttoi(pstrValue));
    }

    return CControlUI::SetAttribute(pstrName, pstrValue);
}

int CAnimPanelUI::GetMinFrame()
{
    return m_iMinFrame;
}


void CAnimPanelUI::SetMinFrame(int iMinFrame)
{
    m_iMinFrame = iMinFrame;
}

int CAnimPanelUI::GetMaxFrame()
{
    return m_iMaxFrame;
}

void CAnimPanelUI::SetMaxFrame(int iMaxFrame)
{
    m_iMaxFrame = iMaxFrame;
}

void CAnimPanelUI::SetStep(int step)
{
    m_Step = step;
}

bool CAnimPanelUI::GetIsLoop()
{
    return m_bIsLoop;
}

void CAnimPanelUI::SetIsLoop(bool bIsLoop)
{
    m_bIsLoop = bIsLoop;
}


bool CAnimPanelUI::GetAutoPlay()
{
    return m_bAutoPlay;
}

void CAnimPanelUI::SetAutoPlay(bool bAutoPlay)
{
    m_bAutoPlay = bAutoPlay;
}

void CAnimPanelUI::SetAnimImage(LPCTSTR pStrImage)
{
    if (pStrImage == nullptr || m_sAnimImage == pStrImage) {
        return;
    }
    m_sAnimImage = pStrImage;
}

UINT CAnimPanelUI::GetTimerEllapse()
{
    return m_uTimerEllapse;
}

void CAnimPanelUI::SetTimerEllapse(UINT uTimerEllapse)
{
    m_uTimerEllapse = uTimerEllapse;
}

LPCTSTR CAnimPanelUI::GetAnimImage()
{
    return m_sAnimImage.GetData();
}

void CAnimPanelUI::StartAnim()
{
    if (IsAnimationRunning(m_iAnimTimerID)) {
        return;
    }

    if (m_iMaxFrame == m_iMinFrame) {
        return;
    }
    s_iAnimTimerID++;
    if (m_iAnimTimerID == -1) {
        m_iAnimTimerID = s_iAnimTimerID;
    }

    int total = 0;

    total = abs(m_iMaxFrame - m_iMinFrame) / m_Step + 1;
    if (abs(m_iMaxFrame - m_iMinFrame) % m_Step != 0) {
        ++total;
    }

    StartAnimation(m_uTimerEllapse, total, m_iAnimTimerID, m_bIsLoop);

    OnTimer(m_iAnimTimerID);
}

void CAnimPanelUI::StopAnim()
{
    StopAnimation(m_iAnimTimerID);
}
}
