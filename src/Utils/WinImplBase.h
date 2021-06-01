/*
 * 2021.03.27-Function enhancements.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

#include "../UIlib.h"
#include "../Utils/VariantAnimation.h"

namespace DuiLib {
class WindowImplBaseHelper;
class UILIB_API WindowImplBase : public CWindowWnd,
    public CNotifyPump,
    public INotifyUI,
    public IMessageFilterUI,
    public IDialogBuilderCallback,
    public IQueryControlText {
    DUI_DECLARE_MESSAGE_MAP()
public:
    explicit WindowImplBase();
    explicit WindowImplBase(bool exitWhenXmlMiss);
    virtual ~WindowImplBase();
    // 只需主窗口重写（初始化资源与多语言接口）
    virtual void InitResource() {};
    // 每个窗口都可以重写
    virtual void InitWindow() {};
    virtual void OnFinalMessage(HWND hWnd);
    void Notify(TNotifyUI &msg) override;
    CPaintManagerUI &GetPaintManagerUI()
    {
        return m_pm;
    }
    void CenterToControl(DuiLib::CControlUI *ctrl);
    virtual void OnClick(TNotifyUI &msg);
    void CenterToParentWindow();
    CPaintManagerUI m_pm;
protected:
    virtual CDuiString GetSkinType()
    {
        return _T("");
    }
    virtual CDuiString GetSkinFile() = 0;
    virtual LPCTSTR GetWindowClassName(void) const = 0;
    virtual LPCTSTR GetManagerName()
    {
        return nullptr;
    }
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    WindowImplBaseHelper *helper = nullptr;

public:
    void Move(int x, int y);
    bool AnimateMove(int x, int y, VariantAnimation::EasingCurve easingCurve = VariantAnimation::Linear,
        int timerElapse = 500);
    enum AnimateResizePolicy { FixTopLeft, FixTopRight, FixBottomLeft, FixBottomRight, FixCenter };
    void Resize(int w, int h, AnimateResizePolicy keepInScreen = FixTopLeft);
    bool AnimateResize(int w, int h, VariantAnimation::EasingCurve easingCurve = VariantAnimation::Linear,
        int timerElapse = 500, AnimateResizePolicy keepInScreen = FixTopLeft);
    RECT GetRect() const;
    void EnsureVisible();
    void BringToForeground();
    virtual UINT GetClassStyle() const;
    virtual CControlUI *CreateControl(LPCTSTR pstrClass);
    virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType);

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM, bool &);
    virtual LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL &bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
    virtual LRESULT OnNcActivate(UINT, WPARAM wParam, LPARAM, BOOL &bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcPaint(UINT, WPARAM, LPARAM, BOOL &);
    virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcPreHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam,
        BOOL &bHandled); // added by fanzhenyin to reduce complexity
    virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnMouseWheel(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnInput(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
#endif
    enum class ActivateState { Active, ClickActive, InActive };
    enum class MinimizedState { Minimized, NotMinimized };
    virtual void OnActivate(ActivateState activeState, MinimizedState minimizedState) {}
    virtual void OnDisplayResolutionChanged(UINT width, UINT height) {}
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnKeyDown(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnKillFocus(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnSetFocus(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnLButtonDown(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnLButtonUp(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT OnMouseMove(UINT, WPARAM, LPARAM, BOOL &bHandled);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual bool MessageSysTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, BOOL &bHandled);
    virtual bool MessageDeviceTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, BOOL &bHandled);
    virtual bool MessageNormalTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, BOOL &bHandled);
    virtual LONG GetStyle();
    virtual void OnResize(const SIZE &sz) {}
    virtual void OnResizeAnimationEnd() {}
    virtual void OnMoveAnimationEnd() {}
};
}

#endif // WIN_IMPL_BASE_HPP
