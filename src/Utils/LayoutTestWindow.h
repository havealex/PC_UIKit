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
#ifndef _LAYOUTTESTWINDOW_INCLUDE__
#define _LAYOUTTESTWINDOW_INCLUDE__

#include "../UILib.h"
#include "../Utils/MainWindow.h"

NAMESPACE_DUILIB_BEGIN

class LayoutTestWindowData;
class UILIB_API LayoutTestWindow : public MainWindow {
public:
    explicit LayoutTestWindow(const DuiLib::CDuiString &xmlPath);
    ~LayoutTestWindow();

    void Refresh();

protected:
    LayoutTestWindowData *data;
    void ResetCentralWidget(DuiLib::CControlUI *control);
    virtual void OnRefresh();
    friend class LayoutTestWindowData;
};

NAMESPACE_DUILIB_END

#endif // _LAYOUTTESTWINDOW_INCLUDE__
