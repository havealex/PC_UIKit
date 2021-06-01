/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UILIST_H__
#define __UILIST_H__

namespace DuiLib {

using PULVCompareFunc = int(CALLBACK *)(UINT_PTR, UINT_PTR, UINT_PTR);

class CListHeaderUI;

constexpr int UILIST_MAX_COLUMNS = 32;

struct tagTListInfoUI {
    int nColumns;
    RECT rcColumn[UILIST_MAX_COLUMNS];
    int nFont;
    UINT uTextStyle;
    RECT rcTextPadding;
    DWORD dwTextColor;
    DWORD dwBkColor;
    CDuiString sBkImage;
    bool bAlternateBk;
    DWORD dwSelectedTextColor;
    DWORD dwSpecialTextColor;
    DWORD dwSelectedBkColor;
    CDuiString sSelectedImage;
    DWORD dwHotTextColor;
    DWORD dwHotBkColor;
    CDuiString sHotImage;
    DWORD dwDisabledTextColor;
    DWORD dwDisabledBkColor;
    CDuiString sDisabledImage;
    DWORD dwLineColor;
    bool bShowRowLine;
    bool bShowColumnLine;
    bool bShowHtml;
    bool bMultiExpandable;
    bool bOptimizeScroll = true;
    DWORD dwLinePaddingHead = 0;
    DWORD dwLinePaddingTail = 0;
};
using TListInfoUI = tagTListInfoUI;

class IListCallbackUI {
public:
    virtual LPCTSTR GetItemText(CControlUI *pList, int iItem, int iSubItem) = 0;
};

class IListOwnerUI {
public:
    virtual TListInfoUI *GetListInfo() = 0;
    virtual int GetCurSel() = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false) = 0;
    virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) = 0;
    virtual bool UnSelectItem(int iIndex, bool bOthers = false) = 0;
    virtual void DoEvent(TEventUI &event) = 0;
};

class IListUI : public IListOwnerUI {
public:
    virtual CListHeaderUI *GetHeader() const = 0;
    virtual CContainerUI *GetList() const = 0;
    virtual IListCallbackUI *GetTextCallback() const = 0;
    virtual void SetTextCallback(IListCallbackUI *pCallback) = 0;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
    virtual int GetExpandedItem() const = 0;

    virtual void SetMultiSelect(bool bMultiSel) = 0;
    virtual bool IsMultiSelect() const = 0;
    virtual void SelectAllItems() = 0;
    virtual void UnSelectAllItems() = 0;
    virtual int GetSelectItemCount() = 0;
    virtual int GetNextSelItem(int nItem) = 0;
};

class IListItemUI {
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual IListOwnerUI *GetOwner() = 0;
    virtual void SetOwner(CControlUI *pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true) = 0;
    virtual bool SelectMulti(bool bSelect = true) = 0;
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(HDC hDC, const RECT &rcItem) = 0;
};

class CListBodyUI;
class CListHeaderUI;
class CEditUI;
class CComboBoxUI;
class UILIB_API CListUI : public CVerticalLayoutUI, public IListUI {
    DECLARE_DUICONTROL(CListUI)

public:
    CListUI();
    ~CListUI() {}
    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() override;
    int GetCurSelActivate() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false) override;
    bool SelectItemActivate(int iIndex); // 双击选中

    bool SelectMultiItem(int iIndex, bool bTakeFocus = false) override;
    void SetMultiSelect(bool bMultiSel) override;
    bool IsMultiSelect() const override;
    bool UnSelectItem(int iIndex, bool bOthers = false) override;
    void SelectAllItems() override;
    void UnSelectAllItems() override;
    int GetSelectItemCount() override;
    int GetNextSelItem(int nItem) override;

    CListHeaderUI *GetHeader() const override;
    CContainerUI *GetList() const override;
    TListInfoUI *GetListInfo() override;

    CControlUI *GetItemAt(int iIndex) override;
    int GetItemIndex(CControlUI *pControl) override;
    bool SetItemIndex(CControlUI *pControl, int iIndex) override;
    int GetCount() override;
    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;
    bool RemoveAt(int iIndex) override;
    void RemoveAll() override;

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    bool IsDelayedDestroy() const override;
    void SetDelayedDestroy(bool bDelayed) override;
    int GetChildPadding() const override;
    void SetChildPadding(int iPadding) override;

    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
    void SetItemTextPadding(RECT rc);
    void SetItemTextColor(DWORD dwTextColor);
    void SetItemBkColor(DWORD dwBkColor);
    void SetItemBkImage(LPCTSTR pStrImage);
    void SetAlternateBk(bool bAlternateBk);
    void SetSelectedItemTextColor(DWORD dwTextColor);
    void SetSpecialItemTextColor(DWORD dwTextColor);
    void SetSelectedItemBkColor(DWORD dwBkColor);
    void SetSelectedItemImage(LPCTSTR pStrImage);
    void SetHotItemTextColor(DWORD dwTextColor);
    void SetHotItemBkColor(DWORD dwBkColor);
    void SetHotItemImage(LPCTSTR pStrImage);
    void SetDisabledItemTextColor(DWORD dwTextColor);
    void SetDisabledItemBkColor(DWORD dwBkColor);
    void SetDisabledItemImage(LPCTSTR pStrImage);
    void SetItemLineColor(DWORD dwLineColor);
    void SetItemLinePadding(int head, int tail);
    void SetItemShowRowLine(bool bShowLine = false);
    void SetItemShowColumnLine(bool bShowLine = false);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);
    RECT GetItemTextPadding() const;
    DWORD GetItemTextColor() const;
    DWORD GetItemBkColor() const;
    LPCTSTR GetItemBkImage() const;
    bool IsAlternateBk() const;
    DWORD GetSelectedItemTextColor() const;
    DWORD GetSelectedItemBkColor() const;
    LPCTSTR GetSelectedItemImage() const;
    DWORD GetHotItemTextColor() const;
    DWORD GetHotItemBkColor() const;
    LPCTSTR GetHotItemImage() const;
    DWORD GetDisabledItemTextColor() const;
    DWORD GetDisabledItemBkColor() const;
    LPCTSTR GetDisabledItemImage() const;
    DWORD GetItemLineColor() const;

    void SetMultiExpanding(bool bMultiExpandable);
    int GetExpandedItem() const override;
    bool ExpandItem(int iIndex, bool bExpand = true) override;

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    IListCallbackUI *GetTextCallback() const override;
    void SetTextCallback(IListCallbackUI *pCallback) override;

    SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos, bool bMsg = true);
    void LineUp() override;
    void LineDown() override;
    void PageUp() override;
    void PageDown() override;
    void HomeUp() override;
    void EndDown() override;
    void LineLeft() override;
    void LineRight() override;
    void PageLeft() override;
    void PageRight() override;
    void HomeLeft() override;
    void EndRight() override;
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false) override;
    CScrollBarUI *GetVerticalScrollBar() const override;
    CScrollBarUI *GetHorizontalScrollBar() const override;
    BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

    virtual BOOL CheckColumEditable(int nColum)
    {
        return FALSE;
    };
    virtual CEditUI *GetEditUI()
    {
        return nullptr;
    };

    virtual BOOL CheckColumComboBoxable(int nColum)
    {
        return FALSE;
    };
    virtual CComboBoxUI *GetComboBoxUI()
    {
        return nullptr;
    };
    void AddClipRect(const RECT &rc);
    void ClearClipRect();

protected:
    virtual void DoPaint(HDC hDC, const RECT &rcPaint);
    int GetMinSelItemIndex();
    int GetMaxSelItemIndex();

protected:
    bool m_bScrollSelect;
    int m_iCurSel;
    bool m_bMultiSel;
    CStdPtrArray m_aSelItems;
    int m_iCurSelActivate; // 双击的列
    int m_iExpandedItem;
    std::vector<RECT> m_rcClipRectList;

public:
    CListBodyUI *m_pList;
    CListHeaderUI *m_pHeader;
    IListCallbackUI *m_pCallback;
    TListInfoUI m_ListInfo;
};

class UILIB_API CListBodyUI : public CVerticalLayoutUI {
public:
    explicit CListBodyUI(CListUI *pOwner);
    ~CListBodyUI() {};
    void SetScrollPos(SIZE szPos, bool bMsg = true);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoEvent(TEventUI &event) override;
    BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

protected:
    static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
    int __cdecl ItemComareFunc(const void *item1, const void *item2);

protected:
    PULVCompareFunc m_pCompareFunc = nullptr;
    UINT_PTR m_compareData = NULL;

public:
    CListUI *m_pOwner;
};


class UILIB_API CListHeaderUI : public CHorizontalLayoutUI {
    DECLARE_DUICONTROL(CListHeaderUI)
public:
    CListHeaderUI();
    ~CListHeaderUI() {};

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetScaleHeader(bool bIsScale);
    bool IsScaleHeader() const;

private:
    bool m_bIsScaleHeader;
};

class UILIB_API CListHeaderItemUI : public CContainerUI {
    DECLARE_DUICONTROL(CListHeaderItemUI)

public:
    CListHeaderItemUI();
    ~CListHeaderItemUI() {};

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true) override;

    bool IsDragable() const;
    void SetDragable(bool bDragable);
    DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
    DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
    DWORD GetTextColor() const;
    void SetTextColor(DWORD dwTextColor);
    void SetTextPadding(RECT rc);
    RECT GetTextPadding() const;
    void SetFont(int index);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSepImage() const;
    void SetSepImage(LPCTSTR pStrImage);
    void SetScale(int nScale);
    int GetScale() const;

    void DoEvent(TEventUI &event) override;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    RECT GetThumbRect() const;

    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    POINT ptLastMouse;
    bool m_bDragable;
    UINT m_uButtonState;
    int m_iSepWidth;
    DWORD m_dwTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    bool m_bShowHtml;
    RECT m_rcTextPadding;
    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sSepImage;
    CDuiString m_sSepImageModify;
    int m_nScale;
};

class UILIB_API CListElementUI : public CControlUI, public IListItemUI {
public:
    CListElementUI();
    ~CListElementUI() {};
    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true) override;

    int GetIndex() const override;
    void SetIndex(int iIndex) override;

    IListOwnerUI *GetOwner() override;
    void SetOwner(CControlUI *pOwner) override;
    void SetVisible(bool bVisible = true) override;

    bool IsSelected() const override;
    bool Select(bool bSelect = true) override;
    bool SelectMulti(bool bSelect = true) override;
    bool IsExpanded() const override;
    bool Expand(bool bExpand = true) override;
    // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    void Invalidate();
    bool Activate() override;

    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);


    void SetTextPadding(RECT rc)
    {
        m_rcTextPadding = rc;
        m_bSetTextPadding = true;
    }

    void SetFont(int index)
    {
        m_iFont = index;
        m_bFontSet = true;
    }

    void SetEndellipsis(bool val)
    {
        m_bEndellipsis = val;
    }

    void DrawItemBk(HDC hDC, const RECT &rcItem);

protected:
    int m_iIndex;
    bool m_bSelected;
    UINT m_uButtonState;

    RECT m_rcTextPadding;
    bool m_bSetTextPadding;
    bool m_bEndellipsis;

    int m_iFont;
    bool m_bFontSet;

public:
    IListOwnerUI *m_pOwner;
};

class UILIB_API CListLabelElementUI : public CListElementUI {
    DECLARE_DUICONTROL(CListLabelElementUI)
public:
    CListLabelElementUI();
    ~CListLabelElementUI() {};
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoEvent(TEventUI &event) override;
    SIZE EstimateSize(SIZE szAvailable);
    void DoPaint(HDC hDC, const RECT &rcPaint);

    void DrawItemText(HDC hDC, const RECT &rcItem);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetFixedWidth(int cx) override;
};

class UILIB_API CListTextElementUI : public CListLabelElementUI {
    DECLARE_DUICONTROL(CListTextElementUI)
public:
    CListTextElementUI();
    ~CListTextElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    LPCTSTR GetText(int iIndex);
    void SetText(int iIndex, LPCTSTR pstrText);

    void SetOwner(CControlUI *pOwner) override;
    CDuiString *GetLinkContent(int iIndex);

    void DoEvent(TEventUI &event) override;
    SIZE EstimateSize(SIZE szAvailable);

    void DrawItemText(HDC hDC, const RECT &rcItem);

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    int m_nHoverLink;
    IListUI *m_pOwner;
    CStdPtrArray m_aTexts;

public:
    CDuiString m_sLinks[MAX_LINK];
};

class UILIB_API CListContainerElementUI : public CHorizontalLayoutUI, public IListItemUI {
    DECLARE_DUICONTROL(CListContainerElementUI)
public:
    CListContainerElementUI();
    ~CListContainerElementUI() {};

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    int GetIndex() const override;
    void SetIndex(int iIndex) override;

    IListOwnerUI *GetOwner() override;
    void SetOwner(CControlUI *pOwner) override;
    void SetVisible(bool bVisible = true) override;
    void SetEnabled(bool bEnable = true) override;

    void SetUseSpecialColor(bool bSpecial);
    bool IsSelected() const override;
    bool IsUseSpecialColor() const;
    bool Select(bool bSelect = true) override;
    bool SelectMulti(bool bSelect = true) override;
    bool IsExpanded() const override;
    bool Expand(bool bExpand = true) override;
    // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    void Invalidate();
    bool Activate() override;

    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void DoPaint(HDC hDC, const RECT &rcPaint);

    virtual void DrawItemText(HDC hDC, const RECT &rcItem);
    virtual void DrawItemBk(HDC hDC, const RECT &rcItem);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

protected:
    int m_iIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    bool m_bUseSpecialColor;

public:
    IListOwnerUI *m_pOwner;
    RECT bkRoundPadding = { 0 }; // 距离边框距离
    LONG bkRound = 0;

private:
    void PaintBorder(HDC hDC);
    // 不可以选中
    bool unselectState = false;
};
} // namespace DuiLib

#endif // __UILIST_H__
