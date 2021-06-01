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
#include "UIRollText.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CRollTextUI)

CRollTextUI::CRollTextUI(void)
{
    const int stepValue = 5;
    m_nScrollPos = 0;
    m_nText_W_H = 0;
    m_nStep = stepValue; // ����
    m_bUseRoll = FALSE;
    m_nRollDirection = ROLLTEXT_LEFT;
}

CRollTextUI::~CRollTextUI(void)
{
    if (m_pManager != nullptr) {
        m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
        m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
    }
}

LPCTSTR CRollTextUI::GetClass() const
{
    return _T("RollTextUI");
}

LPVOID CRollTextUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("RollText")) == 0)
        return static_cast<CRollTextUI *>(this);
    return CLabelUI::GetInterface(pstrName);
}

void CRollTextUI::BeginRoll(int nDirect, LONG lTimeSpan, LONG lMaxTimeLimited)
{
    const int multiples = 1000;
    m_nRollDirection = nDirect;
    if (m_bUseRoll) {
        EndRoll();
    }
    m_nText_W_H = 0;
    if (m_pManager == nullptr) {
        return;
    }

    m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
    m_pManager->SetTimer(this, ROLLTEXT_TIMERID, lTimeSpan);

    m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
    m_pManager->SetTimer(this, ROLLTEXT_ROLL_END, lMaxTimeLimited * multiples);

    m_bUseRoll = TRUE;
}

void CRollTextUI::EndRoll()
{
    if (!m_bUseRoll) {
        return;
    }
    if (m_pManager == nullptr) {
        return;
    }

    m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
    m_pManager->KillTimer(this, ROLLTEXT_TIMERID);

    m_bUseRoll = FALSE;
}

void CRollTextUI::SetPos(RECT rc)
{
    CLabelUI::SetPos(rc);
    m_nText_W_H = 0; // ���ֱ仯���¼���
}

void CRollTextUI::SetText(LPCTSTR pstrText)
{
    CLabelUI::SetText(pstrText);
    m_nText_W_H = 0; // �ı��仯���¼���
}

void CRollTextUI::DoEvent(TEventUI &event)
{
    if (m_pManager == nullptr) {
        return;
    }
    if (event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_ROLL_END) {
        m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
        m_pManager->SendNotify(this, DUI_MSGTYPE_TEXTROLLEND);
    } else if (event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_TIMERID) {
        Invalidate();
        return;
    }
    CLabelUI::DoEvent(event);
}

void CRollTextUI::PaintText(HDC hDC)
{
    if (m_pManager == nullptr) {
        return;
    }
    if (m_dwTextColor == 0)
        m_dwTextColor = m_pManager->GetDefaultFontColor();
    if (m_dwDisabledTextColor == 0)
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    DWORD dwTextColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
    CDuiString sText = GetText();
    if (sText.IsEmpty())
        return;
    RECT rcTextPadding = GetTextPadding();
    CDuiRect rcClient;
    rcClient = m_rcItem;
    rcClient.left += rcTextPadding.left;
    rcClient.right -= rcTextPadding.right;
    rcClient.top += rcTextPadding.top;
    rcClient.bottom -= rcTextPadding.bottom;

    if (m_nText_W_H > 0) {
        int nScrollRange = 0;

        if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT) { // �����ƶ�
            nScrollRange = m_nText_W_H + rcClient.GetWidth();

            rcClient.Offset((m_nRollDirection == ROLLTEXT_LEFT ? rcClient.GetWidth() : -rcClient.GetWidth()), 0);
            rcClient.Offset((m_nRollDirection == ROLLTEXT_LEFT ? -m_nScrollPos : m_nScrollPos), 0);
            rcClient.right += (m_nText_W_H - rcClient.GetWidth());
        } else { // �����ƶ�
            nScrollRange = m_nText_W_H + rcClient.GetHeight();

            rcClient.Offset(0, (m_nRollDirection == ROLLTEXT_UP ? rcClient.GetHeight() : -rcClient.GetHeight()));
            rcClient.Offset(0, (m_nRollDirection == ROLLTEXT_UP ? -m_nScrollPos : m_nScrollPos));
            rcClient.bottom += (m_nText_W_H - rcClient.GetHeight());
        }

        m_nScrollPos += m_nStep;
        if (m_nScrollPos > nScrollRange) {
            m_nScrollPos = 0;
        }
    }

    RECT rc = rcClient;

    UINT uTextStyle = DT_WORDBREAK | DT_EDITCONTROL;
    const int moveStep = 10000;
    if (m_nText_W_H == 0) {
        uTextStyle |= DT_CALCRECT; // ��һ�μ����ı���Ȼ�߶�
        if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT) { // �����ƶ�
            rc.right += moveStep;
        } else { // �����ƶ�
            rc.bottom += moveStep;
        }
    }

    if (m_bShowHtml) {
        int nLinks = 0;
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont, nullptr, nullptr,
            nLinks, uTextStyle);
    } else {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont, uTextStyle);
    }

    if (m_nText_W_H == 0) {
        m_nText_W_H = (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT) ?
            (rc.right - rc.left) :
            (rc.bottom - rc.top); // �����ı���Ȼ�߶�
    }
}
}