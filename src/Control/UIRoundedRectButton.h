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

#ifndef _UIROUNDEDRECTBUTTON_INCLUDE__
#define _UIROUNDEDRECTBUTTON_INCLUDE__

#include "UILib.h"

NAMESPACE_DUILIB_BEGIN

class UIRoundedRectButtonHelper;
class UILIB_API CRoundedRectButtonUI : public DuiLib::CButtonUI {
    DECLARE_DUICONTROL(CRoundedRectButtonUI)
public:
    static LPCTSTR MSG_CLICK;

    enum StateRole {
        NormalRole,
        HoveredRole,
        PressedRole,
        CheckedRole,
        CheckedHoveredRole,
        CheckedPressedRole,
        DisableRole,
        DisableHoveredRole,
        DisablePressedRole,
    };

    enum class Style { White, Blue };

    CRoundedRectButtonUI();
    ~CRoundedRectButtonUI();

    void SetStyle(Style style);

    void SetRoundedWidth(int rwidth = -1);

    void SetText(const DuiLib::CDuiString &text, StateRole role = NormalRole);

    void SetTextColor(DWORD color, StateRole role = NormalRole);

    void SetBackgroundColor(DWORD color, StateRole role = NormalRole);

    void SetBorderColor(DWORD color, StateRole role = NormalRole);

    void SetCheckable(bool checkable);

    void SetChecked(bool checked);

    void SetEnabled(bool enabled) override;

    bool IsChecked() const;

    void DoPaint(HDC hDC, const RECT &rcPaint) override;

    void DoEvent(DuiLib::TEventUI &e) override;

    UINT GetControlFlags() const override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

protected:
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

private:
    UIRoundedRectButtonHelper *helper;
};

NAMESPACE_DUILIB_END

#endif // _UIROUNDEDRECTBUTTON_INCLUDE__
