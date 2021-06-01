/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "VariantAnimation.h"

namespace DuiLib {
class MoveAnimationImpl : public IAnimationCallback {
public:
    explicit MoveAnimationImpl(WindowImplBase *window) : animation()
    {
        this->window = window;
        if (!window) {
            return;
        }

        animation = std::make_unique<VariantAnimation>(window->GetPaintManagerUI().GetRoot());
        animation->SetCallback(this);
    }

    ~MoveAnimationImpl() {}

    bool Start(int x, int y, VariantAnimation::EasingCurve easingCurve, int timerElapse)
    {
        if (!animation || this->window == nullptr) {
            return false;
        }

        if (animation->IsRunning()) {
            animation->Stop();
            baseMoveX = currentMoveX;
            baseMoveY = currentMoveY;
        } else {
            RECT rc = window->GetRect();
            baseMoveX = rc.left;
            baseMoveY = rc.top;
        }

        destMoveX = x;
        destMoveY = y;
        animation->SetDuration(timerElapse);
        animation->SetLoopCount(1);
        animation->SetEasingCurve(easingCurve);
        animation->SetStartValue(DuiLib::DoubleVariant(0));
        animation->SetEndValue(DuiLib::DoubleVariant(1));
        animation->Start();

        window->ShowWindow();

        return true;
    }

    void Stop()
    {
        if (animation) {
            animation->Stop();
        }
    }

    bool IsRunning() const
    {
        if (!animation) {
            return false;
        }

        return animation->IsRunning();
    }

    void OnValueChanged(const DuiLib::Variable &value) override
    {
        if (!window) {
            return;
        }

        const DuiLib::DoubleVariant &val = dynamic_cast<const DuiLib::DoubleVariant &>(value);
        double moveRatio = val.value;
        int dx = (destMoveX - baseMoveX) * moveRatio;
        int dy = (destMoveY - baseMoveY) * moveRatio;
        currentMoveX = baseMoveX + dx;
        currentMoveY = baseMoveY + dy;

        ::SendMessage(window->GetPaintManagerUI().GetPaintWindow(), WM_PAINT, 0, 0);

        ::SetWindowPos(window->GetPaintManagerUI().GetPaintWindow(), nullptr, currentMoveX, currentMoveY, -1, -1,
            SWP_NOZORDER | SWP_NOSIZE);
    }

    void OnAnimationEnd() override
    {
        if (!window) {
            return;
        }

        ::SetWindowPos(window->GetPaintManagerUI().GetPaintWindow(), nullptr, destMoveX, destMoveY, -1, -1,
            SWP_NOZORDER | SWP_NOSIZE);
        window->OnMoveAnimationEnd();
    }

private:
    std::unique_ptr<VariantAnimation> animation;
    int baseMoveX = 0;
    int baseMoveY = 0;
    int destMoveX = 0;
    int destMoveY = 0;
    int currentMoveX = 0;
    int currentMoveY = 0;
    WindowImplBase *window = nullptr;
};

class ResizeAnimationImpl : public IAnimationCallback {
public:
    explicit ResizeAnimationImpl(WindowImplBase *window) : animation()
    {
        this->window = window;
        if (!window) {
            return;
        }

        animation = std::make_unique<VariantAnimation>(window->GetPaintManagerUI().GetRoot());
        animation->SetCallback(this);
    }

    ~ResizeAnimationImpl() {}

    bool Start(int width, int height, VariantAnimation::EasingCurve easingCurve, int timerElapse,
        WindowImplBase::AnimateResizePolicy keepInScreen)
    {
        if (!window || !animation) {
            return false;
        }

        if (animation->IsRunning()) {
            animation->Stop();
        }

        this->keepInScreen = keepInScreen;

        RECT rcWindow = window->GetRect();
        baseX = rcWindow.left;
        baseY = rcWindow.top;
        baseWidth = rcWindow.right - rcWindow.left;
        baseHeight = rcWindow.bottom - rcWindow.top;
        destWidth = width;
        destHeight = height;

        animation->SetDuration(timerElapse);
        animation->SetEasingCurve(easingCurve);
        animation->SetLoopCount(1);
        animation->SetLoopBack(false);
        animation->SetStartValue(DuiLib::DoubleVariant(0));
        animation->SetEndValue(DuiLib::DoubleVariant(1));
        return animation->Start();
    }

    void OnValueChanged(const DuiLib::Variable &value) override
    {
        if (!window) {
            return;
        }

        const DuiLib::DoubleVariant &val = dynamic_cast<const DuiLib::DoubleVariant &>(value);
        double resizeRatio = val.value;
        int dx = (destWidth - baseWidth) * resizeRatio;
        int dy = (destHeight - baseHeight) * resizeRatio;
        int resultWidth = baseWidth + dx;
        int resultHeight = baseHeight + dy;

        window->Resize(baseWidth + dx, baseHeight + dy, keepInScreen);
    }

    void OnAnimationEnd() override
    {
        if (!window) {
            return;
        }

        window->Resize(destWidth, destHeight, keepInScreen);
        window->OnResizeAnimationEnd();
    }

private:
    std::unique_ptr<VariantAnimation> animation;
    WindowImplBase *window = nullptr;
    int baseX = 0;
    int baseY = 0;
    int baseWidth = 0;
    int baseHeight = 0;
    int destWidth = 0;
    int destHeight = 0;
    WindowImplBase::AnimateResizePolicy keepInScreen = WindowImplBase::FixTopLeft;
};

class WindowImplBaseHelper {
public:
    bool exitWhenXmlMiss = false;
    WindowImplBase *window = nullptr;
    MoveAnimationImpl *moveAnimation = nullptr;
    ResizeAnimationImpl *resizeAnimation = nullptr;

    bool AnimateResize(int width, int height, VariantAnimation::EasingCurve easingCurve, int timerElapse,
        WindowImplBase::AnimateResizePolicy keepInScreen)
    {
        if (!window || !resizeAnimation) {
            return false;
        }

        return resizeAnimation->Start(width, height, easingCurve, timerElapse, keepInScreen);
    }

    bool AnimateMove(int x, int y, VariantAnimation::EasingCurve easingCurve, int timerElapse)
    {
        if (!window || !moveAnimation) {
            return false;
        }

        return moveAnimation->Start(x, y, easingCurve, timerElapse);
    }
};


WindowImplBase::WindowImplBase(bool exitWhenXmlMiss) : helper(new WindowImplBaseHelper)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->exitWhenXmlMiss = exitWhenXmlMiss;
    helper->window = this;
}

WindowImplBase::WindowImplBase() : helper(new WindowImplBaseHelper)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->window = this;
}

WindowImplBase::~WindowImplBase()
{
    if (this->helper == nullptr) {
        return;
    }
    if (helper->moveAnimation) {
        delete helper->moveAnimation;
        helper->moveAnimation = nullptr;
    }

    if (helper->resizeAnimation) {
        delete helper->resizeAnimation;
        helper->resizeAnimation = nullptr;
    }


    if (helper != nullptr) {
        delete helper;
        helper = nullptr;
    }
}

DUI_BEGIN_MESSAGE_MAP(WindowImplBase, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

void WindowImplBase::OnFinalMessage(HWND hWnd)
{
    m_pm.RemovePreMessageFilter(this);
    m_pm.RemoveNotifier(this);
    m_pm.ReapObjects(m_pm.GetRoot());
}

LRESULT WindowImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN) {
        return FALSE;
    } else if (wParam == VK_ESCAPE) {
        return TRUE;
    }

    return FALSE;
}

void WindowImplBase::Resize(int w, int h, AnimateResizePolicy keepInScreen)
{
    RECT rcWindow = GetRect();
    int baseX = rcWindow.left;
    int baseY = rcWindow.top;
    int baseWidth = rcWindow.right - rcWindow.left;
    int baseHeight = rcWindow.bottom - rcWindow.top;
    const int posSize = 2;

    switch (keepInScreen) {
        case DuiLib::WindowImplBase::FixTopLeft: { // Default
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
            break;
        }
        case DuiLib::WindowImplBase::FixTopRight: {
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, baseX + baseWidth - w, baseY, w, h,
                SWP_NOZORDER);
            break;
        }
        case DuiLib::WindowImplBase::FixBottomLeft: {
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, baseX, baseY + baseHeight - h, w, h,
                SWP_NOZORDER);
            break;
        }
        case DuiLib::WindowImplBase::FixBottomRight: {
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, baseX + baseWidth - w,
                baseY + baseHeight - h, w, h, SWP_NOZORDER);
            break;
        }
        case DuiLib::WindowImplBase::FixCenter: {
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, baseX + (baseWidth - w) / posSize,
                baseY + (baseHeight - h) / posSize, w, h, SWP_NOZORDER);
            break;
        }
        default: {
            ::SetWindowPos(GetPaintManagerUI().GetPaintWindow(), nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
            break;
        }
    }
}

void WindowImplBase::Move(int x, int y)
{
    if (helper == nullptr) {
        return;
    }
    if (helper->moveAnimation && helper->moveAnimation->IsRunning()) {
        helper->moveAnimation->Stop();
    }

    ::SendMessage(GetPaintManagerUI().GetPaintWindow(), WM_PAINT, 0, 0);
    ::SetWindowPos(m_pm.GetPaintWindow(), nullptr, x, y, -1, -1, SWP_NOZORDER | SWP_NOSIZE);
}

bool WindowImplBase::AnimateMove(int x, int y, VariantAnimation::EasingCurve easingCurve, int timerElapse)
{
    if (helper == nullptr) {
        return false;
    }
    if (!helper->AnimateMove(x, y, easingCurve, timerElapse)) {
        Move(x, y);
        return false;
    }

    return true;
}

bool WindowImplBase::AnimateResize(int w, int h, VariantAnimation::EasingCurve easingCurve,
    int timerElapse, AnimateResizePolicy keepInScreen)
{
    if (helper == nullptr) {
        return false;
    }
    if (!helper->AnimateResize(w, h, easingCurve, timerElapse, keepInScreen)) {
        Resize(w, h);
        return false;
    }

    return true;
}

RECT WindowImplBase::GetRect() const
{
    RECT rc;
    SecureZeroMemory(&rc, sizeof(rc));
    ::GetWindowRect(m_hWnd, &rc);
    return rc;
}

void WindowImplBase::EnsureVisible()
{
    if (IsWindowVisible(GetHWND()) == FALSE) {
        return;
    }
    RECT _rcWindow;
    SecureZeroMemory(&_rcWindow, sizeof(_rcWindow));
    ::GetWindowRect(GetHWND(), &_rcWindow);

    RECT rcWorkArea;
    SecureZeroMemory(&rcWorkArea, sizeof(rcWorkArea));
    if (!SystemParametersInfo(SPI_GETWORKAREA, 0, (PVOID)&rcWorkArea, 0)) {
        return;
    }

    int windowX = _rcWindow.left;
    int windowY = _rcWindow.top;
    int actualWidth = _rcWindow.right - _rcWindow.left;

    if (windowX + actualWidth > rcWorkArea.right) {
        windowX = rcWorkArea.right - actualWidth;
    }

    if (windowY + actualWidth > rcWorkArea.bottom) {
        windowY = rcWorkArea.bottom - actualWidth;
    }

    if (windowX < 0) {
        windowX = 0;
    }

    if (windowY < 0) {
        windowY = 0;
    }

    Move(windowX, windowY);
}

void WindowImplBase::BringToForeground()
{
    ::SetForegroundWindow(GetHWND());
}

UINT WindowImplBase::GetClassStyle() const
{
    return CS_DBLCLKS;
}

CControlUI *WindowImplBase::CreateControl(LPCTSTR pstrClass)
{
    return nullptr;
}

LPCTSTR WindowImplBase::QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType)
{
    return nullptr;
}

LRESULT WindowImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM, bool &)
{
    if (uMsg == WM_KEYDOWN) {
        switch (wParam) {
            case VK_RETURN:
            case VK_ESCAPE:
                return ResponseDefaultKeyEvent(wParam);
            default:
                break;
        }
    }
    return FALSE;
}

LRESULT WindowImplBase::OnClose(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnDestroy(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)
LRESULT WindowImplBase::OnNcActivate(UINT, WPARAM wParam, LPARAM, BOOL &bHandled)
{
    if (::IsIconic(*this)) {
        bHandled = FALSE;
    }
    return (wParam == 0) ? TRUE : FALSE;
}

LRESULT WindowImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    return 0;
}

LRESULT WindowImplBase::OnNcPaint(UINT, WPARAM, LPARAM, BOOL &)
{
    return 0;
}
LRESULT WindowImplBase::OnNcPreHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    SecureZeroMemory(&rcClient, sizeof(rcClient));
    ::GetClientRect(*this, &rcClient);
    if (!::IsZoomed(*this)) {
        RECT rcSizeBox = m_pm.GetSizeBox();
        if (pt.y < rcClient.top + rcSizeBox.top) {
            if (pt.x < rcClient.left + rcSizeBox.left) {
                return HTTOPLEFT;
            }
            if (pt.x > rcClient.right - rcSizeBox.right) {
                return HTTOPRIGHT;
            }
            return HTTOP;
        } else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
            if (pt.x < rcClient.left + rcSizeBox.left) {
                return HTBOTTOMLEFT;
            }
            if (pt.x > rcClient.right - rcSizeBox.right) {
                return HTBOTTOMRIGHT;
            }
            return HTBOTTOM;
        }

        if (pt.x < rcClient.left + rcSizeBox.left) {
            return HTLEFT;
        }
        if (pt.x > rcClient.right - rcSizeBox.right) {
            return HTRIGHT;
        }
    }

    return 0;
}
LRESULT WindowImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    SecureZeroMemory(&rcClient, sizeof(rcClient));
    ::GetClientRect(*this, &rcClient);
    LRESULT Res = OnNcPreHitTest(uMsg, wParam, lParam, bHandled);
    if (Res) {
        return Res;
    }
    bool IsNcHit = true;
    CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(pt));
    RECT rcCaption = m_pm.GetCaptionRect();

    bool isOldControlUI = false;

    if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right &&
        pt.y >= rcCaption.top && pt.y < rcCaption.bottom && pControl && pControl->GetClass()) {
        CDuiString ControlName = pControl->GetClass();
        isOldControlUI = pControl && _tcsicmp(ControlName, _T("ButtonUI")) != 0 &&
            _tcsicmp(ControlName, _T("FadeButtonUI")) != 0 && _tcsicmp(ControlName, _T("OptionUI")) != 0 &&
            _tcsicmp(ControlName, _T("BrowserTab")) != 0 && _tcsicmp(ControlName, _T("BrowserTabBar")) != 0 &&
            _tcsicmp(ControlName, _T("EditUI")) != 0 && _tcsicmp(ControlName, _T("TextUI")) != 0 &&
            _tcsicmp(ControlName, _T("SliderUI")) != 0;
    }

    if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right &&
        pt.y >= rcCaption.top && pt.y < rcCaption.bottom && pControl) {
        if (isOldControlUI) {
            // *****************added  to solve maximize problem***************//
            auto styleValue = static_cast<unsigned long>(GetStyle());
            styleValue &= ~WS_THICKFRAME;
            ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            // ****************************************************************//
            return HTCAPTION;
        }
    }
    return HTCLIENT;
}

LRESULT WindowImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    MONITORINFO Monitor = {};
    Monitor.cbSize = sizeof(Monitor);
    ::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &Monitor);
    RECT rcWork = Monitor.rcWork;
    if (Monitor.dwFlags != MONITORINFOF_PRIMARY) {
        ::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
    }

    LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    lpMMI->ptMaxPosition.x = rcWork.left;
    lpMMI->ptMaxPosition.y = rcWork.top;
    lpMMI->ptMaxSize.x = rcWork.right - rcWork.left;
    lpMMI->ptMaxSize.y = rcWork.bottom - rcWork.top;
    lpMMI->ptMinTrackSize.x = m_pm.GetMinInfo().cx;
    lpMMI->ptMinTrackSize.y = m_pm.GetMinInfo().cy;

    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnMouseWheel(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnInput(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}
#endif

LRESULT WindowImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
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
#endif

    CDuiRect rcWnd;
    ::GetWindowRect(*this, &rcWnd);
    OnResize(SIZE { rcWnd.GetWidth(), rcWnd.GetHeight() });
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (wParam == SC_CLOSE) {
        bHandled = TRUE;
        SendMessage(WM_CLOSE, TOPWINDOW_CLOSED);
        return 0;
    }
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(*this);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(*this) != bZoomed) {
        if (!bZoomed) {
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(_T("maxbtn")));
            if (pControl) {
                pControl->SetVisible(false);
            }
            pControl = static_cast<CControlUI *>(m_pm.FindControl(_T("restorebtn")));
            if (pControl) {
                pControl->SetVisible(true);
            }
        } else {
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(_T("maxbtn")));
            if (pControl) {
                pControl->SetVisible(true);
            }
            pControl = static_cast<CControlUI *>(m_pm.FindControl(_T("restorebtn")));
            if (pControl) {
                pControl->SetVisible(false);
            }
        }
    }
#else
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
    return lRes;
}

LRESULT WindowImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    // 调整窗口样式
    auto styleValue = static_cast<unsigned long>(::GetWindowLong(*this, GWL_STYLE));
    styleValue &= ~WS_CAPTION;
    ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    // 调整窗口尺寸
    RECT rcClient;
    SecureZeroMemory(&rcClient, sizeof(rcClient));
    ::GetClientRect(*this, &rcClient);
    ::SetWindowPos(*this, nullptr, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top, SWP_FRAMECHANGED | SWP_NOMOVE);

    // 关联UI管理器
    m_pm.Init(m_hWnd, GetManagerName());
    // 注册PreMessage回调
    m_pm.AddPreMessageFilter(this);

    // 创建主窗口
    CControlUI *pRoot = nullptr;
    CDialogBuilder builder;
    CDuiString sSkinType = GetSkinType();
    if (!sSkinType.IsEmpty()) {
        STRINGorID xml(_ttoi(GetSkinFile().GetData()));
        pRoot = builder.Create(xml, sSkinType, this, &m_pm);
    } else {
        pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_pm);
    }

    if (helper == nullptr) {
        return 0;
    }

    if (pRoot == nullptr) {
        CDuiString sError = _T("加载资源文件失败：");
        sError += GetSkinFile();
        MessageBox(nullptr, sError, _T("Duilib"), MB_OK | MB_ICONERROR);
        if (helper->exitWhenXmlMiss) {
            ExitProcess(1);
        }
        return 0;
    }
    m_pm.AttachDialog(pRoot);
    // 添加Notify事件接口
    m_pm.AddNotifier(this);
    // 窗口初始化完毕
    InitWindow();

    // 初始化两个动画窗口对象。
    if (!helper->moveAnimation) {
        helper->moveAnimation = new MoveAnimationImpl(this);
    }

    if (!helper->resizeAnimation) {
        helper->resizeAnimation = new ResizeAnimationImpl(this);
    }

    return 0;
}

LRESULT WindowImplBase::OnKeyDown(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnKillFocus(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnSetFocus(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnLButtonDown(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnLButtonUp(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::OnMouseMove(UINT, WPARAM, LPARAM, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    bool bSysEventFinde = MessageSysTreat(uMsg, wParam, lParam, lRes, bHandled);
    bool bDeviceEventFinde = MessageDeviceTreat(uMsg, wParam, lParam, lRes, bHandled);
    bool bNormalSysEventFinde = MessageNormalTreat(uMsg, wParam, lParam, lRes, bHandled);

    if (!bSysEventFinde && !bDeviceEventFinde && !bNormalSysEventFinde) {
        bHandled = FALSE;
    }

    if (bHandled) {
        return lRes;
    }

    lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
    if (bHandled) {
        return lRes;
    }

    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) {
        return lRes;
    }
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

bool WindowImplBase::MessageSysTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, BOOL &bHandled)
{
    bool bEventFindTag = true;
    switch (uMsg) {
        case WM_ACTIVATE: {
            ActivateState as = ActivateState::Active;
            switch (LOWORD(wParam)) {
                case WA_ACTIVE: {
                    as = ActivateState::Active;
                    break;
                }
                case WA_CLICKACTIVE: {
                    as = ActivateState::ClickActive;
                    break;
                }
                case WA_INACTIVE: {
                    as = ActivateState::InActive;
                    break;
                }
                default:
                    break;
            }
            MinimizedState ms = MinimizedState::NotMinimized;
            if (HIWORD(wParam) > 0) {
                ms = MinimizedState::Minimized;
            }
            OnActivate(as, ms);
        } break;
        case WM_DISPLAYCHANGE: {
            OnDisplayResolutionChanged(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_CREATE: {
            lRes = OnCreate(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_CLOSE: {
            lRes = OnClose(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_DESTROY: {
            lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_SIZE: {
            lRes = OnSize(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_CHAR: {
            lRes = OnChar(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_SYSCOMMAND: {
            lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_KEYDOWN: {
            lRes = OnKeyDown(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_KILLFOCUS: {
            lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_SETFOCUS: {
            lRes = OnSetFocus(uMsg, wParam, lParam, bHandled);
            break;
        }
        default: {
            bEventFindTag = false;
            break;
        }
    }
    return bEventFindTag;
}


bool WindowImplBase::MessageDeviceTreat(UINT uMsg, WPARAM wParam, LPARAM lParam,
    LRESULT &lRes, BOOL &bHandled)
{
    bool bEventFindTag = true;
    switch (uMsg) {
        case WM_LBUTTONUP: {
            lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_LBUTTONDOWN: {
            lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_MOUSEMOVE: {
            lRes = OnMouseMove(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_MOUSEHOVER: {
            lRes = OnMouseHover(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_MOUSELEAVE: {
            lRes = OnMouseLeave(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_INPUT: {
            lRes = OnInput(uMsg, wParam, lParam, bHandled);
            break;
        }
        default: {
            bEventFindTag = false;
            break;
        }
    }
    return bEventFindTag;
}

bool WindowImplBase::MessageNormalTreat(UINT uMsg, WPARAM wParam, LPARAM lParam,
    LRESULT &lRes, BOOL &bHandled)
{
    bool bEventFindTag = true;
    switch (uMsg) {
#if defined(WIN32) && !defined(UNDER_CE)
        case WM_NCACTIVATE: {
            lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_NCCALCSIZE: {
            lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_NCPAINT: {
            lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_NCHITTEST: {
            lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_GETMINMAXINFO: {
            lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
            break;
        }
        case WM_MOUSEWHEEL: {
            lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled);
            break;
        }
#endif
        default: {
            bEventFindTag = false;
            break;
        }
    }
    return bEventFindTag;
}

LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LONG WindowImplBase::GetStyle()
{
    auto value = static_cast<unsigned long>(::GetWindowLong(*this, GWL_STYLE));
    value &= ~WS_CAPTION;

    return static_cast<LONG>(value);
}

void WindowImplBase::OnClick(TNotifyUI &msg)
{
    if (msg.pSender == nullptr) {
        return;
    }
    CDuiString sCtrlName = msg.pSender->GetName();
    if (sCtrlName == _T("closebtn")) {
        Close();
        return;
    } else if (sCtrlName == _T("minbtn")) {
        SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return;
    } else if (sCtrlName == _T("maxbtn")) {
        SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        return;
    } else if (sCtrlName == _T("restorebtn")) {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
        return;
    }
    return;
}

void WindowImplBase::CenterToControl(DuiLib::CControlUI *ctrl)
{
    if (!ctrl || !ctrl->GetManager()) {
        return;
    }

    RECT rcControl = ctrl->GetPos();
    RECT rcWindow = { 0 };
    GetWindowRect(ctrl->GetManager()->GetPaintWindow(), &rcWindow);
    int left = rcWindow.left + rcControl.left;
    int top = rcWindow.top + rcControl.top;
    int ctrlWidth = rcControl.right - rcControl.left;
    int ctrlHeight = rcControl.bottom - rcControl.top;

    RECT rcDialog = { 0 };
    GetWindowRect(GetHWND(), &rcDialog);
    int width = rcDialog.right - rcDialog.left;
    int height = rcDialog.bottom - rcDialog.top;
    const int moveSize = 2;
    Move(left + (ctrlWidth - width) / moveSize, top + (ctrlHeight - height) / moveSize);
}

void WindowImplBase::Notify(TNotifyUI &msg)
{
    return CNotifyPump::NotifyPump(msg);
}

void WindowImplBase::CenterToParentWindow()
{
    HWND mainHwnd = GetParent(m_hWnd);
    RECT dlgMainRect = { 0 };
    ::GetWindowRect(mainHwnd, &dlgMainRect);
    ::SetWindowPos(GetHWND(), nullptr, dlgMainRect.left, dlgMainRect.top, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
}