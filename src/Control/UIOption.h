/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib {
class UILIB_API COptionUI : public CButtonUI {
    DECLARE_DUICONTROL(COptionUI)
public:
    COptionUI();
    ~COptionUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true) override;

    bool Activate() override;
    void SetEnabled(bool bEnable = true) override;

    LPCTSTR GetSelectedImage();
    void SetSelectedImage(LPCTSTR pStrImage);

    LPCTSTR GetSelectedHotImage();
    void SetSelectedHotImage(LPCTSTR pStrImage);

    LPCTSTR GetSelectedPushedImage();
    void SetSelectedPushedImage(LPCTSTR pStrImage);

    void SetSelectedTextColor(DWORD dwTextColor);
    DWORD GetSelectedTextColor();

    void SetSelectedBkColor(DWORD dwBkColor);
    DWORD GetSelectBkColor();

    void SetSelectedBkColor2(DWORD dwBkColor);
    DWORD GetSelectBkColor2();

    void SetSelectedBkColor4(DWORD dwBkColor);
    DWORD GetSelectBkColor4();

    LPCTSTR GetSelectedForedImage();
    void SetSelectedForedImage(LPCTSTR pStrImage);
    void SetSelectedDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = nullptr);
    bool IsSelected() const;
    virtual void Selected(bool bSelected, bool bforce = true);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintBkColor(HDC hDC);
    void PaintStatusImage(HDC hDC);
    void PaintForeImage(HDC hDC);
    void PaintText(HDC hDC);

    void SetSelected(bool bSelected);
    CDuiString GetToolTip() const override;

protected:
    bool m_bSelected;
    CDuiString m_sGroupName;

    DWORD m_dwSelectedBkColor;
    DWORD m_dwSelectedBkColor2;
    DWORD m_dwSelectedBkColor4;
    DWORD m_dwSelectedTextColor;

    CDuiString m_sSelectedImage;
    CDuiString m_sSelectedHotImage;
    CDuiString m_sSelectedPushedImage;
    CDuiString m_sSelectedForeImage;
    CDuiString selectedDisabledImage;
    int selectedFont = -1;
};

class UILIB_API CCheckBoxUI : public COptionUI {
    DECLARE_DUICONTROL(CCheckBoxUI)

public:
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);

    void SetCheck(bool bCheck);
    bool GetCheck() const;

protected:
    bool m_bAutoCheck;

public:
    CCheckBoxUI();
    ~CCheckBoxUI();
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetAutoCheck(bool bEnable);
    virtual void DoEvent(TEventUI &event) override;
    virtual void Selected(bool bSelected);
};
} // namespace DuiLib

#endif // __UIOPTION_H__