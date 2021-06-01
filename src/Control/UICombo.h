/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI {
    DECLARE_DUICONTROL(CComboUI)
    friend class CComboWnd;

public:
    CComboUI();
    ~CComboUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetDropBoxInitType(int _type)
    {
        m_typeInit = _type;
    };

    void DoInit() override;
    UINT GetControlFlags() const;

    CDuiString GetText();
    void SetEnabled(bool bEnable = true) override;

    void SetTextStyle(UINT uStyle);
    UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() override;
    bool SelectItem(int iIndex, bool bTakeFocus = false) override;
    bool SelectMultiItem(int iIndex, bool bTakeFocus = false) override;
    bool UnSelectItem(int iIndex, bool bOthers = false) override;
    bool SetItemIndex(CControlUI *pControl, int iIndex) override;
    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;
    bool RemoveAt(int iIndex) override;
    void RemoveAll() override;

    bool Activate() override;

    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    TListInfoUI *GetListInfo() override;
    void SetItemFont(int index);
    void SetItemPtFont(int nSize, bool bBold = false, DuiLib::FontType ft = FontType::Text);
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
    DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
    DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
    LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
    DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DoPaint(HDC hDC, const RECT &rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

    void SetScrollSelect(bool bScrollSelect);
    bool GetScrollSelect();
    void DoEventWhenScrollWheel(TEventUI &event);
    void SetAttributeItemAlign(LPCTSTR pstrValue);

protected:
    CComboWnd *m_pWindow;

    int m_iCurSel;
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
    CDuiString m_sDropBoxAttributes;
    SIZE m_szDropBox;
    UINT m_uButtonState;

    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;

    bool bItemUsePtFont = false;
    int nItemFontSize = 10;
    bool bItemFontBold = false;
    DuiLib::FontType itemFontType = FontType::Text;
    int m_typeInit;
    bool m_bScrollSelect;

public:
    TListInfoUI m_ListInfo;
};
} // namespace DuiLib

#endif // __UICOMBO_H__
