/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace DuiLib {
class IContainerUI {
public:
    virtual CControlUI *GetItemAt(int iIndex) = 0;
    virtual int GetItemIndex(CControlUI *pControl) = 0;
    virtual bool SetItemIndex(CControlUI *pControl, int iIndex) = 0;
    virtual int GetCount() = 0;
    virtual bool Add(CControlUI *pControl) = 0;
    virtual bool AddAt(CControlUI *pControl, int iIndex) = 0;
    virtual bool Remove(CControlUI *pControl) = 0;
    virtual bool RemoveAt(int iIndex) = 0;
    virtual void RemoveAll() = 0;
};

class CScrollBarUI;
class CContainerUI;
class ICustomScrollBar {
public:
    virtual ~ICustomScrollBar() {}

    virtual bool ProcessScrollBar(CContainerUI *ctrl, RECT rc, int cxRequired, int cyRequired) = 0;

    virtual bool SetScrollPos(CContainerUI *ctrl, SIZE szPos, bool bMsg) = 0;
};

class UILIB_API CContainerUI : public CControlUI, public IContainerUI {
    DECLARE_DUICONTROL(CContainerUI)

public:
    CContainerUI();
    virtual ~CContainerUI();

public:
    void SetCustomScrollBar(ICustomScrollBar *customScrollBar);

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CControlUI *GetItemAt(int iIndex) override;
    int GetItemIndex(CControlUI *pControl) override;
    bool SetItemIndex(CControlUI *pControl, int iIndex) override;
    int GetCount() override;

    void SetScrollBarPadding(const RECT &rc);

    template <class T>
    T *AddItem(LPCTSTR pstrName = nullptr)
    {
        T *item = new T();
        if (item == nullptr) {
            return nullptr;
        }
        if (pstrName != nullptr) {
            item->SetName(pstrName);
        }
        if (Add(item)) {
            return item;
        } else {
            delete item;
            return nullptr;
        }
    }

    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;
    bool RemoveAt(int iIndex) override;
    void RemoveAll() override;

    void DoEvent(TEventUI &event) override;
    void SetVisible(bool bVisible = true) override;
    void SetInternVisible(bool bVisible = true) override;
    void SetEnabled(bool bEnabled) override;
    void SetMouseEnabled(bool bEnable = true) override;
    void FocusTreat(TEventUI &event);
    void HScrollTreat(TEventUI &event);
    void VScrollTreat(TEventUI &event);
    bool IsScrollValid(CScrollBarUI *scrollBar);
    virtual RECT GetInset() const;
    virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
    virtual int GetChildPadding() const;
    virtual void SetChildPadding(int iPadding);
    virtual UINT GetChildAlign() const;
    virtual void SetChildAlign(UINT iAlign);
    virtual UINT GetChildVAlign() const;
    virtual void SetChildVAlign(UINT iVAlign);
    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);

    virtual int FindSelectable(int iIndex, bool bForward = true);

    RECT GetClientPos() const;
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoPaint(HDC hDC, const RECT &rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true) override;
    CControlUI *FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    bool SetSubControlText(LPCTSTR pstrSubControlName, LPCTSTR pstrText);
    bool SetSubControlFixedHeight(LPCTSTR pstrSubControlName, int cy);
    bool SetSubControlFixedWdith(LPCTSTR pstrSubControlName, int cx);
    bool SetSubControlUserData(LPCTSTR pstrSubControlName, LPCTSTR pstrText);

    CDuiString GetSubControlText(LPCTSTR pstrSubControlName);
    int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
    int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
    const CDuiString GetSubControlUserData(LPCTSTR pstrSubControlName);
    CControlUI *FindSubControl(LPCTSTR pstrSubControlName);

    virtual SIZE GetScrollPos() const;
    virtual SIZE GetScrollRange() const;
    virtual void SetScrollPos(SIZE szPos, bool bMsg = true);
    virtual void SetScrollStepSize(int nSize);
    virtual int GetScrollStepSize() const;
    virtual void PointerUp();
    virtual void PointerDown();
    virtual void LineUp();
    virtual void LineDown();
    virtual void PageUp();
    virtual void PageDown();
    virtual void HomeUp();
    virtual void EndDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void HomeLeft();
    virtual void EndRight();
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI *GetVerticalScrollBar() const;
    virtual CScrollBarUI *GetHorizontalScrollBar() const;

    void SetOpacity(int n) override;

    void SetBlur(int n) override;

protected:
    virtual void SetFloatPos(int iIndex);
    virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
    ICustomScrollBar *customScrollBar = nullptr;
    CStdPtrArray m_items;
    RECT m_rcInset;
    RECT m_rcScrollBarPadding;
    int m_iChildPadding;
    UINT m_iChildAlign;
    UINT m_iChildVAlign;
    bool m_bAutoDestroy;
    bool m_bDelayedDestroy;
    bool m_bMouseChildEnabled;
    int m_nScrollStepSize;
    POINT m_downPointer;
    SIZE m_downScrollPos;
    CDuiString m_sVerticalScrollBarStyle;
    CDuiString m_sHorizontalScrollBarStyle;

public:
    CScrollBarUI *m_pHorizontalScrollBar;
    CScrollBarUI *m_pVerticalScrollBar;
};
} // namespace DuiLib

#endif // __UICONTAINER_H__
