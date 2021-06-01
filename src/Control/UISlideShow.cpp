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
#include "UISlideShow.h"
#include "../Core/UIPainter.h"

constexpr int TIMER_ID_SLIDE = 0x1234;
constexpr int TIMER_ID_SLIDE_TIMEOUT = 0x1235;

constexpr int SLIDE_INTERVAL = 10;

namespace DuiLib {
IMPLEMENT_DUICONTROL(CSlideShowUI)
IMPLEMENT_DUICONTROL(CSlideShowPrevButtonUI)
IMPLEMENT_DUICONTROL(CSlideShowNextButtonUI)

using ChildrenList = std::vector<DuiLib::CControlUI *>;
using CachedBitmapList = std::vector<DuiLib::UIImage *>;
using CachedPointList = std::vector<Gdiplus::Rect>;

enum class SlideDirection {
    Next,
    Prev
};

class CSlideShowUIHelper {
public:
    ChildrenList childrenList;
    CachedBitmapList cachedBitmapList;
    CachedPointList cachedPointList;

    DuiLib::CButtonUI *btnPrev = nullptr;
    DuiLib::CButtonUI *btnNext = nullptr;

    int slideIndex = 0;
    int cachedWidth = 0;
    int cachedHeight = 0;
    CSlideShowUI *self = nullptr;
    int slideTimeout = 3000;
    int buttonOffset = 0;

    int slideCurrentIndex = 0;
    int slideDuring = 500;
    int slideSingleDistance = 0;
    int slideLeft = 0;
    bool sliding = false;

    int pointInflate = 0;
    int pointSize = 10;
    int pointBottomMargin = 0;
    int pointPadding = 10;

    SlideDirection direction = SlideDirection::Next;

    void SlideEnd()
    {
        if (self == nullptr) {
            return;
        }
        if (direction == SlideDirection::Next) {
            slideLeft = 0;
            slideIndex = slideCurrentIndex;
            if (slideDuring > 0) {
                self->SetTimer(TIMER_ID_SLIDE_TIMEOUT, slideTimeout);
            }

            sliding = false;
            self->KillTimer(TIMER_ID_SLIDE);
        } else {
            slideLeft = 0;

            slideIndex = slideCurrentIndex - 1;
            if (slideIndex < 0) {
                slideIndex = static_cast<int>(cachedBitmapList.size()) - 1;
            }

            if (slideDuring > 0) {
                self->SetTimer(TIMER_ID_SLIDE_TIMEOUT, slideTimeout);
            }

            sliding = false;
            self->KillTimer(TIMER_ID_SLIDE);
        }
    }

    void UpdateChildrenCache(int width, int height, bool force = false)
    {
        if (!self || width == 0 || height == 0) {
            return;
        }

        if (!force && cachedWidth == width && cachedHeight == height) {
            return;
        }

        cachedWidth = width;
        cachedHeight = height;

        cachedBitmapList.clear();

            for each(auto ctrl in childrenList) {
                    if (!ctrl || !ctrl->IsVisible()) {
                        continue;
                    }

                    DuiLib::UIImage *image = ctrl->GrabImage(SIZE { width, height });
                    if (image) {
                        cachedBitmapList.push_back(image);
                    }
                }

            if (btnPrev) {
                btnPrev->SetVisible(cachedBitmapList.size() > 1);
            }

            if (btnNext) {
                btnNext->SetVisible(cachedBitmapList.size() > 1);
            }

            cachedPointList.clear();

            int pointCount = static_cast<int>(cachedBitmapList.size());
            int pointWidth = DPI_SCALE(pointSize);
            int pointPadding_ = DPI_SCALE(pointPadding);
            int pointTotalWidth = pointWidth * pointCount + pointPadding_ * (pointCount - 1);
            int pointMargin = DPI_SCALE(pointBottomMargin);
            int pointTop = height - pointMargin - pointWidth;
            const int multiples = 2;
            int pointOffset = (width / multiples) - pointTotalWidth / multiples;

            for (int i = 0; i < pointCount; ++i) {
                Gdiplus::Rect rcPoint(pointOffset, pointTop, pointWidth, pointWidth);
                cachedPointList.push_back(rcPoint);
                pointOffset += (pointPadding_ + pointWidth);
            }

            slideIndex = 0;
    }

    bool OnButtonNotify(void *param)
    {
        auto noti = reinterpret_cast<TNotifyUI *>(param);
        if (!noti) {
            return true;
        }
        if (self == nullptr) {
            return false;
        }
        if (noti->sType == DUI_MSGTYPE_CLICK && noti->pSender == btnNext && btnNext) {
            self->SlideNext();
        }

        if (noti->sType == DUI_MSGTYPE_CLICK && noti->pSender == btnPrev && btnPrev) {
            self->SlidePrev();
        }

        return true;
    }
};

CSlideShowUI::CSlideShowUI() : helper(new CSlideShowUIHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->self = this;
}

CSlideShowUI::~CSlideShowUI()
{
    if (helper) {
        for each(auto image in helper->cachedBitmapList) {
            if (image) {
                delete image;
            }
        }

        helper->cachedBitmapList.clear();
        helper->childrenList.clear();

        helper->btnNext = nullptr;
        helper->btnPrev = nullptr;

        delete helper;
        helper = nullptr;
    }
}

void CSlideShowUI::SetSlideDuring(int during)
{
    if (helper == nullptr) {
        return;
    }
    helper->slideDuring = during;
}

void CSlideShowUI::SetSlideTimeout(int timeout)
{
    if (timeout == 0) {
        KillTimer(TIMER_ID_SLIDE_TIMEOUT);
        return;
    }
    if (helper == nullptr) {
        return;
    }
    helper->slideTimeout = timeout;
    KillTimer(TIMER_ID_SLIDE_TIMEOUT);
    SetTimer(TIMER_ID_SLIDE_TIMEOUT, timeout);
}

void CSlideShowUI::SlideNext()
{
    if (helper == nullptr) {
        return;
    }
    if (helper->sliding && helper->direction == SlideDirection::Next) {
        return;
    }

    KillTimer(TIMER_ID_SLIDE_TIMEOUT);

    bool increaseOne = !helper->sliding || helper->direction == SlideDirection::Next;

    helper->direction = SlideDirection::Next;

    int slideTotalDistance = m_rcItem.right - m_rcItem.left;
    int totalFrameCount = helper->slideDuring / SLIDE_INTERVAL;
    if (totalFrameCount == 0) {
        return;
    }

    helper->slideSingleDistance = slideTotalDistance / totalFrameCount;
    helper->slideCurrentIndex = helper->slideIndex + (increaseOne ? 1 : 0);
    if (helper->slideCurrentIndex >= static_cast<int>(helper->cachedBitmapList.size())) {
        helper->slideCurrentIndex = 0;
    }

    if (!helper->sliding) {
        helper->slideLeft = slideTotalDistance;
    }

    helper->sliding = true;
    SetTimer(TIMER_ID_SLIDE, SLIDE_INTERVAL);
}

void CSlideShowUI::SlidePrev()
{
    if (helper == nullptr) {
        return;
    }
    if (helper->sliding && helper->direction == SlideDirection::Prev) {
        return;
    }

    KillTimer(TIMER_ID_SLIDE_TIMEOUT);

    bool decreaseOne = !helper->sliding || helper->direction == SlideDirection::Prev;

    helper->direction = SlideDirection::Prev;

    int slideTotalDistance = m_rcItem.right - m_rcItem.left;
    int totalFrameCount = helper->slideDuring / SLIDE_INTERVAL;
    if (totalFrameCount == 0) {
        return;
    }

    helper->slideSingleDistance = slideTotalDistance / totalFrameCount;
    helper->slideCurrentIndex = helper->slideIndex + (decreaseOne ? 0 : 1);
    if (helper->slideCurrentIndex >= helper->cachedBitmapList.size()) {
        helper->slideCurrentIndex = 0;
    }

    if (!helper->sliding) {
        helper->slideLeft = 0;
    }

    helper->sliding = true;
    SetTimer(TIMER_ID_SLIDE, SLIDE_INTERVAL);
}

LPCTSTR CSlideShowUI::GetClass() const
{
    return _T("SlideShowUI");
}

UINT CSlideShowUI::GetControlFlags() const
{
    if (IsEnabled()) {
        return UIFLAG_SETCURSOR;
    } else {
        return 0;
    }
}

LPVOID CSlideShowUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_SLIDESHOW) == 0) {
        return static_cast<CSlideShowUI *>(this);
    }
    return __super::GetInterface(pstrName);
}

void CSlideShowUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    CDuiString wstrName = pstrName;
    CDuiString wstrValue = pstrValue;
    if (wstrName == _T("slidetimeout")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            SetSlideTimeout(ret);
        }
    }
    if (wstrName == _T("slideduring")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->slideDuring = ret;
        }
    }
    if (wstrName == _T("buttonoffset")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->buttonOffset = ret;
        }
    }
    if (wstrName == _T("pointsize")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->pointSize = ret;
        }
    }
    if (wstrName == _T("pointbottommargin")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->pointBottomMargin = ret;
        }
    }
    if (wstrName == _T("pointpadding")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->pointPadding = ret;
        }
    }
    if (wstrName == _T("pointinflate")) {
        bool ok = false;
        int ret = wstrValue.ToInt(&ok);
        if (ok) {
            helper->pointInflate = ret;
        }
    } else {
        __super::SetAttribute(pstrName, pstrValue);
    }
}

bool CSlideShowUI::Add(CControlUI *pControl)
{
    if (!pControl) {
        return false;
    }
    if (helper == nullptr) {
        return false;
    }
    auto btnPrev = dynamic_cast<CSlideShowPrevButtonUI *>(pControl);
    if (btnPrev) {
        if (helper->btnPrev) {
            return false;
        }

        btnPrev->SetFloat(true);
        btnPrev->OnNotify += DuiLib::MakeDelegate(helper, &CSlideShowUIHelper::OnButtonNotify);
        helper->btnPrev = btnPrev;
    }

    auto btnNext = dynamic_cast<CSlideShowNextButtonUI *>(pControl);
    if (btnNext) {
        if (helper->btnNext) {
            return false;
        }

        btnNext->SetFloat(true);
        btnNext->OnNotify += DuiLib::MakeDelegate(helper, &CSlideShowUIHelper::OnButtonNotify);
        helper->btnNext = btnNext;
    }

    if (!btnPrev && !btnNext) {
        helper->childrenList.push_back(pControl);
    }

    return __super::Add(pControl);
}

bool CSlideShowUI::Remove(CControlUI *pControl)
{
    if (helper == nullptr) {
        return false;
    }
    ChildrenList::iterator it = std::find(helper->childrenList.begin(), helper->childrenList.end(), pControl);
    if (it != helper->childrenList.end()) {
        helper->childrenList.erase(it);
        helper->UpdateChildrenCache(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
    }

    if (pControl == helper->btnNext) {
        helper->btnNext = nullptr;
    }

    if (pControl == helper->btnPrev) {
        helper->btnPrev = nullptr;
    }

    return __super::Remove(pControl);
}

void CSlideShowUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    __super::SetPos(rc, bNeedInvalidate);

    int width = m_rcItem.right - m_rcItem.left;
    int height = m_rcItem.bottom - m_rcItem.top;
    if (helper == nullptr) {
        return;
    }
    if (width > 0 && height > 0) {
        helper->UpdateChildrenCache(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
    }

    if (width == 0 || height == 0) {
        return;
    }
    const int multiples = 2;
    if (helper->btnPrev) {
        int btnWidth = helper->btnPrev->GetFixedWidth();
        int btnHeight = helper->btnPrev->GetFixedHeight();
        if (btnWidth > 0 && btnHeight > 0) {
            int left = 0; // rc.left;
            int top = (height - btnHeight) / multiples + DPI_SCALE(helper->buttonOffset);
            helper->btnPrev->SetPos(RECT { left, top, left + btnWidth, top + btnHeight }, bNeedInvalidate);
        }
    }

    if (helper->btnNext) {
        int btnWidth = helper->btnNext->GetFixedWidth();
        int btnHeight = helper->btnNext->GetFixedHeight();
        if (btnWidth > 0 && btnHeight > 0) {
            int left = width - btnWidth;
            int top = (height - btnHeight) / multiples + DPI_SCALE(helper->buttonOffset);
            helper->btnNext->SetPos(RECT { left, top, left + btnWidth, top + btnHeight }, bNeedInvalidate);
        }
    }
}

void CSlideShowUI::DoInit()
{
    if (helper == nullptr) {
        return;
    }
    if (helper->slideDuring > 0) {
        SetTimer(TIMER_ID_SLIDE_TIMEOUT, helper->slideTimeout);
    }

    if (helper->btnNext) {
        helper->btnNext->SetManager(GetManager(), this);
    }

    if (helper->btnPrev) {
        helper->btnPrev->SetManager(GetManager(), this);
    }
}

void CSlideShowUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    PaintBkColor(hDC);
    PaintBorder(hDC);
    PaintBkImage(hDC);
    PaintStatusImage(hDC);
    PaintForeColor(hDC);
    PaintForeImage(hDC);

    DrawSliding(hDC, rcPaint);

    DrawPoints(hDC, rcPaint);
    if (helper == nullptr) {
        return;
    }

    if (helper->btnPrev && helper->btnPrev->IsVisible()) {
        helper->btnPrev->DoPaint(hDC, rcPaint);
    }

    if (helper->btnNext && helper->btnNext->IsVisible()) {
        helper->btnNext->DoPaint(hDC, rcPaint);
    }
}

void CSlideShowUI::DoEvent(TEventUI &ev)
{
    if (helper == nullptr) {
        return;
    }
    switch (ev.Type) {
        case UIEVENT_TIMER: {
            switch (ev.wParam) {
                case TIMER_ID_SLIDE: {
                    if (helper->direction == SlideDirection::Next) {
                        helper->slideLeft -= helper->slideSingleDistance;
                        if (helper->slideLeft <= 0) {
                            helper->SlideEnd();
                        }
                    } else {
                        helper->slideLeft += helper->slideSingleDistance;
                        if (helper->slideLeft >= m_rcItem.right - m_rcItem.left) {
                            helper->SlideEnd();
                        }
                    }

                    Invalidate();
                } break;
                case TIMER_ID_SLIDE_TIMEOUT: {
                    if (helper->cachedBitmapList.size() > 1) {
                        SlideNext();
                    }
                } break;
                default:
                    break;
            }
        } break;
        case UIEVENT_BUTTONDOWN: {
            if (helper->cachedPointList.size() > 1) {
                int index = 0;
                bool clickedOnPoint = false;
                for each(auto pt in helper->cachedPointList) {
                        Gdiplus::Rect rcPoint = pt;
                        rcPoint.X += m_rcItem.left;
                        rcPoint.Y += m_rcItem.top;

                        if (rcPoint.Contains(ev.ptMouse.x, ev.ptMouse.y)) {
                            clickedOnPoint = true;
                            break;
                        }

                        index++;
                    }

                if (clickedOnPoint) {
                    ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(IDC_HAND)));
                    KillTimer(TIMER_ID_SLIDE);
                    KillTimer(TIMER_ID_SLIDE_TIMEOUT);
                    helper->sliding = false;
                    helper->slideIndex = index;
                    Invalidate();
                    if (helper->slideTimeout) {
                        SetTimer(TIMER_ID_SLIDE_TIMEOUT, helper->slideTimeout);
                    }
                }
            }
        } break;
        case UIEVENT_DBLCLICK:
        case UIEVENT_MOUSEMOVE: {
            if (helper->cachedPointList.size() > 1) {
                bool clickedOnPoint = false;
                for each(auto pt in helper->cachedPointList) {
                        Gdiplus::Rect rcPoint = pt;
                        rcPoint.X += m_rcItem.left;
                        rcPoint.Y += m_rcItem.top;

                        if (rcPoint.Contains(ev.ptMouse.x, ev.ptMouse.y)) {
                            ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(IDC_HAND)));
                            break;
                        }
                    }
            }
        } break;
        default:
            break;
    }
    __super::DoEvent(ev);
}

void CSlideShowUI::SetInternVisible(bool bVisible)
{
    if (helper == nullptr) {
        return;
    }
    if (bVisible) {
        if (helper->slideTimeout > 0) {
            SetTimer(TIMER_ID_SLIDE_TIMEOUT, helper->slideTimeout);
        }
    } else {
        helper->sliding = false;
        KillTimer(TIMER_ID_SLIDE);
        KillTimer(TIMER_ID_SLIDE_TIMEOUT);
    }

    __super::SetInternVisible(bVisible);
}

void CSlideShowUI::SetVisible(bool bVisible)
{
    if (helper == nullptr) {
        return;
    }
    if (bVisible) {
        if (helper->slideTimeout > 0) {
            SetTimer(TIMER_ID_SLIDE_TIMEOUT, helper->slideTimeout);
        }
    } else {
        helper->sliding = false;
        KillTimer(TIMER_ID_SLIDE);
        KillTimer(TIMER_ID_SLIDE_TIMEOUT);
    }

    __super::SetVisible(bVisible);
}

void CSlideShowUI::DrawSliding(HDC hDC, const RECT &rcPaint)
{
    DuiLib::UIPainter painter(this, hDC);
    if (helper == nullptr) {
        return;
    }

    if (helper->cachedBitmapList.empty()) {
        __super::DoPaint(hDC, rcPaint);
        return;
    }

    if (!helper->sliding) {
        int drawIndex = 0;
        if (helper->slideIndex < helper->cachedBitmapList.size()) {
            drawIndex = helper->slideIndex;
        }

        painter.DrawImage(0, 0, helper->cachedBitmapList[drawIndex]);
        return;
    }

    if (helper->slideCurrentIndex >= helper->cachedBitmapList.size()) {
        return;
    }

    int prevIndex = helper->slideCurrentIndex - 1;
    if (prevIndex < 0) {
        prevIndex = static_cast<int>(helper->cachedBitmapList.size()) - 1;
    }

    int nextIndex = helper->slideCurrentIndex + 1;
    if (nextIndex >= helper->cachedBitmapList.size()) {
        nextIndex = 0;
    }

    UIImage *imageLeft = helper->cachedBitmapList[prevIndex];
    UIImage *imageMid = helper->cachedBitmapList[helper->slideCurrentIndex];
    UIImage *imageRight = helper->cachedBitmapList[nextIndex];

    if (!imageLeft || !imageMid || !imageRight) {
        return;
    }

    int width = m_rcItem.right - m_rcItem.left;
    if (helper->slideLeft > 0) {
        painter.DrawImage(helper->slideLeft - width, 0, imageLeft);
    }

    painter.DrawImage(helper->slideLeft, 0, imageMid);

    if (helper->slideLeft + width > 0) {
        painter.DrawImage(helper->slideLeft + width, 0, imageRight);
    }
}

void CSlideShowUI::DrawPoints(HDC hDC, const RECT &rcPaint)
{
    const int multiples = 2;
    if (helper == nullptr) {
        return;
    }
    if (helper->cachedPointList.size() < multiples) {
        return;
    }


    int pointCount = static_cast<int>(helper->cachedPointList.size());
    int width = m_rcItem.right - m_rcItem.left;
    int pointWidth = DPI_SCALE(helper->pointSize);
    int pointPadding = DPI_SCALE(helper->pointPadding);
    int pointTotalWidth = pointWidth * pointCount + pointPadding * (pointCount - 1);
    int pointMargin = DPI_SCALE(helper->pointBottomMargin);
    int pointTop = m_rcItem.bottom - pointMargin - pointWidth;

    int pointOffset = m_rcItem.left + (width / multiples) - pointTotalWidth / multiples;

    Gdiplus::Graphics graphics(hDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    int index = 0;
        for each(auto pt in helper->cachedPointList) {
                DWORD pointColor = 0x80aaaaaa;
                if (!helper->sliding && index == helper->slideIndex) {
                    if (helper->pointInflate > 0) {
                        pt.Inflate(DPI_SCALE(helper->pointInflate), DPI_SCALE(helper->pointInflate));
                    }
                    pointColor = 0xff007dff;
                }

                Gdiplus::SolidBrush brush(pointColor);
                Gdiplus::Rect rcPoint = pt;
                rcPoint.X += m_rcItem.left;
                rcPoint.Y += m_rcItem.top;
                graphics.FillEllipse(&brush, rcPoint);
                pointOffset += (pointPadding + pointWidth);
                index++;
            }

        if (helper->sliding && width > 0) {
            int pointIndexStart = helper->slideIndex;
            int pointIndexEnd = helper->slideIndex + (helper->direction == SlideDirection::Next ? 1 : -1);
            if (pointIndexEnd >= static_cast<int>(helper->cachedPointList.size())) {
                pointIndexEnd = 0;
            } else if (pointIndexEnd < 0) {
                pointIndexEnd = static_cast<int>(helper->cachedPointList.size()) - 1;
            }

            Gdiplus::Rect rcPointStart = helper->cachedPointList[pointIndexStart];
            Gdiplus::Rect rcPointEnd = helper->cachedPointList[pointIndexEnd];

            rcPointStart.X += (INT)((rcPointEnd.X - rcPointStart.X) * (helper->direction == SlideDirection::Next ?
                ((width - helper->slideLeft) / static_cast<double>(width)) :
                ((helper->slideLeft) / static_cast<double>(width))));

            rcPointStart.X += m_rcItem.left;
            rcPointStart.Y += m_rcItem.top;
            DWORD pointColor = 0xff007dff;
            Gdiplus::SolidBrush brush(pointColor);

            if (helper->pointInflate > 0) {
                rcPointStart.Inflate(DPI_SCALE(helper->pointInflate), DPI_SCALE(helper->pointInflate));
            }

            graphics.FillEllipse(&brush, rcPointStart);
        }
}

void CSlideShowUI::Update()
{
    int width = m_rcItem.right - m_rcItem.left;
    int height = m_rcItem.bottom - m_rcItem.top;
    if (helper == nullptr) {
        return;
    }
    if (width > 0 && height > 0) {
        helper->UpdateChildrenCache(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, true);
        helper->sliding = false;
        KillTimer(TIMER_ID_SLIDE);
        KillTimer(TIMER_ID_SLIDE_TIMEOUT);
        SetTimer(TIMER_ID_SLIDE_TIMEOUT, helper->slideTimeout);
    }
}
}
