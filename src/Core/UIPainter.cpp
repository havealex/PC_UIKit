/*
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
#include "UIPainter.h"

NAMESPACE_DUILIB_BEGIN

class UIPenHelper {
public:
    DWORD color = 0;
};

UIPen::UIPen(DWORD color) : helper(new UIPenHelper)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->color = color;
}

UIPen::UIPen() : helper(new UIPenHelper) {}

UIPen::UIPen(const UIPen &other) : helper(new UIPenHelper)
{
    if (helper == nullptr || other.helper == nullptr) {
        return;
    }
    *helper = *other.helper;
}

UIPen::~UIPen()
{
    if (helper != nullptr) {
        delete helper;
        helper = nullptr;
    }
}

UIPen &UIPen::operator = (const UIPen &other)
{
    if (helper != nullptr && other.helper != nullptr) {
        *helper = *other.helper;
    }
    return *this;
}

DWORD DuiLib::UIPen::GetColor() const
{
    if (this->helper == nullptr) {
        return 0;
    }
    return helper->color;
}

class UIBrushHelper {
public:
    DWORD color = 0;
};

UIBrush::UIBrush() : helper(new UIBrushHelper) {}

UIBrush::UIBrush(DWORD color) : helper(new UIBrushHelper)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->color = color;
}

UIBrush::UIBrush(const UIBrush &other) : helper(new UIBrushHelper)
{
    if (helper != nullptr && other.helper != nullptr) {
        *helper = *other.helper;
    }
}

DWORD DuiLib::UIBrush::GetColor() const
{
    if (this->helper == nullptr) {
        return 0;
    }
    return helper->color;
}

UIBrush::~UIBrush()
{
    if (helper != nullptr) {
        delete helper;
        helper = nullptr;
    }
}

UIBrush &UIBrush::operator = (const UIBrush &other)
{
    if (helper != nullptr && other.helper != nullptr) {
        *helper = *other.helper;
    }
    return *this;
}

class UIState {
public:
    UIPen pen;
    UIBrush brush;
};

class UIPainterHelper {
public:
    UIPainterHelper() : graphics(), stateStack()
    {
        stateStack.push_back(UIState());
    }
    ~UIPainterHelper() {}

    DuiLib::CControlUI *control = nullptr;
    std::unique_ptr<Gdiplus::Graphics> graphics = {};
    std::vector<UIState> stateStack = {};
    HDC hdc = nullptr;

    POINT GetBaseLocation() const
    {
        if (!control) {
            return POINT { 0, 0 };
        }

        RECT rc = control->GetPos();
        return POINT { rc.left, rc.top };
    }
};

UIPainter::UIPainter(DuiLib::CControlUI *control, HDC hdc) : helper(new UIPainterHelper)
{
    if (helper == nullptr || control == nullptr) {
        return;
    }
    helper->control = control;

    if (hdc == nullptr) {
        CPaintManagerUI *pm = control->GetManager();
        if (!pm || pm->GetPaintDC() == nullptr) {
            return;
        }

        hdc = pm->GetPaintDC();
    }

    helper->hdc = hdc;
    helper->graphics = std::make_unique<Gdiplus::Graphics>(hdc);
    helper->graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    SetRenderHint(AntiAlias);
}

UIPainter::UIPainter(DuiLib::UIImage *image) : helper(new UIPainterHelper)
{
    if (helper == nullptr || image == nullptr) {
        return;
    }
    helper->graphics.reset(Gdiplus::Graphics::FromImage(image->GetBitmap()));
}

UIPainter::~UIPainter()
{
    if (helper != nullptr) {
        delete helper;
        helper = nullptr;
    }
}

void DuiLib::UIPainter::SetRenderHint(RenderHint rh)
{
    if (helper == nullptr || !helper->graphics) {
        return;
    }

    if (rh == UIPainter::AntiAlias) {
        helper->graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    }
}

void DuiLib::UIPainter::SetPen(const UIPen &pen)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->stateStack.back().pen = pen;
}

void DuiLib::UIPainter::SetBrush(const UIBrush &brush)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->stateStack.back().brush = brush;
}

void UIPainter::DrawImage(int x, int y, int width, int height, UIImage *image, int srcX, int srcY, int srcWidth,
    int srcHeight)
{
    if (!image || this->helper == nullptr) {
        return;
    }

    int baseX = helper->GetBaseLocation().x;
    int baseY = helper->GetBaseLocation().y;

    if (!image->GetBitmap() || !helper->graphics) {
        return;
    }

    Gdiplus::ImageAttributes imageAttr;

    if (image->GetOpacity() < 100) { // scale 100%
        float alpha = image->GetOpacity() / 100.0f;
        Gdiplus::ColorMatrix cm = {
            1, 0, 0, 0, 0,
            0, 1, 0, 0, 0,
            0, 0, 1, 0, 0,
            0, 0, 0, alpha, 0,
            0, 0, 0, 0, 1 };
        imageAttr.SetColorMatrix(&cm);
    }

    helper->graphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    helper->graphics->DrawImage(image->GetBitmap(), Gdiplus::Rect(x + baseX, y + baseY, width, height), srcX, srcY,
        srcWidth, srcHeight, Gdiplus::UnitPixel, &imageAttr);
}

void UIPainter::DrawImage(int x, int y, int width, int height, UIImage *image)
{
    if (image == nullptr) {
        return;
    }
    DrawImage(x, y, width, height, image, 0, 0, image->GetWidth(), image->GetHeight());
}

void UIPainter::DrawImage(const RECT &rc, UIImage *image)
{
    DrawImage(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, image);
}

void UIPainter::DrawImage(int x, int y, UIImage *image)
{
    if (!image || this->helper == nullptr) {
        return;
    }

    if (!image->GetBitmap() || !helper->graphics) {
        return;
    }

    DrawImage(x, y, image->GetWidth(), image->GetHeight(), image);
}

void UIPainter::DrawLine(const POINT &p1, const POINT &p2)
{
    DrawLine(p1.x, p1.y, p2.x, p2.y);
}

void UIPainter::DrawLine(int x1, int y1, int x2, int y2)
{
    if (this->helper == nullptr || !helper->graphics) {
        return;
    }

    Gdiplus::Pen pen(helper->stateStack.back().pen.GetColor());

    int baseX = helper->GetBaseLocation().x;
    int baseY = helper->GetBaseLocation().y;

    helper->graphics->DrawLine(&pen, Gdiplus::Point(x1 + baseX, y1 + baseY), Gdiplus::Point(x2 + baseX, y2 + baseY));
}

void DuiLib::UIPainter::FillRectange(const RECT &rc)
{
    if (this->helper == nullptr || !helper->graphics) {
        return;
    }

    Gdiplus::SolidBrush brush(helper->stateStack.back().brush.GetColor());
    Gdiplus::Rect rcDraw = Utils::FromWin32RECT(rc);
    int baseX = helper->GetBaseLocation().x;
    int baseY = helper->GetBaseLocation().y;
    rcDraw.Offset(baseX, baseY);
    helper->graphics->FillRectangle(&brush, rcDraw);
}

void DuiLib::UIPainter::DrawRectange(const RECT &rc)
{
    if (this->helper == nullptr || !helper->graphics) {
        return;
    }

    Gdiplus::Pen pen(helper->stateStack.back().pen.GetColor());
    Gdiplus::Rect rcDraw = Utils::FromWin32RECT(rc);
    int baseX = helper->GetBaseLocation().x;
    int baseY = helper->GetBaseLocation().y;
    rcDraw.Offset(baseX, baseY);
    helper->graphics->DrawRectangle(&pen, rcDraw);
}

void UIPainter::Translate(int x, int y)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->graphics->TranslateTransform(x, y);
}

void UIPainter::ResetTransform()
{
    if (this->helper == nullptr) {
        return;
    }
    helper->graphics->ResetTransform();
}

void UIPainter::Save()
{
    if (this->helper == nullptr) {
        return;
    }
    helper->stateStack.push_back(helper->stateStack.back());
}

void UIPainter::Restore()
{
    if (this->helper == nullptr) {
        return;
    }
    if (helper->stateStack.size() > 1) {
        helper->stateStack.pop_back();
    }
}

NAMESPACE_DUILIB_END
