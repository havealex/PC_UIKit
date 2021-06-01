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
#include "UICircleShareProgress.h"

static const int ARGB_TRANSFER_COLOR_OFFSET = 16;
static const int IMAGE_INFO_STRIDE = 4;
static const int GDIPLUS_POINT_CENTER = 2;
static const int TRANSFER_HEXADECIMAL = 16;
static const float PROGRESS_XPOS = -1.0F;
static const float INFLATE_CENTER = -0.5;
static const int START_ANGLE = 270;
static const int SWEEP_ANGLE = 360;
static const int BLEND_COUNT = 3;

namespace DuiLib {
const float CIRCLE_DEFAULT_WIDTH = 10.0f; // default width 10

IMPLEMENT_DUICONTROL(CCircleShareProgressUI)

Gdiplus::Color ARGB2Color(DWORD dwColor)
{
    return Gdiplus::Color(HIBYTE((dwColor) >> ARGB_TRANSFER_COLOR_OFFSET),
        GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
}

CCircleShareProgressUI::CCircleShareProgressUI(void)
    : progressCircular(FALSE),
      progressClockwise(TRUE),
      progressCircleWidth(CIRCLE_DEFAULT_WIDTH),
      progressFgColor(),
      progressBgColor(),
      progressEnableCircleEffect(FALSE),
      progressGradientColor1(),
      progressGradientColor2(),
      progressIndicatorImage(L""),
      progressIndicator(nullptr)
{}

CCircleShareProgressUI::~CCircleShareProgressUI(void)
{
    if (progressIndicator != nullptr) {
        delete progressIndicator;
        progressIndicator = nullptr;
    }
}

void CCircleShareProgressUI::SetCircular(BOOL bCircular)
{
    progressCircular = bCircular;
    Invalidate();
}

void CCircleShareProgressUI::SetClockwiseRotation(BOOL bClockwise)
{
    if (bClockwise != progressClockwise) {
        progressClockwise = bClockwise;
        if (progressIndicator != nullptr) {
            // 已经旋转了图片，旋转到相反的方向
            progressIndicator->RotateFlip(Gdiplus::Rotate180FlipNone);
        }
    }
}

void CCircleShareProgressUI::SetCircleWidth(DWORD dwCircleWidth)
{
    progressCircleWidth = dwCircleWidth;
    Invalidate();
}

void CCircleShareProgressUI::SetBgColor(DWORD dwBgColor)
{
    progressBgColor = dwBgColor;
    Invalidate();
}

DWORD CCircleShareProgressUI::GetBgColor() const
{
    return progressBgColor;
}

void CCircleShareProgressUI::SetFgColor(DWORD dwFgColor)
{
    progressFgColor = dwFgColor;
    Invalidate();
}

DWORD CCircleShareProgressUI::GetFgColor() const
{
    return progressFgColor;
}

void CCircleShareProgressUI::SetIndicator(LPCTSTR lpIndicatorImage)
{
    ASSERT(lpIndicatorImage);
    if (progressIndicatorImage != lpIndicatorImage) {
        progressIndicatorImage = lpIndicatorImage;
        const TImageInfo *imgInfo = GetManager()->GetImageEx(progressIndicatorImage);
        if (imgInfo == nullptr || imgInfo->hBitmap == nullptr) {
            return;
        }
        BITMAP bmp;
        GetObject(imgInfo->hBitmap, sizeof(BITMAP), &bmp);

        progressIndicator = new Gdiplus::Bitmap(imgInfo->nX, imgInfo->nY, imgInfo->nX * IMAGE_INFO_STRIDE,
            PixelFormat32bppARGB, static_cast<BYTE *>(bmp.bmBits));
        if (progressIndicator == nullptr) {
            return;
        }
        Gdiplus::Status state = progressIndicator->GetLastStatus();
        if (Gdiplus::Ok == state) {
            // 假定图片指向上
            progressIndicator->RotateFlip(progressClockwise ? Gdiplus::Rotate90FlipNone :
                Gdiplus::Rotate270FlipNone);
            Invalidate();
        }
    }
}

void CCircleShareProgressUI::SetEnableCircleEffect(BOOL bEnableCircleEffect)
{
    progressEnableCircleEffect = bEnableCircleEffect;
}

void CCircleShareProgressUI::SetCircleGradientColor1(DWORD dwColor)
{
    progressGradientColor1 = dwColor;
    Invalidate();
}

void CCircleShareProgressUI::SetCircleGradientColor2(DWORD dwColor)
{
    progressGradientColor2 = dwColor;
    Invalidate();
}

void CCircleShareProgressUI::PaintBkColor(HDC hDC)
{
    CProgressUI::PaintBkColor(hDC);
    if (progressCircular) {
        if (m_nMax <= m_nMin) {
            m_nMax = m_nMin + 1;
        }
        if (m_nValue > m_nMax) {
            m_nValue = m_nMax;
        }
        if (m_nValue < m_nMin) {
            m_nValue = m_nMin;
        }
        int direction = progressClockwise ? 1 : -1;
        Gdiplus::REAL bordersize = 1.0;
        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        // 圆形中心
        Gdiplus::PointF center;
        center.X = m_rcItem.left + (m_rcItem.right - m_rcItem.left) / GDIPLUS_POINT_CENTER;
        center.Y = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top) / GDIPLUS_POINT_CENTER;
        // 控件矩形内的最大正方形的边界
        int side = min(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
        Gdiplus::RectF rcBorder;
        rcBorder.X = center.X - side / GDIPLUS_POINT_CENTER;
        rcBorder.Y = center.Y - side / GDIPLUS_POINT_CENTER;
        rcBorder.Width = side;
        rcBorder.Height = side;
        // 进度弧形的边界
        Gdiplus::RectF outer = rcBorder;
        if (progressIndicator) {
            outer.Inflate(PROGRESS_XPOS * progressIndicator->GetWidth() / GDIPLUS_POINT_CENTER,
                PROGRESS_XPOS * progressIndicator->GetWidth() / GDIPLUS_POINT_CENTER);
        } else {
            outer.Inflate(INFLATE_CENTER * progressCircleWidth, INFLATE_CENTER * progressCircleWidth);
        }
        outer.Inflate(PROGRESS_XPOS, PROGRESS_XPOS);
        Gdiplus::Pen borderPen(Gdiplus::Color::White, bordersize);
        Gdiplus::Pen bgPen(progressBgColor, progressCircleWidth);
        Gdiplus::RectF rcArk;
        rcArk.X = outer.X + outer.Width / GDIPLUS_POINT_CENTER -
            progressCircleWidth / GDIPLUS_POINT_CENTER;
        rcArk.Y = outer.Y - progressCircleWidth / GDIPLUS_POINT_CENTER;
        rcArk.Width = rcArk.Height = progressCircleWidth;
        if (!progressEnableCircleEffect) {
            Gdiplus::Pen fgPen(progressFgColor, progressCircleWidth);
            graphics.DrawArc(&bgPen, outer, START_ANGLE, SWEEP_ANGLE);
            graphics.DrawArc(&fgPen, outer, START_ANGLE,
                direction * SWEEP_ANGLE * (m_nValue - m_nMin) / (m_nMax - m_nMin));
        } else {
            Gdiplus::REAL factors[4] = { 0.0f, 0.9f, 0.0f };  // blend init valaue
            Gdiplus::REAL position[4] = { 0.0f, 0.5f, 1.0f }; // position init valaue
            Gdiplus::LinearGradientBrush lgbrush(rcBorder, ARGB2Color(progressGradientColor1),
                ARGB2Color(progressGradientColor2), Gdiplus::LinearGradientModeVertical);
            lgbrush.SetBlend(factors, position, BLEND_COUNT);
            graphics.DrawArc(&bgPen, outer, START_ANGLE, SWEEP_ANGLE);
            Gdiplus::Pen fgPen(&lgbrush, progressCircleWidth);
            graphics.DrawArc(&fgPen, outer, START_ANGLE,
                direction * SWEEP_ANGLE * (m_nValue - m_nMin) / (m_nMax - m_nMin));
        }
        Gdiplus::Matrix matrix;
        matrix.RotateAt(direction * SWEEP_ANGLE * (m_nValue - m_nMin) / (m_nMax - m_nMin), center,
            Gdiplus::MatrixOrderAppend);
        graphics.SetTransform(&matrix);
        if (progressIndicator) {
            Gdiplus::RectF rectf;
            rectf.X = center.X - progressIndicator->GetWidth() / GDIPLUS_POINT_CENTER;
            rectf.Y = outer.Y + bordersize / GDIPLUS_POINT_CENTER -
                progressIndicator->GetHeight() / GDIPLUS_POINT_CENTER;
            rectf.Width = progressIndicator->GetWidth();
            rectf.Height = progressIndicator->GetHeight();
            graphics.DrawImage(progressIndicator, rectf);
        }
    }
}

void CCircleShareProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcscmp(pstrName, _T("circular")) == 0) {
        SetCircular(_tcscmp(pstrValue, _T("true")) == 0);
    } else if (_tcscmp(pstrName, _T("clockwise")) == 0) {
        SetClockwiseRotation(_tcscmp(pstrValue, _T("true")) == 0);
    } else if (_tcscmp(pstrName, _T("circlewidth")) == 0) {
        SetCircleWidth(_ttoi(pstrValue));
    } else if (_tcscmp(pstrName, _T("bgcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        DWORD clrColor = _tcstoul(pstrValue, nullptr, TRANSFER_HEXADECIMAL);
        SetBgColor(clrColor);
    } else if (_tcscmp(pstrName, _T("fgcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        DWORD clrColor = _tcstoul(pstrValue, nullptr, TRANSFER_HEXADECIMAL);
        SetFgColor(clrColor);
    } else if (_tcscmp(pstrName, _T("indicator")) == 0) {
        SetIndicator(pstrValue);
    } else if (_tcscmp(pstrName, _T("enablecircleeffect")) == 0) {
        SetEnableCircleEffect(_tcscmp(pstrValue, _T("true")) == 0);
    } else if (_tcscmp(pstrName, _T("gradientcolor1")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        DWORD clrColor = _tcstoul(pstrValue, nullptr, TRANSFER_HEXADECIMAL);
        SetCircleGradientColor1(clrColor);
    } else if (_tcscmp(pstrName, _T("gradientcolor2")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        DWORD clrColor = _tcstoul(pstrValue, nullptr, TRANSFER_HEXADECIMAL);
        SetCircleGradientColor2(clrColor);
    } else {
        CProgressUI::SetAttribute(pstrName, pstrValue);
    }
}

LPCTSTR CCircleShareProgressUI::GetClass() const
{
    return DUI_CTR_CIRCLE_SHARE_PROGRES;
}

LPVOID CCircleShareProgressUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_CIRCLE_SHARE_PROGRES) == 0) {
        return static_cast<CCircleShareProgressUI *>(this);
    }
    return CProgressUI::GetInterface(pstrName);
}
}