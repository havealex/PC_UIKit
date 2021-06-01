/*
 * Copyright (C) 2021. Huawei Device Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 */

#include "../StdAfx.h"
#include "UITileLayoutX.h"

namespace DuiLib {
constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10

IMPLEMENT_DUICONTROL(CTileLayoutXUI)

CTileLayoutXUI::CTileLayoutXUI()
{
    if (DIRECTION_RTL == UICulture::GetInstance()->getDirection()) {
        mDirectionRTL = true;
    }
}

void CTileLayoutXUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("itemsize")) == 0) {
        SIZE szItem = { 0 };
        LPTSTR pstr = nullptr;
        szItem.cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
        szItem.cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        if (m_szItem.cx != szItem.cx || m_szItem.cy != szItem.cy) {
            m_szItem = szItem;
            NeedUpdate();
        }
    } else if (_tcsicmp(pstrName, _T("columns")) == 0) {
        m_nColumns = _ttoi(pstrValue);
        NeedUpdate();
    } else {
        CContainerUI::SetAttribute(pstrName, pstrValue);
    }
}

void CTileLayoutXUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    PositionAllElements(rc);
}

void CTileLayoutXUI::PositionAllElements(RECT rc)
{
    int item_width = CResourceManager::GetInstance()->Scale(m_szItem.cx);
    int item_height = CResourceManager::GetInstance()->Scale(m_szItem.cy);

    int tileX = rc.left;     // вСио╫г
    int tileXRTL = rc.right; // срио╫г
    int tileY = rc.top;

    for (int k = 0; k < m_items.GetSize(); k++) {
        if (k > 0) {
            if ((k % m_nColumns) == 0) {
                tileX = rc.left;
                tileXRTL = rc.right;
                tileY = tileY + item_height;
            } else {
                tileX += item_width;
                tileXRTL -= item_width;
            }
        }

        CControlUI *pControl = reinterpret_cast<CControlUI *>(m_items.GetAt(k));
        if (pControl == nullptr) {
            continue;
        }

        SIZE child_size = pControl->EstimateSize(SIZE { 0, 0 });
        int child_left = mDirectionRTL ? (tileXRTL - child_size.cx) : tileX;
        RECT child_rc = RECT { child_left, tileY, child_left + child_size.cx, tileY + child_size.cy };

        pControl->SetPos(child_rc);
    }
}

int CTileLayoutXUI::GetHeightNeed()
{
    int count = m_items.GetSize();
    int rows = (count + m_nColumns - 1) / m_nColumns;
    int height1 = rows * m_szItem.cy;
    int height2 = CResourceManager::GetInstance()->Scale(height1);
    return height2;
}
}
