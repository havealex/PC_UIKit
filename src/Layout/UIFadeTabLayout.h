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

#ifndef __UIFADETABLAYOUT_H__
#define __UIFADETABLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CFadeTabLayoutUI : public CTabLayoutUI, public CUIAnimation {
    DECLARE_DUICONTROL(CFadeTabLayoutUI)
public:
    CFadeTabLayoutUI();
    virtual ~CFadeTabLayoutUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool SelectItem(int iIndex) override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);

    virtual void DoPaint(HDC hDC, const RECT &rcPaint);

    void DoEvent(TEventUI &event) override;
    void OnTimer(int nTimerID);

    virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop);
    virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
    virtual void OnAnimationStop(INT nAnimationID);

    void AnimationSwitch();

protected:
    int m_iLastSel;
    BYTE m_bFadeAlpha;

    enum {
        FADETAB_ANIMATION_ID = 1,
        FADETAB_ANIMATION_ELLAPSE = 10,
        FADETAB_ANIMATION_FRAME_COUNT = 25,
    };
};
}
#endif // __UIFADETABLAYOUT_H__
