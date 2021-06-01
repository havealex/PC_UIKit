/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIScrollBar.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CScrollBarUI)
const int RANGE_TYPE_VALUE = 100;
const int LINE_SIZE_VALUE = 8;

CScrollBarUI::CScrollBarUI()
    : m_bHorizontal(false),
      m_nRange(RANGE_TYPE_VALUE),
      m_nScrollPos(0),
      m_nLineSize(LINE_SIZE_VALUE),
      m_pOwner(nullptr),
      m_nLastScrollPos(0),
      m_nLastScrollOffset(0),
      m_nScrollRepeatDelay(0),
      m_uButton1State(0),
      m_uButton2State(0),
      m_uThumbState(0),
      m_bShowButton1(true),
      m_bShowButton2(true)
{
    m_cxyFixed.cx = DuiLib::CResourceManager::GetInstance()->Scale(DEFAULT_SCROLLBAR_SIZE);
    ptLastMouse.x = ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
    ::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
    ::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
}

CScrollBarUI::~CScrollBarUI()
{
    if (m_pManager) {
        m_pManager->RemovePreMessageFilter(this);
    }
}

LPCTSTR CScrollBarUI::GetClass() const
{
    return _T("ScrollBarUI");
}

LPVOID CScrollBarUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_SCROLLBAR) == 0)
        return static_cast<CScrollBarUI *>(this);
    return CControlUI::GetInterface(pstrName);
}

CContainerUI *CScrollBarUI::GetOwner() const
{
    return m_pOwner;
}

void CScrollBarUI::SetOwner(CContainerUI *pOwner)
{
    m_pOwner = pOwner;
}

void CScrollBarUI::SetVisible(bool bVisible)
{
    if (m_bVisible == bVisible) {
        return;
    }

    (void)IsVisible();
    m_bVisible = bVisible;
    if (m_bFocused) {
        m_bFocused = false;
    }
}

void CScrollBarUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);
    if (!IsEnabled()) {
        m_uButton1State = 0;
        m_uButton2State = 0;
        m_uThumbState = 0;
    }
}

void CScrollBarUI::SetFocus()
{
    if (m_pOwner != nullptr)
        m_pOwner->SetFocus();
    else
        CControlUI::SetFocus();
}

bool CScrollBarUI::IsHorizontal()
{
    return m_bHorizontal;
}

void CScrollBarUI::SetHorizontal(bool bHorizontal)
{
    if (m_bHorizontal == bHorizontal) {
        return;
    }

    m_bHorizontal = bHorizontal;
    if (m_bHorizontal) {
        if (m_cxyFixed.cy == 0) {
            m_cxyFixed.cx = 0;
            m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
        }
    } else {
        if (m_cxyFixed.cx == 0) {
            m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
            m_cxyFixed.cy = 0;
        }
    }

    if (m_pOwner != nullptr) {
        m_pOwner->NeedUpdate();
    } else {
        NeedParentUpdate();
    }
}

int CScrollBarUI::GetScrollRange() const
{
    return m_nRange;
}

void CScrollBarUI::SetScrollRange(int nRange)
{
    if (m_nRange == nRange) {
        return;
    }

    m_nRange = nRange;
    if (m_nRange < 0) {
        m_nRange = 0;
    }
    if (m_nScrollPos > m_nRange) {
        m_nScrollPos = m_nRange;
    }
    SetPos(m_rcItem, false);
}

int CScrollBarUI::GetScrollPos() const
{
    return m_nScrollPos;
}

void CScrollBarUI::SetScrollPos(int nPos)
{
    if (m_nScrollPos == nPos) {
        return;
    }

    m_nScrollPos = nPos;
    if (m_nScrollPos < 0) {
        m_nScrollPos = 0;
    }
    if (m_nScrollPos > m_nRange) {
        m_nScrollPos = m_nRange;
    }
    SetPos(m_rcItem);
}

int CScrollBarUI::GetLineSize() const
{
    return m_nLineSize;
}

void CScrollBarUI::SetLineSize(int nSize)
{
    m_nLineSize = nSize;
}

bool CScrollBarUI::GetShowButton1()
{
    return m_bShowButton1;
}

void CScrollBarUI::SetShowButton1(bool bShow)
{
    m_bShowButton1 = bShow;
    SetPos(m_rcItem);
}

LPCTSTR CScrollBarUI::GetButton1NormalImage()
{
    return m_sButton1NormalImage;
}

void CScrollBarUI::SetButton1NormalImage(LPCTSTR pStrImage)
{
    m_sButton1NormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton1HotImage()
{
    return m_sButton1HotImage;
}

void CScrollBarUI::SetButton1HotImage(LPCTSTR pStrImage)
{
    m_sButton1HotImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton1PushedImage()
{
    return m_sButton1PushedImage;
}

void CScrollBarUI::SetButton1PushedImage(LPCTSTR pStrImage)
{
    m_sButton1PushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton1DisabledImage()
{
    return m_sButton1DisabledImage;
}

void CScrollBarUI::SetButton1DisabledImage(LPCTSTR pStrImage)
{
    m_sButton1DisabledImage = pStrImage;
    Invalidate();
}

bool CScrollBarUI::GetShowButton2()
{
    return m_bShowButton2;
}

void CScrollBarUI::SetShowButton2(bool bShow)
{
    m_bShowButton2 = bShow;
    SetPos(m_rcItem);
}

LPCTSTR CScrollBarUI::GetButton2NormalImage()
{
    return m_sButton2NormalImage;
}

void CScrollBarUI::SetButton2NormalImage(LPCTSTR pStrImage)
{
    m_sButton2NormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton2HotImage()
{
    return m_sButton2HotImage;
}

void CScrollBarUI::SetButton2HotImage(LPCTSTR pStrImage)
{
    m_sButton2HotImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton2PushedImage()
{
    return m_sButton2PushedImage;
}

void CScrollBarUI::SetButton2PushedImage(LPCTSTR pStrImage)
{
    m_sButton2PushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetButton2DisabledImage()
{
    return m_sButton2DisabledImage;
}

void CScrollBarUI::SetButton2DisabledImage(LPCTSTR pStrImage)
{
    m_sButton2DisabledImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetThumbNormalImage()
{
    return m_sThumbNormalImage;
}

void CScrollBarUI::SetThumbNormalImage(LPCTSTR pStrImage)
{
    m_sThumbNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetThumbHotImage()
{
    return m_sThumbHotImage;
}

void CScrollBarUI::SetThumbHotImage(LPCTSTR pStrImage)
{
    m_sThumbHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetThumbPushedImage()
{
    return m_sThumbPushedImage;
}

void CScrollBarUI::SetThumbPushedImage(LPCTSTR pStrImage)
{
    m_sThumbPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetThumbDisabledImage()
{
    return m_sThumbDisabledImage;
}

void CScrollBarUI::SetThumbDisabledImage(LPCTSTR pStrImage)
{
    m_sThumbDisabledImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetRailNormalImage()
{
    return m_sRailNormalImage;
}

void CScrollBarUI::SetRailNormalImage(LPCTSTR pStrImage)
{
    m_sRailNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetRailHotImage()
{
    return m_sRailHotImage;
}

void CScrollBarUI::SetRailHotImage(LPCTSTR pStrImage)
{
    m_sRailHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetRailPushedImage()
{
    return m_sRailPushedImage;
}

void CScrollBarUI::SetRailPushedImage(LPCTSTR pStrImage)
{
    m_sRailPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetRailDisabledImage()
{
    return m_sRailDisabledImage;
}

void CScrollBarUI::SetRailDisabledImage(LPCTSTR pStrImage)
{
    m_sRailDisabledImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetBkNormalImage()
{
    return m_sBkNormalImage;
}

void CScrollBarUI::SetBkNormalImage(LPCTSTR pStrImage)
{
    m_sBkNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetBkHotImage()
{
    return m_sBkHotImage;
}

void CScrollBarUI::SetBkHotImage(LPCTSTR pStrImage)
{
    m_sBkHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetBkPushedImage()
{
    return m_sBkPushedImage;
}

void CScrollBarUI::SetBkPushedImage(LPCTSTR pStrImage)
{
    m_sBkPushedImage = pStrImage;
    Invalidate();
}

LPCTSTR CScrollBarUI::GetBkDisabledImage()
{
    return m_sBkDisabledImage;
}

void CScrollBarUI::SetBkDisabledImage(LPCTSTR pStrImage)
{
    m_sBkDisabledImage = pStrImage;
    Invalidate();
}

void CScrollBarUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    const int multiples = 2;
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    SIZE m_cxyFixed = CScrollBarUI::m_cxyFixed;
    m_cxyFixed = CResourceManager::GetInstance()->Scale(m_cxyFixed);

    if (m_bHorizontal) {
        int cx = rc.right - rc.left;
        if (m_bShowButton1)
            cx -= m_cxyFixed.cy;
        if (m_bShowButton2)
            cx -= m_cxyFixed.cy;
        if (cx > m_cxyFixed.cy) {
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if (m_bShowButton1) {
                m_rcButton1.right = rc.left + m_cxyFixed.cy;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
            } else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.top = rc.top;
            m_rcButton2.right = rc.right;
            if (m_bShowButton2) {
                m_rcButton2.left = rc.right - m_cxyFixed.cy;
                m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
            } else {
                m_rcButton2.left = m_rcButton2.right;
                m_rcButton2.bottom = m_rcButton2.top;
            }

            m_rcThumb.top = rc.top;
            m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
            if (m_nRange > 0) {
                int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
                if (cxThumb < m_cxyFixed.cy)
                    cxThumb = m_cxyFixed.cy;

                m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
                m_rcThumb.right = m_rcThumb.left + cxThumb;
                if (m_rcThumb.right > m_rcButton2.left) {
                    m_rcThumb.left = m_rcButton2.left - cxThumb;
                    m_rcThumb.right = m_rcButton2.left;
                }
            } else {
                m_rcThumb.left = m_rcButton1.right;
                m_rcThumb.right = m_rcButton2.left;
            }
        } else {
            int cxButton = (rc.right - rc.left) / multiples;
            if (cxButton > m_cxyFixed.cy)
                cxButton = m_cxyFixed.cy;
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if (m_bShowButton1) {
                m_rcButton1.right = rc.left + cxButton;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
            } else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.top = rc.top;
            m_rcButton2.right = rc.right;
            if (m_bShowButton2) {
                m_rcButton2.left = rc.right - cxButton;
                m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
            } else {
                m_rcButton2.left = m_rcButton2.right;
                m_rcButton2.bottom = m_rcButton2.top;
            }

            ::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
        }
    } else {
        int cy = rc.bottom - rc.top;
        if (m_bShowButton1)
            cy -= m_cxyFixed.cx;
        if (m_bShowButton2)
            cy -= m_cxyFixed.cx;
        if (cy > m_cxyFixed.cx) {
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if (m_bShowButton1) {
                m_rcButton1.right = rc.left + m_cxyFixed.cx;
                m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
            } else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.left = rc.left;
            m_rcButton2.bottom = rc.bottom;
            if (m_bShowButton2) {
                m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
                m_rcButton2.right = rc.left + m_cxyFixed.cx;
            } else {
                m_rcButton2.top = m_rcButton2.bottom;
                m_rcButton2.right = m_rcButton2.left;
            }

            m_rcThumb.left = rc.left;
            m_rcThumb.right = rc.left + m_cxyFixed.cx;
            if (m_nRange > 0) {
                float coefficient = static_cast<float>(rc.bottom - rc.top) /
                    static_cast<float>(m_nRange + rc.bottom - rc.top);
                int minThumbWidth = DuiLib::CResourceManager::GetInstance()->Scale(m_minThumbWidth);
                int cyThumb = static_cast<int>(cy * coefficient);
                if ((cyThumb + minThumbWidth) < m_cxyFixed.cx)
                    cyThumb = m_cxyFixed.cx;

                float coefficient2 = static_cast<float>(cy - cyThumb) / static_cast<float>(m_nRange);
                m_rcThumb.top = static_cast<int>(m_nScrollPos * coefficient2) + m_rcButton1.bottom;
                m_rcThumb.bottom = m_rcThumb.top + cyThumb;
                if (m_rcThumb.bottom > m_rcButton2.top) {
                    m_rcThumb.top = m_rcButton2.top - cyThumb;
                    m_rcThumb.bottom = m_rcButton2.top;
                }
            } else {
                m_rcThumb.top = m_rcButton1.bottom;
                m_rcThumb.bottom = m_rcButton2.top;
            }
        } else {
            int cyButton = (rc.bottom - rc.top) / multiples;
            if (cyButton > m_cxyFixed.cx) {
                cyButton = m_cxyFixed.cx;
            }
            m_rcButton1.left = rc.left;
            m_rcButton1.top = rc.top;
            if (m_bShowButton1) {
                m_rcButton1.right = rc.left + m_cxyFixed.cx;
                m_rcButton1.bottom = rc.top + cyButton;
            } else {
                m_rcButton1.right = m_rcButton1.left;
                m_rcButton1.bottom = m_rcButton1.top;
            }

            m_rcButton2.left = rc.left;
            m_rcButton2.bottom = rc.bottom;
            if (m_bShowButton2) {
                m_rcButton2.top = rc.bottom - cyButton;
                m_rcButton2.right = rc.left + m_cxyFixed.cx;
            } else {
                m_rcButton2.top = m_rcButton2.bottom;
                m_rcButton2.right = m_rcButton2.left;
            }

            ::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
        }
    }
}

void CScrollBarUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pOwner != nullptr) {
            m_pOwner->DoEvent(event);
        } else {
            CControlUI::DoEvent(event);
        }
        return;
    }

    if (event.Type == UIEVENT_SETFOCUS) {
        return;
    }
    if (event.Type == UIEVENT_KILLFOCUS) {
        return;
    }
    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {

#ifdef WIN32_NFF_CHECK
        ButtonDownEvent(event);
        m_bCapture = true;
#endif // WIN32_NFF_CHECK

        return;
    }
    bool flag = event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_POINTERUP;
    if (flag) {

#ifdef WIN32_NFF_CHECK
        m_nScrollRepeatDelay = 0;
        m_nLastScrollOffset = 0;
        m_pManager->KillTimer(this, DEFAULT_TIMERID);

        if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
            m_uThumbState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
            Invalidate();
        } else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
            m_uButton1State &= ~UISTATE_PUSHED;
            Invalidate();
        } else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
            m_uButton2State &= ~UISTATE_PUSHED;
            Invalidate();
        }
        m_bCapture = false;
#endif // WIN32_NFF_CHECK
        return;
    }
    if (event.Type == UIEVENT_MOUSEMOVE) {

#ifdef WIN32_NFF_CHECK
        if (CheckMouseSatus(event) == false) {
            return;
        }
        const int multiples = 2;
        if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
            if (!m_bHorizontal) {
                int vRange = m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top -
                    multiples * m_cxyFixed.cx;
                if (vRange != 0) {
                    m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / vRange;
                }
            } else {
                int hRange = m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left -
                    multiples * m_cxyFixed.cy;
                if (hRange != 0)
                    m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / hRange;
            }
        } else {
            if ((m_uThumbState & UISTATE_HOT) != 0) {
                if (!::PtInRect(&m_rcThumb, event.ptMouse)) {
                    m_uThumbState &= ~UISTATE_HOT;
                    Invalidate();
                }
            } else {
                if (!IsEnabled()) {
                    return;
                }
                if (::PtInRect(&m_rcThumb, event.ptMouse)) {
                    m_uThumbState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
#endif // WIN32_NFF_CHECK
        return;
    }
    if (event.Type == UIEVENT_CONTEXTMENU) {
        return;
    }
    const int times = 5;
    if (event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID) {
        if (CheckMouseSatus(event) == false) {
            return;
        }
        if (m_bCapture == false) {
            return;
        }
        ++m_nScrollRepeatDelay;
        if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
            if (!m_bHorizontal) {
                if (m_pOwner != nullptr) {
                    CDuiSize srcSize = m_pOwner->GetScrollPos();
                    CDuiSize destSize(m_pOwner->GetScrollPos().cx, m_nLastScrollPos + m_nLastScrollOffset);
                    if (srcSize.cx != destSize.cx || srcSize.cy != destSize.cy) {
                        m_pOwner->SetScrollPos(destSize);
                    }
                } else {
                    SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
                }
            } else {
                if (m_pOwner != nullptr) {
                    m_pOwner->SetScrollPos(
                        CDuiSize(m_nLastScrollPos + m_nLastScrollOffset, m_pOwner->GetScrollPos().cy));
                } else {
                    SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
                }
            }
            Invalidate();
        } else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
            if (m_nScrollRepeatDelay <= times) {
                return;
            }
            if (!m_bHorizontal) {
                if (m_pOwner != nullptr) {
                    m_pOwner->LineUp();
                } else {
                    SetScrollPos(m_nScrollPos - m_nLineSize);
                }
            } else {
                if (m_pOwner != nullptr) {
                    m_pOwner->LineLeft();
                } else {
                    SetScrollPos(m_nScrollPos - m_nLineSize);
                }
            }
        } else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
            if (m_nScrollRepeatDelay <= times) {
                return;
            }
            if (!m_bHorizontal) {
                if (m_pOwner != nullptr) {
                    m_pOwner->LineDown();
                } else {
                    SetScrollPos(m_nScrollPos + m_nLineSize);
                }
            } else {
                if (m_pOwner != nullptr) {
                    m_pOwner->LineRight();
                } else {
                    SetScrollPos(m_nScrollPos + m_nLineSize);
                }
            }
        } else {
            if (m_nScrollRepeatDelay <= times) {
                return;
            }
            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
            if (!m_bHorizontal) {
                if (pt.y < m_rcThumb.top) {
                    if (m_pOwner != nullptr) {
                        m_pOwner->PageUp();
                    } else {
                        SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
                    }
                } else if (pt.y > m_rcThumb.bottom) {
                    if (m_pOwner != nullptr) {
                        m_pOwner->PageDown();
                    } else {
                        SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);
                    }
                }
            } else {
                if (pt.x < m_rcThumb.left) {
                    if (m_pOwner != nullptr) {
                        m_pOwner->PageLeft();
                    } else {
                        SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
                    }
                } else if (pt.x > m_rcThumb.right) {
                    if (m_pOwner != nullptr) {
                        m_pOwner->PageRight();
                    } else {
                        SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);
                    }
                }
            }
        }
        if (m_pManager != nullptr && m_pOwner == nullptr)
            m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
        return;
    }
    if (event.Type == UIEVENT_MOUSEENTER) {
        if (IsEnabled()) {
            m_uButton1State |= UISTATE_HOT;
            m_uButton2State |= UISTATE_HOT;
            if (::PtInRect(&m_rcThumb, event.ptMouse)) {
                m_uThumbState |= UISTATE_HOT;
            }
            Invalidate();
        }
        return;
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        if (IsEnabled()) {
            m_uButton1State &= ~UISTATE_HOT;
            m_uButton2State &= ~UISTATE_HOT;
            m_uThumbState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if (event.Type == UIEVENT_POINTERUPDATE) {
        return;
    }
    if (m_pOwner != nullptr) {
        m_pOwner->DoEvent(event);
    } else {
        CControlUI::DoEvent(event);
    }
}

void CScrollBarUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("button1normalimage")) == 0) {
        SetButton1NormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button1hotimage")) == 0) {
        SetButton1HotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button1pushedimage")) == 0) {
        SetButton1PushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button1disabledimage")) == 0) {
        SetButton1DisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button2normalimage")) == 0) {
        SetButton2NormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button2hotimage")) == 0) {
        SetButton2HotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button2pushedimage")) == 0) {
        SetButton2PushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("button2disabledimage")) == 0) {
        SetButton2DisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("thumbnormalimage")) == 0) {
        SetThumbNormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("thumbhotimage")) == 0) {
        SetThumbHotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("thumbpushedimage")) == 0) {
        SetThumbPushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("thumbdisabledimage")) == 0) {
        SetThumbDisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("railnormalimage")) == 0) {
        SetRailNormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("railhotimage")) == 0) {
        SetRailHotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("railpushedimage")) == 0) {
        SetRailPushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("raildisabledimage")) == 0) {
        SetRailDisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bknormalimage")) == 0) {
        SetBkNormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bkhotimage")) == 0) {
        SetBkHotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bkpushedimage")) == 0) {
        SetBkPushedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bkdisabledimage")) == 0) {
        SetBkDisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("hor")) == 0) {
        SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("linesize")) == 0) {
        SetLineSize(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("range")) == 0) {
        SetScrollRange(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("value")) == 0) {
        SetScrollPos(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("showbutton1")) == 0) {
        SetShowButton1(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("showbutton2")) == 0) {
        SetShowButton2(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("width")) == 0) {
        SetScrollBarWidth(_ttoi(pstrValue));
    } else {
        CControlUI::SetAttribute(pstrName, pstrValue);
    }
}

void CScrollBarUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) {
        return;
    }
    PaintBk(hDC);
    PaintButton1(hDC);
    PaintButton2(hDC);
    PaintThumb(hDC);
    PaintRail(hDC);
}

void CScrollBarUI::PaintBk(HDC hDC)
{
    if (!IsEnabled()) {
        m_uThumbState |= UISTATE_DISABLED;
    } else {
        m_uThumbState &= ~UISTATE_DISABLED;
    }

    if ((m_uThumbState & UISTATE_DISABLED) != 0) {
        if (!m_sBkDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sBkDisabledImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
        if (!m_sBkPushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sBkPushedImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_HOT) != 0) {
        if (!m_sBkHotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sBkHotImage)) {
            } else {
                return;
            }
        }
    }

    if (!m_sBkNormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sBkNormalImage)) {
        } else {
            return;
        }
    }
}

void CScrollBarUI::PaintButton1(HDC hDC)
{
    if (!m_bShowButton1) {
        return;
    }

    if (!IsEnabled()) {
        m_uButton1State |= UISTATE_DISABLED;
    } else {
        m_uButton1State &= ~UISTATE_DISABLED;
    }

    m_sImageModify.Empty();
    m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), m_rcButton1.left - m_rcItem.left,
        m_rcButton1.top - m_rcItem.top, m_rcButton1.right - m_rcItem.left, m_rcButton1.bottom - m_rcItem.top);

    if ((m_uButton1State & UISTATE_DISABLED) != 0) {
        if (!m_sButton1DisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton1DisabledImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
        if (!m_sButton1PushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton1PushedImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uButton1State & UISTATE_HOT) != 0) {
        if (!m_sButton1HotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton1HotImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    }

    if (!m_sButton1NormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sButton1NormalImage, (LPCTSTR)m_sImageModify)) {
        } else {
            return;
        }
    }

    DWORD dwBorderColor = 0xFF85E4FF;
    int nBorderSize = 2;
    CRenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
}

void CScrollBarUI::PaintButton2(HDC hDC)
{
    if (!m_bShowButton2) {
        return;
    }

    if (!IsEnabled()) {
        m_uButton2State |= UISTATE_DISABLED;
    } else {
        m_uButton2State &= ~UISTATE_DISABLED;
    }

    m_sImageModify.Empty();
    m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), m_rcButton2.left - m_rcItem.left,
        m_rcButton2.top - m_rcItem.top, m_rcButton2.right - m_rcItem.left, m_rcButton2.bottom - m_rcItem.top);

    if ((m_uButton2State & UISTATE_DISABLED) != 0) {
        if (!m_sButton2DisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton2DisabledImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
        if (!m_sButton2PushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton2PushedImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uButton2State & UISTATE_HOT) != 0) {
        if (!m_sButton2HotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sButton2HotImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    }

    if (!m_sButton2NormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sButton2NormalImage, (LPCTSTR)m_sImageModify)) {
        } else {
            return;
        }
    }

    DWORD dwBorderColor = 0xFF85E4FF;
    int nBorderSize = 2;
    CRenderEngine::DrawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
}

void CScrollBarUI::PaintThumb(HDC hDC)
{
    if (m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0) {
        return;
    }
    if (!IsEnabled()) {
        m_uThumbState |= UISTATE_DISABLED;
    } else {
        m_uThumbState &= ~UISTATE_DISABLED;
    }

    m_sImageModify.Empty();
    m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"),
        CResourceManager::GetInstance()->RestoreScale(m_rcThumb.left - m_rcItem.left),
        CResourceManager::GetInstance()->RestoreScale(m_rcThumb.top - m_rcItem.top),
        CResourceManager::GetInstance()->RestoreScale(m_rcThumb.right - m_rcItem.left),
        CResourceManager::GetInstance()->RestoreScale(m_rcThumb.bottom - m_rcItem.top));

    if ((m_uThumbState & UISTATE_DISABLED) != 0) {
        if (!m_sThumbDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sThumbDisabledImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
        if (!m_sThumbPushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sThumbPushedImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_HOT) != 0) {
        if (!m_sThumbHotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sThumbHotImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    }

    if (!m_sThumbNormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sThumbNormalImage, (LPCTSTR)m_sImageModify)) {
        } else {
            return;
        }
    }

    DWORD dwBorderColor = 0xFF85E4FF;
    int nBorderSize = 2;
    CRenderEngine::DrawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
}

void CScrollBarUI::PaintRail(HDC hDC)
{
    if (m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0) {
        return;
    }
    if (!IsEnabled()) {
        m_uThumbState |= UISTATE_DISABLED;
    } else {
        m_uThumbState &= ~UISTATE_DISABLED;
    }
    const int multiples = 2;
    m_sImageModify.Empty();
    if (!m_bHorizontal) {
        m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), m_rcThumb.left - m_rcItem.left,
            (m_rcThumb.top + m_rcThumb.bottom) / multiples - m_rcItem.top - m_cxyFixed.cx / multiples,
            m_rcThumb.right - m_rcItem.left,
            (m_rcThumb.top + m_rcThumb.bottom) / multiples - m_rcItem.top + m_cxyFixed.cx - m_cxyFixed.cx / multiples);
    } else {
        m_sImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"),
            (m_rcThumb.left + m_rcThumb.right) / multiples - m_rcItem.left - m_cxyFixed.cy / multiples,
            m_rcThumb.top - m_rcItem.top,
            (m_rcThumb.left + m_rcThumb.right) / multiples - m_rcItem.left + m_cxyFixed.cy - m_cxyFixed.cy / multiples,
            m_rcThumb.bottom - m_rcItem.top);
    }

    if ((m_uThumbState & UISTATE_DISABLED) != 0) {
        if (!m_sRailDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sRailDisabledImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_PUSHED) != 0) {
        if (!m_sRailPushedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sRailPushedImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    } else if ((m_uThumbState & UISTATE_HOT) != 0) {
        if (!m_sRailHotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sRailHotImage, (LPCTSTR)m_sImageModify)) {
            } else {
                return;
            }
        }
    }

    if (!m_sRailNormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sRailNormalImage, (LPCTSTR)m_sImageModify)) {
        } else {
            return;
        }
    }
}

void CScrollBarUI::ButtonDownEvent(TEventUI &event)
{
    m_nLastScrollOffset = 0;
    m_nScrollRepeatDelay = 0;
    if (m_pManager == nullptr) {
        return;
    }
    m_pManager->SetTimer(this, DEFAULT_TIMERID, 10U);

    if (::PtInRect(&m_rcButton1, event.ptMouse)) {
        m_uButton1State |= UISTATE_PUSHED;
        if (!m_bHorizontal) {
            if (m_pOwner != nullptr) {
                m_pOwner->LineUp();
            } else {
                SetScrollPos(m_nScrollPos - m_nLineSize);
            }
        } else {
            if (m_pOwner != nullptr) {
                m_pOwner->LineLeft();
            } else {
                SetScrollPos(m_nScrollPos - m_nLineSize);
            }
        }
    } else if (::PtInRect(&m_rcButton2, event.ptMouse)) {
        m_uButton2State |= UISTATE_PUSHED;
        if (!m_bHorizontal) {
            if (m_pOwner != nullptr) {
                m_pOwner->LineDown();
            } else {
                SetScrollPos(m_nScrollPos + m_nLineSize);
            }
        } else {
            if (m_pOwner != nullptr) {
                m_pOwner->LineRight();
            } else {
                SetScrollPos(m_nScrollPos + m_nLineSize);
            }
        }
    } else if (::PtInRect(&m_rcThumb, event.ptMouse)) {
        m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
        ptLastMouse = event.ptMouse;
        m_nLastScrollPos = m_nScrollPos;
    } else {
        ButtonElseEvent(event);
    }
    if (m_pManager != nullptr /* && m_pOwner == nullptr */)
        m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
}

void CScrollBarUI::ButtonElseEvent(TEventUI &event)
{
    if (!m_bHorizontal) {
        if (event.ptMouse.y < m_rcThumb.top) {
            if (m_pOwner != nullptr) {
                m_pOwner->PageUp();
            } else {
                SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
            }
        } else if (event.ptMouse.y > m_rcThumb.bottom) {
            if (m_pOwner != nullptr) {
                m_pOwner->PageDown();
            } else {
                SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);
            }
        }
    } else {
        if (event.ptMouse.x < m_rcThumb.left) {
            if (m_pOwner != nullptr) {
                m_pOwner->PageLeft();
            } else {
                SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
            }
        } else if (event.ptMouse.x > m_rcThumb.right) {
            if (m_pOwner != nullptr) {
                m_pOwner->PageRight();
            } else {
                SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);
            }
        }
    }
}
bool CScrollBarUI::CheckMouseSatus(TEventUI &event)
{
    bool val = true;
    {
        unsigned short statusVal = static_cast<unsigned short>(GetAsyncKeyState(VK_LBUTTON));
        unsigned short status = ((statusVal & 0x8000) ? 1 : 0);
        unsigned short syncKeyState = static_cast<unsigned short>(GetAsyncKeyState(VK_LBUTTON));
        if (m_pManager == nullptr) {
            return false;
        }
        if ((syncKeyState & 0x8000) == 0) { // 左键已经松开了
            m_nScrollRepeatDelay = 0;
            m_nLastScrollOffset = 0;
            m_pManager->KillTimer(this, DEFAULT_TIMERID);
            if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
                m_uThumbState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
                Invalidate();
            }
            if ((m_uButton1State & UISTATE_PUSHED) != 0) {
                m_uButton1State &= ~UISTATE_PUSHED;
                Invalidate();
            }
            if ((m_uButton2State & UISTATE_PUSHED) != 0) {
                m_uButton2State &= ~UISTATE_PUSHED;
                Invalidate();
            }
            val = false;
        }
    }
    return val;
}

LRESULT CScrollBarUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
    bHandled = false;
    if (!IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
        return 0;
    }
    if (m_pManager == nullptr) {
        return false;
    }
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            if (!IsEnabled()) {
                return true;
            }
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            CControlUI *pControl = m_pManager->FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl != static_cast<CControlUI *>(this)) {
                break;
            }
            m_bCapture = true;
            m_pManager->SetCapture();

            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONDOWN;
            event.pSender = pControl;
            event.wParam = 0;
            event.lParam = 0;
            event.ptMouse = pt;
            event.wKeyState = (WORD)0;
            event.dwTimestamp = ::GetTickCount();
            ButtonDownEvent(event);

            bHandled = true;
            break;
        }
        case WM_LBUTTONUP:
        case WM_POINTERUP: {
            if (m_bCapture) {
                m_pManager->ReleaseCapture();
                m_nScrollRepeatDelay = 0;
                m_nLastScrollOffset = 0;
                m_pManager->KillTimer(this, DEFAULT_TIMERID);

                if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
                    m_uThumbState &= ~(UISTATE_CAPTURED | UISTATE_PUSHED);
                    Invalidate();
                } else if ((m_uButton1State & UISTATE_PUSHED) != 0) {
                    m_uButton1State &= ~UISTATE_PUSHED;
                    Invalidate();
                } else if ((m_uButton2State & UISTATE_PUSHED) != 0) {
                    m_uButton2State &= ~UISTATE_PUSHED;
                    Invalidate();
                }
                bHandled = true;
            }
            m_bCapture = false;

            break;
        }
        case WM_MOUSEMOVE: {
            if (!m_bCapture)
                return true;

            bHandled = true;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

            if ((m_uThumbState & UISTATE_CAPTURED) != 0) {
                if (!m_bHorizontal) {
                    int vRange = m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top;
                    if (vRange != 0) {
                        m_nLastScrollOffset = (pt.y - ptLastMouse.y) * m_nRange / vRange;
                    }
                } else {
                    int hRange = m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left;
                    if (hRange != 0) {
                        m_nLastScrollOffset = (pt.x - ptLastMouse.x) * m_nRange / hRange;
                    }
                }
            } else {
                if ((m_uThumbState & UISTATE_HOT) != 0) {
                    if (!::PtInRect(&m_rcThumb, pt)) {
                        m_uThumbState &= ~UISTATE_HOT;
                        Invalidate();
                    }
                } else {
                    if (!IsEnabled()) {
                        return true;
                    }
                    if (::PtInRect(&m_rcThumb, pt)) {
                        m_uThumbState |= UISTATE_HOT;
                        Invalidate();
                    }
                }
            }
            return true;
        }
        default:
            break;
    }

    return true;
}

void CScrollBarUI::DoInit()
{
    if (m_pManager) {
        m_pManager->AddPreMessageFilter(this);
    }
}


void CScrollBarUI::SetScrollBarWidth(int width_)
{
    const int widValue = 8;
    if (width_ < widValue) {
        width_ = widValue;
    }
    SetFixedWidth(width_);
    SetMinWidth(width_);
}
void CScrollBarUI::SetMinThumbWidth(int minThumbWidth)
{
    m_minThumbWidth = minThumbWidth;
}
}
