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

#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#pragma once

// ���ÿؼ����һ������dtstyle
namespace DuiLib {
class CIPAddressWnd;

// / ʱ��ѡ��ؼ�
class UILIB_API CIPAddressUI : public CLabelUI {
    DECLARE_DUICONTROL(CIPAddressUI)

    friend class CIPAddressWnd;

public:
    CIPAddressUI();
    ~CIPAddressUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    DWORD GetIP();
    void SetIP(DWORD dwIP);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    void DoEvent(TEventUI &event) override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
    DWORD m_dwIP;
    bool m_bReadOnly;
    int m_nIPUpdateFlag;

    CIPAddressWnd *m_pWindow;
};
}
#endif // __UIIPADDRESS_H__