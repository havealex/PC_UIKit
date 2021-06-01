/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#pragma once

namespace DuiLib {
class IDialogBuilderCallback {
public:
    virtual CControlUI *CreateControl(LPCTSTR pstrClass) = 0;
};


class UILIB_API CDialogBuilder {
public:
    CDialogBuilder();
    ~CDialogBuilder() {}
    CControlUI *Create(STRINGorID xml, LPCTSTR type = nullptr, IDialogBuilderCallback *pCallback = nullptr,
        CPaintManagerUI *pManager = nullptr, CControlUI *pParent = nullptr);
    CControlUI *Create(IDialogBuilderCallback *pCallback = nullptr, CPaintManagerUI *pManager = nullptr,
        CControlUI *pParent = nullptr);

    CMarkup *GetMarkup();

    void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
    void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
    void SetInstance(HINSTANCE instance)
    {
        m_instance = instance;
    };

private:
    CControlUI *_Parse(CMarkupNode *parent, CControlUI *pParent = nullptr, CPaintManagerUI *pManager = nullptr);

    IDialogBuilderCallback *m_pCallback;
    LPCTSTR m_pstrtype;
    HINSTANCE m_instance;

public:
    CMarkup m_xml;
};
} // namespace DuiLib

#endif // __UIDLGBUILDER_H__
