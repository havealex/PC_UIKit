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

#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__
#pragma once
namespace DuiLib {
class UILIB_API CCircleProgressUI : public CProgressUI {
    DECLARE_DUICONTROL(CCircleProgressUI)
public:
    CCircleProgressUI();
    ~CCircleProgressUI();
    LPCTSTR GetClass() const;
    void PaintBkColor(HDC hDC);
    void PaintForeColor(HDC hDC);
    void PaintForeImage(HDC hDC);
};

constexpr int CHARTVIEW_PIE = 0x0;
constexpr int CHARTVIEW_HISTOGRAM = 0x01;

struct tagCHARTITEM {
    CDuiString name;
    double value;
};
using CHARTITEM = tagCHARTITEM;
using PCHARTITEM = tagCHARTITEM *;

class UILIB_API CChartViewUI : public CControlUI {
public:
    DECLARE_DUICONTROL(CChartViewUI)
public:
    CChartViewUI(void);
    ~CChartViewUI(void);

    bool Add(LPCTSTR name, double value);
    bool AddAt(LPCTSTR name, double value, int iIndex);

public:
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    virtual void DoPaint(HDC hDC, const RECT &rcPaint);
    virtual void DoPaintPie(HDC hDC, const RECT &rcPaint);
    virtual void DoPaintHistogram(HDC hDC, const RECT &rcPaint);

private:
    vector<CHARTITEM> m_items;
    int m_ViewStyle;
    CDuiString m_sShadowImage;
    int m_ShadowImageHeight;
    CDuiString m_sPillarImage;
    int m_PillarImageWidth;
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    bool m_bShowHtml;
    bool m_bShowText;
    int m_iFont;
};

class UILIB_API CCircleProgressExUI : public CProgressUI {
    DECLARE_DUICONTROL(CCircleProgressExUI)
public:
    CCircleProgressExUI();
    ~CCircleProgressExUI();
    LPCTSTR GetClass() const;
    void PaintBkColor(HDC hDC);
    void PaintForeColor(HDC hDC);
    void PaintForeImage(HDC hDC);
};
}
#endif __CONTROLEX_H__