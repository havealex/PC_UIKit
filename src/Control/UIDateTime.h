/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib {
class CDateTimeWnd;

// / 时间选择控件
class UILIB_API CDateTimeUI : public CLabelUI {
    DECLARE_DUICONTROL(CDateTimeUI)
    friend class CDateTimeWnd;

public:
    CDateTimeUI();
    ~CDateTimeUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    SYSTEMTIME &GetTime();
    void SetTime(SYSTEMTIME *pst);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    void DoEvent(TEventUI &event) override;

protected:
    int m_nDTUpdateFlag;
    bool m_bReadOnly;

    CDateTimeWnd *m_pWindow;

public:
    SYSTEMTIME m_sysTime;
};
}
#endif // __UIDATETIME_H__