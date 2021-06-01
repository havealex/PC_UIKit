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

#ifndef __ANIMPANEL_H__
#define __ANIMPANEL_H__

namespace DuiLib {
constexpr int PANEL_ANIMATION_ID = 2000;

class UILIB_API CAnimPanelUI : public CControlUI, public CUIAnimation {
    DECLARE_DUICONTROL(CAnimPanelUI)
public:
    CAnimPanelUI();
    ~CAnimPanelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit() override;

    void DoEvent(TEventUI &event) override;
    void OnTimer(int nTimerID);

    virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop);
    virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
    virtual void OnAnimationStop(INT nAnimationID);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    int GetMinFrame();
    void SetMinFrame(int iMinFrame);

    int GetMaxFrame();
    void SetMaxFrame(int iMaxFrame);
    void SetStep(int step);

    bool GetIsLoop();
    void SetIsLoop(bool bIsLoop);

    bool GetAutoPlay();
    void SetAutoPlay(bool bAutoPlay);

    LPCTSTR GetAnimImage();
    void SetAnimImage(LPCTSTR pStrImage);

    UINT GetTimerEllapse();
    void SetTimerEllapse(UINT uTimerEllapse);

    void StartAnim();
    void StopAnim();

private:
    int m_iMinFrame = 0;
    int m_iMaxFrame = 0;
    int m_Step = 1; // 不知这样设初值起作用吗, 所以在构造函数中也初始化了
    bool m_bIsLoop = false;
    bool m_bAutoPlay = false;
    CDuiString m_sAnimImage;
    UINT m_uTimerEllapse = 25;

    int m_iAnimTimerID = -1;
    static int s_iAnimTimerID;

protected:
};
}

#endif
