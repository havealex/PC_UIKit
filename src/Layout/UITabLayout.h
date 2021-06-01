/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CTabLayoutUI : public CContainerUI {
    DECLARE_DUICONTROL(CTabLayoutUI)
public:
    CTabLayoutUI();
    ~CTabLayoutUI() {}

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;
    void RemoveAll() override;
    int GetCurSel();
    virtual bool SelectItem(int iIndex);
    virtual bool SelectItem(CControlUI *pControl);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

public:
    bool AddSubPageWithName(CControlUI *subpage, const wchar_t *page_name);
    bool SelectPageByName(const wchar_t *page_name);
    std::wstring GetCurrentPageName();

    bool SelectItemByName(CDuiString _controlName);

private:
    struct TTabPageIndex {
        std::wstring Name;
        int Index;
    };
    std::vector<TTabPageIndex> m_PageIndex;

protected:
    int m_iCurSel;
};
}
#endif // __UITABLAYOUT_H__
