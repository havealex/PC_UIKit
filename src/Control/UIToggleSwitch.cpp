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
#include "UIToggleSwitch.h"
#include "../Control/UIAnimation.h"

#define TOGGLESWITCH_BKCOLOR_ON 0xff007dff
#define TOGGLESWITCH_BKCOLOR_OFF 0xffdedede
#define TOGGLESWITCH_BKCOLOR_DISABLE 0xffcccccc

NAMESPACE_DUILIB_BEGIN

IMPLEMENT_DUICONTROL(CToggleSwitchUI)

class UIToggleSwitchHelper : public DuiLib::CUIAnimation {
public:
    enum {
        ToggleAnimation = 0
    };

    const int BALL_MARGIN_PIXEL = 3;

    explicit UIToggleSwitchHelper(CToggleSwitchUI *sw) : DuiLib::CUIAnimation(sw)
    {
        if (sw == nullptr) {
            return;
        }
        this->sw = sw;

        ballMargin = BALL_MARGIN_PIXEL;
        switchOn = false;
        mouseHover = false;
        mousePressed = false;
        inToggleBallOffset = 0;
        inToggleAnimation = false;

        // timer要用这个。
        sw->OnNotify += DuiLib::MakeDelegate(this, &UIToggleSwitchHelper::OnDoNofify);
    }

    ~UIToggleSwitchHelper() {}

    void OnClicked()
    {
        if (toggleOnClick) {
            Toggle(!switchOn);
        }

        sw->GetManager()->SendNotify(sw, CToggleSwitchUI::MSG_TOGGLED, switchOn);
    }

    int GetBallLeftPositoin() const
    {
        return ballMargin;
    }

    int GetBallRightPostion() const
    {
        RECT rcItem = { 0 };
        GetAvailableRect(rcItem);
        int ballWidth = GetBallWidth();
        return rcItem.right - ballMargin - ballWidth;
    }

    int GetBallMoveLength() const
    {
        RECT rcItem = { 0 };
        GetAvailableRect(rcItem);
        return rcItem.right - rcItem.left - (rcItem.bottom - rcItem.top) - 1;
    }

    int GetBallWidth() const
    {
        RECT rcItem = { 0 };
        const int marginNum = 2; // 计算宽（或高）时需要考虑左右（或上下）边缘部分
        GetAvailableRect(rcItem);
        return rcItem.bottom - rcItem.top - ballMargin * marginNum;
    }

    void Toggle(bool onOff)
    {
        if (switchOn == onOff) {
            return;
        }

        const int moveLenMul = 2;
        switchOn = onOff;
        StopAnimation(ToggleAnimation);
        RECT rcItem = { 0 };
        GetAvailableRect(rcItem);
        int moveLength = GetBallMoveLength();
        inToggleBallOffset = switchOn ? -moveLength : moveLength;
        inToggleAnimation = true;
        const int fadeEllapse = 10; // 动画消逝
        StartAnimation(fadeEllapse, moveLength / moveLenMul, ToggleAnimation);
    }

    virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID)
    {
        const int multiples = 2;
        if (sw == nullptr) {
            return;
        }
        inToggleBallOffset = switchOn ? (-nTotalFrame + nCurFrame) * multiples : (nTotalFrame - nCurFrame) * multiples;
        sw->Invalidate();
    }

    virtual void OnAnimationStop(int nAnimationID)
    {
        if (sw == nullptr) {
            return;
        }
        inToggleAnimation = false;
        sw->Invalidate();
    };

    void GetAvailableRect(RECT &rc) const
    {
        if (sw == nullptr) {
            return;
        }
        rc = sw->GetPos();
        rc.right--;
        rc.bottom--;
    }

    bool OnDoNofify(void *param)
    {
        DuiLib::TNotifyUI *noti = (DuiLib::TNotifyUI *)(param);
        if (noti == nullptr) {
            return false;
        }
        if (noti->pSender == sw && noti->sType == _T("timer")) {
            DuiLib::CUIAnimation::OnAnimationElapse(static_cast<int>(noti->wParam));
        }

        return true;
    } // UIToggleSwitchHelper::OnDoNofify

    int ballMargin;
    bool switchOn = false;
    bool toggleOnClick = true;
    bool mouseHover = false;
    bool mousePressed = false;
    bool inToggleAnimation = false;
    int inToggleBallOffset;

    CToggleSwitchUI *sw;
};

LPCTSTR CToggleSwitchUI::MSG_TOGGLED = _T("UIToggleSwitch_Toggled");

CToggleSwitchUI::CToggleSwitchUI() : helper(new UIToggleSwitchHelper(this))
{
    SetCursor(DUI_HAND);
    SetBkColor(0xffff0000);
}

void CToggleSwitchUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    Gdiplus::Graphics gp(hDC);
    gp.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    RECT rcItem = m_rcItem;
    if (helper == nullptr) {
        return;
    }
    helper->GetAvailableRect(rcItem);

    // 绘制背景。
    Gdiplus::SolidBrush brushBK(helper->switchOn ? TOGGLESWITCH_BKCOLOR_ON : TOGGLESWITCH_BKCOLOR_OFF);
    if (!IsEnabled()) {
        brushBK.SetColor(TOGGLESWITCH_BKCOLOR_DISABLE);
    }

    Gdiplus::SolidBrush brushBKAlpha(Gdiplus::Color(0x44, 0xff, 0xff, 0xff));

    if (helper->inToggleAnimation) {
        int ballMoveLength = helper->GetBallMoveLength();
        float moved = static_cast<float>(ballMoveLength) - abs(helper->inToggleBallOffset);
        Gdiplus::Color transColor =
            Utils::GetTransitionColor(helper->switchOn ? TOGGLESWITCH_BKCOLOR_OFF : TOGGLESWITCH_BKCOLOR_ON,
                helper->switchOn ? TOGGLESWITCH_BKCOLOR_ON : TOGGLESWITCH_BKCOLOR_OFF, moved / ballMoveLength);
        brushBK.SetColor(transColor);
    }

    int pieWidth = rcItem.bottom - rcItem.top;
    const int startAngle = 90;
    const int sweepAngle = 180;
    const int negativeStartAngle = -90;
    const int pieMul = 2;
    // 左半圆。
    Gdiplus::Rect rcLeftPie(rcItem.left, rcItem.top, pieWidth, pieWidth);
    gp.FillPie(&brushBK, rcLeftPie, startAngle, sweepAngle);
    if (helper->mouseHover && !helper->mousePressed) {
        gp.FillPie(&brushBKAlpha, rcLeftPie, startAngle, sweepAngle);
    }
    // 中间
    Gdiplus::Rect rcCenter(rcItem.left + pieWidth / pieMul - 1, rcItem.top, rcItem.right - rcItem.left - pieWidth + 1,
        pieWidth);
    gp.FillRectangle(&brushBK, rcCenter);
    if (helper->mouseHover && !helper->mousePressed) {
        gp.FillRectangle(&brushBKAlpha, rcCenter);
    }
    // 右半圆。
    Gdiplus::Rect rcRightPie(rcItem.right - pieWidth - 1, rcItem.top, pieWidth, pieWidth);
    gp.FillPie(&brushBK, rcRightPie, negativeStartAngle, sweepAngle);
    if (helper->mouseHover && !helper->mousePressed) {
        gp.FillPie(&brushBKAlpha, rcRightPie, negativeStartAngle, sweepAngle);
    }
    const int marginNum = 2; // 计算宽（或高）时需要考虑左右（或上下）边缘部分
    int ballWidth = pieWidth - helper->ballMargin * marginNum;
    Gdiplus::Rect rectBall;
    rectBall.Width = rectBall.Height = ballWidth;
    rectBall.Y = rcItem.top + helper->ballMargin;
    rectBall.X = helper->switchOn ? rcItem.right - 1 - helper->ballMargin - ballWidth :
        rcItem.left + helper->ballMargin;
    if (helper->inToggleAnimation) {
        rectBall.X += helper->inToggleBallOffset;
    }

    const int toGetShadowSize = 4;
    Gdiplus::Bitmap bmBallShadow(
        rectBall.Width / toGetShadowSize, rectBall.Height / toGetShadowSize, &gp);
    Gdiplus::Graphics *gpBMBallShadow = Gdiplus::Graphics::FromImage(&bmBallShadow);
    if (!gpBMBallShadow) {
        return;
    }
    Gdiplus::SolidBrush brushBallShadow(Gdiplus::Color(0x33, 0, 0, 0));

    gpBMBallShadow->FillEllipse(&brushBallShadow, 0, 0, bmBallShadow.GetWidth(), bmBallShadow.GetHeight());

    Gdiplus::GraphicsState st = gp.Save();
    Gdiplus::Rect rcBallShadow = rectBall;
    const int shadowWidth = 3;
    const int shadowHight = 3;
    rcBallShadow.Offset(shadowWidth, shadowHight);
    gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
    gp.DrawImage(&bmBallShadow, rcBallShadow, 0, 0, bmBallShadow.GetWidth(), bmBallShadow.GetHeight(),
        Gdiplus::UnitPixel);
    gp.Restore(st);

    Gdiplus::SolidBrush brushBall(0xffffffff);
    gp.FillEllipse(&brushBall, rectBall);

    delete gpBMBallShadow;
}

void CToggleSwitchUI::DoEvent(DuiLib::TEventUI &ev)
{
    if (helper == nullptr) {
        return;
    }
    if (ev.Type == DuiLib::UIEVENT_MOUSEENTER) {
        if (IsEnabled()) {
            helper->mouseHover = true;
        }
    } else if (ev.Type == DuiLib::UIEVENT_MOUSELEAVE) {
        helper->mouseHover = false;
    } else if (ev.Type == DuiLib::UIEVENT_BUTTONDOWN) {
        if (IsEnabled()) {
            helper->mousePressed = true;
        }
    } else if (ev.Type == DuiLib::UIEVENT_BUTTONUP) {
        helper->mousePressed = false;
        if (PtInRect(&m_rcItem, ev.ptMouse) && IsEnabled()) {
            helper->OnClicked();
        }
    }

    DuiLib::CControlUI::DoEvent(ev);
    if (!helper->IsAnimationRunning(UIToggleSwitchHelper::ToggleAnimation)) {
        Invalidate();
    }
}

UINT CToggleSwitchUI::GetControlFlags() const
{
    return IsEnabled() ? UIFLAG_SETCURSOR : __super::GetControlFlags();
}

CToggleSwitchUI::~CToggleSwitchUI()
{
    delete helper;
    helper = nullptr;
}

void CToggleSwitchUI::SetOn(bool on)
{
    if (helper == nullptr) {
        return;
    }
    helper->switchOn = on;
    Invalidate();
}

bool CToggleSwitchUI::IsOn() const
{
    if (helper == nullptr) {
        return false;
    }
    return helper->switchOn;
}

void CToggleSwitchUI::SetToggleOnClick(bool b)
{
    if (helper == nullptr) {
        return;
    }
    helper->toggleOnClick = b;
}

void CToggleSwitchUI::Toggle(bool toggled)
{
    if (helper == nullptr) {
        return;
    }
    helper->Toggle(toggled);
}

LPVOID CToggleSwitchUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_TOGGLESWITCH) == 0)
        return static_cast<CToggleSwitchUI *>(this);
    return CControlUI::GetInterface(pstrName);
}

LPCTSTR CToggleSwitchUI::GetClass() const
{
    return _T("ToggleSwitchUI");
}

NAMESPACE_DUILIB_END
