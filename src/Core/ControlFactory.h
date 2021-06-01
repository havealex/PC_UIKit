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

#pragma once
#ifndef _CONTROLFACTORY_INCLUDE__
#define _CONTROLFACTORY_INCLUDE__

#include <map>
namespace DuiLib {
using CreateClass = CControlUI *(*)();
using MAP_DUI_CTRATECLASS = std::map<CDuiString, CreateClass>;

class UILIB_API CControlFactory {
public:
    CControlUI *CreateControl(CDuiString strClassName);
    void RegistControl(CDuiString strClassName, CreateClass pFunc);

    static CControlFactory *GetInstance();

private:
    CControlFactory();
    virtual ~CControlFactory();

private:
    MAP_DUI_CTRATECLASS m_mapControl;
};

#define DECLARE_DUICONTROL(class_name) \
public:                                \
    static CControlUI *CreateControl();

#define IMPLEMENT_DUICONTROL(class_name)    \
    CControlUI *class_name::CreateControl() \
    {                                       \
        return new class_name;              \
    }

#define REGIST_DUICONTROL(class_name) \
    CControlFactory::GetInstance()->RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);

#define INNER_REGISTER_DUICONTROL(class_name) RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);
}
#endif // _CONTROLFACTORY_INCLUDE__