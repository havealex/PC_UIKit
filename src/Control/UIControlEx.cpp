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
#include "UIControlEx.h"
#include <sstream>
namespace DuiLib {
IMPLEMENT_DUICONTROL(CCircleProgressUI)
CCircleProgressUI::CCircleProgressUI() {}

CCircleProgressUI::~CCircleProgressUI() {}

LPCTSTR CCircleProgressUI::GetClass() const
{
    return _T("CircleProgress");
}

void CCircleProgressUI::PaintBkColor(HDC hDC)
{
    const int sweepAngle = 360;
    Gdiplus::Graphics g(hDC);
    RECT rcPos = GetPos();
    Gdiplus::SolidBrush brush(m_dwBackColor);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, sweepAngle);
    g.ReleaseHDC(hDC);
}

void CCircleProgressUI::PaintForeColor(HDC hDC)
{
    const int multiples = 2;
    const int sweepAngle = 360;
    Gdiplus::Graphics g(hDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    RECT rcPos = GetPos();
    Gdiplus::SolidBrush brush(m_dwBackColor2);
    int nStartDegree = 270;
    int nSweepDegree = static_cast<int>(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
    int nSweepWidth = m_dwSweepWidth;
    g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree,
        nSweepDegree);
    Gdiplus::SolidBrush brushbk(m_dwForeColor);
    g.FillPie(&brushbk,
        rcPos.left + nSweepWidth, rcPos.top + nSweepWidth,
        rcPos.right - rcPos.left - multiples * nSweepWidth,
        rcPos.bottom - rcPos.top - multiples * nSweepWidth,
        0, sweepAngle);

    g.ReleaseHDC(hDC);
}

void CCircleProgressUI::PaintForeImage(HDC hDC) {}

IMPLEMENT_DUICONTROL(CChartViewUI)
CChartViewUI::CChartViewUI(void)
    : m_ViewStyle(CHARTVIEW_PIE),
      m_sShadowImage(_T("")),
      m_ShadowImageHeight(0),
      m_sPillarImage(_T("")),
      m_PillarImageWidth(0),
      m_dwTextColor(0),
      m_dwDisabledTextColor(0),
      m_bShowHtml(false),
      m_bShowText(true),
      m_iFont(-1)
{}

CChartViewUI::~CChartViewUI(void) {}

bool CChartViewUI::Add(LPCTSTR name, double value)
{
    CHARTITEM item;
    item.name = name;
    item.value = value;

    m_items.push_back(item);
    return true;
}

bool CChartViewUI::AddAt(LPCTSTR name, double value, int iIndex)
{
    if (iIndex < 0 || iIndex > static_cast<int>(m_items.size())) {
        return false;
    }

    CHARTITEM item;
    item.name = name;
    item.value = value;

    m_items.insert(m_items.begin() + iIndex, item);

    return true;
}

void CChartViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    const int radix = 10;
    CControlUI::SetAttribute(pstrName, pstrValue);
    if (_tcscmp(pstrName, _T("view")) == 0) {
        if (_tcscmp(pstrValue, _T("pie")) == 0) {
            m_ViewStyle = CHARTVIEW_PIE;
        } else if (_tcscmp(pstrValue, _T("histpgram")) == 0) {
            m_ViewStyle = CHARTVIEW_HISTOGRAM;
        }
    } else if (_tcscmp(pstrName, _T("shadowimage")) == 0) {
        m_sShadowImage = pstrValue;
    } else if (_tcscmp(pstrName, _T("shadowimageheight")) == 0) {
        LPTSTR pstr = nullptr;
        m_ShadowImageHeight = _tcstol(pstrValue, &pstr, radix);
    } else if (_tcscmp(pstrName, _T("pillarimage")) == 0) {
        m_sPillarImage = pstrValue;
    } else if (_tcscmp(pstrName, _T("pillarimagewidth")) == 0) {
        LPTSTR pstr = nullptr;
        m_PillarImageWidth = _tcstol(pstrValue, &pstr, radix);
    } else if (_tcscmp(pstrName, _T("showhtml")) == 0) {
        m_bShowHtml = (_tcscmp(pstrValue, _T("true")) == 0);
    } else if (_tcscmp(pstrName, _T("showtext")) == 0) {
        m_bShowText = (_tcscmp(pstrValue, _T("true")) == 0);
    } else if (_tcscmp(pstrName, _T("font")) == 0) {
        LPTSTR pstr = nullptr;
        m_iFont = _tcstol(pstrValue, &pstr, radix);
    }
}

void CChartViewUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem))
        return;
    CControlUI::DoPaint(hDC, rcPaint);

    if (m_ViewStyle == CHARTVIEW_PIE) {
        DoPaintPie(hDC, rcPaint);
    } else if (m_ViewStyle == CHARTVIEW_HISTOGRAM) {
        DoPaintHistogram(hDC, rcPaint);
    }
}

void CChartViewUI::DoPaintPie(HDC hDC, const RECT &rcPaint)
{
    // 颜色总数以及颜色
    const int NumOfColor = 18;
    const int topPaddingMultiples = 2;
    const int txtPadding = 10;
    const int leftPaddingMultiples = 6;
    const int txtTopMultiples = 4;
    const int sweepAngle = 360;
    const int lfTxtStep = 20;
    const int dtwValues = 100;
    const int rcLeftMultiples = 3;
    const int txtTopStep = 30;
    const int rgbValue = 242;
    static COLORREF bkColor[] = {
        RGB(63, 150, 245),
        RGB(255, 90, 1),
        RGB(106, 244, 192),
        RGB(245, 234, 106),
        RGB(252, 123, 151),
        RGB(162, 245, 63),
        RGB(157, 144, 252),
        RGB(210, 144, 252),
        RGB(127, 176, 203),
        RGB(244, 196, 113),
        RGB(162, 194, 133),
        RGB(189, 23, 23),
        RGB(189, 23, 179),
        RGB(97, 23, 189),
        RGB(23, 43, 189),
        RGB(18, 136, 76),
        RGB(136, 121, 18),
        RGB(136, 38, 18)
    };

    int numOfItem = static_cast<int>(m_items.size());
    if (numOfItem == 0) {
        return;
    }

    // 绘画文字留出该值的宽度
    int drawTextWidth = (m_bShowText ? dtwValues : 0);

    int i = 0;
    double amountOfItem = 0.f;

    for (i = 0; i < numOfItem; i++) {
        amountOfItem += m_items[i].value;
    }

    Gdiplus::Graphics graphics(hDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    int drawPadding = 5; // 留出一定位置画阴影
    // 计算绘图的合适位置(居中画饼图)
    int itemWidth = m_rcItem.right - m_rcItem.left;
    int itemHeight = m_rcItem.bottom - m_rcItem.top;
    int drawWidth = itemWidth - drawTextWidth - drawPadding * topPaddingMultiples;
    int drawHeight = itemHeight - drawPadding * txtPadding;
    int drawMaxSize = min(drawWidth, drawHeight);
    int drawLeftPadding = leftPaddingMultiples * drawPadding;
    int drawLeft = m_rcItem.left + drawLeftPadding;
    int drawTop = m_rcItem.top + drawPadding + (drawHeight - drawMaxSize) / topPaddingMultiples;
    int drawTextTop = m_rcItem.top + txtTopMultiples * drawPadding;
    int drawTextLeft = drawLeft + drawMaxSize + lfTxtStep;

    int drawTextHeight = 18;
    if (drawTextTop <= m_rcItem.top + drawPadding) {
        drawTextTop = m_rcItem.top + drawPadding;
    }
    Gdiplus::REAL startAngle = 0;
    const int graphicStep = 5;
    for (int i = 0; i < numOfItem; i++) {
        // 画饼图
        Gdiplus::Color color;
        color.SetFromCOLORREF(bkColor[i % NumOfColor]);
        Gdiplus::SolidBrush brush(color);

        Gdiplus::REAL endAngle = (Gdiplus::REAL)(m_items[i].value / amountOfItem * sweepAngle);
        graphics.FillPie(&brush, drawLeft, drawTop, drawMaxSize, drawMaxSize, startAngle, endAngle);
        startAngle += endAngle;

        // 画文字
        if (m_bShowText) {
            // 绘画文字
            if (i % txtTopMultiples == 0) {
                drawTextLeft = drawLeft + drawMaxSize + lfTxtStep;
            }
            RECT rcText = { 0 };
            rcText.left = drawTextLeft + drawTextHeight;
            rcText.top = drawTextTop;
            rcText.right = rcText.left + drawTextWidth + drawTextHeight;
            rcText.bottom = rcText.top + drawTextHeight;

            if (::IntersectRect(&m_rcPaint, &rcText, &m_rcItem)) {
                graphics.FillPie(&brush, rcText.left, rcText.top, drawTextHeight, drawTextHeight, 0, sweepAngle);
                // 因为在文字前面画了一个色块， 所以文字要在色块后面输出, 5为文字和色块的间距
                rcText.left += drawTextHeight + graphicStep;
                int nLinks = 0;
                DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
                if (m_bShowHtml) {
                    CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_items[i].name, m_dwTextColor, m_iFont,
                        nullptr, nullptr, nLinks, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                } else {
                    CRenderEngine::DrawText(hDC, m_pManager, rcText, m_items[i].name, m_dwTextColor, m_iFont,
                        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                }
            }

            // 计算下一个文字输出垂直位置的偏移量， 4为文字垂直间距
            if ((i + 1) % txtTopMultiples == 0) {
                drawTextTop += drawTextHeight + txtTopStep;
            }
            drawTextLeft += drawTextWidth + txtTopMultiples;
        }
    }
    // 填充内部
    Gdiplus::Color intern_color;
    intern_color.SetFromCOLORREF(RGB(rgbValue, rgbValue, rgbValue));
    Gdiplus::SolidBrush intern_brush(intern_color); //
    graphics.FillPie(&intern_brush, drawLeft + graphicStep, drawTop + graphicStep, drawMaxSize - txtPadding,
        drawMaxSize - txtPadding, 0, sweepAngle);

    RECT rcText = { 0 };
    rcText.left = drawLeft + lfTxtStep;
    rcText.right = rcText.left + drawMaxSize;
    rcText.top = drawTop + +drawMaxSize + txtPadding;
    rcText.bottom = rcText.top + txtPadding;
    wostringstream strSize;
    strSize << m_items[amountOfItem - 1].value;
    wstring wstr = strSize.str() + L"GB/";
    wstring s;
    strSize.str(s);
    strSize << amountOfItem;
    wstr += strSize.str() + L"GB";
    CRenderEngine::DrawText(hDC, m_pManager, rcText, wstr.c_str(), m_dwTextColor, m_iFont,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    int nLinks = 0;
    int ratio = (1 - m_items[amountOfItem - 1].value / amountOfItem) * dtwValues;
    strSize.str(s);
    strSize << ratio;
    wstr = strSize.str();
    CDuiString ratio_str = _T("{c#000000}");
    ratio_str.Append(wstr.c_str());
    ratio_str.Append(_T("%{/c}"));
    wstr = L"{ c #000000 }" + strSize.str() + L"%{/c}";
    rcText.left = drawLeft + drawMaxSize / rcLeftMultiples;
    rcText.top = drawTop + +drawMaxSize;
    rcText.right = rcText.left + drawMaxSize;
    rcText.bottom = rcText.top + txtPadding;
    CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, ratio_str, m_dwTextColor, m_iFont, nullptr, nullptr, nLinks,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void CChartViewUI::DoPaintHistogram(HDC hDC, const RECT &rcPaint)
{
    const int highValue = 25;
    const int multiples = 2;
    // 画柱状图
    int drawTextHeight = (m_bShowText ? highValue : 0); // 柱状图底部留出该值的高度以绘制文字

    // 画柱状图阴影
    if (!m_sShadowImage.IsEmpty()) {
        RECT rcShadow = m_rcItem;
        rcShadow.top = m_rcItem.bottom - m_ShadowImageHeight - drawTextHeight;
        rcShadow.right = m_rcItem.right;
        rcShadow.bottom = m_rcItem.bottom - drawTextHeight;
        if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcShadow, m_rcPaint, m_sShadowImage, nullptr)) {
            m_sShadowImage.Empty();
        }
    }

    int numOfItem = static_cast<int>(m_items.size());
    if (numOfItem == 0) {
        return;
    }

    int i = 0;
    double maxOfItem = 0.f;
    double amountOfItem = 0.f;
    for (i = 0; i < numOfItem; i++) {
        if (m_items[i].value > maxOfItem) {
            maxOfItem = m_items[i].value;
        }

        amountOfItem += m_items[i].value;
    }

    int drawWidth = m_rcItem.right - m_rcItem.left; // 绘图宽度
    int drawHeight = m_rcItem.bottom - m_rcItem.top - drawTextHeight; // 绘图高度
    int pillarSpacing = (drawWidth / m_PillarImageWidth) * multiples; // 柱子间间距

    // 柱子的绘制水平偏移位置
    int drawLeft = pillarSpacing;
    for (i = 0; i < numOfItem; i++) {
        // 绘画柱子
        if (!m_sPillarImage.IsEmpty()) {
            double rateOfItem = m_items[i].value / maxOfItem;
            RECT rcPillar = m_rcItem;
            rcPillar.left = m_rcItem.left + drawLeft;
            rcPillar.top = m_rcItem.bottom - drawTextHeight - static_cast<int>(drawHeight * rateOfItem);
            rcPillar.right = m_rcItem.left + drawLeft + m_PillarImageWidth;
            rcPillar.bottom = m_rcItem.bottom - drawTextHeight;
            if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcPillar, m_rcPaint, m_sPillarImage, nullptr)) {
                m_sPillarImage.Empty();
            }
        }

        if (m_bShowText) {
            // 绘画文字
            RECT rcText = m_rcItem;
            rcText.left = m_rcItem.left + drawLeft - pillarSpacing;
            rcText.top = m_rcItem.bottom - drawTextHeight;
            rcText.right = m_rcItem.left + drawLeft + m_PillarImageWidth + pillarSpacing;
            rcText.bottom = m_rcItem.bottom;

            int nLinks = 0;
            DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
            if (m_bShowHtml) {
                CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_items[i].name, m_dwTextColor, m_iFont, nullptr,
                    nullptr, nLinks, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            } else {
                CRenderEngine::DrawText(hDC, m_pManager, rcText, m_items[i].name, m_dwTextColor, m_iFont,
                    DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }


        // 计算下一个柱子的偏移位置
        drawLeft += pillarSpacing;
        drawLeft += m_PillarImageWidth;
    }
}

IMPLEMENT_DUICONTROL(CCircleProgressExUI)
CCircleProgressExUI::CCircleProgressExUI() {}

CCircleProgressExUI::~CCircleProgressExUI() {}

LPCTSTR CCircleProgressExUI::GetClass() const
{
    return _T("CircleProgressEx");
}

void CCircleProgressExUI::PaintBkColor(HDC hDC)
{
    const int sweepAngle = 360;
    Gdiplus::Graphics g(hDC);
    RECT rcPos = GetPos();
    Gdiplus::SolidBrush brush(m_dwBackColor);
    g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, sweepAngle);
    g.ReleaseHDC(hDC);
}

void CCircleProgressExUI::PaintForeColor(HDC hDC)
{
    const int sweepAngle = 360;
    const int posStep = 10;
    const int negStep = 20;
    Gdiplus::Graphics g(hDC);
    g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    RECT rcPos = GetPos();
    Gdiplus::SolidBrush brush(m_dwForeColor);
    int nStartDegree = 90;
    int nSweepDegree = static_cast<int>(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
    g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree,
        nSweepDegree);

    Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
    g.FillPie(&brushbk, rcPos.left + posStep, rcPos.top + posStep, rcPos.right - rcPos.left - negStep,
        rcPos.bottom - rcPos.top - negStep, 0, sweepAngle);
    g.ReleaseHDC(hDC);
}

void CCircleProgressExUI::PaintForeImage(HDC hDC) {}
}