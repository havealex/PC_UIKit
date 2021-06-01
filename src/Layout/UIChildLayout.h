/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CChildLayoutUI : public CContainerUI {
    DECLARE_DUICONTROL(CChildLayoutUI)
public:
    CChildLayoutUI();
    ~CChildLayoutUI() {}

    void Init() override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    void SetChildLayoutXML(CDuiString pXML);
    CDuiString GetChildLayoutXML();
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;

private:
    CDuiString m_pstrXMLFile;
};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
