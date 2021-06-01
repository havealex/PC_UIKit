/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "../Debug/LogOut.h"
#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace DuiLib {
void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
    CDuiString strMsg;
    va_list Args;

    va_start(Args, pstrFormat);
    strMsg.Format(pstrFormat, Args);
    va_end(Args);

    strMsg += _T("\n");
    OutputDebugString(strMsg.GetData());

#endif
}

LPCTSTR DUI__TraceMsg(UINT uMsg)
{
#define MSGDEF(x)  \
    if (uMsg == x) \
    return _T(#x)
    MSGDEF(WM_SETCURSOR);
    MSGDEF(WM_NCHITTEST);
    MSGDEF(WM_NCPAINT);
    MSGDEF(WM_PAINT);
    MSGDEF(WM_ERASEBKGND);
    MSGDEF(WM_NCMOUSEMOVE);
    MSGDEF(WM_MOUSEMOVE);
    MSGDEF(WM_MOUSELEAVE);
    MSGDEF(WM_MOUSEHOVER);
    MSGDEF(WM_NOTIFY);
    MSGDEF(WM_COMMAND);
    MSGDEF(WM_MEASUREITEM);
    MSGDEF(WM_DRAWITEM);
    MSGDEF(WM_LBUTTONDOWN);
    MSGDEF(WM_LBUTTONUP);
    MSGDEF(WM_LBUTTONDBLCLK);
    MSGDEF(WM_RBUTTONDOWN);
    MSGDEF(WM_RBUTTONUP);
    MSGDEF(WM_RBUTTONDBLCLK);
    MSGDEF(WM_SETFOCUS);
    MSGDEF(WM_KILLFOCUS);
    MSGDEF(WM_MOVE);
    MSGDEF(WM_SIZE);
    MSGDEF(WM_SIZING);
    MSGDEF(WM_MOVING);
    MSGDEF(WM_GETMINMAXINFO);
    MSGDEF(WM_CAPTURECHANGED);
    MSGDEF(WM_WINDOWPOSCHANGED);
    MSGDEF(WM_WINDOWPOSCHANGING);
    MSGDEF(WM_NCCALCSIZE);
    MSGDEF(WM_NCCREATE);
    MSGDEF(WM_NCDESTROY);
    MSGDEF(WM_TIMER);
    MSGDEF(WM_KEYDOWN);
    MSGDEF(WM_KEYUP);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SYSKEYDOWN);
    MSGDEF(WM_SYSKEYUP);
    MSGDEF(WM_SYSCOMMAND);
    MSGDEF(WM_SYSCHAR);
    MSGDEF(WM_VSCROLL);
    MSGDEF(WM_HSCROLL);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SHOWWINDOW);
    MSGDEF(WM_PARENTNOTIFY);
    MSGDEF(WM_CREATE);
    MSGDEF(WM_NCACTIVATE);
    MSGDEF(WM_ACTIVATE);
    MSGDEF(WM_ACTIVATEAPP);
    MSGDEF(WM_CLOSE);
    MSGDEF(WM_DESTROY);
    MSGDEF(WM_GETICON);
    MSGDEF(WM_GETTEXT);
    MSGDEF(WM_GETTEXTLENGTH);
    const int msgLen = 10;
    static TCHAR szMsg[msgLen];
    swprintf_s(szMsg, _countof(szMsg), L"0x%04X", uMsg);
    return szMsg;
}

DUI_BASE_BEGIN_MESSAGE_MAP(CNotifyPump)
DUI_END_MESSAGE_MAP()

static const DUI_MSGMAP_ENTRY *DuiFindMessageEntry(const DUI_MSGMAP_ENTRY *lpEntry, TNotifyUI &msg)
{
    if (msg.pSender == nullptr) {
        return nullptr;
    }

    CDuiString sMsgType = msg.sType;
    CDuiString sCtrlName = msg.pSender->GetName();
    const DUI_MSGMAP_ENTRY *pMsgTypeEntry = nullptr;
    while (lpEntry->nSig != DuiSig_end) {
        if (lpEntry->sMsgType == sMsgType) {
            if (!lpEntry->sCtrlName.IsEmpty()) {
                if (lpEntry->sCtrlName == sCtrlName) {
                    return lpEntry;
                }
            } else {
                pMsgTypeEntry = lpEntry;
            }
        }
        lpEntry++;
    }
    return pMsgTypeEntry;
}

bool CNotifyPump::AddVirtualWnd(CDuiString strName, CNotifyPump *pObject)
{
    if (m_VirtualWndMap.Find(strName) == nullptr) {
        m_VirtualWndMap.Insert(strName.GetData(), (LPVOID)pObject);
        return true;
    }
    return false;
}

bool CNotifyPump::RemoveVirtualWnd(CDuiString strName)
{
    if (m_VirtualWndMap.Find(strName) != nullptr) {
        m_VirtualWndMap.Remove(strName);
        return true;
    }
    return false;
}

bool CNotifyPump::LoopDispatch(TNotifyUI &msg)
{
    const DUI_MSGMAP_ENTRY *lpEntry = nullptr;
    const DUI_MSGMAP *pMessageMap = nullptr;

#ifndef UILIB_STATIC
    for (pMessageMap = GetMessageMap(); pMessageMap != nullptr; pMessageMap = (*pMessageMap->pfnGetBaseMap)()) {
#else
    for (pMessageMap = GetMessageMap(); pMessageMap != nullptr; pMessageMap = pMessageMap->pBaseMap) {
#endif
#ifndef UILIB_STATIC
        if (pMessageMap == (*pMessageMap->pfnGetBaseMap)()) {
            return false;
        }
#else
        if (pMessageMap == pMessageMap->pBaseMap) {
            return false;
        }
#endif
        if (pMessageMap->lpEntries == nullptr) {
            return false;
        }
        if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries, msg)) != nullptr) {
            goto LDispatch;
        }
    }
    return false;

LDispatch:
    union DuiMessageMapFunctions mmf;
    mmf.pfn = lpEntry->pfn;

    bool bRet = false;
    int nSig;
    nSig = lpEntry->nSig;
    switch (nSig) {
        default:
            break;
        case DuiSig_lwl:
            (this->*mmf.pfn_Notify_lwl)(msg.wParam, msg.lParam);
            bRet = true;
            break;
        case DuiSig_vn:
            (this->*mmf.pfn_Notify_vn)(msg);
            bRet = true;
            break;
    }
    return bRet;
}

void CNotifyPump::NotifyPump(TNotifyUI &msg)
{
    // /遍历虚拟窗口
    if (!msg.sVirtualWnd.IsEmpty()) {
        for (int i = 0; i < m_VirtualWndMap.GetSize(); i++) {
            if (LPCTSTR key = m_VirtualWndMap.GetAt(i)) {
                if (_tcsicmp(key, msg.sVirtualWnd.GetData()) == 0) {
                    CNotifyPump *pObject = static_cast<CNotifyPump *>(m_VirtualWndMap.Find(key, false));
                    if (pObject && pObject->LoopDispatch(msg)) {
                        return;
                    }
                }
            }
        }
    }

    // 遍历主窗口
    LoopDispatch(msg);
}


void CNotifyPump::ForceNotifyPump(TNotifyUI &msg)
{
    // /遍历虚拟窗口
    for (int i = 0; i < m_VirtualWndMap.GetSize(); i++) {
        if (LPCTSTR key = m_VirtualWndMap.GetAt(i)) {
            CNotifyPump *pObject = static_cast<CNotifyPump *>(m_VirtualWndMap.Find(key, false));
            if (pObject) {
                pObject->LoopDispatch(msg);
            }
        }
    }
}

CWindowWnd::CWindowWnd() : m_hWnd(nullptr), m_OldWndProc(::DefWindowProc), m_bSubclassed(false), m_bMsgLoop(true) {}

CWindowWnd::~CWindowWnd()
{
    if (::IsWindow(m_hWnd)) {
        ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0L);
        ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)::DefWindowProc);
        ::DestroyWindow(m_hWnd);
    }
    if (smallIcon != nullptr) {
        DestroyIcon(smallIcon);
        smallIcon = nullptr;
    }
    if (bigIcon != nullptr) {
        DestroyIcon(bigIcon);
        bigIcon = nullptr;
    }
}

HWND CWindowWnd::GetHWND() const
{
    return m_hWnd;
}

UINT CWindowWnd::GetClassStyle() const
{
    return 0;
}

LPCTSTR CWindowWnd::GetSuperClassName() const
{
    return nullptr;
}

CWindowWnd::operator HWND() const
{
    return m_hWnd;
}

HWND CWindowWnd::CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle, DWORD dwExStyle)
{
    return Create(hwndParent, pstrWindowName, dwStyle, dwExStyle, 0, 0, 0, 0, nullptr);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        hMenu);
}

HWND CWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy,
    HMENU hMenu)
{
    if (GetSuperClassName() != nullptr && !RegisterSuperclass()) {
        return nullptr;
    }
    if (GetSuperClassName() == nullptr && !RegisterWindowClass()) {
        return nullptr;
    }
    m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu,
        CPaintManagerUI::GetInstance(), this);
    if (m_hWnd == nullptr) {
        return nullptr;
    }

    wstring name = L"NULL";
    if (GetWindowClassName()) {
        name = GetWindowClassName();
    }
    LOGGER_DEBUG(L"DUILIB  Create wnd:" << m_hWnd << L", name:%s" << name);
    return m_hWnd;
}

HWND CWindowWnd::Subclass(HWND hWnd)
{
    if (hWnd == nullptr || ::IsWindow(hWnd) == FALSE || m_hWnd == nullptr) {
        return nullptr;
    }
    m_OldWndProc = SubclassWindow(hWnd, __WndProc);
    if (m_OldWndProc == nullptr) {
        return nullptr;
    }
    m_bSubclassed = true;
    m_hWnd = hWnd;
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
    return m_hWnd;
}

void CWindowWnd::Unsubclass()
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return;
    }
    if (!::IsWindow(m_hWnd)) {
        return;
    }
    if (!m_bSubclassed) {
        return;
    }
    SubclassWindow(m_hWnd, m_OldWndProc);
    m_OldWndProc = ::DefWindowProc;
    m_bSubclassed = false;
}

void CWindowWnd::ShowWindow(bool bShow, bool bTakeFocus)
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return;
    }

    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetWindowClassName()) {
            temp = GetWindowClassName();
        }
    }

    if (!::IsWindow(m_hWnd)) {
        return;
    }
    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
    if (!bShow) {
        m_inModal = false;
    }
}

UINT CWindowWnd::ShowModal()
{
    UINT nRet = 0;
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return nRet;
    }
    HWND hWndParent = GetWindowOwner(m_hWnd);
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
    if (m_bPopup) {
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
    } else {
        ::EnableWindow(hWndParent, FALSE);
    }
    m_inModal = true;
    if (!m_bMsgLoop) {
        return nRet;
    }
    MSG msg = { 0 };
    while (m_inModal && ::IsWindow(m_hWnd) && ::GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_CLOSE && msg.hwnd == m_hWnd) {
            nRet = static_cast<UINT>(msg.wParam);
            ::EnableWindow(hWndParent, TRUE);
            ::SetFocus(hWndParent);
        } else if (msg.message == UIMSG_UPDATE_ON_MAIN) {
            // 主线程执行队列，用于在工作线程里面一键切换到主线程执行UI的更新
            UIMainQueue::getInstance().finish();
        }

        if (!CPaintManagerUI::TranslateMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) {
            break;
        }
    }
    ::EnableWindow(hWndParent, TRUE);
    ::SetFocus(hWndParent);
    if (msg.message == WM_QUIT) {
        ::PostQuitMessage(static_cast<int>(msg.wParam));
    }
    return nRet;
}

void CWindowWnd::Close(UINT nRet)
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return;
    }
    if (!::IsWindow(m_hWnd)) {
        return;
    }
    PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);

    if (CPaintManagerUI::IsMainThread() == false) {
        wstring name;
        if (GetWindowClassName()) {
            name = GetWindowClassName();
        }
        LOGGER_DEBUG(L"DUILIB wnd Close name:" << name);
    }
}

void CWindowWnd::CenterWindow()
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return;
    }
    if ((GetWindowStyle(m_hWnd) & WS_CHILD) != 0) {
        return;
    }
    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
    HWND hWnd = *this;
    HWND hWndParent = ::GetParent(m_hWnd);
    HWND hWndCenter = ::GetWindowOwner(m_hWnd);
    if (hWndCenter != nullptr) {
        hWnd = hWndCenter;
    }

    // 处理多显示器模式下屏幕居中
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    rcArea = oMonitor.rcWork;

    if (hWndCenter == nullptr || ::IsIconic(hWndCenter)) {
        rcCenter = rcArea;
    } else {
        ::GetWindowRect(hWndCenter, &rcCenter);
    }

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    const int centerSize = 2;
    int xLeft = (rcCenter.left + rcCenter.right) / centerSize - DlgWidth / centerSize;
    int yTop = (rcCenter.top + rcCenter.bottom) / centerSize - DlgHeight / centerSize;

    // The dialog is outside the screen, move it inside
    if (xLeft < rcArea.left) {
        xLeft = rcArea.left;
    } else if (xLeft + DlgWidth > rcArea.right) {
        xLeft = rcArea.right - DlgWidth;
    }
    if (yTop < rcArea.top) {
        yTop = rcArea.top;
    } else if (yTop + DlgHeight > rcArea.bottom) {
        yTop = rcArea.bottom - DlgHeight;
    }
    ::SetWindowPos(m_hWnd, nullptr, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWindowWnd::SetIcon(UINT nRes)
{
    if (m_hWnd == nullptr) {
        return;
    }
    HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
        ((UINT)::GetSystemMetrics(SM_CXICON) + 15) & ~15,
        ((UINT)::GetSystemMetrics(SM_CYICON) + 15) & ~15, // 防止高DPI下图标模糊
        LR_DEFAULTCOLOR);
    if (hIcon == nullptr) {
        return;
    }
    ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);
    bigIcon = hIcon;
    hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON,
        ((UINT)::GetSystemMetrics(SM_CXICON) + 15) & ~15,
        ((UINT)::GetSystemMetrics(SM_CYICON) + 15) & ~15, // 防止高DPI下图标模糊
        LR_DEFAULTCOLOR);
    if (hIcon == nullptr) {
        return;
    }
    ::SendMessage(m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
    smallIcon = hIcon;
}


void CWindowWnd::SetPopup(bool popup)
{
    m_bPopup = popup;
}

bool CWindowWnd::RegisterWindowClass()
{
    WNDCLASS wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = nullptr;
    wc.lpfnWndProc = CWindowWnd::__WndProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClass(&wc);
#ifdef WIN32_NFF_CHECK
    if (::GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
        ::UnregisterClass(GetWindowClassName(), CPaintManagerUI::GetInstance());
        ret = ::RegisterClass(&wc);
    }
#endif // WIN32_NFF_CHECK
    if (ret == NULL && ::GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool CWindowWnd::RegisterSuperclass()
{
    // Get the class information from an existing
    // window so we can subclass it later on...
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    if (!::GetClassInfoEx(nullptr, GetSuperClassName(), &wc)) {
        if (!::GetClassInfoEx(CPaintManagerUI::GetInstance(), GetSuperClassName(), &wc)) {
            return nullptr;
        }
    }
    m_OldWndProc = wc.lpfnWndProc;
    wc.lpfnWndProc = CWindowWnd::__ControlProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassEx(&wc);

#ifdef WIN32_NFF_CHECK
    if (::GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
        ::UnregisterClass(GetWindowClassName(), CPaintManagerUI::GetInstance());
        ret = ::RegisterClassEx(&wc);
    }
#endif // WIN32_NFF_CHECK

    if (ret == NULL && ::GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd *pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd *>(lpcs->lpCreateParams);
        pThis->m_hWnd = hWnd;
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
    } else if (uMsg == WM_DPICHANGED) {
        LOGGER_DEBUG("DUILIB WM_DPICHANGED proc wnd " << hWnd);
        if (CPaintManagerUI::IsMainThread() == false) {
            LOGGER_DEBUG("DUILIB WM_DPICHANGED NOTIFY outside ui thread, wnd %0x", hWnd);
        } else {
            pThis = reinterpret_cast<CWindowWnd *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
            CPaintManagerUI::UpdateDPIByMonitor(hWnd);
        }
    } else {
        pThis = reinterpret_cast<CWindowWnd *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (uMsg == WM_NCDESTROY && pThis != nullptr) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            if (pThis->m_hWnd != nullptr) {
                ::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
            }
            if (pThis->m_bSubclassed) {
                pThis->Unsubclass();
            }
            pThis->m_hWnd = nullptr;
            pThis->OnFinalMessage(hWnd);
            LOGGER_DEBUG("DUILIB OnFinalMessage pthis:" << pThis << ", hWnd:" << hWnd);
            return lRes;
        }
    }
    if (pThis != nullptr) {
        _try
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        _except(EXCEPTION_EXECUTE_HANDLER)
        {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    } else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK CWindowWnd::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowWnd *pThis = nullptr;
    if (uMsg == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = static_cast<CWindowWnd *>(lpcs->lpCreateParams);
        ::SetProp(hWnd, _T("WndX"), (HANDLE)pThis);
        pThis->m_hWnd = hWnd;
    } else {
        pThis = reinterpret_cast<CWindowWnd *>(::GetProp(hWnd, _T("WndX")));
        if (uMsg == WM_NCDESTROY && pThis != nullptr) {
            LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
            if (pThis->m_bSubclassed) {
                pThis->Unsubclass();
            }
            ::SetProp(hWnd, _T("WndX"), nullptr);
            pThis->m_hWnd = nullptr;
            pThis->OnFinalMessage(hWnd);
            return lRes;
        }
    }
    if (pThis != nullptr) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    } else {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */)
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return FALSE;
    }
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */)
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return FALSE;
    }
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::ResizeClient(int cx                  /* = -1 */, int cy /* = -1 */)
{
    if (m_hWnd == nullptr || ::IsWindow(m_hWnd) == FALSE) {
        return;
    }
    RECT rc = { 0 };
    if (!::GetClientRect(m_hWnd, &rc)) {
        return;
    }
    if (cx != -1) {
        rc.right = cx;
    }
    if (cy != -1) {
        rc.bottom = cy;
    }
    if (!::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd),
        (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != nullptr)), GetWindowExStyle(m_hWnd))) {
        return;
    }
    ::SetWindowPos(m_hWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_EXITSIZEMOVE) {
        DuiLib::UIMainQueue::getInstance().dispatch([=]() {});
    }

    return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::OnFinalMessage(HWND) {}


void CWindowWnd::Center2ParentWindow()
{
    if (::IsWindow(m_hWnd) == false) {
        return;
    }

    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);

    HWND hWndParent = ::GetParent(m_hWnd);
    if (hWndParent == nullptr) {
        return;
    }

    RECT rcDlgParent = { 0 };
    ::GetWindowRect(hWndParent, &rcDlgParent);

    int mainDlgWidth = rcDlgParent.right - rcDlgParent.left;
    int mainDlgHeight = rcDlgParent.bottom - rcDlgParent.top;

    int dlgWidth = rcDlg.right - rcDlg.left;
    int dlgHeight = rcDlg.bottom - rcDlg.top;

    int xLeft = mainDlgWidth / 2 - dlgWidth / 2; // 取宽度一半
    int yTop = mainDlgHeight / 2 - dlgHeight / 2; // 取高度一半

    ::MoveWindow(m_hWnd, xLeft, yTop, dlgWidth, dlgHeight, true);
}

void CWindowWnd::SetMsgLoop(bool bMsgLoop)
{
    m_bMsgLoop = bMsgLoop;
}
} // namespace DuiLib
