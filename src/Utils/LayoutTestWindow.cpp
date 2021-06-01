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

#include "LayoutTestWindow.h"

NAMESPACE_DUILIB_BEGIN

class LayoutTestWindowData {
public:
    LayoutTestWindow *window = nullptr;
    DuiLib::CContainerUI *container = nullptr;
    DuiLib::CButtonUI *btnRefresh = nullptr;
    DuiLib::CDuiString xmlPath;

    bool OnDoNotify(void *p)
    {
        auto noti = (DuiLib::TNotifyUI *)p;
        if (noti == nullptr || this->window == nullptr) {
            return false;
        }

        if (noti->sType == DUI_MSGTYPE_CLICK) {
            window->OnRefresh();
        }

        return true;
    }

    void RefreshControl()
    {
        if (!container) {
            return;
        }

        DuiLib::CDialogBuilder builder;
        if (DuiLib::CControlUI *control = builder.Create(xmlPath.GetData())) {
            ResetCentralWidget(control);
        }
    }

    void ResetCentralWidget(DuiLib::CControlUI *ctr)
    {
        if (!container) {
            return;
        }

        container->RemoveAll();
        container->Add(ctr);
    }
};

LayoutTestWindow::LayoutTestWindow(const DuiLib::CDuiString &xmlPath) : data(new LayoutTestWindowData)
{
    if (this->data == nullptr) {
        return;
    }
    data->window = this;
    Create();
    SetWindowBackgroundColor(0x0);
    SetTitleColor(0x88007dff);
    data->xmlPath = xmlPath;
    DuiLib::CHorizontalLayoutUI *title = GetTitleLayout();
    if (title) {
        data->btnRefresh = new (std::nothrow) DuiLib::CButtonUI;
        if (data->btnRefresh == nullptr) {
            return;
        }
        data->btnRefresh->SetBkColor(0x66666666);
        data->btnRefresh->SetHotBkColor(0x55888888);
        data->btnRefresh->SetPushedBkColor(0x55aaaaaa);
        data->btnRefresh->SetTextColor(0xffffffff);
        const int width = 140;
        data->btnRefresh->SetFixedWidth(width);
        const int height = 32;
        data->btnRefresh->SetFixedHeight(height);
        data->btnRefresh->SetText(_T("Refresh"));
        title->Add(data->btnRefresh);
        data->btnRefresh->OnNotify += DuiLib::MakeDelegate(data, &LayoutTestWindowData::OnDoNotify);
    }

    data->container = new (std::nothrow) DuiLib::CContainerUI;
    if (data->container != nullptr) {
        SetCentralControl(data->container);
    }
    data->RefreshControl();
}

LayoutTestWindow::~LayoutTestWindow()
{
    delete data;
    data = nullptr;
}

void DuiLib::LayoutTestWindow::Refresh()
{
    OnRefresh();
}

void LayoutTestWindow::ResetCentralWidget(DuiLib::CControlUI *control)
{
    if (this->data == nullptr) {
        return;
    }
    data->ResetCentralWidget(control);
}

void LayoutTestWindow::OnRefresh()
{
    if (this->data == nullptr) {
        return;
    }
    data->RefreshControl();
}

NAMESPACE_DUILIB_END
