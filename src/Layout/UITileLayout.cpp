/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UITileLayout.h"

namespace DuiLib {
constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10
constexpr int TWO_UNIT = 2;
IMPLEMENT_DUICONTROL(CTileLayoutUI)
CTileLayoutUI::CTileLayoutUI() : m_nColumns(1)
{
    m_szItem.cx = m_szItem.cy = 0;
}

LPCTSTR CTileLayoutUI::GetClass() const
{
    return _T("TileLayoutUI");
}

LPVOID CTileLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_TILELAYOUT) == 0) {
        return static_cast<CTileLayoutUI *>(this);
    }
    return CContainerUI::GetInterface(pstrName);
}

SIZE CTileLayoutUI::GetItemSize() const
{
    return m_szItem;
}

void CTileLayoutUI::SetItemSize(SIZE szItem)
{
    if (m_szItem.cx != szItem.cx || m_szItem.cy != szItem.cy) {
        m_szItem = szItem;
        NeedUpdate();
    }
}

int CTileLayoutUI::GetColumns() const
{
    return m_nColumns;
}

void CTileLayoutUI::SetColumns(int nCols)
{
    if (nCols <= 0) {
        return;
    }
    m_nColumns = nCols;
    NeedUpdate();
}

void CTileLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("itemsize")) == 0) {
        SIZE szItem = { 0 };
        LPTSTR pstr = nullptr;
        szItem.cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
        ASSERT(pstr);
        szItem.cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        ASSERT(pstr);
        SetItemSize(szItem);
    } else if (_tcsicmp(pstrName, _T("columns")) == 0) {
        SetColumns(_ttoi(pstrValue));
    } else {
        CContainerUI::SetAttribute(pstrName, pstrValue);
    }
}

void CTileLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;
    int m_iChildPaddingScaled = CResourceManager::GetInstance()->Scale(m_iChildPadding);

    RECT m_rcInset = CTileLayoutUI::m_rcInset;
    CResourceManager::GetInstance()->Scale(&m_rcInset);

    // Adjust for inset
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    if (m_items.GetSize() == 0) {
        ProcessScrollBar(rc, 0, 0);
        return;
    }
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
            rc.left += m_pVerticalScrollBar->GetFixedWidth();
        }
    } else {
        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
            rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        }
    }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    int cyNeeded = GetHeightNeed(rc, m_iChildPaddingScaled,
        [=](DuiLib::CControlUI *pControl, const RECT &rcPos) {
        if (pControl == nullptr) {
            return;
        }
        pControl->SetPos(rcPos, bNeedInvalidate);
    });

    // Process the scrollbar
    ProcessScrollBar(rc, 0, cyNeeded);
}

int CTileLayoutUI::GetHeightNeed(RECT &rc, int m_iChildPaddingScaled,
    const std::function<void(DuiLib::CControlUI *, const RECT &)> &f, bool ignoreVisible)
{
    if (m_szItem.cx > 0) {
        m_nColumns = (rc.right - rc.left) / DuiLib::CResourceManager::GetInstance()->Scale(m_szItem.cx);
    }
    if (m_nColumns == 0) {
        m_nColumns = 1;
    }
    int cyNeeded = 0;
    int cxWidth = (rc.right - rc.left) / m_nColumns;
    if (m_pHorizontalScrollBar && (ignoreVisible ? true : m_pHorizontalScrollBar->IsVisible())) {
        cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange()) / m_nColumns;
    }

    int cyHeight = 0;
    int iCount = 0;
    int iPosX = rc.left;
    POINT ptTile = { rc.left, rc.top };
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        ptTile = { rc.right, rc.top };
        iPosX = rc.right;
    }
    if (m_pVerticalScrollBar && (ignoreVisible ? true : m_pVerticalScrollBar->IsVisible())) {
        ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
    }
    if (m_pHorizontalScrollBar && (ignoreVisible ? true : m_pHorizontalScrollBar->IsVisible())) {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
        ptTile.x = iPosX;
    }
    for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it1));
        if (pControl == nullptr) {
            continue;
        }
        if ((ignoreVisible ? false : !pControl->IsVisible())) {
            continue;
        }
        if (pControl->IsFloat()) {
            SetFloatPos(it1);
            continue;
        }

        // Determine size
        RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
        if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
            rcTile = { ptTile.x - cxWidth, ptTile.y, ptTile.x, ptTile.y };
        }
        if ((iCount % m_nColumns) == 0) {
            int iIndex = iCount;
            for (int it2 = it1; it2 < m_items.GetSize(); it2++) {
                CControlUI *pLineControl = static_cast<CControlUI *>(m_items.GetAt(it2));
                if (pLineControl == nullptr) {
                    continue;
                }
                if ((ignoreVisible ? false : !pLineControl->IsVisible()))
                    continue;
                if (pLineControl->IsFloat())
                    continue;

                RECT rcPadding = pLineControl->GetPadding();
                SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
                if (iIndex == iCount || (iIndex + 1) % m_nColumns == 0) {
                    szAvailable.cx -= m_iChildPaddingScaled / TWO_UNIT;
                } else {
                    szAvailable.cx -= m_iChildPaddingScaled;
                }

                if (szAvailable.cx < pControl->GetMinWidth())
                    szAvailable.cx = pControl->GetMinWidth();
                if (szAvailable.cx > pControl->GetMaxWidth())
                    szAvailable.cx = pControl->GetMaxWidth();

                SIZE szTile = pLineControl->EstimateSize(szAvailable);
                if (szTile.cx < pControl->GetMinWidth())
                    szTile.cx = pControl->GetMinWidth();
                if (szTile.cx > pControl->GetMaxWidth())
                    szTile.cx = pControl->GetMaxWidth();
                if (szTile.cy < pControl->GetMinHeight())
                    szTile.cy = pControl->GetMinHeight();
                if (szTile.cy > pControl->GetMaxHeight())
                    szTile.cy = pControl->GetMaxHeight();

                cyHeight = MAX(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
                if ((++iIndex % m_nColumns) == 0)
                    break;
            }
        }

        RECT rcPadding = pControl->GetPadding();

        rcTile.left += rcPadding.left + m_iChildPaddingScaled / TWO_UNIT;
        rcTile.right -= rcPadding.right + m_iChildPaddingScaled / TWO_UNIT;
        if ((iCount % m_nColumns) == 0) {
            rcTile.left -= m_iChildPaddingScaled / TWO_UNIT;
        }

        if (((iCount + 1) % m_nColumns) == 0) {
            rcTile.right += m_iChildPaddingScaled / TWO_UNIT;
        }

        // Set position
        rcTile.top = ptTile.y + rcPadding.top;
        rcTile.bottom = ptTile.y + cyHeight;

        SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
        SIZE szTile = pControl->EstimateSize(szAvailable);
        if (szTile.cx == 0)
            szTile.cx = szAvailable.cx;
        if (szTile.cy == 0)
            szTile.cy = szAvailable.cy;
        if (szTile.cx < pControl->GetMinWidth())
            szTile.cx = pControl->GetMinWidth();
        if (szTile.cx > pControl->GetMaxWidth())
            szTile.cx = pControl->GetMaxWidth();
        if (szTile.cy < pControl->GetMinHeight())
            szTile.cy = pControl->GetMinHeight();
        if (szTile.cy > pControl->GetMaxHeight())
            szTile.cy = pControl->GetMaxHeight();
        RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / TWO_UNIT,
                      (rcTile.top + rcTile.bottom - szTile.cy) / TWO_UNIT,
                      (rcTile.left + rcTile.right - szTile.cx) / TWO_UNIT + szTile.cx,
                      (rcTile.top + rcTile.bottom - szTile.cy) / TWO_UNIT + szTile.cy};
        if (f) {
            f(pControl, rcPos);
        }

        if ((++iCount % m_nColumns) == 0) {
            ptTile.x = iPosX;
            ptTile.y += cyHeight + m_iChildPaddingScaled;
            cyHeight = 0;
        } else {
            if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
                ptTile.x -= cxWidth;
            } else {
                ptTile.x += cxWidth;
            }
        }
        cyNeeded = rcTile.bottom - rc.top;
        if (m_pVerticalScrollBar && (ignoreVisible ? true : m_pVerticalScrollBar->IsVisible()))
            cyNeeded += m_pVerticalScrollBar->GetScrollPos();
    }

    return cyNeeded;
}
}
