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
#ifndef MAINWINDOW_DUILIB_INCLUDE
#define MAINWINDOW_DUILIB_INCLUDE

#include "../Utils/WinImplBase.h"

NAMESPACE_DUILIB_BEGIN

class UILIB_API MainWindow : public WindowImplBase {
public:
    MainWindow();
    ~MainWindow() override;

    void Create();

    void SetTitleColor(DWORD color);

    void SetWindowBackgroundColor(DWORD color);

    void SetCentralControl(DuiLib::CControlUI *centralControl);

    DuiLib::CHorizontalLayoutUI *GetTitleLayout() const;

protected:
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) override;
    LPCTSTR GetWindowClassName(void) const override;
    DuiLib::CDuiString GetSkinFile() override;
};

NAMESPACE_DUILIB_END

#endif // MAINWINDOW_DUILIB_INCLUDE
