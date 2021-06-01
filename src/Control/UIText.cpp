/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIText.h"

constexpr int TIMERID_TEXT_SHARK = 0x15000;

namespace DuiLib {
IMPLEMENT_DUICONTROL(CTextUI)

CTextUI::CTextUI() : m_nLinks(0), m_nHoverLink(-1)
{
    m_uTextStyle = DT_WORDBREAK;
    ::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
    ::ZeroMemory(m_iLinkIndex, sizeof(m_iLinkIndex));
}

CTextUI::~CTextUI() {}

LPCTSTR CTextUI::GetClass() const
{
    return _T("TextUI");
}

LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_TEXT) == 0) {
        return static_cast<CTextUI *>(this);
    }
    return CLabelUI::GetInterface(pstrName);
}

UINT CTextUI::GetControlFlags() const
{
    if (IsEnabled() && m_nLinks > 0) {
        return UIFLAG_SETCURSOR;
    } else {
        return 0;
    }
}

CDuiString *CTextUI::GetLinkContent(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_nLinks) {
        return &m_sLinks[iIndex];
    }
    return nullptr;
}

void CTextUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pParent != nullptr) {
            m_pParent->DoEvent(event);
        } else {
            CLabelUI::DoEvent(event);
        }
        return;
    }

    if (event.Type == UIEVENT_SETCURSOR) {
        for (int i = 0; i < m_nLinks; i++) {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
                ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(IDC_HAND)));
                return;
            }
        }
    }
    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK && IsEnabled()) {
        for (int i = 0; i < m_nLinks; i++) {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
                Invalidate();
                return;
            }
        }
    }
    if (m_pManager == nullptr) {
        return;
    }
    if (event.Type == UIEVENT_BUTTONUP && IsEnabled()) {
        for (int i = 0; i < m_nLinks; i++) {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
                m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                return;
            }
        }
    }
    if (event.Type == UIEVENT_CONTEXTMENU) {
        return;
    }
    // When you move over a link
    if (m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE && IsEnabled()) {
        int nHoverLink = -1;
        for (int i = 0; i < m_nLinks; i++) {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse)) {
                nHoverLink = i;
                break;
            }
        }

        if (m_nHoverLink != nHoverLink) {
            m_nHoverLink = nHoverLink;
            Invalidate();
            return;
        }
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        if (m_nLinks > 0 && IsEnabled()) {
            if (m_nHoverLink != -1) {
                m_nHoverLink = -1;
                Invalidate();
                return;
            }
        }
    }

    if (event.Type == UIEVENT_TIMER) {
        if (event.wParam == TIMERID_TEXT_SHARK) {
            if (IsVisible() == false) {
                return;
            }
            UINT64 offtime = (::GetTickCount64() - m_sharkStartTick);
            if (offtime > m_durationTime) {
                m_bShark = false;
                KillTimer(TIMERID_TEXT_SHARK);
            }
            Invalidate();
        }
    }
    CLabelUI::DoEvent(event);
}

SIZE CTextUI::EstimateSize(SIZE szAvailable)
{
    bool bOutMaxWidth = false;
    if (m_uLastScale != CResourceManager::GetInstance()->GetScale()) {
        m_bUpdateSize = true;
    }
    if (m_bUpdateSize == false) {
        return m_lastSize;
    }
    m_bUpdateSize = false;
    m_uLastScale = CResourceManager::GetInstance()->GetScale();
    CDuiString sText = GetText();

    RECT m_rcTextPadding = CTextUI::m_rcTextPadding;
    CResourceManager::GetInstance()->Scale(&m_rcTextPadding);
    const int maxSize = 9999;
    RECT rcText = { 0, 0, m_bAutoCalcWidth ? maxSize : (CResourceManager::GetInstance()->Scale(m_cxyFixed.cx)),
        maxSize };
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;

    if (m_bShowHtml) {
        int nLinks = 0;
        CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor,
            m_iFont, nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
    } else {
        CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, m_dwTextColor, m_iFont,
            DT_CALCRECT | m_uTextStyle);
    }
    SIZE cXY = { rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right,
                 rcText.bottom - rcText.top + m_rcTextPadding.top + m_rcTextPadding.bottom };

    if (m_bAutoCalcWidth) {
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
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText2, sText, m_dwTextColor,
                        m_iFont, nullptr, nullptr, nLinks, DT_CALCRECT | m_uTextStyle);
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
    if (m_bAutoCalcHeight) {
        LONG curY = cXY.cy ? cXY.cy : 1;
        if (curY < CResourceManager::GetInstance()->Scale(m_cxyFixed.cy)) {
            curY = CResourceManager::GetInstance()->Scale(m_cxyFixed.cy);
        }

        m_lastSize = CDuiSize(CResourceManager::GetInstance()->Scale(m_cxyFixed.cx), curY);
        return m_lastSize;
    }
    m_lastSize = CControlUI::EstimateSize(szAvailable);
    return m_lastSize;
}

void CTextUI::PaintText(HDC hDC)
{
    CDuiString sText = GetText();
    if (sText.IsEmpty()) {
        m_nLinks = 0;
        return;
    }
    if (m_pManager == nullptr) {
        return;
    }
    if (m_dwTextColor == 0) {
        m_dwTextColor = m_pManager->GetDefaultFontColor();
    }
    if (m_dwDisabledTextColor == 0) {
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    }
    RECT m_rcTextPadding = CTextUI::m_rcTextPadding;
    CResourceManager::GetInstance()->Scale(&m_rcTextPadding);
    m_nLinks = lengthof(m_rcLinks);
    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

    if (m_bAutoHeightByTextMulti) {
        m_uTextStyle &= ~(DT_SINGLELINE | DT_VCENTER);
    }
    const int timeMultiple = 100;
    const int iMultiple = 4;
    const int iModulos = 2;
    if (m_bShark) {
        static vector<int> s_vec;
        if (m_sharkpos == 0) {
            s_vec.clear();
            int cnt = m_durationTime / timeMultiple;
            int maxOff = cnt * iMultiple;
            for (int i = 0; i < cnt + 1; ++i) {
                if (maxOff < i * iMultiple) {
                    break;
                }
                if (i % iModulos == 0) {
                    s_vec.push_back(-(maxOff - i * iMultiple));
                } else {
                    s_vec.push_back(maxOff - i * iMultiple);
                }
            }
        }
        if (m_sharkpos >= s_vec.size()) {
            return;
        }
        int off = CResourceManager::GetInstance()->Scale(s_vec[m_sharkpos]);
        rc.left += off;
        m_sharkpos++;
    }
    if (IsEnabled()) {
        if (m_bShowHtml) {
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, m_rcLinks, m_sLinks,
                m_nLinks, m_uTextStyle, m_iLinkIndex);
        } else {
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, m_uTextStyle);
        }
    } else {
        if (m_bShowHtml) {
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_rcLinks, m_sLinks,
                m_nLinks, m_uTextStyle);
        } else {
            CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle);
        }
    }

#ifdef TEST_TIPS_BUILD
    ShowToolTips();
#endif
}
int CTextUI::GetLinkContentIndex(const POINT &ptMouse)
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
void CTextUI::Shark(int duration)
{
    const UINT nElapse = 100;
    m_bShark = true;
    m_sharkStartTick = ::GetTickCount64();
    m_durationTime = duration;
    SetTimer(TIMERID_TEXT_SHARK, nElapse);
    m_sharkpos = 0;
    Invalidate();
}
}
