/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIButton.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CButtonUI)

CButtonUI::CButtonUI()
    : m_uButtonState(0),
      m_dwHotTextColor(0),
      m_dwPushedTextColor(0),
      m_dwFocusedTextColor(0),
      m_dwNormalBkColor(0),
      m_dwHotBkColor(0),
      m_dwPushedBkColor(0),
      m_iBindTabIndex(-1),
      m_nHoverLink(0),
      m_iLinkIndex{ 0 },
      m_rcLinks{ 0 }
{
    m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
}

CButtonUI::~CButtonUI() {}

LPCTSTR CButtonUI::GetClass() const
{
    return _T("ButtonUI");
}

LPVOID CButtonUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_BUTTON) == 0) {
        return static_cast<CButtonUI *>(this);
    }
    return CLabelUI::GetInterface(pstrName);
}

UINT CButtonUI::GetControlFlags() const
{
    return (UINT)((IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) |
        (IsEnabled() ? UIFLAG_SETCURSOR : 0));
}

void CButtonUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pParent != nullptr) {
            m_pParent->DoEvent(event);
        } else {
            CLabelUI::DoEvent(event);
        }
        return;
    }
    if (event.Type == UIEVENT_SETFOCUS) {
        Invalidate();
    }
    if (event.Type == UIEVENT_KILLFOCUS) {
        Invalidate();
    }
    if (event.Type == UIEVENT_KEYDOWN) {
        if (IsKeyboardEnabled()) {
            if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
                Activate();
                return;
            }
        }
    }
    if (m_pManager == nullptr) {
        return;
    }
    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled()) {
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        }
        if (event.Type == UIEVENT_BUTTONDOWN) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_BUTTONDOWN, event.wParam, event.lParam);
        }
        if (IsEnabled()) {
            for (int i = 0; i < m_nLinks && i < MAX_LINK; i++) {
                if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
                    m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                    return;
                }
            }
        }
        return;
    }
    if (event.Type == UIEVENT_MOUSEHOVER) {
        if (IsEnabled() && ::PtInRect(&m_rcItem, event.ptMouse) && (m_uButtonState & UISTATE_HOT) == 0) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if (event.Type == UIEVENT_MOUSEMOVE) {
        if (IsEnabled() && ::PtInRect(&m_rcItem, event.ptMouse) && (m_uButtonState & UISTATE_HOT) == 0) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
            if (::PtInRect(&m_rcItem, event.ptMouse))
                m_uButtonState |= UISTATE_PUSHED;
            else
                m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }
        return;
    }
    if (event.Type == UIEVENT_BUTTONUP) {
        if ((m_uButtonState & UISTATE_CAPTURED) != 0) {
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
            if (::PtInRect(&m_rcItem, event.ptMouse))
                Activate();
        }
        m_pManager->SendNotify(this, DUI_MSGTYPE_LBTN_UP, event.wParam, event.lParam);
        return;
    }
    if (event.Type == UIEVENT_CONTEXTMENU) {
        if (IsContextMenuUsed()) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
        }
        return;
    }
    if (event.Type == UIEVENT_MOUSEENTER) {
        if (IsEnabled()) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
            // add event by fjj
            m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
        }
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        if (IsEnabled()) {
            m_uButtonState &= ~UISTATE_HOT;
            m_uButtonState &= ~UISTATE_PUSHED;
            m_uButtonState &= ~UISTATE_CAPTURED;
            // end modify by yangzhipeng
            Invalidate();
            // add event by fjj
            m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE);
        }
    }
    if (event.Type == UIEVENT_SETCURSOR) {
        ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(IDC_HAND)));
        return;
    }
    CLabelUI::DoEvent(event);
}

bool CButtonUI::Activate()
{
    if (!CControlUI::Activate()) {
        return false;
    }
    if (m_pManager != nullptr) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
        BindTriggerTabSel();
    }
    return true;
}

void CButtonUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);
    if (!IsEnabled()) {
        m_uButtonState = 0;
    }
}

void CButtonUI::SetNormalBkColor(DWORD dwColor)
{
    m_dwNormalBkColor = dwColor;
}

void CButtonUI::SetHotBkColor(DWORD dwColor)
{
    m_dwHotBkColor = dwColor;
}

DWORD CButtonUI::GetHotBkColor() const
{
    return m_dwHotBkColor;
}

void CButtonUI::SetPushedBkColor(DWORD dwColor)
{
    m_dwPushedBkColor = dwColor;
}

DWORD CButtonUI::GetPushedBkColor() const
{
    return m_dwPushedBkColor;
}

void CButtonUI::SetHotTextColor(DWORD dwColor)
{
    m_dwHotTextColor = dwColor;
}

DWORD CButtonUI::GetHotTextColor() const
{
    return m_dwHotTextColor;
}

void CButtonUI::SetPushedTextColor(DWORD dwColor)
{
    m_dwPushedTextColor = dwColor;
}

DWORD CButtonUI::GetPushedTextColor() const
{
    return m_dwPushedTextColor;
}

void CButtonUI::SetFocusedTextColor(DWORD dwColor)
{
    m_dwFocusedTextColor = dwColor;
}

DWORD CButtonUI::GetFocusedTextColor() const
{
    return m_dwFocusedTextColor;
}

LPCTSTR CButtonUI::GetNormalImage()
{
    return m_sNormalImage;
}

void CButtonUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetHotImage()
{
    return m_sHotImage;
}

void CButtonUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetPushedImage()
{
    return m_sPushedImage;
}

void CButtonUI::SetPushedImage(LPCTSTR pStrImage)
{
    m_sPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetFocusedImage()
{
    return m_sFocusedImage;
}

void CButtonUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetDisabledImage()
{
    return m_sDisabledImage;
}

void CButtonUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

LPCTSTR CButtonUI::GetHotForeImage()
{
    return m_sHotForeImage;
}

void CButtonUI::SetHotForeImage(LPCTSTR pStrImage)
{
    m_sHotForeImage = pStrImage;
    Invalidate();
}

void CButtonUI::BindTabIndex(int _BindTabIndex)
{
    if (_BindTabIndex >= 0) {
        m_iBindTabIndex = _BindTabIndex;
    }
}

void CButtonUI::BindTabLayoutName(LPCTSTR _TabLayoutName)
{
    if (_TabLayoutName) {
        m_sBindTabLayoutName = _TabLayoutName;
    }
}

void CButtonUI::BindTriggerTabSel(int _SetSelectIndex)
{
    LPCTSTR pstrName = GetBindTabLayoutName();
    if (pstrName == nullptr || (GetBindTabLayoutIndex() < 0 && _SetSelectIndex < 0)) {
        return;
    }

    CTabLayoutUI *pTabLayout = static_cast<CTabLayoutUI *>(GetManager()->FindControl(pstrName));
    if (pTabLayout == nullptr) {
        return;
    }
    pTabLayout->SelectItem(_SetSelectIndex >= 0 ? _SetSelectIndex : GetBindTabLayoutIndex());
}

void CButtonUI::RemoveBindTabIndex()
{
    m_iBindTabIndex = -1;
    m_sBindTabLayoutName.Empty();
}

int CButtonUI::GetBindTabLayoutIndex()
{
    return m_iBindTabIndex;
}

LPCTSTR CButtonUI::GetBindTabLayoutName()
{
    return m_sBindTabLayoutName;
}

void CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    const int radix = 16;
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("normalimage")) == 0) {
        SetNormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("hotimage")) == 0) {
        SetHotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("pushedimage")) == 0) {
        SetPushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("focusedimage")) == 0) {
        SetFocusedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("disabledimage")) == 0) {
        SetDisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("hotforeimage")) == 0) {
        SetHotForeImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bindtabindex")) == 0) {
        BindTabIndex(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("bindtablayoutname")) == 0) {
        BindTabLayoutName(pstrValue);
    } else if (_tcsicmp(pstrName, _T("hotbkcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetHotBkColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("normalbkcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetNormalBkColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("pushedbkcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetPushedBkColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("hottextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetHotTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("pushedtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetPushedTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("focusedtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetFocusedTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("hovertextshow")) == 0) {
        SetHoverTextShow(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("roundenhance")) == 0) {
            SetRoundEnhance(_tcsicmp(pstrValue, _T("true")) == 0);
    } else {
        CLabelUI::SetAttribute(pstrName, pstrValue);
    }
}

RECT CButtonUI::SubPaintText()
{
    if (m_pManager == nullptr) {
        return { 0 };
    }
    if (IsFocused()) {
        m_uButtonState |= UISTATE_FOCUSED;
    } else {
        m_uButtonState &= ~UISTATE_FOCUSED;
    }
    if (!IsEnabled()) {
        m_uButtonState |= UISTATE_DISABLED;
    } else {
        m_uButtonState &= ~UISTATE_DISABLED;
    }
    if (m_dwTextColor == 0) {
        m_dwTextColor = m_pManager->GetDefaultFontColor();
    }
    if (m_dwDisabledTextColor == 0) {
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    }
    RECT m_rcTextPadding = CButtonUI::m_rcTextPadding;
    CResourceManager::GetInstance()->Scale(&m_rcTextPadding);
    int nLinks = 0;
    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;
    // added by fanzhenyin to support vcenter when using wordbreak in Label and subcontrols
    const int multiples = 2;
    if (m_pManager->IsLayered() == false) {
        int iadd = 0;
        if (m_vcenterForce && m_wordbreak) {
            SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
            CDuiRect rcText = GetTextRect(szAvailable);
            iadd = ((rc.bottom - rc.top) - (rcText.bottom - rcText.top)) / multiples;
            if (iadd < 0) {
                iadd = 0;
            }
        }
        rc.top += iadd;
    }
    return rc;
}
void CButtonUI::PaintText(HDC hDC)
{
    if (GetName() == L"btn_twologin_getauthcode") {
        OutputDebugString(L"");
    }
    CDuiString sText = m_bEnabled ? GetText() : (GetDisableText().GetLength() ? GetDisableText() : GetText());
    if (sText.IsEmpty()) {
        return;
    }
    int nLinks = 0;
    if ((GetHoverTextShow() == false) && (m_uButtonState & UISTATE_HOT) == 0) {
        return;
    }
    RECT rc = SubPaintText();
    DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
    if (((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0)) {
        clrColor = GetPushedTextColor();
    } else if (((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0)) {
        clrColor = GetHotTextColor();
    } else if (((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0)) {
        clrColor = GetFocusedTextColor();
    }
    UINT m_uTextStyle = CButtonUI::m_uTextStyle;
    if (m_bAutoCalcHeight && m_cxyFixed.cx) {
        m_uTextStyle &= ~(DT_SINGLELINE);
        m_uTextStyle = m_uTextStyle | DT_WORDBREAK;
    }
    if (m_bShowHtml) {
        m_nLinks = lengthof(m_rcLinks);
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, clrColor, m_iFont, m_rcLinks, m_sLinks,
            m_nLinks, m_uTextStyle, m_iLinkIndex);
    } else {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, clrColor, m_iFont, m_uTextStyle);
    }
}

void CButtonUI::PaintBkColor(HDC hDC)
{
    int round = CResourceManager::GetInstance()->Scale(m_bkRound);

    if (m_dwNormalBkColor != 0) {
        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwNormalBkColor), round, m_roundEnhance);
    }
    if ((m_uButtonState & UISTATE_PUSHED) != 0) {
        if (m_dwPushedBkColor != 0) {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwPushedBkColor), round, m_roundEnhance);
            return;
        }
    } else if ((m_uButtonState & UISTATE_HOT) != 0) {
        if (m_dwHotBkColor != 0) {
            if (m_pManager) {
                POINT mousePoint = { 0 };
                ::GetCursorPos(&mousePoint);
                ::ScreenToClient(m_pManager->GetPaintWindow(), &mousePoint);
                CControlUI *pHover = m_pManager->FindControl(mousePoint);
                if (pHover != this) {
                    m_uButtonState &= ~UISTATE_HOT;
                    return;
                }
            }

            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor), round, m_roundEnhance);
            return;
        }
    }
    return CControlUI::PaintBkColor(hDC);
}

void CButtonUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) {
        m_uButtonState |= UISTATE_FOCUSED;
    } else {
        m_uButtonState &= ~UISTATE_FOCUSED;
    }
    if (!IsEnabled()) {
        m_uButtonState |= UISTATE_DISABLED;
    } else {
        m_uButtonState &= ~UISTATE_DISABLED;
    }
    if (m_pManager != nullptr && !::IsWindowEnabled(m_pManager->GetPaintWindow())) {
        m_uButtonState &= UISTATE_DISABLED;
    }
    if (m_pManager != nullptr) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
        CControlUI *pControl = m_pManager->FindControl(pt);
        if ((static_cast<CControlUI *>(this)) != pControl) {
            m_uButtonState &= ~UISTATE_HOT;
            m_uButtonState &= ~UISTATE_PUSHED;
            m_uButtonState &= ~UISTATE_FOCUSED;
        }
    }
    if ((m_uButtonState & UISTATE_DISABLED) != 0) {
        if (!m_sDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
        if (!m_sPushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sPushedImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_HOT) != 0) {
        if (!m_sHotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
        if (!m_sFocusedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) {
            } else {
                return;
            }
        }
    }
    if (!m_sNormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) {
        }
    }
}

void CButtonUI::PaintForeImage(HDC hDC)
{
    if ((m_uButtonState & UISTATE_PUSHED) != 0) {
        if (!m_sPushedForeImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sPushedForeImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_HOT) != 0) {
        if (!m_sHotForeImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sHotForeImage)) {
            } else {
                return;
            }
        }
    }
    if (!m_sForeImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sForeImage)) {
        }
    }
}
UINT CButtonUI::GetButtonState()
{
    return m_uButtonState;
}
void CButtonUI::SetButtonState(UINT state)
{
    m_uButtonState = state;
    Invalidate();
}

void CButtonUI::SetVisible(bool bVisible)
{
    if (!bVisible) {
        m_uButtonState = 0;
    }
    __super::SetVisible(bVisible);
}

void CButtonUI::SetHoverTextShow(bool val)
{
    if (m_bHoverTextShow == val) {
        return;
    }

    m_bHoverTextShow = val;
    Invalidate();
}

bool CButtonUI::GetHoverTextShow()
{
    return m_bHoverTextShow;
}

int CButtonUI::GetLinkContentIndex(const POINT &ptMouse)
{
    int index = 0;
    int i = 0;
    for (; i < m_nLinks; i++) {
        if (::PtInRect(&m_rcLinks[i], ptMouse)) {
            int j = 0;
            for (; j < m_nLinks; j++) {
                if (j == i) {
                    return index;
                }
                if (m_iLinkIndex[j] == 1) {
                    index++;
                }
            }
        }
    }
    return -1;
}

bool CButtonUI::GetNcHit()
{
    if (!m_IsNcHit) {
        return false;
    }

    return !IsEnabled();
}


CDuiString CButtonUI::GetToolTip() const
{
    CDuiString tooltip = CControlUI::GetToolTip();

    if (tooltip.IsEmpty() && (!m_bShowHtml)) {
        if ((m_uTextStyle & DT_END_ELLIPSIS) != FALSE) {
            tooltip = GetText();

            int availableWidth = m_rcItem.right - m_rcItem.left -
                CResourceManager::GetInstance()->Scale(m_rcTextPadding.right) -
                CResourceManager::GetInstance()->Scale(m_rcTextPadding.left);

            int availableHeight = m_rcItem.bottom - m_rcItem.top -
                CResourceManager::GetInstance()->Scale(m_rcTextPadding.bottom) -
                CResourceManager::GetInstance()->Scale(m_rcTextPadding.top);

            const int maxSize = 9999;

            // 有换行属性时，需要控制宽度；无换行属性时，无需控制宽度
            bool hasWordbreak = ((m_uTextStyle & DT_WORDBREAK) != 0);
            int limitedWidth = hasWordbreak ? availableWidth : maxSize;
            RECT rcText = { 0, 0, limitedWidth, maxSize };

            CDuiString sText = m_bEnabled ? GetText() :
                (GetDisableText().GetLength() ? GetDisableText() : GetText());

            CRenderEngine::DrawText(m_pManager->GetPaintDC(),
                m_pManager, rcText, sText, m_dwTextColor,
                m_iFont, DT_CALCRECT | m_uTextStyle);

            int actualWidth = rcText.right - rcText.left;
            int actualHeight = rcText.bottom - rcText.top;

            if (hasWordbreak) {
                // 有换行属性，根据高度判断是否显示tooltip；
                if (actualHeight <= availableHeight) {
                    tooltip = _T("");
                }
            } else {
                // 无换行属性，根据实际宽度和可用宽度判断是否显示tooltip
                if (actualWidth <= availableWidth) {
                    tooltip = _T("");
                }
            }
        }
    }

    return tooltip;
}

void CButtonUI::SetRoundEnhance(bool val)
{
    m_roundEnhance = val;
}
}