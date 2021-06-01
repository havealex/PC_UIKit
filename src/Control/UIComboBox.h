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

#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#pragma once

namespace DuiLib {
// / 扩展下拉列表框
// / 增加arrowimage属性,一张图片平均分成5份,Normal/Hot/Pushed/Focused/Disabled(必须有source属性)
// / <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
class UILIB_API CComboBoxUI : public CComboUI {
    DECLARE_DUICONTROL(CComboBoxUI)
public:
    CComboBoxUI();
    ~CComboBoxUI();
    LPCTSTR GetClass() const;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    CDuiString m_sArrowImage;
    int m_nArrowWidth;
};
}

#endif // __UICOMBOBOX_H__
