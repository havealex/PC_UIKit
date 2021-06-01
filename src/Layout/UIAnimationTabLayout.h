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

#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

namespace DuiLib {
class UILIB_API CAnimationTabLayoutUI : public CTabLayoutUI, public CUIAnimation {
    DECLARE_DUICONTROL(CAnimationTabLayoutUI)
public:
    CAnimationTabLayoutUI();
    ~CAnimationTabLayoutUI() {}

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool SelectItem(int iIndex) override;
    void AnimationSwitch();
    void DoEvent(TEventUI &event) override;
    void OnTimer(int nTimerID);

    virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
    virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
    virtual void OnAnimationStop(INT nAnimationID);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
    bool m_bIsVerticalDirection;
    int m_nPositiveDirection;
    RECT m_rcCurPos = {0};
    RECT m_rcItemOld = {0};
    CControlUI *m_pCurrentControl;
    bool m_bControlVisibleFlag;
    enum {
        TAB_ANIMATION_ID = 1,
        TAB_ANIMATION_ELLAPSE = 10,
        TAB_ANIMATION_FRAME_COUNT = 25,
    };
};
}
#endif // __UIANIMATIONTABLAYOUT_H__