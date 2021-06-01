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
#ifndef __UISLIDESHOW_H__
#define __UISLIDESHOW_H__

namespace DuiLib {
class UILIB_API CSlideShowPrevButtonUI : public CButtonUI {
    DECLARE_DUICONTROL(CSlideShowPrevButtonUI)
public:
};

class UILIB_API CSlideShowNextButtonUI : public CButtonUI {
    DECLARE_DUICONTROL(CSlideShowNextButtonUI)
public:
};

class CSlideShowUIHelper;
class UILIB_API CSlideShowUI : public CHorizontalLayoutUI {
    DECLARE_DUICONTROL(CSlideShowUI)
public:
    CSlideShowUI();
    ~CSlideShowUI();

    void SetSlideDuring(int during);

    void SetSlideTimeout(int timeout);

    void SlideNext();

    void SlidePrev();

    LPCTSTR GetClass() const override;

    UINT GetControlFlags() const override;

    LPVOID GetInterface(LPCTSTR pstrName) override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    bool Add(CControlUI *pControl) override;

    bool Remove(CControlUI *pControl) override;

    virtual void SetPos(RECT rc, bool bNeedInvalidate);

    void DoInit() override;

    void DoPaint(HDC hDC, const RECT &rcPaint) override;

    void DoEvent(TEventUI &ev) override;

    void SetInternVisible(bool bVisible) override;

    void SetVisible(bool bVisible) override;

    void DrawSliding(HDC hDC, const RECT &rcPaint);

    void DrawPoints(HDC hDC, const RECT &rcPaint);

    void Update();

protected:
    CSlideShowUIHelper *helper = nullptr;
};
}

#endif // __UISLIDESHOW_H__
