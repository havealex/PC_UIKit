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
#include "UIMenu.h"
#include <string>

namespace DuiLib {
const int REMOVE_ALL = 1;
const int REMOVE_SUB_MENU = 2;
const int SECURITY_BOUNDARY = 2;
const int TAKE_HALF = 2;
const int RADIX_TYPE_ONE = 10;
const int RADIX_TYPE_TWO = 16;

IMPLEMENT_DUICONTROL(CMenuUI)

CMenuUI::CMenuUI()
{
    if (GetHeader() != nullptr)
        GetHeader()->SetVisible(false);
}

CMenuUI::~CMenuUI() {}

LPCTSTR CMenuUI::GetClass() const
{
    return _T("MenuUI");
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("Menu")) == 0) {
        return static_cast<CMenuUI *>(this);
    }
    return CListUI::GetInterface(pstrName);
}

void CMenuUI::DoEvent(TEventUI &event)
{
    return __super::DoEvent(event);
}

bool CMenuUI::Add(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    CMenuElementUI *pMenuItem = static_cast<CMenuElementUI *>(pControl->GetInterface(_T("MenuElement")));
    if (pMenuItem == nullptr) {
        return false;
    }

    for (int i = 0; i < pMenuItem->GetCount(); ++i) {
        // Null pointer dereferences, Dereference null return (stat)
        CControlUI *item1 = pMenuItem->GetItemAt(i);

        if (item1 == nullptr) {
            continue;
        }

        CMenuElementUI *item2 = reinterpret_cast<CMenuElementUI *>(item1->GetInterface(L"MenuElement"));

        if (item2 == nullptr) {
            continue;
        }

        item2->SetInternVisible(false);
    }
    return CListUI::Add(pControl);
}

bool CMenuUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (pControl == nullptr) {
        return false;
    }
    CMenuElementUI *pMenuItem = static_cast<CMenuElementUI *>(pControl->GetInterface(_T("MenuElement")));
    if (pMenuItem == nullptr) {
        return false;
    }

    for (int i = 0; i < pMenuItem->GetCount(); ++i) {
        // Null pointer dereferences, Dereference null return (stat)
        CControlUI *item1 = pMenuItem->GetItemAt(i);

        if (item1 == nullptr) {
            continue;
        }

        CMenuElementUI *item2 = reinterpret_cast<CMenuElementUI *>(item1->GetInterface(L"MenuElement"));

        if (item2 == nullptr) {
            continue;
        }

        item2->SetInternVisible(false);
    }
    return CListUI::AddAt(pControl, iIndex);
}

int CMenuUI::GetItemIndex(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return -1;
    }
    CMenuElementUI *pMenuItem = static_cast<CMenuElementUI *>(pControl->GetInterface(_T("MenuElement")));
    if (pMenuItem == nullptr) {
        return -1;
    }

    return __super::GetItemIndex(pControl);
}

bool CMenuUI::SetItemIndex(CControlUI *pControl, int iIndex)
{
    if (pControl == nullptr) {
        return false;
    }
    CMenuElementUI *pMenuItem = static_cast<CMenuElementUI *>(pControl->GetInterface(_T("MenuElement")));
    if (pMenuItem == nullptr) {
        return false;
    }

    return __super::SetItemIndex(pControl, iIndex);
}

bool CMenuUI::Remove(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    CMenuElementUI *pMenuItem = static_cast<CMenuElementUI *>(pControl->GetInterface(_T("MenuElement")));
    if (pMenuItem == nullptr) {
        return false;
    }

    return __super::Remove(pControl);
}

SIZE CMenuUI::EstimateSize(SIZE szAvailable)
{
    int cxFixed = 0;
    int cyFixed = 0;
    for (int it = 0; it < GetCount(); it++) {
        CControlUI *pControl = static_cast<CControlUI *>(GetItemAt(it));
        if (pControl == nullptr) {
            return { 0 };
        }
        if (!pControl->IsVisible()) {
            continue;
        }
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
        if (cxFixed < sz.cx) {
            cxFixed = sz.cx;
        }
    }
    return CDuiSize(cxFixed, cyFixed);
}

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    CListUI::SetAttribute(pstrName, pstrValue);
}


CMenuWnd::CMenuWnd() : m_pOwner(nullptr), m_pLayout(), m_xml(_T("")), m_BasedPoint{ 0 }
{
    m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
}

CMenuWnd::~CMenuWnd()
{
    CMenuWnd::GetGlobalContextMenuObserver().RemoveReceiver(this);
}

BOOL CMenuWnd::Receive(ContextMenuParam param)
{
    switch (param.wParam) {
        case REMOVE_ALL: // remove all
            Close();
            break;
        case REMOVE_SUB_MENU: { // remove the sub menu
            HWND hParent = GetParent(m_hWnd);
            while (hParent != nullptr) {
                if (hParent == param.hWnd) {
                    Close();
                    break;
                }
                hParent = GetParent(hParent);
            }
        } break;
        default:
            break;
    }

    return TRUE;
}

void CMenuWnd::Init(CMenuElementUI *pOwner, STRINGorID xml, POINT point, CPaintManagerUI *pMainPaintManager,
    std::map<CDuiString, bool> *pMenuCheckInfo, DWORD dwAlignment)
{
    m_BasedPoint = point;
    m_pOwner = pOwner;
    m_pLayout = nullptr;
    m_xml = xml;
    m_dwAlignment = dwAlignment;

    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        if ((m_dwAlignment & eMenuAlignment_Right) == eMenuAlignment_Right) {
            m_dwAlignment ^= eMenuAlignment_Right;
            m_dwAlignment |= eMenuAlignment_Left;
        } else if ((m_dwAlignment & eMenuAlignment_Left) == eMenuAlignment_Left) {
            m_dwAlignment ^= eMenuAlignment_Left;
            m_dwAlignment |= eMenuAlignment_Right;
        }
    }

    // 如果是一级菜单的创建
    if (pOwner == nullptr) {
        ASSERT(pMainPaintManager != nullptr);
        CMenuWnd::GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
        if (pMenuCheckInfo != nullptr) {
            CMenuWnd::GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
        }
    }

    CMenuWnd::GetGlobalContextMenuObserver().AddReceiver(this);
    if (m_pOwner == nullptr && pMainPaintManager == nullptr) {
        return;
    }
    Create((m_pOwner == nullptr) ?
        pMainPaintManager->GetPaintWindow() : m_pOwner->GetManager()->GetPaintWindow(), nullptr,
        WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());

    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while (::GetParent(hWndParent) != nullptr) {
        hWndParent = ::GetParent(hWndParent);
    }

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
    return _T("DuiMenuWnd");
}


void CMenuWnd::Notify(TNotifyUI &msg)
{
    if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != nullptr) {
        if (msg.sType == _T("click") || msg.sType == _T("valuechanged")) {
            CMenuWnd::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, false);
        }
    }
}

CControlUI *CMenuWnd::CreateControl(LPCTSTR pstrClassName)
{
    if (_tcsicmp(pstrClassName, _T("Menu")) == 0) {
        return new CMenuUI();
    } else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0) {
        return new CMenuElementUI();
    }
    return nullptr;
}

void CMenuWnd::OnFinalMessage(HWND hWnd)
{
    RemoveObserver();
    if (m_pOwner != nullptr) {
        for (int i = 0; i < m_pOwner->GetCount(); i++) {
            // Null pointer dereferences, Dereference null return (stat)
            CMenuElementUI *item1 = reinterpret_cast<CMenuElementUI *>(m_pOwner->GetItemAt(i));

            if (item1 == nullptr) {
                continue;
            }

            CMenuElementUI *item2 = reinterpret_cast<CMenuElementUI *>(item1->GetInterface(L"MenuElement"));

            if (item2 == nullptr) {
                continue;
            }

            item1->SetOwner(m_pOwner->GetParent());
            item1->SetVisible(false);
            item2->SetInternVisible(false);
        }
        m_pOwner->m_pWindow = nullptr;
        m_pOwner->m_uButtonState &= ~UISTATE_PUSHED;
        m_pOwner->Invalidate();

        // 内部创建的内部删除
        delete this;
    }
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (m_pOwner != nullptr) {
        unsigned long styleValue = static_cast<unsigned long>(::GetWindowLong(*this, GWL_STYLE));
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        RECT rcClient;
        SecureZeroMemory(&rcClient, sizeof(rcClient));
        ::GetClientRect(*this, &rcClient);
        ::SetWindowPos(*this, nullptr, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
            rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CMenuUI();
        if (m_pLayout == nullptr) {
            return 0;
        }
        m_pm.SetForceUseSharedRes(true);
        m_pLayout->SetManager(&m_pm, nullptr, true);
        LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
        if (pDefaultAttributes) {
            m_pLayout->ApplyAttributeList(pDefaultAttributes);
        }
        m_pLayout->SetAutoDestroy(false);
        for (int i = 0; i < m_pOwner->GetCount(); i++) {
            if (m_pOwner->GetItemAt(i) == nullptr) {
                continue;
            }
            if (m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement")) != nullptr) {
                // Null pointer dereferences, Dereference null return (stat)
                CMenuElementUI *item1 = reinterpret_cast<CMenuElementUI *>(m_pOwner->GetItemAt(i));
                if (item1 == nullptr) {
                    continue;
                }

                item1->SetOwner(m_pLayout);
                m_pLayout->Add(item1);
            }
        }

        CShadowUI *pShadow = m_pOwner->GetManager()->GetShadow();
        pShadow->CopyShadow(m_pm.GetShadow());
        pShadow->ShowShadow(false);

        m_pm.AttachDialog(m_pLayout);
        m_pm.AddNotifier(this);

        ResizeSubMenu();
    } else {
        m_pm.Init(m_hWnd);

        CDialogBuilder builder;
        CControlUI *pRoot = builder.Create(m_xml, UINT(0), this, &m_pm);
        if (pRoot == nullptr) {
            return 0;
        }
        m_pm.GetShadow()->ShowShadow(false);
        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);
        ResizeMenu();
    }

    m_pm.GetShadow()->ShowShadow(true);
    m_pm.GetShadow()->Create(&m_pm);
    return 0;
}

CMenuUI *CMenuWnd::GetMenuUI()
{
    return static_cast<CMenuUI *>(m_pm.GetRoot());
}

void CMenuWnd::ResizeMenu()
{
    CControlUI *pRoot = m_pm.GetRoot();
    if (pRoot == nullptr) {
        return;
    }
#if defined(WIN32) && !defined(UNDER_CE)
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
#else
    CDuiRect rcWork;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
    SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
    szAvailable = pRoot->EstimateSize(szAvailable);
    m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

    // 必须是Menu标签作为xml的根节点
    auto pMenuRoot = static_cast<CMenuUI *>(pRoot);
    ASSERT(pMenuRoot);

    SIZE szInit = m_pm.GetInitSize();
    CDuiRect rc;
    CDuiPoint point = m_BasedPoint;
    rc.left = point.x;
    rc.top = point.y;
    rc.right = rc.left + szInit.cx;
    rc.bottom = rc.top + szInit.cy;

    int nWidth = rc.GetWidth();
    int nHeight = rc.GetHeight();

    if (m_dwAlignment & eMenuAlignment_Right) {
        rc.right = point.x;
        rc.left = rc.right - nWidth;
    }

    if (m_dwAlignment & eMenuAlignment_Bottom) {
        rc.bottom = point.y;
        rc.top = rc.bottom - nHeight;
    }

    SetForegroundWindow(m_hWnd);
    MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
    SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(),
        rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top, SWP_SHOWWINDOW);
}

void CMenuWnd::ResizeSubMenu()
{
    // Position the popup window in absolute space
    if (m_pOwner == nullptr) {
        return;
    }
    RECT rcOwner = m_pOwner->GetPos();
    RECT rc = rcOwner;

    int cxFixed = 0;
    int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
#else
    CDuiRect rcWork;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
    SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

    for (int it = 0; it < m_pOwner->GetCount(); it++) {
        // Null pointer dereferences, Dereference null return (stat)
        CMenuElementUI *item1 = reinterpret_cast<CMenuElementUI *>(m_pOwner->GetItemAt(it));
        if (item1 == nullptr) {
            continue;
        }

        CMenuElementUI *item2 = reinterpret_cast<CMenuElementUI *>(item1->GetInterface(L"MenuElement"));
        if (item2 == nullptr) {
            continue;
        }

        SIZE sz = item1->EstimateSize(szAvailable);
        cyFixed += sz.cy;

        if (cxFixed < sz.cx) {
            cxFixed = sz.cx;
        }
    }

    RECT rcWindow;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

    rc.top = rcOwner.top;
    rc.bottom = rc.top + cyFixed;
    ::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    rc.left = rcWindow.right;
    rc.right = rc.left + cxFixed;
    rc.right += SECURITY_BOUNDARY;

    bool bReachBottom = false;
    bool bReachRight = false;
    LONG chRightAlgin = 0;
    LONG chBottomAlgin = 0;

    RECT rcPreWindow = { 0 };
    MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
    MenuMenuReceiverImplBase *pReceiver = iterator.next();
    while (pReceiver != nullptr) {
        auto pContextMenu = dynamic_cast<CMenuWnd *>(pReceiver);
        if (pContextMenu != nullptr) {
            GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

            bReachRight = rcPreWindow.left >= rcWindow.right;
            bReachBottom = rcPreWindow.top >= rcWindow.bottom;
            if (pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() || bReachBottom || bReachRight)
                break;
        }
        pReceiver = iterator.next();
    }

    if (bReachBottom) {
        rc.bottom = rcWindow.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (bReachRight) {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;
    }

    if (rc.bottom > rcWork.bottom) {
        rc.bottom = rc.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (rc.right > rcWork.right) {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;
    }

    if (rc.top < rcWork.top) {
        rc.top = rcOwner.top;
        rc.bottom = rc.top + cyFixed;
    }

    if (rc.left < rcWork.left) {
        rc.left = rcWindow.right;
        rc.right = rc.left + cxFixed;
    }
    if (m_pLayout == nullptr) {
        return;
    }
    MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left,
        rc.bottom - rc.top + m_pLayout->GetInset().top + m_pLayout->GetInset().bottom, FALSE);
}


LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    HWND hFocusWnd = (HWND)wParam;

    BOOL bInMenuWindowList = FALSE;
    ContextMenuParam param;
    param.hWnd = GetHWND();

    MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
    MenuMenuReceiverImplBase *pReceiver = iterator.next();
    while (pReceiver != nullptr) {
        auto pContextMenu = dynamic_cast<CMenuWnd *>(pReceiver);
        if (pContextMenu != nullptr && pContextMenu->GetHWND() == hFocusWnd) {
            bInMenuWindowList = TRUE;
            break;
        }
        pReceiver = iterator.next();
    }

    if (!bInMenuWindowList) {
        param.wParam = 1;
        CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
    }

    if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != nullptr) {
        ::PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENU_LOSTFOCUS,
            (WPARAM)this, NULL);
    }
    return 0;
}
LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    SIZE szRoundCorner = m_pm.GetRoundCorner();
    if (!::IsIconic(*this)) {
        CDuiRect rcWnd;
        ::GetWindowRect(*this, &rcWnd);
        rcWnd.Offset(-rcWnd.left, -rcWnd.top);
        rcWnd.right++;
        rcWnd.bottom++;
        HRGN hRgn =
            ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
        ::SetWindowRgn(*this, hRgn, TRUE);
        ::DeleteObject(hRgn);
    }
    bHandled = FALSE;
    return 0;
}

CMenuElementUI *FindMenuItemAtPoint(CPaintManagerUI *pm, const POINT &pt, CMenuElementUI **otherItem)
{
    if (!pm) {
        return nullptr;
    }
    auto pControl = pm->FindControl(pt);
    if (pControl) {
        CMenuElementUI *menuItem = dynamic_cast<CMenuElementUI *>(pControl);
        while (pControl && !menuItem) {
            pControl = pControl->GetParent();
            menuItem = dynamic_cast<CMenuElementUI *>(pControl);
        }
        if (menuItem && dynamic_cast<CMenuUI *>(menuItem->GetOwner()) == pm->GetRoot()) {
            return menuItem;
        } else if (menuItem) {
            if (otherItem == nullptr) {
                return nullptr;
            }
            *otherItem = menuItem;
        }
    }

    return nullptr;
}

CMenuElementUI *GetOwnMenuItem(CPaintManagerUI *pm, CControlUI *ctrl)
{
    if (!pm) {
        return nullptr;
    }
    CControlUI *pControl = ctrl;
    if (pControl) {
        auto menuItem = dynamic_cast<CMenuElementUI *>(pControl);
        while (pControl && !menuItem) {
            pControl = pControl->GetParent();
            menuItem = dynamic_cast<CMenuElementUI *>(pControl);
        }

        if (menuItem) {
            IListOwnerUI *owner = menuItem->GetOwner();
            auto menuOwner = dynamic_cast<CMenuUI *>(owner);
            CControlUI *rootCtrl = pm->GetRoot();

            if (menuOwner == rootCtrl && menuOwner && menuOwner->GetName() == rootCtrl->GetName()) {
                return menuItem;
            }
        }
    }

    return nullptr;
}

DuiLib::CControlUI *CMenuUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    CControlUI *ctrl = __super::FindControl(Proc, pData, uFlags);
    if (GetOwnMenuItem(GetManager(), ctrl)) {
        return ctrl;
    }

    return nullptr;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    switch (uMsg) {
        case WM_CREATE:
            lRes = OnCreate(uMsg, wParam, lParam, bHandled);
            break;
        case WM_KILLFOCUS:
            lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE || wParam == VK_LEFT) {
                Close();
            }
            break;
        case WM_SIZE:
            lRes = OnSize(uMsg, wParam, lParam, bHandled);
            break;
        case WM_CLOSE:
            if (m_pOwner != nullptr) {
                m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
                m_pOwner->SetPos(m_pOwner->GetPos());
                m_pOwner->SetFocus();
            }
            break;
        case WM_MOUSEMOVE: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            CMenuElementUI *otherItem = nullptr;
            CMenuElementUI *menuItem = FindMenuItemAtPoint(&m_pm, pt, &otherItem);
            if (menuItem) {
                CMenuUI *menuUI = dynamic_cast<CMenuUI *>(menuItem->GetOwner());
                if (menuUI == nullptr) {
                    return 0;
                }
                for (int i = 0; i < menuUI->GetCount(); ++i) {
                    CMenuElementUI *item = dynamic_cast<CMenuElementUI *>(menuUI->GetItemAt(i));
                    if (item == nullptr) {
                        return 0;
                    }
                    item->ResetHoverStatus();
                }
                menuItem->SetHover();
                if (otherItem) {
                    otherItem->ResetHoverStatus();
                }
            }
        } break;
        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
            return 0L;
            break;
        default:
            bHandled = FALSE;
            break;
    }

    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) {
        return lRes;
    }
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

IMPLEMENT_DUICONTROL(CMenuElementUI)

CMenuElementUI::CMenuElementUI()
    : m_pWindow(nullptr),
      m_bDrawLine(false),
      m_dwLineColor(DEFAULT_LINE_COLOR),
      m_bCheckItem(false),
      m_bShowExplandIcon(false)
{
    m_cxyFixed.cy = ITEM_DEFAULT_HEIGHT;
    m_cxyFixed.cx = ITEM_DEFAULT_WIDTH;
    m_szIconSize.cy = ITEM_DEFAULT_ICON_SIZE;
    m_szIconSize.cx = ITEM_DEFAULT_ICON_SIZE;

    m_rcLinePadding.top = m_rcLinePadding.bottom = 0;
    m_rcLinePadding.left = DEFAULT_LINE_LEFT_INSET;
    m_rcLinePadding.right = DEFAULT_LINE_RIGHT_INSET;
}

CMenuElementUI::~CMenuElementUI() {}

LPCTSTR CMenuElementUI::GetClass() const
{
    return _T("MenuElementUI");
}

LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("MenuElement")) == 0) {
        return static_cast<CMenuElementUI *>(this);
    }
    return CListContainerElementUI::GetInterface(pstrName);
}

void CMenuElementUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) {
        return;
    }

    if (m_bDrawLine) {
        RECT m_rcLinePadding = CResourceManager::GetInstance()->Scale(CMenuElementUI::m_rcLinePadding);
        SIZE m_cxyFixed = CResourceManager::GetInstance()->Scale(CMenuElementUI::m_cxyFixed);
        RECT rcLine = { m_rcItem.left + m_rcLinePadding.left, m_rcItem.top + m_cxyFixed.cy / TAKE_HALF,
                        m_rcItem.right - m_rcLinePadding.right, m_rcItem.top + m_cxyFixed.cy / TAKE_HALF };
        CRenderEngine::DrawLine(hDC, rcLine, 1, m_dwLineColor);
    } else {
        CMenuElementUI::DrawItemBk(hDC, m_rcItem);
        DrawItemText(hDC, m_rcItem);
        DrawItemIcon(hDC, m_rcItem);
        DrawItemExpland(hDC, m_rcItem);
        for (int i = 0; i < GetCount(); ++i) {
            // Null pointer dereferences, Dereference null return (stat)
            // Cause crash problem by last commit.
            CControlUI *item1 = GetItemAt(i);
            if (item1 && item1->GetInterface(_T("MenuElement")) == nullptr) {
                item1->DoPaint(hDC, rcPaint);
            }
        }
    }
}

void CMenuElementUI::DrawItemIcon(HDC hDC, const RECT &rcItem)
{
    if (m_strIcon != _T("")) {
        wstring tempStr = m_strIcon.GetData();
        size_t pos = tempStr.find(L"##");
        if (pos != wstring::npos) {
            if (pos != 0) {
                wstring subStr = tempStr.substr(0, pos);
                wstring imgStr = tempStr.substr(pos + SECURITY_BOUNDARY);
                if (subStr == L"left") {
                    CDuiString pStrImage;
                    int left = m_cxyFixed.cx - m_szIconSize.cx - RADIX_TYPE_ONE;
                    pStrImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), imgStr.c_str(), left,
                        (m_cxyFixed.cy - m_szIconSize.cy) / TAKE_HALF, left + m_szIconSize.cx,
                        (m_cxyFixed.cy - m_szIconSize.cy) / TAKE_HALF + m_szIconSize.cy);
                    CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, _T(""));
                }
            }

            return;
        }

        if (!(m_bCheckItem && !GetChecked())) {
            CDuiString pStrImage;
            pStrImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), m_strIcon.GetData(),
                (ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx) / TAKE_HALF,
                (m_cxyFixed.cy - m_szIconSize.cy) / TAKE_HALF,
                (ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx) / TAKE_HALF + m_szIconSize.cx,
                (m_cxyFixed.cy - m_szIconSize.cy) / TAKE_HALF + m_szIconSize.cy);
            CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, _T(""));
        }
    }
}

void CMenuElementUI::DrawItemExpland(HDC hDC, const RECT &rcItem)
{
    if (m_bShowExplandIcon) {
        CDuiString strExplandIcon;
        strExplandIcon = GetManager()->GetDefaultAttributeList(_T("ExplandIcon"));
        CDuiString strBkImage;
        strBkImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), strExplandIcon.GetData(),
            m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH +
            (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / TAKE_HALF,
            (m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / TAKE_HALF,
            m_cxyFixed.cx - ITEM_DEFAULT_EXPLAND_ICON_WIDTH +
            (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / TAKE_HALF
            + ITEM_DEFAULT_EXPLAND_ICON_SIZE, (m_cxyFixed.cy - ITEM_DEFAULT_EXPLAND_ICON_SIZE) / TAKE_HALF
            + ITEM_DEFAULT_EXPLAND_ICON_SIZE);

        CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, strBkImage, _T(""));
    }
}

void CMenuElementUI::DrawItemText(HDC hDC, const RECT &rcItem)
{
    CDuiString sText = GetText();
    if (sText.IsEmpty()) {
        return;
    }

    if (m_pOwner == nullptr) {
        return;
    }
    TListInfoUI *pInfo = m_pOwner->GetListInfo();
    if (pInfo == nullptr) {
        return;
    }
    RECT rcTextPadding = CResourceManager::GetInstance()->Scale(pInfo->rcTextPadding);
    DWORD iTextColor = pInfo->dwTextColor;
    if ((m_uButtonState & UISTATE_HOT) != 0) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if (IsSelected()) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if (IsUseSpecialColor()) {
        iTextColor = pInfo->dwSpecialTextColor;
    }
    if (!IsEnabled()) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += rcTextPadding.left;
    rcText.right -= rcTextPadding.right;
    rcText.top += rcTextPadding.top;
    rcText.bottom -= rcTextPadding.bottom;

    if (pInfo->bShowHtml) {
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, sText, iTextColor, pInfo->nFont, nullptr, nullptr, nLinks,
            DT_SINGLELINE | pInfo->uTextStyle);
    } else {
        CRenderEngine::DrawText(hDC, m_pManager, rcText, sText, iTextColor, pInfo->nFont,
            DT_SINGLELINE | pInfo->uTextStyle);
    }
}


SIZE CMenuElementUI::EstimateSize(SIZE szAvailable)
{
    SIZE m_cxyFixed = CResourceManager::GetInstance()->Scale(CMenuElementUI::m_cxyFixed);

    SIZE cXY = { 0 };
    for (int it = 0; it < GetCount(); it++) {
        CControlUI *pControl = static_cast<CControlUI *>(GetItemAt(it));

        // Null pointer dereferences, Dereference null return (stat)
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }

        SIZE sz = pControl->EstimateSize(szAvailable);
        cXY.cy += sz.cy;
        if (cXY.cx < sz.cx)
            cXY.cx = sz.cx;
    }
    if (cXY.cy == 0) {
        TListInfoUI *pInfo = m_pOwner->GetListInfo();
        if (pInfo == nullptr) {
            return { 0 };
        }
        RECT rcTextPadding = CResourceManager::GetInstance()->Scale(pInfo->rcTextPadding);

        DWORD iTextColor = pInfo->dwTextColor;
        if ((m_uButtonState & UISTATE_HOT) != 0) {
            iTextColor = pInfo->dwHotTextColor;
        }
        if (IsSelected()) {
            iTextColor = pInfo->dwSelectedTextColor;
        }
        if (!IsEnabled()) {
            iTextColor = pInfo->dwDisabledTextColor;
        }
        CDuiString sText = GetText();

        RECT rcText = { 0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999 };
        rcText.left += rcTextPadding.left;
        rcText.right -= rcTextPadding.right;
        if (pInfo->bShowHtml) {
            int nLinks = 0;
            CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, iTextColor, pInfo->nFont,
                nullptr, nullptr, nLinks, DT_CALCRECT | pInfo->uTextStyle);
        } else {
            CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, iTextColor, pInfo->nFont,
                DT_CALCRECT | pInfo->uTextStyle);
        }
        cXY.cx = rcText.right - rcText.left + rcTextPadding.left + rcTextPadding.right;
        cXY.cy = rcText.bottom - rcText.top + rcTextPadding.top + rcTextPadding.bottom;
    }

    if (m_cxyFixed.cy != 0)
        cXY.cy = m_cxyFixed.cy;
    if (cXY.cx < m_cxyFixed.cx)
        cXY.cx = m_cxyFixed.cx;

    CMenuElementUI::m_cxyFixed.cy = CResourceManager::GetInstance()->RestoreScale(cXY.cy);
    CMenuElementUI::m_cxyFixed.cx = CResourceManager::GetInstance()->RestoreScale(cXY.cx);

    return cXY;
}

void CMenuElementUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_MOUSEENTER) {
        CListContainerElementUI::DoEvent(event);
        if (m_pWindow) {
            return;
        }
        bool hasSubMenu = false;
        for (int i = 0; i < GetCount(); ++i) {
            if (GetItemAt(i) == nullptr) {
                continue;
            }
            if (GetItemAt(i)->GetInterface(_T("MenuElement")) != nullptr) {
                (static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
                (static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

                hasSubMenu = true;
            }
        }
        if (m_pOwner == nullptr) {
            return;
        }
        if (hasSubMenu) {
            m_pOwner->SelectItem(GetIndex(), true);
            CreateMenuWnd();
        } else {
            ContextMenuParam param;
            param.hWnd = m_pManager->GetPaintWindow();
            param.wParam = REMOVE_SUB_MENU;
            CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
            m_pOwner->SelectItem(GetIndex(), true);
        }
        return;
    }

    if (event.Type == UIEVENT_BUTTONUP) {
        if (IsEnabled()) {
            CListContainerElementUI::DoEvent(event);
            if (m_pWindow) {
                return;
            }

            bool hasSubMenu = false;
            for (int i = 0; i < GetCount(); ++i) {
                if (GetItemAt(i) != nullptr && GetItemAt(i)->GetInterface(_T("MenuElement")) != nullptr) {
                    (static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
                    (static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(_T("MenuElement"))))
                        ->SetInternVisible(true);

                    hasSubMenu = true;
                }
            }
            if (hasSubMenu) {
                CreateMenuWnd();
            } else {
                SetChecked(!GetChecked());

                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = 1;
                CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

                if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != nullptr) {
                    if (!PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(),
                        WM_MENUCLICK, (WPARAM)this, NULL)) {
                    }
                }
            }
        }

        return;
    }

    if (event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RIGHT) {
        if (m_pWindow == nullptr || m_pOwner == nullptr) {
            return;
        }
        bool hasSubMenu = false;
        for (int i = 0; i < GetCount(); ++i) {
            // Null pointer dereferences, Dereference null return (stat)
            CControlUI *item1 = GetItemAt(i);

            if (item1 == nullptr) {
                continue;
            }

            CMenuElementUI *item2 = reinterpret_cast<CMenuElementUI *>(item1->GetInterface(L"MenuElement"));

            if (item2 == nullptr) {
                continue;
            }

            item2->SetVisible(true);
            item2->SetInternVisible(true);
            hasSubMenu = true;
        }
        if (hasSubMenu) {
            m_pOwner->SelectItem(GetIndex(), true);
            CreateMenuWnd();
        } else {
            ContextMenuParam param;
            param.hWnd = m_pManager->GetPaintWindow();
            param.wParam = REMOVE_SUB_MENU;
            CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
            m_pOwner->SelectItem(GetIndex(), true);
        }

        return;
    }

    CListContainerElementUI::DoEvent(event);
}

CMenuWnd *CMenuElementUI::GetMenuWnd()
{
    return m_pWindow;
}

void CMenuElementUI::CreateMenuWnd()
{
    if (m_pWindow) {
        return;
    }
    m_pWindow = new CMenuWnd();

    if (m_pWindow == nullptr || m_pManager == nullptr) {
        return;
    }
    ContextMenuParam param;
    param.hWnd = m_pManager->GetPaintWindow();
    param.wParam = REMOVE_SUB_MENU;
    CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

    m_pWindow->Init(static_cast<CMenuElementUI *>(this), _T(""), CDuiPoint(), nullptr);
}

void CMenuElementUI::SetLineType()
{
    m_bDrawLine = true;
    if (GetFixedHeight() == 0 || GetFixedHeight() == ITEM_DEFAULT_HEIGHT) {
        SetFixedHeight(DEFAULT_LINE_HEIGHT);
    }

    SetMouseChildEnabled(false);
    SetMouseEnabled(false);
    SetEnabled(false);
}

void CMenuElementUI::SetLineColor(DWORD color)
{
    m_dwLineColor = color;
}

DWORD CMenuElementUI::GetLineColor() const
{
    return m_dwLineColor;
}
void CMenuElementUI::SetLinePadding(RECT rcInset)
{
    m_rcLinePadding = rcInset;
}

RECT CMenuElementUI::GetLinePadding() const
{
    return m_rcLinePadding;
}

void CMenuElementUI::SetIcon(LPCTSTR strIcon)
{
    if (strIcon != _T("")) {
        m_strIcon = strIcon;
    }
}

void CMenuElementUI::SetIconSize(LONG cx, LONG cy)
{
    m_szIconSize.cx = cx;
    m_szIconSize.cy = cy;
}

void CMenuElementUI::SetChecked(bool bCheck)
{
    if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == nullptr) {
        return;
    }
    std::map<CDuiString, bool>::iterator it =
        CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
    if (it == CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end()) {
        CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->insert(
            std::map<CDuiString, bool>::value_type(GetName(), bCheck));
    } else {
        it->second = bCheck;
    }
}

bool CMenuElementUI::GetChecked() const
{
    if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == nullptr ||
        CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->size() == 0) {
        return false;
    }

    std::map<CDuiString, bool>::iterator it =
        CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
    if (it != CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end()) {
        return it->second;
    }
    return false;
}

void CMenuElementUI::SetCheckItem(bool bCheckItem /* = false */)
{
    m_bCheckItem = bCheckItem;
}

bool CMenuElementUI::GetCheckItem() const
{
    return m_bCheckItem;
}

void CMenuElementUI::SetShowExplandIcon(bool bShow)
{
    m_bShowExplandIcon = bShow;
}

void CMenuElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("icon")) == 0) {
        SetIcon(pstrValue);
    } else if (_tcsicmp(pstrName, _T("iconsize")) == 0) {
        LPTSTR pstr = nullptr;

        LONG cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        LONG cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        SetIconSize(cx, cy);
    } else if (_tcsicmp(pstrName, _T("checkitem")) == 0) {
        SetCheckItem(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
    } else if (_tcsicmp(pstrName, _T("ischeck")) == 0) {
        if (CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() != nullptr &&
            CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName()) ==
            CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end()) {
            SetChecked(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
        }
    } else if (_tcsicmp(pstrName, _T("linetype")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            SetLineType();
        }
    } else if (_tcsicmp(pstrName, _T("expland")) == 0) {
        SetShowExplandIcon(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
    } else if (_tcsicmp(pstrName, _T("linecolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        SetLineColor(_tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO));
    } else if (_tcsicmp(pstrName, _T("linepadding")) == 0) {
        RECT rcInset = { 0 };
        LPTSTR pstr = nullptr;
        rcInset.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        rcInset.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        rcInset.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        rcInset.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE);
        ASSERT(pstr);
        SetLinePadding(rcInset);
    } else if (_tcsicmp(pstrName, _T("height")) == 0) {
        SetFixedHeight(_ttoi(pstrValue));
    } else {
        CListContainerElementUI::SetAttribute(pstrName, pstrValue);
    }
}

void CMenuElementUI::SetHover()
{
    m_uButtonState |= UISTATE_HOT;
    Invalidate();
}

void CMenuElementUI::ResetHoverStatus()
{
    m_uButtonState &= ~UISTATE_HOT;
    Invalidate();
}
} // namespace DuiLib
