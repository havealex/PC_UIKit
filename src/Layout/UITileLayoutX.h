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

#ifndef __UITILELAYOUT_XH__
#define __UITILELAYOUT_XH__

#pragma once

namespace DuiLib {
class UILIB_API CTileLayoutXUI : public CContainerUI {
    DECLARE_DUICONTROL(CTileLayoutXUI)
public:
    CTileLayoutXUI();
    ~CTileLayoutXUI() {}

public: //  Ù–‘
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    SIZE m_szItem = { 0, 0 };
    int m_nColumns = 0;

public: // ≤ºæ÷
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    int GetHeightNeed();
    void PositionAllElements(RECT rc);
    bool mDirectionRTL = false;
};
}
#endif // __UITILELAYOUT_XH__
