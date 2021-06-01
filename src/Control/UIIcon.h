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

#pragma once
#ifndef _UIICON_INCLUDE__
#define _UIICON_INCLUDE__

namespace DuiLib {
// ////////////////////////////////////////////////////////////////////////
// CUIIcon
class UILIB_API CIconUI : public CControlUI {
    DECLARE_DUICONTROL(CIconUI)
public:
    CIconUI(void);
    ~CIconUI(void);

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetPos(RECT rc);
    void SetInset(RECT rc);
    void SetVisible(bool bVisible) override;
    void SetInternVisible(bool bVisible) override;
    void DoPaint(HDC hDC, const RECT &rcPaint);
    void DoEvent(TEventUI &event) override;

private:
    CDuiString m_sIcoImage;
    CDuiString m_sDefaultIcon;
    RECT m_rcInset;
};
}
#endif
