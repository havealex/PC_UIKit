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
#include "UIPointsWaitControl.h"

NAMESPACE_DUILIB_BEGIN

IMPLEMENT_DUICONTROL(CPointsWaitControlUI)

class UIPointsWaitControlHelper : public DuiLib::CUIAnimation {
public:
    enum {
        PointAnimation = 0
    };

    UIPointsWaitControlHelper(CPointsWaitControlUI *control) : DuiLib::CUIAnimation(control)
    {
        this->control = control;
        inAnimation = false;

        // timer要用这个。
        if (control == nullptr) {
            return;
        }
        control->OnNotify += DuiLib::MakeDelegate(this, &UIPointsWaitControlHelper::OnDoNofify);
    }

    ~UIPointsWaitControlHelper() {}

    bool OnDoNofify(void *param)
    {
        DuiLib::TNotifyUI *noti = (DuiLib::TNotifyUI *)(param);
        if (noti != nullptr && noti->pSender == control && noti->sType == _T("timer")) {
            DuiLib::CUIAnimation::OnAnimationElapse(static_cast<int>(noti->wParam));
        }

        return true;
    } // UIPointsWaitControlHelper::OnDoNofify

    virtual void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID)
    {
        if (control == nullptr) {
            return;
        }
        highlightPointIndex++;
        control->Invalidate();
    }

    virtual void OnAnimationStop(int nAnimationID)
    {
        if (control == nullptr) {
            return;
        }
        control->Invalidate();
    };

    void OnPaint(HDC hDC)
    {
        const int multiples = 2;
        if (control == nullptr) {
            return;
        }
        Gdiplus::Rect rcItem = DuiLib::Utils::FromWin32RECT(control->GetPos());
        if (rcItem.Width == 0 || rcItem.Height == 0 || rcItem.Width <= (rcItem.Height + pointPadding) * multiples) {
            return;
        }

        rcItem.Inflate(-padding, -padding);

        int pointWidth = rcItem.Height;

        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        int pointOffset = -((colorChangeCount - 1) * (pointWidth + pointPadding));
        int drawWidth = rcItem.Width + (colorChangeCount * (pointWidth + pointPadding)) * multiples;
        int currentHighlightPointIndex = highlightPointIndex % (drawWidth / (pointWidth + pointPadding));
        if (hightlightAll) {
            currentHighlightPointIndex = drawWidth / multiples / (pointWidth + pointPadding) - 1;
        }
        int currentIndex = 0;
        while (pointOffset < drawWidth - pointWidth) {
            Gdiplus::SolidBrush normalPointBrush(normalColor);
            Gdiplus::Color colorHighlight = highlightColor;

            if (currentIndex < currentHighlightPointIndex) {
                int currentDeltaIndex = currentHighlightPointIndex - currentIndex;
                if (currentDeltaIndex < colorChangeCount) {
                    colorHighlight = Utils::GetTransitionColor(normalColor, highlightColor,
                        1.0f - currentDeltaIndex / colorChangeCount);
                } else {
                    colorHighlight = normalColor;
                }
            } else if (currentIndex > currentHighlightPointIndex) {
                int currentDeltaIndex = currentIndex - currentHighlightPointIndex;
                if (currentDeltaIndex < colorChangeCount) {
                    colorHighlight = Utils::GetTransitionColor(highlightColor, normalColor,
                        currentDeltaIndex * 1.0f / colorChangeCount);
                } else {
                    colorHighlight = normalColor;
                }
            }
            Gdiplus::SolidBrush highlightPointBrush(colorHighlight);
            Gdiplus::Rect rcPoint(rcItem.X + pointOffset, rcItem.Y, pointWidth, pointWidth);

            if (pointOffset > 0 && pointOffset < rcItem.Width - pointWidth) {
                if (inAnimation || hightlightAll) {
                    graphics.FillEllipse(&highlightPointBrush, rcPoint);
                } else {
                    graphics.FillEllipse(&normalPointBrush, rcPoint);
                }
            }
            pointOffset += (pointWidth + pointPadding);
            currentIndex++;
        }
    }

    void SetAnimationStart(bool start)
    {
        const int nElapse = 110;
        const int nTotalFrame = 1;
        inAnimation = start;
        highlightPointIndex = 0;
        StopAnimation(PointAnimation);
        if (start) {
            StartAnimation(nElapse, nTotalFrame, PointAnimation, true);
        }
    }

    CPointsWaitControlUI *control = 0;
    DWORD normalColor = 0xffa5a5a5;
    DWORD highlightColor = 0xff007dff;
    int padding = 0;
    int pointPadding = 0;
    int highlightPointIndex = 0;
    bool inAnimation = false;
    bool hightlightAll = false;
    int colorChangeCount = 4;
};

CPointsWaitControlUI::CPointsWaitControlUI() : helper(new UIPointsWaitControlHelper(this))
{
    const int pointValue = 3;
    const int padding = 2;
    if (helper == nullptr) {
        return;
    }
    helper->highlightPointIndex = 0;
    // SetNormalPointColor(0xff007dff);
    // SetHightlightPointColor(0xffcccccc);
    SetNormalPointColor(0xffcccccc);
    SetHightlightPointColor(0xff007dff);
    SetPointPadding(pointValue);
    SetControlPadding(padding);
}

CPointsWaitControlUI::~CPointsWaitControlUI()
{
    delete helper;
    helper = nullptr;
}

void CPointsWaitControlUI::SetControlPadding(int padding)
{
    if (helper == nullptr) {
        return;
    }
    helper->padding = padding;
    Invalidate();
}

void CPointsWaitControlUI::SetAnimationStart(bool start)
{
    if (helper == nullptr) {
        return;
    }
    helper->SetAnimationStart(start);
}

void CPointsWaitControlUI::SetPointPadding(int pointPadding)
{
    if (helper == nullptr) {
        return;
    }
    helper->pointPadding = pointPadding;
    Invalidate();
}

void CPointsWaitControlUI::SetNormalPointColor(DWORD color)
{
    if (helper == nullptr) {
        return;
    }
    helper->normalColor = color;
    Invalidate();
}

void CPointsWaitControlUI::SetHightlightPointColor(DWORD color)
{
    if (helper == nullptr) {
        return;
    }
    helper->highlightColor = color;
    Invalidate();
}

void CPointsWaitControlUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (helper == nullptr) {
        return;
    }
    helper->OnPaint(hDC);
}

LPCTSTR CPointsWaitControlUI::GetClass() const
{
    return _T("PointsWaitControlUI");
}

LPVOID CPointsWaitControlUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_POINTSWAITCONTROL) == 0) {
        return static_cast<CPointsWaitControlUI *>(this);
    }
    return CControlUI::GetInterface(pstrName);
}

void CPointsWaitControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    const int radix = 16;
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("paint_padding")) == 0) {
        SetControlPadding(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("show_highlight")) == 0 && helper != nullptr) {
        helper->hightlightAll = (_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("point_padding")) == 0) {
        SetPointPadding(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("normalcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
            SetNormalPointColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("highlightcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
            SetHightlightPointColor(clrColor);
        }
    }
    if (pstrValue != nullptr) {
        __super::SetAttribute(pstrName, pstrValue);
    }
}

NAMESPACE_DUILIB_END
