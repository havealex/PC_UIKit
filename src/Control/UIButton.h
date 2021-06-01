/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib {
class UILIB_API CButtonUI : public CLabelUI {
    DECLARE_DUICONTROL(CButtonUI)

public:
    CButtonUI();
    ~CButtonUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool Activate() override;
    void SetEnabled(bool bEnable = true) override;
    void DoEvent(TEventUI &event) override;

    virtual LPCTSTR GetNormalImage();
    virtual void SetNormalImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetHotImage();
    virtual void SetHotImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetPushedImage();
    virtual void SetPushedImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetFocusedImage();
    virtual void SetFocusedImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetDisabledImage();
    virtual void SetDisabledImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetHotForeImage();
    virtual void SetHotForeImage(LPCTSTR pStrImage);

    void BindTabIndex(int _BindTabIndex);
    void BindTabLayoutName(LPCTSTR _TabLayoutName);
    void BindTriggerTabSel(int _SetSelectIndex = -1);
    void RemoveBindTabIndex();
    int GetBindTabLayoutIndex();
    LPCTSTR GetBindTabLayoutName();

    void SetNormalBkColor(DWORD dwColor);
    void SetHotBkColor(DWORD dwColor);
    DWORD GetHotBkColor() const;
    void SetPushedBkColor(DWORD dwColor);
    DWORD GetPushedBkColor() const;
    void SetHotTextColor(DWORD dwColor);
    DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
    DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
    DWORD GetFocusedTextColor() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);
    RECT SubPaintText();
    void PaintBkColor(HDC hDC);
    void PaintStatusImage(HDC hDC);
    void PaintForeImage(HDC hDC);
    UINT GetButtonState();
    void SetButtonState(UINT state);
    void SetVisible(bool bVisible = true) override;
    void SetHoverTextShow(bool val);
    bool GetHoverTextShow();
    void SetRoundEnhance(bool val);
    int GetLinkContentIndex(const POINT &ptMouse);
    bool GetNcHit() override;
    CDuiString GetToolTip() const override;

protected:
    UINT m_uButtonState;

    DWORD m_dwNormalBkColor;
    DWORD m_dwHotBkColor;
    DWORD m_dwPushedBkColor;
    DWORD m_dwHotTextColor;
    DWORD m_dwPushedTextColor;
    DWORD m_dwFocusedTextColor;

    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sHotForeImage;
    CDuiString m_sPushedImage;
    CDuiString m_sPushedForeImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;

    int m_iBindTabIndex;
    CDuiString m_sBindTabLayoutName;

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks = 0;
    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    int m_nHoverLink;
    int m_iLinkIndex[MAX_LINK];
    bool m_bHoverTextShow = true;
    bool m_roundEnhance = false;
};
} // namespace DuiLib

#endif // __UIBUTTON_H__