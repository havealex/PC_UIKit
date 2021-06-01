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

#ifndef __UIGROUPBOX_H__
#define __UIGROUPBOX_H__

#pragma once

constexpr int GROUPBOX_TEXT_OFFSET = 20; // 定义GroupBox中的Text相对于左边的偏移

namespace DuiLib {
class UILIB_API CGroupBoxUI : public CVerticalLayoutUI {
    DECLARE_DUICONTROL(CGroupBoxUI)
public:
    CGroupBoxUI();
    ~CGroupBoxUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    void SetFont(int index);
    int GetFont() const;

protected:
    // Paint
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
    SIZE CalcrectSize(SIZE szAvailable);

protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
};
}
#endif // __UIGROUPBOX_H__