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

#ifndef _UIPOINTSWAIT_INCLUDE__
#define _UIPOINTSWAIT_INCLUDE__

#include "UILib.h"

NAMESPACE_DUILIB_BEGIN

class UIPointsWaitControlHelper;
class UILIB_API CPointsWaitControlUI : public DuiLib::CControlUI {
    DECLARE_DUICONTROL(CPointsWaitControlUI)
public:
    CPointsWaitControlUI();
    ~CPointsWaitControlUI();

    void SetControlPadding(int padding);
    void SetAnimationStart(bool start);
    void SetPointPadding(int pointPadding);
    void SetNormalPointColor(DWORD color);
    void SetHightlightPointColor(DWORD color);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

private:
    virtual void DoPaint(HDC hDC, const RECT &rcPaint);
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    UIPointsWaitControlHelper *helper;
};

NAMESPACE_DUILIB_END

#endif // _UIPOINTSWAIT_INCLUDE__
