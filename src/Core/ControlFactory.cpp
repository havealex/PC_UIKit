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
#include "ControlFactory.h"
#include "../Control/UISlideShow.h"

namespace DuiLib {
CControlFactory::CControlFactory()
{
    INNER_REGISTER_DUICONTROL(CControlUI);
    INNER_REGISTER_DUICONTROL(CToggleSwitchUI);
    INNER_REGISTER_DUICONTROL(CRoundedRectButtonUI);
    INNER_REGISTER_DUICONTROL(CLoadingControlUI);
    INNER_REGISTER_DUICONTROL(CImageAnimationControlUI);
    INNER_REGISTER_DUICONTROL(CPointsWaitControlUI);
    INNER_REGISTER_DUICONTROL(CContainerUI);
    INNER_REGISTER_DUICONTROL(CButtonUI);
    INNER_REGISTER_DUICONTROL(CComboUI);
    INNER_REGISTER_DUICONTROL(CComboBoxUI);
    INNER_REGISTER_DUICONTROL(CDateTimeUI);
    INNER_REGISTER_DUICONTROL(CEditUI);
    INNER_REGISTER_DUICONTROL(CActiveXUI);
    INNER_REGISTER_DUICONTROL(CFlashUI);
    INNER_REGISTER_DUICONTROL(CGifAnimUI);
#ifdef USE_XIMAGE_EFFECT
    INNER_REGISTER_DUICONTROL(CGifAnimExUI);
#endif
    INNER_REGISTER_DUICONTROL(CGroupBoxUI);
    INNER_REGISTER_DUICONTROL(CIPAddressUI);
    INNER_REGISTER_DUICONTROL(CLabelUI);
    INNER_REGISTER_DUICONTROL(CListUI);
    INNER_REGISTER_DUICONTROL(CListHeaderUI);
    INNER_REGISTER_DUICONTROL(CListHeaderItemUI);
    INNER_REGISTER_DUICONTROL(CListLabelElementUI);
    INNER_REGISTER_DUICONTROL(CListTextElementUI);
    INNER_REGISTER_DUICONTROL(CListContainerElementUI);
    INNER_REGISTER_DUICONTROL(CMenuUI);
    INNER_REGISTER_DUICONTROL(CMenuElementUI);
    INNER_REGISTER_DUICONTROL(COptionUI);
    INNER_REGISTER_DUICONTROL(CCheckBoxUI);
    INNER_REGISTER_DUICONTROL(CProgressUI);
    INNER_REGISTER_DUICONTROL(CRichEditUI);
    INNER_REGISTER_DUICONTROL(CScrollBarUI);
    INNER_REGISTER_DUICONTROL(CSliderUI);
    INNER_REGISTER_DUICONTROL(CTextUI);
    INNER_REGISTER_DUICONTROL(CTreeNodeUI);
    INNER_REGISTER_DUICONTROL(CTreeViewUI);
    INNER_REGISTER_DUICONTROL(CWebBrowserUI);
    INNER_REGISTER_DUICONTROL(CAnimationTabLayoutUI);
    INNER_REGISTER_DUICONTROL(CChildLayoutUI);
    INNER_REGISTER_DUICONTROL(CHorizontalLayoutUI);
    INNER_REGISTER_DUICONTROL(CTabLayoutUI);
    INNER_REGISTER_DUICONTROL(CTileLayoutUI);
    INNER_REGISTER_DUICONTROL(CTileLayoutXUI);
    INNER_REGISTER_DUICONTROL(CVerticalLayoutUI);
    INNER_REGISTER_DUICONTROL(CRollTextUI);
    INNER_REGISTER_DUICONTROL(CColorPaletteUI);
    INNER_REGISTER_DUICONTROL(CListExUI);
    INNER_REGISTER_DUICONTROL(CListContainerHeaderItemUI);
    INNER_REGISTER_DUICONTROL(CListTextExtElementUI);
    INNER_REGISTER_DUICONTROL(CHotKeyUI);
    INNER_REGISTER_DUICONTROL(CFadeButtonUI);
    INNER_REGISTER_DUICONTROL(CIconUI);
    INNER_REGISTER_DUICONTROL(CAnimPanelUI);
    INNER_REGISTER_DUICONTROL(CAnimationLayoutUI);
    INNER_REGISTER_DUICONTROL(CFadeTabLayoutUI);
    INNER_REGISTER_DUICONTROL(CSlideShowUI);
    INNER_REGISTER_DUICONTROL(CSlideShowPrevButtonUI);
    INNER_REGISTER_DUICONTROL(CSlideShowNextButtonUI);
    INNER_REGISTER_DUICONTROL(CCircleShareProgressUI);
}

CControlFactory::~CControlFactory() {}

CControlUI *CControlFactory::CreateControl(CDuiString strClassName)
{
    MAP_DUI_CTRATECLASS::iterator iter = m_mapControl.find(strClassName);
    if (iter == m_mapControl.end()) {
        return nullptr;
    } else {
        return reinterpret_cast<CControlUI *>(iter->second());
    }
}

void CControlFactory::RegistControl(CDuiString strClassName, CreateClass pFunc)
{
    m_mapControl.insert(MAP_DUI_CTRATECLASS::value_type(strClassName, pFunc));
}

CControlFactory *CControlFactory::GetInstance()
{
    static auto pInstance = new CControlFactory;
    return pInstance;
}

}
