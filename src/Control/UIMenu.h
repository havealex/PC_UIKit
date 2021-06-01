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

#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

#include "../Utils/observer_impl_base.h"


namespace DuiLib {
struct ContextMenuParam {
    // 1: remove all
    // 2: remove the sub menu
    WPARAM wParam;
    HWND hWnd;
};

struct MenuCmd {
    TCHAR szName[256];
    TCHAR szUserData[1024];
    TCHAR szText[1024];
    BOOL bChecked;
};

enum MenuAlignment {
    eMenuAlignment_Left = 1 << 1,
    eMenuAlignment_Top = 1 << 2,
    eMenuAlignment_Right = 1 << 3,
    eMenuAlignment_Bottom = 1 << 4,
};


enum MenuItemDefaultInfo {
    ITEM_DEFAULT_HEIGHT = 30, // ÿһ��item��Ĭ�ϸ߶ȣ�ֻ����״����ʱ�Զ��壩
    ITEM_DEFAULT_WIDTH = 150, // ���ڵ�Ĭ�Ͽ��

    ITEM_DEFAULT_ICON_WIDTH = 26, // Ĭ��ͼ����ռ���
    ITEM_DEFAULT_ICON_SIZE = 16,  // Ĭ��ͼ��Ĵ�С

    ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20, // Ĭ���¼��˵���չͼ����ռ���
    ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,   // Ĭ���¼��˵���չͼ��Ĵ�С

    DEFAULT_LINE_LEFT_INSET = ITEM_DEFAULT_ICON_WIDTH + 3, // Ĭ�Ϸָ��ߵ���߾�
    DEFAULT_LINE_RIGHT_INSET = 7,                          // Ĭ�Ϸָ��ߵ��ұ߾�
    DEFAULT_LINE_HEIGHT = 6,                               // Ĭ�Ϸָ�����ռ�߶�
    DEFAULT_LINE_COLOR = 0xFFBCBFC4                        // Ĭ�Ϸָ�����ɫ

};

#define WM_MENUCLICK WM_USER + 121      // �������հ�ť��������Ϣ
#define WM_MENU_LOSTFOCUS WM_USER + 122 // ����֪ͨMenuʧȥ�������Ϣ


class MenuMenuReceiverImplBase;
class MenuMenuObserverImplBase {
public:
    virtual void AddReceiver(MenuMenuReceiverImplBase *receiver) = 0;
    virtual void RemoveReceiver(MenuMenuReceiverImplBase *receiver) = 0;
    virtual BOOL RBroadcast(ContextMenuParam param) = 0;
};

class MenuMenuReceiverImplBase {
public:
    virtual void AddObserver(MenuMenuObserverImplBase *observer) = 0;
    virtual void RemoveObserver() = 0;
    virtual BOOL Receive(ContextMenuParam param) = 0;
};
class MenuReceiverImpl;
class UILIB_API MenuObserverImpl : public MenuMenuObserverImplBase {
    friend class Iterator;

public:
    MenuObserverImpl() : m_pMainWndPaintManager(nullptr), m_pMenuCheckInfo(nullptr)
    {
        pReceivers_ = new ReceiversVector;
    }

    ~MenuObserverImpl()
    {
        if (pReceivers_ != nullptr) {
            delete pReceivers_;
            pReceivers_ = nullptr;
        }
    }

    void AddReceiver(MenuMenuReceiverImplBase *receiver) override
    {
        if (receiver == nullptr || pReceivers_ == nullptr) {
            return;
        }

        pReceivers_->push_back(receiver);
        receiver->AddObserver(this);
    }

    void RemoveReceiver(MenuMenuReceiverImplBase *receiver) override
    {
        if (receiver == nullptr || pReceivers_ == nullptr) {
            return;
        }
        ReceiversVector::iterator it = pReceivers_->begin();
        for (; it != pReceivers_->end(); ++it) {
            if (*it == receiver) {
                pReceivers_->erase(it);
                break;
            }
        }
    }

    virtual BOOL RBroadcast(ContextMenuParam param)
    {
        if (pReceivers_ == nullptr) {
            return FALSE;
        }
        ReceiversVector::reverse_iterator it = pReceivers_->rbegin();
        for (; it != pReceivers_->rend(); ++it) {
            (*it)->Receive(param);
        }

        return BOOL();
    }


    class Iterator {
        MenuObserverImpl &_tbl;
        DWORD index;

    public:
        explicit Iterator(MenuObserverImpl &table) : _tbl(table), index(0), ptr(nullptr) {}

        Iterator(const Iterator &v) : _tbl(v._tbl), index(v.index), ptr(v.ptr) {}

        Iterator & operator=(const Iterator &v) = default;

        ~Iterator() {};

        MenuMenuReceiverImplBase *next()
        {
            if (index >= _tbl.pReceivers_->size()) {
                return nullptr;
            }

            for (; index < _tbl.pReceivers_->size();) {
                ptr = (*(_tbl.pReceivers_))[index++];
                if (ptr) {
                    return ptr;
                }
            }
            return nullptr;
        }

        MenuMenuReceiverImplBase *ptr;
    };

    virtual void SetManger(CPaintManagerUI *pManager)
    {
        if (pManager != nullptr) {
            m_pMainWndPaintManager = pManager;
        }
    }

    virtual CPaintManagerUI *GetManager() const
    {
        return m_pMainWndPaintManager;
    }

    virtual void SetMenuCheckInfo(std::map<CDuiString, bool> *pInfo)
    {
        if (pInfo != nullptr) {
            m_pMenuCheckInfo = pInfo;
        } else {
            m_pMenuCheckInfo = nullptr;
        }
    }

    virtual std::map<CDuiString, bool> *GetMenuCheckInfo() const
    {
        return m_pMenuCheckInfo;
    }

    using ReceiversVector = std::vector<MenuMenuReceiverImplBase *>;
    ReceiversVector *pReceivers_;

    CPaintManagerUI *m_pMainWndPaintManager;
    std::map<CDuiString, bool> *m_pMenuCheckInfo;
};

// //////////////////////////////////////////////////
class UILIB_API MenuReceiverImpl : public MenuMenuReceiverImplBase {
public:
    MenuReceiverImpl()
    {
        pObservers_ = new ObserversVector;
    }

    ~MenuReceiverImpl()
    {
        if (pObservers_ != nullptr) {
            delete pObservers_;
            pObservers_ = nullptr;
        }
    }

    void AddObserver(MenuMenuObserverImplBase *observer) override
    {
        if (pObservers_ == nullptr) {
            return;
        }
        pObservers_->push_back(observer);
    }

    void RemoveObserver() override
    {
        if (pObservers_ == nullptr) {
            return;
        }
        ObserversVector::iterator it = pObservers_->begin();
        for (; it != pObservers_->end(); ++it) {
            (*it)->RemoveReceiver(this);
        }
    }

    BOOL Receive(ContextMenuParam param) override
    {
        return BOOL();
    }

protected:
    using ObserversVector = std::vector<MenuMenuObserverImplBase *>;
    ObserversVector *pObservers_;
};

class CListUI;
class UILIB_API CMenuUI : public CListUI {
    DECLARE_DUICONTROL(CMenuUI)
public:
    CMenuUI();
    virtual ~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoEvent(TEventUI &event) override;

    bool Add(CControlUI *pControl) override;
    bool AddAt(CControlUI *pControl, int iIndex) override;

    int GetItemIndex(CControlUI *pControl) override;
    bool SetItemIndex(CControlUI *pControl, int iIndex) override;
    bool Remove(CControlUI *pControl) override;

    SIZE EstimateSize(SIZE szAvailable) override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    CControlUI *FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags) override;
};

class CMenuElementUI;
class UILIB_API CMenuWnd : public CWindowWnd, public MenuReceiverImpl, public INotifyUI, public IDialogBuilderCallback {
public:
    static MenuObserverImpl &GetGlobalContextMenuObserver()
    {
        static MenuObserverImpl s_context_menu_observer;
        return s_context_menu_observer;
    }

public:
    CMenuWnd();
    ~CMenuWnd();

    /*
     * 	@pOwner һ���˵���Ҫָ��������������ǲ˵��ڲ�ʹ�õ�
     * 	@xml	�˵��Ĳ����ļ�
     * 	@point	�˵������Ͻ�����
     * 	@pMainPaintManager	�˵��ĸ����������ָ��
     * 	@pMenuCheckInfo	����˵��ĵ�ѡ�͸�ѡ��Ϣ�ṹָ��
     * 	@dwAlignment		�˵��ĳ���λ�ã�Ĭ�ϳ������������²ࡣ
     */
    void Init(CMenuElementUI *pOwner, STRINGorID xml, POINT point, CPaintManagerUI *pMainPaintManager,
        std::map<CDuiString, bool> *pMenuCheckInfo = nullptr,
        DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);
    void Notify(TNotifyUI &msg) override;
    CControlUI *CreateControl(LPCTSTR pstrClassName);

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL Receive(ContextMenuParam param);

    // ��ȡ���˵��ؼ������ڶ�̬����Ӳ˵�
    CMenuUI *GetMenuUI();

    // ���µ����˵��Ĵ�С
    void ResizeMenu();

    // ���µ����Ӳ˵��Ĵ�С
    void ResizeSubMenu();

public:
    POINT m_BasedPoint;
    STRINGorID m_xml;
    CPaintManagerUI m_pm;
    CMenuElementUI *m_pOwner;
    CMenuUI *m_pLayout;
    DWORD m_dwAlignment; // �˵����뷽ʽ
};

class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI {
    DECLARE_DUICONTROL(CMenuElementUI)
    friend CMenuWnd;

public:
    CMenuElementUI();
    ~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void DoPaint(HDC hDC, const RECT &rcPaint);
    void DrawItemText(HDC hDC, const RECT &rcItem);
    SIZE EstimateSize(SIZE szAvailable);

    void DoEvent(TEventUI &event) override;

    CMenuWnd *GetMenuWnd();
    void CreateMenuWnd();

    void SetLineType();
    void SetLineColor(DWORD color);
    DWORD GetLineColor() const;
    void SetLinePadding(RECT rcInset);
    RECT GetLinePadding() const;
    void SetIcon(LPCTSTR strIcon);
    void SetIconSize(LONG cx, LONG cy);
    void DrawItemIcon(HDC hDC, const RECT &rcItem);
    void SetChecked(bool bCheck = true);
    bool GetChecked() const;
    void SetCheckItem(bool bCheckItem = false);
    bool GetCheckItem() const;

    void SetShowExplandIcon(bool bShow);
    void DrawItemExpland(HDC hDC, const RECT &rcItem);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetHover();
    void ResetHoverStatus();

protected:
    bool m_bDrawLine;     // ���ָ���
    DWORD m_dwLineColor;  // �ָ�����ɫ
    RECT m_rcLinePadding; // �ָ��ߵ����ұ߾�

    SIZE m_szIconSize; // ��ͼ��
    CDuiString m_strIcon;
    bool m_bCheckItem;

    bool m_bShowExplandIcon;

public:
    CMenuWnd *m_pWindow;
};
} // namespace DuiLib

#endif // __UIMENU_H__
