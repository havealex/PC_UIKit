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

#ifndef __UIANIMATIONLAYOUT_H__
#define __UIANIMATIONLAYOUT_H__

namespace DuiLib {
class UILIB_API CAnimationLayoutUI : public CContainerUI, public CUIAnimation {
    DECLARE_DUICONTROL(CAnimationLayoutUI)
public:
    CAnimationLayoutUI();
    ~CAnimationLayoutUI() {}

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    int SetDirection(int nDirection);

    virtual bool SelectItem(int iIndex);
    virtual bool SelectItem(CControlUI *pControl);

    void AnimationSwitch();
    void DoEvent(TEventUI &event) override;
    void OnTimer(int nTimerID);

    virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
    virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
    virtual void OnAnimationStop(INT nAnimationID);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;
    void RemoveAll() override;
    int GetCurSel();

    virtual void DoPaint(HDC hDC, const RECT &rcPaint);

protected:
    int m_iCurSel;
    bool m_bIsVerticalDirection;
    int m_nPositiveDirection;
    RECT m_rcCurPos = {0};
    RECT m_rcItemOld = {0};
    CControlUI *m_pCurrentControl;
    CControlUI *m_pLastControl;
    RECT m_rcCurrentControlPos = {0};
    RECT m_rcLastControlPos = {0};
    bool m_bControlVisibleFlag;
    bool m_bFirstSetPosFlag;
    enum {
        LAYOUT_ANIMATION_ID = 0,
        LAYOUT_ANIMATION_ELLAPSE = 10,
        LAYOUT_ANIMATION_FRAME_COUNT = 25,
    };
};
}
#endif // __UIANIMATIONLAYOUT_H__