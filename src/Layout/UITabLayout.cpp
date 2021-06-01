/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UITabLayout.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CTabLayoutUI)
CTabLayoutUI::CTabLayoutUI() : m_iCurSel(-1) {}

LPCTSTR CTabLayoutUI::GetClass() const
{
    return _T("TabLayoutUI");
}

LPVOID CTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_TABLAYOUT) == 0) {
        return static_cast<CTabLayoutUI *>(this);
    }
    return CContainerUI::GetInterface(pstrName);
}

bool CTabLayoutUI::Add(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    bool ret = CContainerUI::Add(pControl);
    if (!ret) {
        return ret;
    }

    if (m_iCurSel == -1 && pControl->IsVisible()) {
        m_iCurSel = GetItemIndex(pControl);
    } else {
        pControl->SetVisible(false);
    }

    return ret;
}

bool CTabLayoutUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (pControl == nullptr) {
        return false;
    }
    bool ret = CContainerUI::AddAt(pControl, iIndex);
    if (!ret) {
        return ret;
    }

    if (m_iCurSel == -1 && pControl->IsVisible()) {
        m_iCurSel = GetItemIndex(pControl);
    } else if (m_iCurSel != -1 && iIndex <= m_iCurSel) {
        m_iCurSel += 1;
    } else {
        pControl->SetVisible(false);
    }

    return ret;
}

bool CTabLayoutUI::Remove(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }

    int index = GetItemIndex(pControl);
    bool ret = CContainerUI::Remove(pControl);
    if (!ret) {
        return false;
    }

    if (m_iCurSel == index) {
        if (GetCount() > 0) {
            m_iCurSel = 0;

            // Null pointer dereferences, Dereference null return (stat)
            CControlUI *item1 = GetItemAt(m_iCurSel);

            if (item1 != nullptr) {
                item1->SetVisible(true);
            }
        } else {
            m_iCurSel = -1;
        }
        NeedParentUpdate();
    } else if (m_iCurSel > index) {
        m_iCurSel -= 1;
    }

    return ret;
}

void CTabLayoutUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
    NeedParentUpdate();
}

int CTabLayoutUI::GetCurSel()
{
    return m_iCurSel;
}

bool CTabLayoutUI::SelectItem(int iIndex)
{
    if (iIndex < 0 || iIndex >= m_items.GetSize()) {
        return false;
    }
    if (iIndex == m_iCurSel) {
        return true;
    }

    int iOldSel = m_iCurSel;
    m_iCurSel = iIndex;

    CControlUI *itemDest = nullptr;
    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *item1 = GetItemAt(it);

        if (item1 == nullptr) {
            continue;
        }

        if (it == iIndex) {
            itemDest = item1;
            break;
        }
    }

    if (itemDest) {
        bool blocked = false;
        if (m_pManager) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT_IN_PRE, (WPARAM)&blocked, (LPARAM)iIndex);
        }
        if (blocked) {
            m_iCurSel = iOldSel;
            return false;
        }

        itemDest->SetVisible(true);
        itemDest->SetFocus();
        SetPos(m_rcItem);
    }

    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *item1 = GetItemAt(it);

        if (item1 == nullptr) {
            continue;
        }

        if (it != iIndex) {
            item1->SetVisible(false);
        }
    }

    NeedParentUpdate();

    if (m_pManager != nullptr) {
        m_pManager->SetNextTabControl();
        m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, iOldSel);

        CControlUI *pItemIn = GetItemAt(m_iCurSel);
        if (pItemIn) {
            m_pManager->SendNotify(pItemIn, DUI_MSGTYPE_TABSELECT_IN, m_iCurSel, 0);
        }
        CControlUI *pItemOut = GetItemAt(iOldSel);
        if (pItemOut) {
            m_pManager->SendNotify(pItemOut, DUI_MSGTYPE_TABSELECT_OUT, iOldSel, 0);
        }
    }
    return true;
}

bool CTabLayoutUI::SelectItem(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) {
        return false;
    } else {
        return SelectItem(iIndex);
    }
}

void CTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("selectedid")) == 0) {
        SelectItem(_ttoi(pstrValue));
    }
    return CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CTabLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    RECT m_rcInset = CTabLayoutUI::m_rcInset;
    CResourceManager::GetInstance()->Scale(&m_rcInset);

    // Adjust for inset
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it));
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible())
            continue;
        if (pControl->IsFloat()) {
            SetFloatPos(it);
            continue;
        }

        if (it != m_iCurSel) {
            continue;
        }

        RECT rcPadding = pControl->GetPadding();
        rc.left += rcPadding.left;
        rc.top += rcPadding.top;
        rc.right -= rcPadding.right;
        rc.bottom -= rcPadding.bottom;

        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

        SIZE sz = pControl->EstimateSize(szAvailable);
        if (sz.cx == 0) {
            sz.cx = MAX(0, szAvailable.cx);
        }
        if (sz.cx < pControl->GetMinWidth()) {
            sz.cx = pControl->GetMinWidth();
        }
        if (sz.cx > pControl->GetMaxWidth()) {
            sz.cx = pControl->GetMaxWidth();
        }

        if (sz.cy == 0) {
            sz.cy = MAX(0, szAvailable.cy);
        }
        if (sz.cy < pControl->GetMinHeight())
            sz.cy = pControl->GetMinHeight();
        if (sz.cy > pControl->GetMaxHeight())
            sz.cy = pControl->GetMaxHeight();

        RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
        pControl->SetPos(rcCtrl, bNeedInvalidate);
    }
}

// -----------------------------------------------------
// 功能: 在TAB容器中加入新的页面，并记住它的名字
// 说明:
//
bool CTabLayoutUI::AddSubPageWithName(CControlUI *subpage, const wchar_t *page_name)
{
    for (int i = 0; i < m_PageIndex.size(); i++) {
        if (m_PageIndex[i].Name == page_name) {
            // TAB页的名字已经存在了
            return false;
        }
    }

    int last_index = GetCount();

    if (!Add(subpage)) {
        // 新TAB页加入不成功
        return false;
    }

    // 把名字和索引记下来
    TTabPageIndex item;
    item.Index = last_index;
    item.Name = page_name;
    m_PageIndex.push_back(item);

    return true;
}

// -----------------------------------------------------
// 功能: 根据名字找到页面，并设为当前显示
// 说明:
//
bool CTabLayoutUI::SelectPageByName(const wchar_t *page_name)
{
    for (int i = 0; i < m_PageIndex.size(); i++) {
        if (m_PageIndex[i].Name == page_name) {
            // 找到了
            bool is_ok = SelectItem(m_PageIndex[i].Index);
            return is_ok;
        }
    }

    // 没有找到
    return false;
}

// -----------------------------------------------------
// 功能: 获取当前页面的名字
// 说明:
//
wstring CTabLayoutUI::GetCurrentPageName()
{
    for (int i = 0; i < m_PageIndex.size(); i++) {
        if (m_PageIndex[i].Index == m_iCurSel) {
            // 找到了
            return m_PageIndex[i].Name;
        }
    }

    // 没有找到
    return L"";
}


bool CTabLayoutUI::SelectItemByName(CDuiString _controlName)
{
    if (_controlName.IsEmpty()) {
        return false;
    }

    for (int it = 0; it < m_items.GetSize(); it++) {
        CControlUI *item1 = GetItemAt(it);

        if (item1 == nullptr) {
            continue;
        }

        if (item1->GetName() == _controlName) {
            SelectItem(it);
            return true;
        }
    }

    return false;
}
}
