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

#ifndef _UITOGGLESWITCH_INCLUDE__
#define _UITOGGLESWITCH_INCLUDE__

#include "UIlib.h"

NAMESPACE_DUILIB_BEGIN

class UIToggleSwitchHelper;
class UILIB_API CToggleSwitchUI : public DuiLib::CControlUI {
    DECLARE_DUICONTROL(CToggleSwitchUI)
public:
    static LPCTSTR MSG_TOGGLED;
    CToggleSwitchUI();
    ~CToggleSwitchUI();
    void SetToggleOnClick(bool b);
    void SetOn(bool on);
    void Toggle(bool toggled);
    bool IsOn() const;

protected:
    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    void DoPaint(HDC hDC, const RECT &rcPaint) override;
    void DoEvent(DuiLib::TEventUI &ev)override;
    UINT GetControlFlags() const override;

    UIToggleSwitchHelper *helper;
};

NAMESPACE_DUILIB_END

#endif // _UITOGGLESWITCH_INCLUDE__
