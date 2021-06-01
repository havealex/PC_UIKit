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
#include "UIRoundedRectButton.h"

#include <string>

NAMESPACE_DUILIB_BEGIN

LPCTSTR CRoundedRectButtonUI::MSG_CLICK = DUI_MSGTYPE_CLICK;

IMPLEMENT_DUICONTROL(CRoundedRectButtonUI)

class UIRoundedRectButtonHelper {
public:
    explicit UIRoundedRectButtonHelper(CRoundedRectButtonUI *button)
    {
        this->button = button;
    }

    ~UIRoundedRectButtonHelper() {}

    CRoundedRectButtonUI::StateRole GetCurrentRole() const
    {
        if (button == nullptr) {
            return CRoundedRectButtonUI::NormalRole;
        }
        if (!button->IsEnabled()) {
            return CRoundedRectButtonUI::DisableRole;
        } else if (checkable && checked) {
            if (mousePressed && !outWhenPressed) {
                return CRoundedRectButtonUI::CheckedPressedRole;
            } else if (mouseMoved) {
                return CRoundedRectButtonUI::CheckedHoveredRole;
            } else {
                return CRoundedRectButtonUI::CheckedRole;
            }
        } else if (mousePressed && !outWhenPressed) {
            return CRoundedRectButtonUI::PressedRole;
        } else if (mouseMoved) {
            return CRoundedRectButtonUI::HoveredRole;
        }

        return CRoundedRectButtonUI::NormalRole;
    }

    void OnPaint(HDC hdc)
    {
        const int multiples = 2;
        const int topLeftAngle = 180;
        const int rightStartAngle = -90;
        const int sweepAngle = 90;
        const int leftStartAngle = 90;
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        if (button == nullptr) {
            return;
        }
        RECT rcItem = button->GetPos();
        Gdiplus::Rect rcItemGDIP = Utils::FromWin32RECT(rcItem);
        if (rcItemGDIP.Width == 0 || rcItemGDIP.Height == 0) {
            return;
        }
        rcItemGDIP.X += 1;
        rcItemGDIP.Height -= multiples;
        rcItemGDIP.Width -= multiples;
        int width = rcItemGDIP.Width;
        int height = rcItemGDIP.Height;
        int arcWidth = (roundWidth > height / multiples || roundWidth == -1) ? height : roundWidth * multiples;
        Gdiplus::Point ptTopLeft;
        rcItemGDIP.GetLocation(&ptTopLeft);

        Gdiplus::Rect rcArcLeft(ptTopLeft.X, ptTopLeft.Y, arcWidth, arcWidth);
        Gdiplus::Rect rcArcRight(rcItemGDIP.GetRight() - arcWidth, ptTopLeft.Y, arcWidth, arcWidth);

        Gdiplus::GraphicsPath path;

        // top left arc
        path.AddArc(rcArcLeft, topLeftAngle, sweepAngle);
        // top line
        if (arcWidth < width) {
            path.AddLine(rcItemGDIP.GetLeft() + arcWidth / multiples, rcItemGDIP.GetTop(),
                rcItemGDIP.GetRight() - arcWidth / multiples, rcItemGDIP.GetTop());
        }
        // top right arc
        path.AddArc(rcArcRight, rightStartAngle, sweepAngle);
        if (arcWidth < height) {
            // right line
            path.AddLine(rcItemGDIP.GetRight(), rcItemGDIP.GetTop() + arcWidth / multiples, rcItemGDIP.GetRight(),
                rcItemGDIP.GetBottom() - arcWidth / multiples);
        }
        // bottom right arc
        rcArcRight.Offset(0, rcItemGDIP.Height - arcWidth);
        path.AddArc(rcArcRight, 0, sweepAngle);
        // bottom line
        path.AddLine(rcItemGDIP.GetRight() - arcWidth / multiples, rcItemGDIP.GetBottom(),
            rcItemGDIP.GetLeft() + arcWidth / multiples, rcItemGDIP.GetBottom());
        // bottom left
        rcArcLeft.Offset(0, rcItemGDIP.Height - arcWidth);
        path.AddArc(rcArcLeft, leftStartAngle, sweepAngle);
        // left line
        if (arcWidth < height) {
            // left line
            path.AddLine(rcItemGDIP.GetLeft(), rcItemGDIP.GetTop() + arcWidth / multiples, rcItemGDIP.GetLeft(),
                rcItemGDIP.GetBottom() - arcWidth / multiples);
        }

        Gdiplus::SolidBrush brushBk(0);
        Gdiplus::Pen penBorder(Gdiplus::Color(0), 1);

        int currentState = GetCurrentRole();

        brushBk.SetColor(bkColorMap[currentState]);
        penBorder.SetColor(borderColorMap[currentState]);

        Gdiplus::Color c;
        brushBk.GetColor(&c);
        if (c.GetA() == 0) {
            brushBk.SetColor(bkColorMap[CRoundedRectButtonUI::NormalRole]);
        }
        brushBk.GetColor(&c);
        if (c.GetA() > 0) {
            graphics.FillPath(&brushBk, &path);
        }
        penBorder.GetColor(&c);
        if (c.GetA() == 0) {
            penBorder.SetColor(borderColorMap[CRoundedRectButtonUI::NormalRole]);
        }
        penBorder.GetColor(&c);
        if (c.GetA() > 0) {
            graphics.DrawPath(&penBorder, &path);
        }

        DuiLib::CDuiString currentText = textMap[currentState];
        Gdiplus::SolidBrush brushText(textColorMap[currentState]);
        if (currentText.IsEmpty() && currentState != CRoundedRectButtonUI::NormalRole) {
            currentText = textMap[CRoundedRectButtonUI::NormalRole];
        }

        brushText.GetColor(&c);
        if (c.GetA() == 0 && currentState != CRoundedRectButtonUI::NormalRole) {
            brushText.SetColor(textColorMap[CRoundedRectButtonUI::NormalRole]);
        }

        brushText.GetColor(&c);
        if (button == nullptr) {
            return;
        }
        if (button->GetManager() && !currentText.IsEmpty() && c.GetA() > 0) {
            if (HFONT hfont = button->GetManager()->GetFont(button->GetFont())) {
                Gdiplus::Font f(button->GetManager()->GetPaintDC(), hfont);
                Gdiplus::StringFormat sf;
                sf.SetAlignment(Gdiplus::StringAlignmentCenter);
                sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                Gdiplus::RectF rcText((Gdiplus::REAL)rcItemGDIP.GetLeft(), (Gdiplus::REAL)rcItemGDIP.GetTop(),
                    (Gdiplus::REAL)rcItemGDIP.Width, (Gdiplus::REAL)rcItemGDIP.Height + multiples);
                graphics.DrawString(currentText.GetData(), currentText.GetLength(), &f, rcText, &sf, &brushText);
            }
        }
    }

    CRoundedRectButtonUI *button;
    std::map<int, DWORD> textColorMap;
    std::map<int, DWORD> bkColorMap;
    std::map<int, DWORD> borderColorMap;
    std::map<int, DuiLib::CDuiString> textMap;

    bool checked = false;
    bool checkable = false;
    bool outWhenPressed = false;
    bool mouseMoved = false;
    bool mousePressed = false;
    int roundWidth = 0;
    int fontId = 0;
};

CRoundedRectButtonUI::CRoundedRectButtonUI() : helper(new UIRoundedRectButtonHelper(this))
{
    const int hightValue = 40;
    const int fontValue = 16;
    SetFixedHeight(hightValue);
    SetMouseEnabled(true);
    SetCursor(DUI_HAND);
    if (helper == nullptr) {
        return;
    }
    helper->fontId = -1;
    SetChecked(false);
    SetCheckable(false);
    SetRoundedWidth(-1);

    SetStyle(Style::Blue);
    SetNormalImage(_T("drawable/synergy_common/btn_blue_round_normal.png"));
    SetPushedImage(_T("drawable/synergy_common/btn_blue_round_press.png"));
    SetHotImage(_T("drawable/synergy_common/btn_blue_round_hover.png"));
    SetDisabledImage(_T("drawable/synergy_common/btn_blue_round_disable.png"));

    SetPtFont(fontValue, false);
    SetTextColor(0xffffffff);
    SetDisabledTextColor(0xffFDFDFD);
}

void CRoundedRectButtonUI::SetStyle(Style style)
{
}

void CRoundedRectButtonUI::SetRoundedWidth(int rwidth)
{
    bool result = false;
    DWORD err = 0;
    double scale = Utils::GetDPIFactor(result, err);
    if (helper == nullptr) {
        return;
    }
    helper->roundWidth = static_cast<int>(rwidth * scale);
    Invalidate();
}

void CRoundedRectButtonUI::SetText(const DuiLib::CDuiString &text, StateRole role)
{
    if (helper == nullptr) {
        return;
    }
    helper->textMap[role] = text;
    if (role == NormalRole) {
        __super::SetText(text);
    }
    Invalidate();
}

void CRoundedRectButtonUI::SetBackgroundColor(DWORD color, StateRole role)
{
    if (helper == nullptr) {
        return;
    }
    helper->bkColorMap[role] = color;
    Invalidate();
}

CRoundedRectButtonUI::~CRoundedRectButtonUI()
{
    delete helper;
    helper = nullptr;
}

void CRoundedRectButtonUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    __super::DoPaint(hDC, rcPaint);
}

void CRoundedRectButtonUI::DoEvent(DuiLib::TEventUI &e)
{
    __super::DoEvent(e);
}

void CRoundedRectButtonUI::SetCheckable(bool checkable)
{
    if (helper == nullptr) {
        return;
    }
    helper->checkable = checkable;
    Invalidate();
}

bool CRoundedRectButtonUI::IsChecked() const
{
    if (helper == nullptr) {
        return false;
    }
    return helper->checked;
}

void CRoundedRectButtonUI::SetChecked(bool checked)
{
    if (helper == nullptr) {
        return;
    }
    helper->checked = checked;
    Invalidate();
}

void CRoundedRectButtonUI::SetTextColor(DWORD color, StateRole role)
{
    if (helper == nullptr) {
        return;
    }
    __super::SetTextColor(color);
    helper->textColorMap[role] = color;
    Invalidate();
}

void CRoundedRectButtonUI::SetBorderColor(DWORD color, StateRole role)
{
    if (helper == nullptr) {
        return;
    }
    helper->borderColorMap[role] = color;
    Invalidate();
}

UINT CRoundedRectButtonUI::GetControlFlags() const
{
    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

LPCTSTR CRoundedRectButtonUI::GetClass() const
{
    return _T("RoundedRectButtonUI");
}

void CRoundedRectButtonUI::SetEnabled(bool enabled)
{
    if (helper == nullptr) {
        return;
    }
    helper->mouseMoved = false;
    SetCursor(enabled ? DUI_HAND : DUI_ARROW);
    __super::SetEnabled(enabled);
    Invalidate();
}

LPVOID CRoundedRectButtonUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_ROUNDEDRECTBUTTON) == 0) {
        return static_cast<CRoundedRectButtonUI *>(this);
    }
    return CControlUI::GetInterface(pstrName);
}

void CRoundedRectButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("text")) == 0)
        SetText(pstrValue, NormalRole);
    if (_tcsicmp(pstrName, _T("roundwidth")) == 0) {
        int v = std::stoi(std::wstring(pstrValue));
        SetRoundedWidth(v);
    }
    __super::SetAttribute(pstrName, pstrValue);
}

NAMESPACE_DUILIB_END
