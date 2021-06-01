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

#include "MainWindow.h"

NAMESPACE_DUILIB_BEGIN

static const LPCTSTR WINDOWXML = _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    "<Window mininfo=\"200, 200\" layered=\"true\" size=\"200, 200\""
    "sizebox=\"0, 0, 0, 0\" caption=\"0, 0, 0, 32\" showdirty=\"false\">"
    "<VerticalLayout name=\"windowBackgroundColor\" bkcolor=\"#ffffffff\">"
    "<HorizontalLayout name=\"titleLayout\" height=\"32\" bkcolor = \"#ffcccccc\"/>"
    "<Container name=\"content_container\"/>"
    "</VerticalLayout>"
    "</Window>"
    );

MainWindow::MainWindow() {}

MainWindow::~MainWindow() {}

void MainWindow::Create()
{
    __super::Create(nullptr, GetWindowClassName(), UI_WNDSTYLE_FRAME, 0L);
}

void MainWindow::SetTitleColor(DWORD color)
{
    if (DuiLib::CHorizontalLayoutUI *title = GetTitleLayout()) {
        title->SetBkColor(color);
    }
}

void MainWindow::SetWindowBackgroundColor(DWORD color)
{
    if (DuiLib::CContainerUI *container =
            dynamic_cast<DuiLib::CContainerUI *>(m_pm.FindControl(_T("windowBackgroundColor")))) {
        container->SetBkColor(color);
    }
}

void MainWindow::SetCentralControl(DuiLib::CControlUI *centralControl)
{
    if (DuiLib::CContainerUI *container =
            dynamic_cast<DuiLib::CContainerUI *>(m_pm.FindControl(_T("content_container")))) {
        container->Add(centralControl);
    }
}

DuiLib::CHorizontalLayoutUI *DuiLib::MainWindow::GetTitleLayout() const
{
    DuiLib::CHorizontalLayoutUI *title =
        dynamic_cast<DuiLib::CHorizontalLayoutUI *>(m_pm.FindControl(_T("titleLayout")));
    return title;
}

LRESULT MainWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    m_pm.GetShadow()->ShowShadow(true);
    const int size = 5;
    m_pm.GetShadow()->SetSize(size);
    const int darkness = 44;
    m_pm.GetShadow()->SetDarkness(darkness);
    LRESULT ret = WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
    return ret;
}

LPCTSTR MainWindow::GetWindowClassName(void) const
{
    return _T("DuiLibMainWindow");
}

CDuiString MainWindow::GetSkinFile()
{
    return WINDOWXML;
}

NAMESPACE_DUILIB_END
