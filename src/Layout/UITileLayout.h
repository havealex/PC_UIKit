/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CTileLayoutUI : public CContainerUI {
    DECLARE_DUICONTROL(CTileLayoutUI)
public:
    CTileLayoutUI();
    ~CTileLayoutUI() {}

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

    int GetHeightNeed(RECT &rc, int m_iChildPaddingScaled,
        const std::function<void(DuiLib::CControlUI *, const RECT &)> &f, bool ignoreVisible = false);

    SIZE GetItemSize() const;
    void SetItemSize(SIZE szItem);
    int GetColumns() const;
    void SetColumns(int nCols);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
    SIZE m_szItem;
    int m_nColumns;
};
}
#endif // __UITILELAYOUT_H__
