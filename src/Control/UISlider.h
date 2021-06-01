/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib {
class UILIB_API CSliderUI : public CProgressUI {
    DECLARE_DUICONTROL(CSliderUI)
public:
    CSliderUI();
    ~CSliderUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true) override;

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    LPCTSTR GetThumbImage() const;
    void SetThumbImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage() const;
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage() const;
    void SetThumbPushedImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    void PaintStatusImage(HDC hDC);

    void SetValue(int nValue);
    void SetCanSendMove(bool bCanSend);
    bool GetCanSendMove() const;

protected:
    SIZE m_szThumb;
    UINT m_uButtonState;
    int m_nStep;

    CDuiString m_sThumbImage;
    CDuiString m_sThumbHotImage;
    CDuiString m_sThumbPushedImage;

    CDuiString m_sImageModify;
    bool m_bSendMove;
};
}

#endif // __UISLIDER_H__