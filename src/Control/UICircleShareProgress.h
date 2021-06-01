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

#ifndef UICIRCLEPROGRESS_H
#define UICIRCLEPROGRESS_H

namespace DuiLib {
class UILIB_API CCircleShareProgressUI : public CProgressUI {
    DECLARE_DUICONTROL(CCircleShareProgressUI)
public:
    CCircleShareProgressUI(void);
    ~CCircleShareProgressUI(void);

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetCircular(BOOL bCircular = TRUE);
    void SetClockwiseRotation(BOOL bClockwise = TRUE);
    void SetCircleWidth(DWORD dwCircleWidth);
    void SetBgColor(DWORD dwBgColor);
    DWORD GetBgColor() const;
    void SetFgColor(DWORD dwBgColor);
    DWORD GetFgColor() const;
    void SetIndicator(LPCTSTR lpIndicatorImage);
    void SetEnableCircleEffect(BOOL bEnableCircleEffect = FALSE);
    void SetCircleGradientColor1(DWORD dwColor);
    void SetCircleGradientColor2(DWORD dwColor);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintBkColor(HDC hDC);

private:
    BOOL progressCircular;
    BOOL progressClockwise;
    DWORD progressCircleWidth;
    DWORD progressBgColor;
    DWORD progressFgColor;
    BOOL progressEnableCircleEffect;
    DWORD progressGradientColor1;
    DWORD progressGradientColor2;
    Gdiplus::Image *progressIndicator;
    CDuiString progressIndicatorImage;
};
}

#endif
