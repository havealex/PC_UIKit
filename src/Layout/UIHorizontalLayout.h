/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CHorizontalLayoutUI : public CContainerUI {
    DECLARE_DUICONTROL(CHorizontalLayoutUI)
public:
    CHorizontalLayoutUI();
    ~CHorizontalLayoutUI() {}

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetSepWidth(int iWidth);
    int GetSepWidth() const;
    void SetSepImmMode(bool bImmediately);
    bool IsSepImmMode() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void DoEvent(TEventUI &event) override;

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoPostPaint(HDC hDC, const RECT &rcPaint);

    RECT GetThumbRect(bool bUseNew = false) const;

    SIZE EstimateSize(SIZE szAvailable);

protected:
    int m_iSepWidth;
    UINT m_uButtonState;
    POINT ptLastMouse;
    RECT m_rcNewPos;
    bool m_bImmMode;
};
}
#endif // __UIHORIZONTALLAYOUT_H__
