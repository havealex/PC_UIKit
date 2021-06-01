/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UILabel.h"
#include "../Core/UICulture.h"

#include <atlconv.h>
namespace DuiLib {
IMPLEMENT_DUICONTROL(CLabelUI)

CLabelUI::CLabelUI()
    : m_uTextStyle(DT_VCENTER | DT_SINGLELINE),
      m_dwTextColor(0),
      m_dwDisabledTextColor(0),
      m_bShowHtml(false),
      m_bAutoCalcWidth(false),
      m_vcenterForce(false),
      m_wordbreak(false),
      m_bAutoCalcHeight(false),
      m_bAutoHeightByTextSingle(false),
      m_bAutoHeightByTextMulti(false),
      m_bChangeParentHeight(false)
{
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
    m_uLastScale = CResourceManager::GetInstance()->GetScale();
}

CLabelUI::~CLabelUI()
{
#ifdef TEST_TIPS_BUILD
    if (m_hwndTooltip != nullptr) {
        ::DestroyWindow(m_hwndTooltip);
        m_hwndTooltip = nullptr;
    }
#endif // TEST_TIPS_BUILD
}

LPCTSTR CLabelUI::GetClass() const
{
    return _T("LabelUI");
}

LPVOID CLabelUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("Label")) == 0)
        return static_cast<CLabelUI *>(this);
    return CAnimPanelUI::GetInterface(pstrName);
}

UINT CLabelUI::GetControlFlags() const
{
    return (UINT)(IsEnabled() ? UIFLAG_SETCURSOR : 0);
}
void CLabelUI::SetTextStyle(UINT uStyle)
{
    m_uTextStyle = uStyle;
    Invalidate();
    m_bUpdateSize = true;
}

UINT CLabelUI::GetTextStyle() const
{
    return m_uTextStyle;
}

void CLabelUI::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    Invalidate();
}

DWORD CLabelUI::GetTextColor() const
{
    return m_dwTextColor;
}

void CLabelUI::SetDisabledTextColor(DWORD dwTextColor)
{
    m_dwDisabledTextColor = dwTextColor;
    Invalidate();
}

DWORD CLabelUI::GetDisabledTextColor() const
{
    return m_dwDisabledTextColor;
}

int CLabelUI::GetFont() const
{
    return m_iFont;
}

RECT CLabelUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CLabelUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
    m_bUpdateSize = true;
}

bool CLabelUI::IsShowHtml()
{
    return m_bShowHtml;
}

void CLabelUI::SetShowHtml(bool bShowHtml)
{
    if (m_bShowHtml == bShowHtml) {
        return;
    }
    m_bUpdateSize = true;
    m_bShowHtml = bShowHtml;
    Invalidate();
}

SIZE CLabelUI::EstimateSize(SIZE szAvailable)
{
    constexpr int iValue = 4; // Scaling Value
    if (m_uLastScale != CResourceManager::GetInstance()->GetScale()) {
        m_bUpdateSize = true;
    }
    if (m_bUpdateSize == false) {
        return m_lastSize;
    }
    m_uLastScale = CResourceManager::GetInstance()->GetScale();
    m_bUpdateSize = false;
    if (m_bAutoCalcWidth) {
        RECT m_rcTextPadding = CLabelUI::m_rcTextPadding;
        CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

        CDuiString sText = m_bEnabled ? GetText() : (GetDisableText().GetLength() ? GetDisableText() : GetText());
        RECT rcText = { 0, 0, 9999, 9999 };
        int nLinks = 0;

        if (m_bShowHtml) {
            CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_iFont, m_dwTextColor,
                nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
        } else {
            CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
                DT_CALCRECT | m_uTextStyle);
        }

        SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
                     rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };

        bool bOutMaxWidth = false;
        if (GetMaxWidth() < cXY.cx) {
            bOutMaxWidth = true;
            cXY.cx = GetMaxWidth();
        }

        if (m_bAutoHeightByTextSingle) {
            m_lastSize = CDuiSize(cXY.cx, cXY.cy);
            return m_lastSize;
        } else if (m_bAutoHeightByTextMulti) {
            SIZE cXY2 = cXY;
            if (bOutMaxWidth) {
                RECT rcText2 = { 0, 0, GetMaxWidth() - m_rcTextPadding.left - m_rcTextPadding.right, 9999 };

                UINT m_uTextStyle = CLabelUI::m_uTextStyle;
                m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER);

                if (m_bShowHtml) {
                    int nLinks = 0;
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText2, sText, m_iFont,
                        m_dwTextColor, nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
                } else {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText2, sText, m_dwTextColor,
                        m_iFont, DT_CALCRECT | m_uTextStyle);
                }

                cXY2.cx = rcText2.right - rcText2.left + m_rcTextPadding.left + m_rcTextPadding.right;
                cXY2.cy = rcText2.bottom - rcText2.top + m_rcTextPadding.top + m_rcTextPadding.bottom;
            }
            m_lastSize = CDuiSize(cXY2.cx, cXY2.cy);
            return m_lastSize;
        }

        m_lastSize = CDuiSize(cXY.cx, CResourceManager::GetInstance()->Scale(m_cxyFixed.cy));
        return m_lastSize;
    }

    if (m_cxyFixed.cy == 0) {
        if (m_bAutoCalcHeight && m_cxyFixed.cx) {
            CDuiString sText = GetText();

            RECT m_rcTextPadding = CLabelUI::m_rcTextPadding;
            CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

            RECT rcText = { 0, 0, CResourceManager::GetInstance()->Scale(m_cxyFixed.cx), 9999 };
            rcText.left += m_rcTextPadding.left;
            rcText.right -= m_rcTextPadding.right;


            UINT m_uTextStyle = CLabelUI::m_uTextStyle;
            m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            m_uTextStyle = m_uTextStyle | DT_WORDBREAK;

            if (m_bShowHtml) {
                int nLinks = 0;
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText,
                    m_dwTextColor, m_iFont, nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
            } else {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText,
                    m_dwTextColor, m_iFont,  DT_CALCRECT | m_uTextStyle);
            }
            SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
                         rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };

            m_lastSize = CDuiSize(CResourceManager::GetInstance()->Scale(m_cxyFixed.cx), cXY.cy);
            return m_lastSize;
        }
        if (GetText().IsEmpty() == false) {
            m_lastSize = CDuiSize(CResourceManager::GetInstance()->Scale(m_cxyFixed.cx),
                m_pManager->GetFontInfo(GetFont())->tm.tmHeight + CResourceManager::GetInstance()->Scale(iValue));
        } else {
            m_lastSize = CAnimPanelUI::EstimateSize(szAvailable);
        }
        return m_lastSize;
    } else {
        SIZE sizeT = { 0 };
        if (EstimateSizeYX(sizeT)) {
            m_lastSize = CDuiSize(sizeT.cx, sizeT.cy);
            return m_lastSize;
        }
    }

    m_lastSize = CAnimPanelUI::EstimateSize(szAvailable);
    return m_lastSize;
}

void CLabelUI::DoEvent(TEventUI &event)
{
    if (m_pManager == nullptr) {
        return;
    }
    if (event.Type == UIEVENT_SETFOCUS) {
        m_bFocused = true;
        return;
    }
    if (event.Type == UIEVENT_KILLFOCUS) {
        m_bFocused = false;
        return;
    }
    if (event.Type == UIEVENT_MOUSEENTER) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER);
    }
    if (event.Type == UIEVENT_MOUSEMOVE) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSEMOVE);
    }
    if (event.Type == UIEVENT_BUTTONDOWN) {
        m_pManager->SendNotify(this, DUI_MSGTYPE_BUTTONDOWN);
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        BOOL bPointerInside = ::PtInRect(&m_rcItem, event.ptMouse);
        m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE, bPointerInside);
    }
    CAnimPanelUI::DoEvent(event);
}

void CLabelUI::JudgeDirection(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    bool directionRTL = false;
    switch (UICulture::GetInstance()->getDirection()) {
        case DIRECTION_RTL: // added by fanzhenyin
            directionRTL = true;
        default:
            break;
    }
    if (_tcsicmp(pstrName, _T("align")) == 0) {
        if (_tcsstr(pstrValue, _T("center")) != nullptr) {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_uTextStyle |= DT_CENTER;
        }

        if (_tcsstr(pstrValue, _T("left")) != nullptr) {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_SINGLELINE);
            m_uTextStyle |= DT_LEFT;
        }

        if (_tcsstr(pstrValue, _T("right")) != nullptr) {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_SINGLELINE);
            m_uTextStyle |= DT_RIGHT;
        }
    } else if (_tcsicmp(pstrName, _T("textpadding")) == 0) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = nullptr;
        constexpr int radix = 10;
        if (directionRTL) {
            rcTextPadding.right = _tcstol(pstrValue, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.top = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.left = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
        } else {
            rcTextPadding.left = _tcstol(pstrValue, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.top = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.right = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
            rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, radix);
            ASSERT(pstr);
        }
        SetTextPadding(rcTextPadding);
    }
}
void CLabelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }
    constexpr int radix = 16;
    if (_tcsicmp(pstrName, _T("align")) == 0) {
        JudgeDirection(pstrName, pstrValue);
    } else if (_tcsicmp(pstrName, _T("valign")) == 0) {
        if (_tcsstr(pstrValue, _T("top")) != nullptr) {
            m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
        }
        if (_tcsstr(pstrValue, _T("vcenter")) != nullptr) {
            m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_uTextStyle |= (DT_VCENTER | DT_SINGLELINE);
            if (m_wordbreak)
                m_uTextStyle &= ~(DT_SINGLELINE);
            m_vcenterForce = true;
        }
        if (_tcsstr(pstrValue, _T("bottom")) != nullptr) {
            m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
        }
    } else if (_tcsicmp(pstrName, _T("endellipsis")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0)
            m_uTextStyle |= DT_END_ELLIPSIS;
        else
            m_uTextStyle &= ~DT_END_ELLIPSIS;
    } else if (_tcsicmp(pstrName, _T("wordellipsis")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0)
            m_uTextStyle |= DT_WORD_ELLIPSIS;
        else
            m_uTextStyle &= ~DT_WORD_ELLIPSIS;
    } else if (_tcsicmp(pstrName, _T("pathellipsis")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0)
            m_uTextStyle |= DT_PATH_ELLIPSIS;
        else
            m_uTextStyle &= ~DT_PATH_ELLIPSIS;
    } else if (_tcsicmp(pstrName, _T("wordbreak")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_uTextStyle &= ~DT_SINGLELINE;
            m_uTextStyle |= DT_WORDBREAK | DT_EDITCONTROL;
            m_wordbreak = true;
        } else {
            m_uTextStyle &= ~DT_WORDBREAK & ~DT_EDITCONTROL;
            m_uTextStyle |= DT_SINGLELINE;
            m_vcenterForce = false;
        }
    } else if (_tcsicmp(pstrName, _T("ellipsemask")) == 0) {
        SetEllipseMask(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("noprefix")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_uTextStyle |= DT_NOPREFIX;
        } else {
            m_uTextStyle = m_uTextStyle & ~DT_NOPREFIX;
        }
    } else if (_tcsicmp(pstrName, _T("font")) == 0)
        SetFont(_ttoi(pstrValue));
    else if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
        if (pstrValue != nullptr && *pstrValue == _T('#'))
            pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("disabledtextcolor")) == 0) {
        if (pstrValue != nullptr && *pstrValue == _T('#'))
            pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, radix);
        SetDisabledTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("textpadding")) == 0) {
        JudgeDirection(pstrName, pstrValue);
    } else if (_tcsicmp(pstrName, _T("showhtml")) == 0)
        SetShowHtml(_tcsicmp(pstrValue, _T("true")) == 0);
    else if (_tcsicmp(pstrName, _T("autocalcwidth")) == 0) {
        SetAutoCalcWidth(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("autocalcheight")) == 0) {
        SetAutoCalcHeight(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("autochangeparentheight")) == 0) {
        SetAutoChangeParentHeight(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("autoheightbytextsingle")) == 0) {
        SetAutoHeightByTextSingle(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("autoheightbytextmulti")) == 0) {
        SetAutoHeightByTextMulti(_tcsicmp(pstrValue, _T("true")) == 0);
    } else
        CAnimPanelUI::SetAttribute(pstrName, pstrValue);
}

::SIZE GetFitSize(const ::SIZE &sizeSrc, const ::SIZE &sizeDest)
{
    ::SIZE _sizeResult = { 0, 0 };
    if (static_cast<double>(sizeSrc.cx) / sizeSrc.cy > static_cast<double>(sizeDest.cx) / sizeDest.cy) {
        double scale = static_cast<double>(sizeDest.cx) / sizeSrc.cx;
        _sizeResult.cx = sizeDest.cx;
        _sizeResult.cy = static_cast<int>(sizeSrc.cy * scale);
    } else {
        double scale = static_cast<double>(sizeDest.cy) / sizeSrc.cy;
        _sizeResult.cy = sizeDest.cy;
        _sizeResult.cx = static_cast<int>(sizeSrc.cx * scale);
    }

    return _sizeResult;
}

#define TEST_VALUE(v1, v2) (((v1) & (v2)) == (v2))

#ifdef TEST_TIPS_BUILD
void CLabelUI::ShowToolTips()
{
    static int s_maxLength = 255;
    CDuiString sToolTip = GetToolTip();
    int strLength = sToolTip.GetLength();
    if (strLength > s_maxLength) {
        s_maxLength = strLength;
    }
    if (strLength == 0) {
        return;
    }

    ::ZeroMemory(&m_toolTips, sizeof(TOOLINFO));
    m_toolTips.cbSize = sizeof(TOOLINFO);
    m_toolTips.uFlags = TTF_IDISHWND | TTF_ABSOLUTE | TTF_TRACK;
    m_toolTips.hwnd = m_pManager->GetPaintWindow();
    m_toolTips.uId = (UINT_PTR)m_pManager->GetPaintWindow();
    m_toolTips.hinst = m_pManager->GetInstance();
    m_toolTips.lpszText = const_cast<LPTSTR>((LPCTSTR)sToolTip);
    m_toolTips.rect = GetPos();
    if (m_hwndTooltip == nullptr) {
        m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_pManager->GetPaintWindow(), nullptr,
            m_pManager->GetInstance(), nullptr);
        ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_toolTips);
        ::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, GetToolTipWidth());
    }
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        LONG lStyle = ::GetWindowLong(m_hwndTooltip, GWL_EXSTYLE);
        ::SetWindowLong(m_hwndTooltip, GWL_EXSTYLE, ((unsigned long)lStyle) | WS_EX_LAYOUTRTL);
    }
    RECT rec;
    GetWindowRect(m_pManager->GetPaintWindow(), &rec);
    SendMessage(m_hwndTooltip, TTM_TRACKPOSITION, 0,
        MAKELONG(m_toolTips.rect.left + rec.left, m_toolTips.rect.top + rec.top));

    if (!::IsWindowVisible(m_hwndTooltip)) {
        ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_toolTips);
        ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_toolTips);
    } else {
        ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_toolTips);
        ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_toolTips);
    }
}
#endif

void CLabelUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) {
        if (m_pManager) {
            m_dwTextColor = m_pManager->GetDefaultFontColor();
        } else {
            m_dwTextColor = 0;
        }
    }

    if (m_dwDisabledTextColor == 0 && m_pManager != nullptr)
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

    RECT m_rcTextPadding = CLabelUI::m_rcTextPadding;
    CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;


    CDuiString sText = m_bEnabled ? GetText() : (GetDisableText().GetLength() ? GetDisableText() : GetText());
    if (sText.IsEmpty()) {
        return;
    }
    int nLinks = 0;

    UINT m_uTextStyle = CLabelUI::m_uTextStyle;
    if (m_bAutoCalcHeight && m_cxyFixed.cy == 0 && m_cxyFixed.cx) {
        m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER);
        m_uTextStyle = m_uTextStyle | DT_WORDBREAK;
    }

    if (m_bAutoHeightByTextMulti) {
        m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER);
    }

    if (IsEnsureTextVisible() && !m_bShowHtml) {

        int srcWidth = rc.right - rc.left;
        int srcHeight = rc.bottom - rc.top;
        RECT rcRequired = rc;
        constexpr int rOffset = 16;
        constexpr int gOffset = 8;
        constexpr int multiples = 2;

        CRenderEngine::DrawText(hDC, m_pManager, rcRequired, sText, m_dwTextColor, m_iFont, m_uTextStyle | DT_CALCRECT);

        int requiredWidth = rcRequired.right - rcRequired.left;
        int requiredHeight = rcRequired.bottom - rcRequired.top;

        rcRequired = RECT { 0, 0, requiredWidth, requiredHeight };

        if (requiredHeight > srcHeight || requiredWidth > srcWidth) {
            ::SIZE sizeFit = GetFitSize({ requiredWidth, requiredHeight }, { srcWidth, srcHeight });

            HDC hTextDC = CreateCompatibleDC(hDC);
            HBITMAP hTextBitmap = CreateCompatibleBitmap(hDC, requiredWidth, requiredHeight);
            HBITMAP hOldTextBitmap = (HBITMAP)SelectObject(hTextDC, hTextBitmap);

            DWORD r = (m_dwBackColor & 0x00ff0000) >> rOffset;
            DWORD g = (m_dwBackColor & 0x00ff00) >> gOffset;
            DWORD b = (m_dwBackColor & 0x00ff);
            HBRUSH hBkBrush = CreateSolidBrush(RGB(r, g, b));

            FillRect(hTextDC, &rcRequired, hBkBrush);
            CRenderEngine::DrawText(hTextDC, m_pManager, rcRequired, sText,
                IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor, m_iFont, m_uTextStyle);

            if (TEST_VALUE(m_uTextStyle, DT_RIGHT)) {
                rc.left = rc.right - sizeFit.cx;
            } else if (TEST_VALUE(m_uTextStyle, DT_CENTER)) {
                rc.left = rc.left + (rc.right - rc.left - sizeFit.cx) / multiples;
            }

            if (TEST_VALUE(m_uTextStyle, DT_VCENTER)) {
                rc.top = rc.top + (rc.bottom - rc.top - sizeFit.cy) / multiples;
            } else if (TEST_VALUE(m_uTextStyle, DT_BOTTOM)) {
                rc.top = rc.bottom - sizeFit.cy;
            }

            StretchBlt(hDC, rc.left, rc.top, sizeFit.cx, sizeFit.cy, hTextDC, 0, 0, requiredWidth, requiredHeight,
                SRCCOPY);
            SelectObject(hTextDC, hOldTextBitmap);
            DeleteObject(hTextBitmap);
            DeleteObject(hBkBrush);
            DeleteDC(hTextDC);
            return;
        }
    }

    if (IsEnabled()) {
        if (m_bShowHtml) {
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, nullptr, nullptr, nLinks,
                m_uTextStyle);
        } else {
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, m_uTextStyle);
        }
    } else {
        if (m_bShowHtml) {
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont,
                nullptr, nullptr, nLinks, m_uTextStyle);
        } else {
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle);
        }
    }

#ifdef TEST_TIPS_BUILD
    ShowToolTips();
#endif
}

bool CLabelUI::GetAutoCalcWidth() const
{
    return m_bAutoCalcWidth;
}

void CLabelUI::SetAutoCalcWidth(bool bAutoCalcWidth)
{
    m_bAutoCalcWidth = bAutoCalcWidth;
    m_bUpdateSize = true;
}

bool CLabelUI::GetAutoCalcHeight() const
{
    return m_bAutoCalcHeight;
}

void CLabelUI::SetAutoCalcHeight(bool bAutoCalcHeight)
{
    m_bAutoCalcHeight = bAutoCalcHeight;
    m_bUpdateSize = true;
}

bool CLabelUI::GetAutoHeightByTextSingle() const
{
    return m_bAutoHeightByTextSingle;
}

void CLabelUI::SetAutoHeightByTextSingle(bool bAutoHeightByTextSingle)
{
    m_bAutoHeightByTextSingle = bAutoHeightByTextSingle;
    m_bUpdateSize = true;
}

bool CLabelUI::GetAutoHeightByTextMulti() const
{
    return m_bAutoHeightByTextMulti;
}

void CLabelUI::SetAutoHeightByTextMulti(bool bAutoHeightByTextMulti)
{
    m_bAutoHeightByTextMulti = bAutoHeightByTextMulti;
    m_bUpdateSize = true;
}

void CLabelUI::SetText(LPCTSTR pstrText)
{
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString text(pstrText);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    const int containSize = 2;
    if (valueList.size() == containSize) { // 确认容器的大小
        pstrText = valueList[0]; // 获取容器的第一个元素
        m_sTextAll = valueList[1]; // 获取容器的第二个元素
    }
#endif
    if (m_sText == CDuiString(pstrText))
        return;

    CAnimPanelUI::SetText(pstrText);
    m_bUpdateSize = true;
    if (!IsVisible())
        return;

    if (GetAutoCalcWidth() || m_bAutoCalcHeight) {
        NeedParentUpdate();
    }

    if (GetAutoChangeParentHeight()) {
        CControlUI *pP = GetParent();
        while (pP) {
            if (pP->GetParent() == nullptr) {
                break;
            }
            pP = pP->GetParent();
        };
        if (pP) {
            pP->NeedUpdate();
        }
    }
}

CDuiRect CLabelUI::GetTextRect(SIZE szAvailable)
{
    RECT rcText = { 0, 0, szAvailable.cx, 9999 };
    if (m_bShowHtml) {
        int nLinks = 0;
        CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, GetText(), m_dwTextColor, m_iFont,
            nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
    } else {
        CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, GetText(), m_dwTextColor, m_iFont,
            DT_CALCRECT | m_uTextStyle);
    }
    SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
        rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };
    return rcText;
}


bool CLabelUI::EstimateSizeYX(SIZE &_size)
{
    if (m_bAutoCalcHeight && m_cxyFixed.cx) {
        CDuiString sText = GetText();

        RECT m_rcTextPadding = CLabelUI::m_rcTextPadding;
        CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

        RECT rcText = { 0, 0, CResourceManager::GetInstance()->Scale(m_cxyFixed.cx), 9999 };
        rcText.left += m_rcTextPadding.left;
        rcText.right -= m_rcTextPadding.right;


        UINT m_uTextStyle = CLabelUI::m_uTextStyle;
        m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        m_uTextStyle = m_uTextStyle | DT_WORDBREAK;

        if (m_bShowHtml) {
            int nLinks = 0;
            CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
                nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
        } else {
            CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
                DT_CALCRECT | m_uTextStyle);
        }
        SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
                     rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };


        if (CResourceManager::GetInstance()->Scale(m_cxyFixed.cy) >= cXY.cy) {
            cXY.cy = CResourceManager::GetInstance()->Scale(m_cxyFixed.cy);
        }

        _size.cx = CResourceManager::GetInstance()->Scale(m_cxyFixed.cx);
        _size.cy = cXY.cy;

        return true;
    }

    return false;
}

void CLabelUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    constexpr int boxValues = 2;
    if (m_bEllipseMask && !IsGrabbing()) {
        std::unique_ptr<UIImage> grabImage;
        grabImage.reset(GrabImage(SIZE { m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top }));
        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        if ((grabImage.get() == nullptr) || (grabImage->GetBitmap() == nullptr))
            return;
        Gdiplus::TextureBrush brush(grabImage->GetBitmap(), Gdiplus::WrapModeTile);
        graphics.TranslateTransform(m_rcItem.left, m_rcItem.top);
        graphics.FillEllipse(&brush, 1, 1, m_rcItem.right - m_rcItem.left - boxValues,
            m_rcItem.bottom - m_rcItem.top - boxValues);
    } else {
        __super::DoPaint(hDC, rcPaint);
    }
}

void CLabelUI::SetEllipseMask(bool b)
{
    m_bEllipseMask = b;
    m_bUpdateSize = true;
}


void CLabelUI::SetFixedHeight(int cy)
{
    CAnimPanelUI::SetFixedHeight(cy);
    m_bUpdateSize = true;
}
void CLabelUI::SetFixedHeightEx(int cy)
{
    CAnimPanelUI::SetFixedHeightEx(cy);
    m_bUpdateSize = true;
}
void CLabelUI::SetFixedWidth(int cx)
{
    CAnimPanelUI::SetFixedWidth(cx);
    m_bUpdateSize = true;
}
void CLabelUI::SetFixedWidthEx(int cx)
{
    CAnimPanelUI::SetFixedWidthEx(cx);
    m_bUpdateSize = true;
}
void CLabelUI::SetFixedXY(SIZE szXY)
{
    CAnimPanelUI::SetFixedXY(szXY);
    m_bUpdateSize = true;
}
void CLabelUI::SetMinWidth(int cx)
{
    CAnimPanelUI::SetMinWidth(cx);
    m_bUpdateSize = true;
}
void CLabelUI::SetMaxWidth(int cx)
{
    CAnimPanelUI::SetMaxWidth(cx);
    m_bUpdateSize = true;
}
void CLabelUI::SetMinHeight(int cy)
{
    CAnimPanelUI::SetMinHeight(cy);
    m_bUpdateSize = true;
}
void CLabelUI::SetMaxHeight(int cy)
{
    CAnimPanelUI::SetMaxHeight(cy);
    m_bUpdateSize = true;
}
void CLabelUI::SetVisible(bool bVisible)
{
    CAnimPanelUI::SetVisible(bVisible);
    m_bUpdateSize = true;
#ifdef TEST_TIPS_BUILD
    if (!bVisible) {
        if (m_hwndTooltip != nullptr) {
            ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_toolTips);
        }
    }
#endif
}

#ifdef TEST_TIPS_BUILD
void CLabelUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
    if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
        m_pManager->SetFocus(nullptr);
    }
    if (!bVisible) {
        if (m_hwndTooltip != nullptr) {
            ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_toolTips);
        }
    }
}
#endif // TEST_TIPS_BUILD

void CLabelUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CAnimPanelUI::SetPos(rc, bNeedInvalidate);
    m_bUpdateSize = true;
}
RECT CLabelUI::SetDirectionPos(RECT rc)
{
    m_bUpdateSize = true;
    return CAnimPanelUI::SetDirectionPos(rc);
}
void CLabelUI::SetPadding(RECT rcPadding)
{
    CAnimPanelUI::SetPadding(rcPadding);
    m_bUpdateSize = true;
}
void CLabelUI::SetFloat(bool bFloat)
{
    CAnimPanelUI::SetFloat(bFloat);
    m_bUpdateSize = true;
}
CDuiString CLabelUI::GetToolTip() const
{
    CDuiString temp = CControlUI::GetToolTip();
    if (temp.IsEmpty() && (m_bShowHtml == false)) {
        if ((m_uTextStyle & DT_END_ELLIPSIS) != 0) {
            temp = GetText();

            if ((m_uTextStyle & DT_END_ELLIPSIS) && (m_uTextStyle & DT_WORDBREAK) == False) {
                int width = m_rcItem.right - m_rcItem.left -
                    CResourceManager::GetInstance()->Scale(m_rcTextPadding.right) -
                    CResourceManager::GetInstance()->Scale(m_rcTextPadding.left);
                RECT rcText = { 0, 0, 9999, 9999 };
                int nLinks = 0;
                CDuiString sText =
                    m_bEnabled ? GetText() : (GetDisableText().GetLength() ? GetDisableText() : GetText());
                if (m_bShowHtml)
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_iFont,
                        m_dwTextColor, nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
                else
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
                        DT_CALCRECT | m_uTextStyle);

                int txtWidth = rcText.right - rcText.left;
                if (txtWidth < width) {
                    temp = _T("");
                }
            }
        }
    }

    return temp;
}
}