/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIProgress.h"

namespace DuiLib {
IMPLEMENT_DUICONTROL(CProgressUI)
constexpr int M_NMIN_VALUES = 0;
constexpr int PROCESS_UI_INIT_VAL = 0;
constexpr int M_NMAX_VALUES = 100;

CProgressUI::CProgressUI()
    : m_bShowText(false), m_bHorizontal(true), m_nMin(M_NMIN_VALUES), m_nMax(M_NMAX_VALUES),
      m_nValue(M_NMIN_VALUES), m_bStretchForeImage(true), m_cornerRadius(M_NMIN_VALUES), m_dwSweepWidth(0)
{
    constexpr int fixHigh = 12;
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    SetFixedHeight(fixHigh);
}

CProgressUI::~CProgressUI()
{
}

LPCTSTR CProgressUI::GetClass() const
{
    return _T("ProgressUI");
}

LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_PROGRESS) == 0) {
        return static_cast<CProgressUI *>(this);
    }
    return CLabelUI::GetInterface(pstrName);
}

bool CProgressUI::IsShowText()
{
    return m_bShowText;
}

void CProgressUI::SetShowText(bool bShowText)
{
    if (m_bShowText == bShowText) {
        return;
    }
    m_bShowText = bShowText;
    if (!m_bShowText) {
        SetText(_T(""));
    }
}

bool CProgressUI::IsHorizontal()
{
    return m_bHorizontal;
}

void CProgressUI::SetHorizontal(bool bHorizontal)
{
    if (m_bHorizontal == bHorizontal) {
        return;
    }

    m_bHorizontal = bHorizontal;
    Invalidate();
}

int CProgressUI::GetMinValue() const
{
    return m_nMin;
}

void CProgressUI::SetMinValue(int nMin)
{
    m_nMin = nMin;
    Invalidate();
}

int CProgressUI::GetMaxValue() const
{
    return m_nMax;
}

void CProgressUI::SetMaxValue(int nMax)
{
    m_nMax = nMax;
    Invalidate();
}

int CProgressUI::GetValue() const
{
    return m_nValue;
}

void CProgressUI::SetValue(int nValue)
{
    if (nValue == m_nValue || nValue < m_nMin || nValue > m_nMax) {
        return;
    }
    m_nValue = nValue;
    Invalidate();
    UpdateText();
}

void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("hor")) == 0) {
        SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("min")) == 0) {
        SetMinValue(_ttoi(pstrValue));
    } else if (_tcscmp(pstrName, _T("sweepwidth")) == 0) {
        m_dwSweepWidth = _ttoi(pstrValue);
    } else if (_tcsicmp(pstrName, _T("max")) == 0) {
        SetMaxValue(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("value")) == 0) {
        SetValue(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("isstretchfore")) == 0) {
        SetStretchForeImage(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
    } else if (_tcsicmp(pstrName, _T("cornerradius")) == 0) {
        SetCornerRadius(_ttoi(pstrValue));
    } else {
        CLabelUI::SetAttribute(pstrName, pstrValue);
    }
}

void CProgressUI::PaintForeColor(HDC hDC)
{
    if (m_dwForeColor == 0)
        return;

    if (m_nMax <= m_nMin) {
        m_nMax = m_nMin + 1;
    }
    if (m_nValue > m_nMax) {
        m_nValue = m_nMax;
    }
    if (m_nValue < m_nMin) {
        m_nValue = m_nMin;
    }

    RECT rc = m_rcItem;
    if (m_bHorizontal) {
        rc.right = m_rcItem.left + (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
    } else {
        rc.bottom = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
    }

    CRenderEngine::DrawColor(hDC, rc, GetAdjustColor(m_dwForeColor));
}

void CProgressUI::PaintForeImage(HDC hDC)
{
    if (m_nMax <= m_nMin) {
        m_nMax = m_nMin + 1;
    }
    if (m_nValue > m_nMax) {
        m_nValue = m_nMax;
    }
    if (m_nValue < m_nMin) {
        m_nValue = m_nMin;
    }

    RECT rc = { 0 };
    if (m_bHorizontal) {
        rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    } else {
        rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
        rc.right = m_rcItem.right - m_rcItem.left;
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }

    if (m_sForeImage.IsEmpty()) {
        return;
    }
    m_sForeImageModify.Empty();
    constexpr int offset = 2;
    constexpr int multiplyToStart = 2;
    if (m_bStretchForeImage) {
        m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"),
            DPI_UNSCALE(rc.left), DPI_UNSCALE(rc.top), DPI_UNSCALE(rc.right), DPI_UNSCALE(rc.bottom));
    } else if (m_cornerRadius == PROCESS_UI_INIT_VAL ||
        rc.right <= multiplyToStart * DPI_SCALE(m_cornerRadius) || !m_bHorizontal) {
        m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"),
            DPI_UNSCALE(rc.left), DPI_UNSCALE(rc.top), DPI_UNSCALE(rc.right), DPI_UNSCALE(rc.bottom),
            DPI_UNSCALE(rc.left), DPI_UNSCALE(rc.top), DPI_UNSCALE(rc.right), DPI_UNSCALE(rc.bottom));
    } else {
        // 当前只为水平进度条做了右端圆角
        RECT rightCornorSource = {
            m_rcItem.right - m_rcItem.left - DPI_SCALE(m_cornerRadius + offset),
            rc.top, m_rcItem.right - m_rcItem.left, rc.bottom
        };
        RECT rightCornorDest = { rc.right - DPI_SCALE(m_cornerRadius + offset), rc.top, rc.right, rc.bottom};

        m_cornorImageModify.Empty();
        m_cornorImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"),
            DPI_UNSCALE(rightCornorDest.left), DPI_UNSCALE(rightCornorDest.top),
            DPI_UNSCALE(rightCornorDest.right), DPI_UNSCALE(rightCornorDest.bottom),
            DPI_UNSCALE(rightCornorSource.left), DPI_UNSCALE(rightCornorSource.top),
            DPI_UNSCALE(rightCornorSource.right), DPI_UNSCALE(rightCornorSource.bottom));
        DrawImage(hDC, (LPCTSTR)m_sForeImage, (LPCTSTR)m_cornorImageModify);

        rc.right -= DPI_SCALE(m_cornerRadius + offset);
        m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'"),
            DPI_UNSCALE(rc.left), DPI_UNSCALE(rc.top), DPI_UNSCALE(rc.right), DPI_UNSCALE(rc.bottom),
            DPI_UNSCALE(rc.left), DPI_UNSCALE(rc.top), DPI_UNSCALE(rc.right), DPI_UNSCALE(rc.bottom));
    }

    DrawImage(hDC, (LPCTSTR)m_sForeImage, (LPCTSTR)m_sForeImageModify);
}

bool CProgressUI::IsStretchForeImage()
{
    return m_bStretchForeImage;
}

void CProgressUI::SetStretchForeImage(bool bStretchForeImage)
{
    if (m_bStretchForeImage == bStretchForeImage) {
        return;
    }
    m_bStretchForeImage = bStretchForeImage;
    Invalidate();
}

void CProgressUI::UpdateText()
{
    if (m_bShowText) {
        CDuiString sText;
        sText.Format(_T("%.0f%%"), (m_nValue - m_nMin) * 100.0f / (m_nMax - m_nMin));
        SetText(sText);
    }
}

void CProgressUI::SetCornerRadius(int value)
{
    m_cornerRadius = value;
}
}
