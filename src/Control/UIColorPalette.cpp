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
#include <cmath>

namespace DuiLib {
constexpr int HSLMAX = 255; /* H,L, and S vary over 0-HSLMAX */
constexpr int RGBMAX = 255; /* R,G, and B vary over 0-RGBMAX */
#define HSLUNDEFINED (HSLMAX * 2 / 3)

/*
 * Convert hue value to RGB
 */
static float HueToRGB(float v1, float v2, float vH)
{
    if (vH < 0.0f) {
        vH += 1.0f;
    }
    if (vH > 1.0f) {
        vH -= 1.0f;
    }
    if ((6.0f * vH) < 1.0f) {
        return (v1 + (v2 - v1) * 6.0f * vH);
    }
    if ((2.0f * vH) < 1.0f) {
        return (v2);
    }
    if ((3.0f * vH) < 2.0f) {
        return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6.0f);
    }
    return (v1);
}

/*
 * Convert color RGB to HSL
 * pHue HSL hue value [0 - 1]
 * pSat HSL saturation value [0 - 1]
 * pLue HSL luminance value [0 - 1]
 */
static void RGBToHSL(DWORD clr, float *pHue, float *pSat, float *pLue)
{
    const float maxValue = 255.0f; // RGB最大值为255
    float R = static_cast<float>(GetRValue(clr) / maxValue); // RGB from 0 to 255
    float G = static_cast<float>(GetGValue(clr) / maxValue);
    float B = static_cast<float>(GetBValue(clr) / maxValue);

    float H, S, L;

    float fMin = min(R, min(G, B)); // Min. value of RGB
    float fMax = max(R, max(G, B)); // Max. value of RGB
    float fDelta = fMax - fMin;     // Delta RGB value
    const float len = 0.5;
    L = (fMax + fMin) / 2.0f;

    if (fDelta == 0) { // This is a gray, no chroma...
        H = 0.0f;      // HSL results from 0 to 1
        S = 0.0f;
    } else { // Chromatic data...
        float del_R, del_G, del_B;

        if (L < len) {
            S = fDelta / (fMax + fMin);
        } else {
            S = fDelta / (2.0f - fMax - fMin);
        }

        del_R = (((fMax - R) / 6.0f) + (fDelta / 2.0f)) / fDelta;
        del_G = (((fMax - G) / 6.0f) + (fDelta / 2.0f)) / fDelta;
        del_B = (((fMax - B) / 6.0f) + (fDelta / 2.0f)) / fDelta;

        H = 0.0f;
        if (R == fMax) {
            H = del_B - del_G;
        } else if (G == fMax) {
            H = (1.0f / 3.0f) + del_R - del_B;
        } else if (B == fMax) {
            H = (2.0f / 3.0f) + del_G - del_R;
        }

        if (H < 0.0f) {
            H += 1.0f;
        }
        if (H > 1.0f) {
            H -= 1.0f;
        }
    }

    *pHue = H;
    *pSat = S;
    *pLue = L;
}

/*
 * Convert color HSL to RGB
 * H HSL hue value [0 - 1]
 * S HSL saturation value [0 - 1]
 * L HSL luminance value [0 - 1]
 */
static DWORD HSLToRGB(float H, float S, float L)
{
    BYTE R, G, B;
    float var1;
    float var2;
    const float len = 0.5;

    if (S == 0) {                       // HSL from 0 to 1
        R = G = B = (BYTE)(L * 255.0f); // RGB results from 0 to 255
    } else {
        if (L < len) {
            var2 = L * (1.0f + S);
        } else {
            var2 = (L + S) - (S * L);
        }

        var1 = 2.0f * L - var2;

        R = (BYTE)(255.0f * HueToRGB(var1, var2, H + (1.0f / 3.0f)));
        G = (BYTE)(255.0f * HueToRGB(var1, var2, H));
        B = (BYTE)(255.0f * HueToRGB(var1, var2, H - (1.0f / 3.0f)));
    }

    return RGB(R, G, B);
}

/*
 * _HSLToRGB color HSL value to RGB
 * clr  RGB color value
 * nHue HSL hue value			[0 - 360]
 * nSat HSL saturation value		[0 - 200]
 * nLue HSL luminance value		[0 - 200]
 */
#define _HSLToRGB(h, s, l) ((0xFF << 24) | \
HSLToRGB(static_cast<float>(h) / 360.0f, static_cast<float>(s) / 200.0f, l / 200.0f))

IMPLEMENT_DUICONTROL(CColorPaletteUI)

const int CURRENT_H = 180; // 当前色相
const int CURRENT_S = 200; // 当前饱和度
const int CURRENT_B = 100; // 当前亮度
const int PALLET_HEIGHT = 200; // 托盘高
const int BAR_HEIGHT = 10; // 滚动条高
const int MAX_HEIGHT = 200; // 最大高度
const int MAX_WIDTH = 360; // 最大宽度
const int BIT_TO_BYTE = 8; // 位转成字节需要除以8

CColorPaletteUI::CColorPaletteUI()
    : m_uButtonState(0),
      m_bIsInBar(false),
      m_bIsInPallet(false),
      m_nCurH(CURRENT_H),
      m_nCurS(CURRENT_S),
      m_nCurB(CURRENT_B),
      m_nPalletHeight(PALLET_HEIGHT),
      m_nBarHeight(BAR_HEIGHT),
      m_pBits(nullptr),
      m_MemDc(nullptr)
{
    SecureZeroMemory(&m_bmInfo, sizeof(m_bmInfo));

    m_hMemBitmap = nullptr;
}

CColorPaletteUI::~CColorPaletteUI()
{
    if (m_pBits)
        free(m_pBits);

    if (m_hMemBitmap) {
        ::DeleteObject(m_hMemBitmap);
    }
}

DWORD CColorPaletteUI::GetSelectColor()
{
    const int highOffset = 8;
    const int midOffset = 16;
    const int lowOffset = 24;
    DWORD dwColor = _HSLToRGB(m_nCurH, m_nCurS, m_nCurB);
    // 返回RGB：高八位为透明度、低24位为RGB
    return static_cast<DWORD>((0xFF << lowOffset) | (GetRValue(dwColor) << midOffset) |
        (GetGValue(dwColor) << highOffset) | GetBValue(dwColor));
}

void CColorPaletteUI::SetSelectColor(DWORD dwColor)
{
    float H = 0;
    float S = 0;
    float B = 0;
    const int hueValue = 360;
    const int sbValue = 200;
    COLORREF dwBkClr = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    RGBToHSL(dwBkClr, &H, &S, &B);
    m_nCurH = static_cast<int>(H * hueValue); // 转换为当前色相
    m_nCurS = static_cast<int>(S * sbValue); // 转换为当前饱和度
    m_nCurB = static_cast<int>(B * sbValue); // 转换为当前亮度
    NeedUpdate();
    UpdatePalletData();
}

LPCTSTR CColorPaletteUI::GetClass() const
{
    return _T("ColorPaletteUI");
}

LPVOID CColorPaletteUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_COLORPALETTE) == 0)
        return static_cast<CColorPaletteUI *>(this);
    return CControlUI::GetInterface(pstrName);
}

void CColorPaletteUI::SetPalletHeight(int nHeight)
{
    m_nPalletHeight = nHeight;
}
int CColorPaletteUI::GetPalletHeight() const
{
    return m_nPalletHeight;
}
void CColorPaletteUI::SetBarHeight(int nHeight)
{
    // 限制最大高度，由于当前设计，nheight超出190，程序会因越界访问崩溃
    const int restrictHeight = 150;
    if (nHeight > restrictHeight) {
        nHeight = restrictHeight;
    }
    m_nBarHeight = nHeight;
}
int CColorPaletteUI::GetBarHeight() const
{
    return m_nBarHeight;
}

void CColorPaletteUI::SetThumbImage(LPCTSTR pszImage)
{
    if (m_strThumbImage != pszImage) {
        m_strThumbImage = pszImage;
        NeedUpdate();
    }
}

LPCTSTR CColorPaletteUI::GetThumbImage() const
{
    return m_strThumbImage.GetData();
}

void CColorPaletteUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("palletheight")) == 0)
        SetPalletHeight(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("barheight")) == 0)
        SetBarHeight(_ttoi(pstrValue));
    else if (_tcscmp(pstrName, _T("thumbimage")) == 0)
        SetThumbImage(pstrValue);
    else
        CControlUI::SetAttribute(pstrName, pstrValue);
}

void CColorPaletteUI::DoInit()
{
    // 位图的x、y坐标
    const int compatibleBitmapX = 400;
    const int compatibleBitmapY = 360;
    m_MemDc = CreateCompatibleDC(GetManager()->GetPaintDC());
    m_hMemBitmap = CreateCompatibleBitmap(GetManager()->GetPaintDC(),
        compatibleBitmapX, compatibleBitmapY);
    HBITMAP pOldBit = (HBITMAP)SelectObject(m_MemDc, m_hMemBitmap);

    ::GetObject(m_hMemBitmap, sizeof(m_bmInfo), &m_bmInfo);
    if (m_bmInfo.bmHeight > (MAXLONG / m_bmInfo.bmWidthBytes)) {
        return;
    }
    DWORD dwSize = m_bmInfo.bmHeight * m_bmInfo.bmWidthBytes;
    if (dwSize > 0) {
        m_pBits = reinterpret_cast<BYTE *>(malloc(dwSize));
        if (m_pBits == nullptr) {
            return;
        }
        ::GetBitmapBits(m_hMemBitmap, dwSize, m_pBits);
    }
}

void CColorPaletteUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);

    m_ptLastPalletMouse.x = m_nCurH * (m_rcItem.right - m_rcItem.left) / MAX_WIDTH + m_rcItem.left;
    m_ptLastPalletMouse.y = (MAX_HEIGHT - m_nCurB) * m_nPalletHeight / MAX_HEIGHT + m_rcItem.top;

    UpdatePalletData();
    UpdateBarData();
}

void CColorPaletteUI::DoEvent(TEventUI &event)
{
    CControlUI::DoEvent(event);
    if (event.Type == UIEVENT_BUTTONDOWN) {
        if (event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.top && event.ptMouse.x < m_rcItem.right &&
            event.ptMouse.y < m_rcItem.top + m_nPalletHeight) {
            int x = (event.ptMouse.x - m_rcItem.left) * MAX_WIDTH / (m_rcItem.right - m_rcItem.left);
            int y = (event.ptMouse.y - m_rcItem.top) * MAX_HEIGHT / m_nPalletHeight;
            x = min(max(x, 0), MAX_WIDTH);
            y = min(max(y, 0), MAX_HEIGHT);
            m_ptLastPalletMouse = event.ptMouse;
            if (m_ptLastPalletMouse.x < m_rcItem.left) {
                m_ptLastPalletMouse.x = m_rcItem.left;
            }
            if (m_ptLastPalletMouse.x > m_rcItem.right) {
                m_ptLastPalletMouse.x = m_rcItem.right;
            }
            if (m_ptLastPalletMouse.y < m_rcItem.top) {
                m_ptLastPalletMouse.y = m_rcItem.top;
            }
            if (m_ptLastPalletMouse.y > m_rcItem.top + m_nPalletHeight) {
                m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;
            }
            const int yBrightValue = 200;
            m_nCurH = x;
            m_nCurB = yBrightValue - y;
            m_uButtonState |= UISTATE_PUSHED;
            m_bIsInPallet = true;
            m_bIsInBar = false;
            UpdateBarData();
        }
        // ::PtInRect(&m_rcItem, event.ptMouse)
        if (event.ptMouse.x >= m_rcItem.left && event.ptMouse.y >= m_rcItem.bottom - m_nBarHeight &&
            event.ptMouse.x < m_rcItem.right && event.ptMouse.y < m_rcItem.bottom) {
            m_nCurS = (event.ptMouse.x - m_rcItem.left) * MAX_HEIGHT /
                (m_rcItem.right - m_rcItem.left);
            m_uButtonState |= UISTATE_PUSHED;
            m_bIsInBar = true;
            m_bIsInPallet = false;
            UpdatePalletData();
        }
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_BUTTONUP) {
        DWORD color = 0;
        if ((m_uButtonState | UISTATE_PUSHED) && (IsEnabled())) {
            color = GetSelectColor();
            if (m_pManager == nullptr) {
                return;
            }
            m_pManager->SendNotify(this, DUI_MSGTYPE_COLORCHANGED, color, 0);
        }
        m_uButtonState &= ~UISTATE_PUSHED;
        m_bIsInPallet = false;
        m_bIsInBar = false;
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_MOUSEMOVE) {
        if (!(m_uButtonState & UISTATE_PUSHED)) {
            m_bIsInBar = false;
            m_bIsInPallet = false;
        }
        if (m_bIsInPallet == true) {
            POINT pt = event.ptMouse;
            pt.x -= m_rcItem.left;
            pt.y -= m_rcItem.top;
            if (pt.x >= 0 && pt.y >= 0 && pt.x <= m_rcItem.right &&
                pt.y <= m_rcItem.top + m_nPalletHeight) {
                int x = pt.x * MAX_WIDTH / (m_rcItem.right - m_rcItem.left);
                int y = pt.y * MAX_HEIGHT / m_nPalletHeight;
                x = min(max(x, 0), MAX_WIDTH);
                y = min(max(y, 0), MAX_HEIGHT);
                m_ptLastPalletMouse = event.ptMouse;
                if (m_ptLastPalletMouse.x < m_rcItem.left) {
                    m_ptLastPalletMouse.x = m_rcItem.left;
                }
                if (m_ptLastPalletMouse.x > m_rcItem.right) {
                    m_ptLastPalletMouse.x = m_rcItem.right;
                }
                if (m_ptLastPalletMouse.y < m_rcItem.top) {
                    m_ptLastPalletMouse.y = m_rcItem.top;
                }
                if (m_ptLastPalletMouse.y > m_rcItem.top + m_nPalletHeight) {
                    m_ptLastPalletMouse.y = m_rcItem.top + m_nPalletHeight;
                }
                m_nCurH = x;
                m_nCurB = MAX_HEIGHT - y;
                UpdateBarData();
            }
        }
        if (m_bIsInBar == true) {
            m_nCurS = (event.ptMouse.x - m_rcItem.left) * MAX_HEIGHT / (m_rcItem.right - m_rcItem.left);
            m_nCurS = min(max(m_nCurS, 0), MAX_HEIGHT);
            UpdatePalletData();
        }
        Invalidate();
        return;
    }
}

void CColorPaletteUI::PaintBkColor(HDC hDC)
{
    PaintPallet(hDC);
}

void CColorPaletteUI::PaintPallet(HDC hDC)
{
    int nSaveDC = ::SaveDC(hDC);

    ::SetStretchBltMode(hDC, HALFTONE); // 拉伸模式将内存图画到控件上
    // 拉伸模式下的最值情况
    const int minPalletHeight = 199;
    const int maxPalletHeight = 360;
    const int minBarHeight = 200;
    const int maxBarHeight = 210;
    const int rcStep = 4;
    const int itemLeftMul = 200;
    const int barHeightMul = 2;
    StretchBlt(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
        m_nPalletHeight, m_MemDc, 0, 0, maxPalletHeight, minPalletHeight, SRCCOPY);
    StretchBlt(hDC, m_rcItem.left, m_rcItem.bottom - m_nBarHeight, m_rcItem.right - m_rcItem.left,
        m_nBarHeight, m_MemDc, 0, maxBarHeight, minBarHeight, m_nBarHeight, SRCCOPY);

    RECT rcCurSorPaint = { m_ptLastPalletMouse.x - rcStep, m_ptLastPalletMouse.y - rcStep,
        m_ptLastPalletMouse.x + rcStep, m_ptLastPalletMouse.y + rcStep };
    CRenderEngine::DrawImageString(hDC, m_pManager, rcCurSorPaint, m_rcPaint, m_strThumbImage);

    rcCurSorPaint.left = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / itemLeftMul - rcStep;
    rcCurSorPaint.right = m_rcItem.left + m_nCurS * (m_rcItem.right - m_rcItem.left) / itemLeftMul + rcStep;
    rcCurSorPaint.top = m_rcItem.bottom - m_nBarHeight / barHeightMul - rcStep;
    rcCurSorPaint.bottom = m_rcItem.bottom - m_nBarHeight / barHeightMul + rcStep;
    CRenderEngine::DrawImageString(hDC, m_pManager, rcCurSorPaint, m_rcPaint, m_strThumbImage);
    ::RestoreDC(hDC, nSaveDC);
}

void CColorPaletteUI::UpdatePalletData()
{
    int x;
    int y;
    BYTE *pPiexl = nullptr;
    DWORD dwColor;
    const int maxValue = 199;
    for (y = 0; y < MAX_HEIGHT; ++y) {
        for (x = 0; x < MAX_WIDTH; ++x) {
            pPiexl = LPBYTE(m_pBits) + ((maxValue - y) * m_bmInfo.bmWidthBytes) +
                ((x * m_bmInfo.bmBitsPixel) / BIT_TO_BYTE);
            dwColor = _HSLToRGB(x, m_nCurS, y);
            pPiexl[0] = GetBValue(dwColor);
            pPiexl[1] = GetGValue(dwColor);
            pPiexl[2] = GetRValue(dwColor);
        }
    }

    SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
}


void CColorPaletteUI::UpdateBarData()
{
    int x;
    int y;
    BYTE *pPiexl = nullptr;
    DWORD dwColor;
    const int maxWidth = 200;
    const int addHeight = 210;
    // 这里画出Bar
    for (y = 0; y < m_nBarHeight; ++y) {
        for (x = 0; x < maxWidth; ++x) {
            pPiexl = LPBYTE(m_pBits) + ((addHeight + y) * m_bmInfo.bmWidthBytes) +
                ((x * m_bmInfo.bmBitsPixel) / BIT_TO_BYTE);
            dwColor = _HSLToRGB(m_nCurH, x, m_nCurB);
            pPiexl[0] = GetBValue(dwColor);
            pPiexl[1] = GetGValue(dwColor);
            pPiexl[2] = GetRValue(dwColor);
        }
    }

    SetBitmapBits(m_hMemBitmap, m_bmInfo.bmWidthBytes * m_bmInfo.bmHeight, m_pBits);
}
}