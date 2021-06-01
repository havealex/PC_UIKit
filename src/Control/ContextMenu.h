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

#ifndef FLOATWINDOWMENU_INCLUDE
#define FLOATWINDOWMENU_INCLUDE

#include "../UIlib.h"

NAMESPACE_DUILIB_BEGIN

class ContextMenu;

class IContextMenuCallback {
public:
    virtual ~IContextMenuCallback() {}
    virtual void OnMenuItemClicked(const ContextMenu &menu, const DuiLib::CDuiString &itemName) = 0;
};

class ContextMenuHelper;
class UILIB_API ContextMenu {
public:
    ContextMenu(DuiLib::CControlUI *parentCtrl, const DuiLib::CDuiString &resPath,
        IContextMenuCallback *menuCallback = nullptr, bool controlDropDown = false,
        int ma = DuiLib::eMenuAlignment_Left | DuiLib::eMenuAlignment_Top);
    ContextMenu(DuiLib::CPaintManagerUI *pm, const DuiLib::CDuiString &resPath,
        IContextMenuCallback *menuCallback = nullptr, const POINT &initPos = { 0, 0 },
        int ma = DuiLib::eMenuAlignment_Left | DuiLib::eMenuAlignment_Top);
    ~ContextMenu();

    bool Show(int dx = 0, int dy = 0);

    void SetKeepForeground(bool foreground);

    void Close();

    DuiLib::CMenuElementUI *GetItem(const DuiLib::CDuiString &name) const;

    void SetItemChecked(const DuiLib::CDuiString &name, bool checked);

    bool GetItemChecked(const DuiLib::CDuiString &name) const;

    DuiLib::CDuiString GetClickedItemName() const;

    DuiLib::CMenuWnd *GetMenu() const;

private:
    ContextMenuHelper *helper = nullptr;
};

NAMESPACE_DUILIB_END

#endif // FLOATWINDOWMENU_INCLUDE
