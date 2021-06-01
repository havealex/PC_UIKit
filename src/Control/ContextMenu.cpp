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

#include "../stdafx.h"
#include "ContextMenu.h"
#include "UIMenu.h"

NAMESPACE_DUILIB_BEGIN

struct KeepForegroundInfo {
    ContextMenu *menu = nullptr;
    bool keepForeground = false;
};

static std::vector<KeepForegroundInfo> g_keepForegroundList;

class MenuWndEx : public DuiLib::CMenuWnd {
public:

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    {
        return __super::HandleMessage(uMsg, wParam, lParam);
    }

    UINT ShowModal()
    {
        ASSERT(::IsWindow(m_hWnd));
        UINT nRet = 0;
        HWND hWndParent = GetWindowOwner(m_hWnd);
        ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
        ::SendMessage(m_hWnd, WM_NCACTIVATE, TRUE, 0);
        MSG msg = { 0 };
        while (::IsWindow(m_hWnd) && ::GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd) {
                nRet = (UINT)msg.wParam;
                if (hWndParent != nullptr) {
                    if (!::EnableWindow(hWndParent, TRUE)) {
                        OutputDebugString(_T("ContextMenu EnableWindow failed."));
                    }
                    ::SetFocus(hWndParent);
                }
                MSG msgClose = msg;
                if (::IsWindow(m_hWnd) && ::GetMessage(&msg, nullptr, 0, 0)) {
                    if (!DuiLib::CPaintManagerUI::TranslateMessage(&msg)) {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }
                }
                if (!DuiLib::CPaintManagerUI::TranslateMessage(&msgClose)) {
                    ::TranslateMessage(&msgClose);
                    ::DispatchMessage(&msgClose);
                }
                continue;
            } else if (msg.message == DuiLib::UIMSG_UPDATE_ON_MAIN) {
                // 主线程执行队列，用于在工作线程里面一键切换到主线程执行UI的更新
                DuiLib::UIMainQueue::getInstance().finish();
            }
            auto it = std::find_if(g_keepForegroundList.begin(), g_keepForegroundList.end(), [=](auto info) {
                if (this == nullptr || info.menu == nullptr || m_hWnd == nullptr) {
                    return false;
                }
                if (auto menu = info.menu->GetMenu()) {
                    return menu->GetHWND() == m_hWnd;
                } else {
                    return false;
                }
            });
            if (it != g_keepForegroundList.end() && it->keepForeground && it->menu) {
                if (auto menu = it->menu->GetMenu()) {
                    SetForegroundWindow(menu->GetHWND());
                }
            }
            if (!DuiLib::CPaintManagerUI::TranslateMessage(&msg)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
            if (msg.message == WM_QUIT) {
                break;
            }
        }
        if (msg.message == WM_QUIT) {
            ::PostQuitMessage(static_cast<int>(msg.wParam));
        }
        return nRet;
    }
};

class ContextMenuHelper : public DuiLib::IMessageFilterUI {
public:
    std::unique_ptr<MenuWndEx> menuWnd;
    bool clickedItem = false;
    bool keepForeground = false;
    ContextMenu *contextMenu = nullptr;
    DuiLib::CControlUI *parentCtrl = nullptr;
    DuiLib::CDuiString resPath;
    DuiLib::CDuiString clickedItemName;
    std::map<DuiLib::CDuiString, bool> menuItemCheckList;
    IContextMenuCallback *menuCallback = nullptr;

    ContextMenuHelper() {}

    ~ContextMenuHelper()
    {
        if (parentCtrl && parentCtrl->GetManager()) {
            parentCtrl->GetManager()->RemovePreMessageFilter(this);
        }
    }

    void Create()
    {
        if (parentCtrl && parentCtrl->GetManager()) {
            parentCtrl->GetManager()->AddPreMessageFilter(this);
        }
    }

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override
    {
        if (uMsg == WM_MENUCLICK) {
            if (DuiLib::CMenuElementUI *item = (DuiLib::CMenuElementUI *)wParam) {
                clickedItem = true;
                clickedItemName = item->GetName();
                if (menuCallback) {
                    menuCallback->OnMenuItemClicked(*contextMenu, clickedItemName);
                }
            } else {
            }
        }

        return 0;
    }
};

ContextMenu::ContextMenu(DuiLib::CControlUI *parentCtrl, const DuiLib::CDuiString &resPath,
    IContextMenuCallback *menuCallback, bool controlDropDown, int ma)
    : helper(new ContextMenuHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->menuCallback = menuCallback;
    helper->contextMenu = this;
    helper->resPath = resPath;
    helper->parentCtrl = parentCtrl;
    helper->menuWnd = std::make_unique<MenuWndEx>();

    helper->Create();

    DuiLib::CDuiPoint point(0, 0);
    if (controlDropDown && parentCtrl && parentCtrl->GetManager()) {
        RECT rc = parentCtrl->GetPos();
        RECT rcWindow = { 0, 0, 0, 0 };
        if (GetWindowRect(parentCtrl->GetManager()->GetPaintWindow(), &rcWindow)) {
            point = DuiLib::CDuiPoint(rc.left + rcWindow.left, rc.top + rcWindow.top + (rc.bottom - rc.top));
        } else {
            GetCursorPos(&point);
        }
    } else {
        GetCursorPos(&point);
    }

    DuiLib::STRINGorID xml(helper->resPath.GetData());

    if (helper->parentCtrl) {
        helper->menuWnd->Init(nullptr, xml, point, helper->parentCtrl->GetManager(), &helper->menuItemCheckList, ma);
    }
}

DuiLib::ContextMenu::ContextMenu(DuiLib::CPaintManagerUI *pm, const DuiLib::CDuiString &resPath,
    IContextMenuCallback *menuCallback, const POINT &initPos, int ma)
    : helper(new ContextMenuHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->menuCallback = menuCallback;
    helper->contextMenu = this;
    helper->resPath = resPath;
    helper->parentCtrl = nullptr;
    helper->menuWnd = std::make_unique<MenuWndEx>();

    helper->Create();

    DuiLib::STRINGorID xml(helper->resPath.GetData());

    helper->menuWnd->Init(nullptr, xml, initPos, pm, &helper->menuItemCheckList, ma);
}

ContextMenu::~ContextMenu()
{
    auto it = std::find_if(g_keepForegroundList.begin(), g_keepForegroundList.end(),
        [=](auto info) { return info.menu == this; });

    if (it != g_keepForegroundList.end()) {
        g_keepForegroundList.erase(it);
    }

    if (helper) {
        delete helper;
        helper = nullptr;
    }
}

bool ContextMenu::Show(int dx, int dy)
{
    if (helper == nullptr) {
        return false;
    }
    if (!helper->menuWnd) {
        return false;
    }
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);
    SIZE screenSize = SIZE { cx, cy };
    RECT rc;
    SecureZeroMemory(&rc, sizeof(rc));
    GetWindowRect(helper->menuWnd->GetHWND(), &rc);
    int x = -1;
    if (rc.right > screenSize.cx) {
        x = screenSize.cx - rc.right + rc.left;
    }

    int y = -1;
    if (rc.bottom > screenSize.cy) {
        y = screenSize.cy - rc.bottom + rc.top;
    }

    SetWindowPos(helper->menuWnd->GetHWND(), nullptr, (x == -1 ? rc.left : x) + dx, (y == -1 ? rc.top : y) + dy, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER);

    helper->menuWnd->ShowModal();

    return helper->clickedItem;
}

void DuiLib::ContextMenu::SetKeepForeground(bool foreground)
{
    if (helper == nullptr) {
        return;
    }
    helper->keepForeground = foreground;
    auto it = std::find_if(g_keepForegroundList.begin(), g_keepForegroundList.end(),
        [=](auto info) { return info.menu == this; });

    if (it != g_keepForegroundList.end()) {
        g_keepForegroundList.erase(it);
    }

    KeepForegroundInfo info;
    info.menu = this;
    info.keepForeground = foreground;
    g_keepForegroundList.emplace_back(info);
}

void DuiLib::ContextMenu::Close()
{
    if (helper && helper->menuWnd) {
        helper->menuWnd->Close();
    }
}

DuiLib::CMenuElementUI *ContextMenu::GetItem(const DuiLib::CDuiString &name) const
{
    if (helper == nullptr || helper->menuWnd == nullptr || helper->menuWnd->GetMenuUI() == nullptr ||
        helper->menuWnd->GetMenuUI()->GetManager() == nullptr) {
        return nullptr;
    }

    DuiLib::CControlUI *ctrl = helper->menuWnd->GetMenuUI()->GetManager()->FindControl(name);
    if (ctrl == nullptr) {
        return nullptr;
    }
    return dynamic_cast<DuiLib::CMenuElementUI *>(ctrl);
}

void ContextMenu::SetItemChecked(const DuiLib::CDuiString &name, bool checked)
{
    if (helper == nullptr) {
        return;
    }
    helper->menuItemCheckList[name] = checked;
    if (helper->menuWnd && helper->menuWnd->m_pLayout) {
        DuiLib::CMenuElementUI *item = GetItem(name);
        if (item) {
            int index = helper->menuWnd->m_pLayout->GetItemIndex(item);
            helper->menuWnd->m_pLayout->SelectItem(index);
        }
    }
}

bool ContextMenu::GetItemChecked(const DuiLib::CDuiString &name) const
{
    if (helper == nullptr) {
        return false;
    }
    return helper->menuItemCheckList[name];
}

DuiLib::CDuiString ContextMenu::GetClickedItemName() const
{
    if (helper == nullptr) {
        return nullptr;
    }
    return helper->clickedItemName;
}

DuiLib::CMenuWnd *ContextMenu::GetMenu() const
{
    if (helper == nullptr) {
        return nullptr;
    }
    return helper->menuWnd.get();
}

NAMESPACE_DUILIB_END
