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
#include "UIShadow.h"
#include <cmath>
#include "crtdbg.h"

namespace DuiLib {
const int DARKNESS_DEFUALT = 150; // CShadow明暗度默认值150
const int SHARPNESS_DEFUALT = 5; // CShadow锐利度默认值5
const TCHAR *g_strWndClassName = _T("PerryShadowWnd");
bool CShadowUI::s_bHasInit = FALSE;

CShadowUI::CShadowUI(void)
    : m_hWnd((HWND)nullptr),
      m_OriParentProc(NULL),
      m_Status(0),
      m_nDarkness(DARKNESS_DEFUALT),
      m_nSharpness(SHARPNESS_DEFUALT),
      m_nSize(0),
      m_nxOffset(0),
      m_nyOffset(0),
      m_Color(RGB(0, 0, 0)),
      m_WndSize(0),
      m_bUpdate(false),
      m_bIsImageMode(false),
      m_bIsShowShadow(false),
      m_hParentWnd(nullptr),
      m_pManager(nullptr)
{
    ::ZeroMemory(&m_rcShadowCorner, sizeof(RECT));
}

CShadowUI::~CShadowUI(void) {}

bool CShadowUI::Initialize(HINSTANCE hInstance)
{
    if (s_bHasInit) {
        return false;
    }

    // Register window class for shadow window
    WNDCLASSEX wcex;

    SecureZeroMemory(&wcex, sizeof(wcex));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = DefWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = g_strWndClassName;
    wcex.hIconSm = nullptr;

    s_bHasInit = true;
    return RegisterClassEx(&wcex);
}

void CShadowUI::Create(CPaintManagerUI *pPaintManager)
{
    if (!m_bIsShowShadow) {
        return;
    }

    // Already initialized
    _ASSERT(CPaintManagerUI::GetInstance() != INVALID_HANDLE_VALUE);
    _ASSERT(pPaintManager != nullptr);
    m_pManager = pPaintManager;
    if (this->m_pManager == nullptr) {
        return;
    }
    HWND hParentWnd = m_pManager->GetPaintWindow();
    // Add parent window - shadow pair to the map
    _ASSERT(GetShadowMap().find(hParentWnd) == GetShadowMap().end()); // Only one shadow for each window

    if (GetShadowMap().find(hParentWnd) != GetShadowMap().end()) {
        return;
    }

    GetShadowMap()[hParentWnd] = this;
    m_hParentWnd = hParentWnd;

    // Determine the initial show state of shadow according to parent window's state
    auto lParentStyle = static_cast<ULONG>(GetWindowLongPtr(hParentWnd, GWL_STYLE));

    // Create the shadow window
    ULONG styleValue = lParentStyle & WS_CAPTION;
    m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, g_strWndClassName, nullptr, styleValue | WS_POPUPWINDOW,
        CW_USEDEFAULT, 0, 0, 0, hParentWnd, nullptr, CPaintManagerUI::GetInstance(), nullptr);

    if (!(WS_VISIBLE & lParentStyle)) {
        m_Status = SS_ENABLED;
    } else if ((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle) {
        m_Status = SS_ENABLED | SS_PARENTVISIBLE;
    } else {
        m_Status = SS_ENABLED | SS_VISABLE | SS_PARENTVISIBLE;
        ::ShowWindow(m_hWnd, SW_SHOWNA);
        Update(hParentWnd);
    }

    // Replace the original WndProc of parent window to steal messages
    m_OriParentProc = GetWindowLongPtr(hParentWnd, GWLP_WNDPROC);

#pragma warning(disable : 4311) // temporrarily disable the type_cast warning in Win32
    SetWindowLongPtr(hParentWnd, GWLP_WNDPROC, (LONG_PTR)ParentProc);
#pragma warning(default : 4311)
}

std::map<HWND, CShadowUI *> &CShadowUI::GetShadowMap()
{
    static std::map<HWND, CShadowUI *> s_Shadowmap;
    return s_Shadowmap;
}

LRESULT CShadowUI::MyParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    if (m_pManager == nullptr) {
        return 0;
    }
    switch (uMsg) {
        case WM_WININICHANGE:
            Update(hwnd);
            break;
        case WM_ACTIVATE:
            if (m_pManager->PreMessageHandler(uMsg, wParam, lParam, res)) {
                return res;
            } else {
                m_pManager->MessageHandler(uMsg, wParam, lParam, res);
            }
            break;
        case WM_CLOSE:
            if (m_pManager->PreMessageHandler(uMsg, wParam, lParam, res)) {
                return res;
            } else {
                m_pManager->MessageHandler(uMsg, wParam, lParam, res);
            }
            break;
        case WM_WINDOWPOSCHANGED:
            SetWindowPos(m_hWnd, hwnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
            break;
        case WM_MOVE: {
            RECT WndRect = { 0 };
            GetWindowRect(hwnd, &WndRect);
            if (m_bIsImageMode) {
                SetWindowPos(m_hWnd, 0, WndRect.left - m_rcShadowCorner.left, WndRect.top - m_rcShadowCorner.top, 0, 0,
                    SWP_NOSIZE | SWP_NOACTIVATE);
            } else {
                SetWindowPos(m_hWnd, 0, WndRect.left + m_nxOffset - m_nSize, WndRect.top + m_nyOffset - m_nSize, 0, 0,
                    SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
            }
            break;
        }
        case WM_SIZE: {
            if ((m_Status & SS_ENABLED) == SS_ENABLED) {
                if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED) {
                    ::ShowWindow(m_hWnd, SW_HIDE);
                    m_Status &= ~SS_VISABLE;
                } else if (m_Status & SS_PARENTVISIBLE) {
                    if (!(m_Status & SS_VISABLE) && !IsZoomed(hwnd) && !IsIconic(hwnd)) {
                        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
                        m_Status |= SS_VISABLE;
                    }
                    if (LOWORD(lParam) > LOWORD(m_WndSize) || HIWORD(lParam) > HIWORD(m_WndSize)) {
                        m_bUpdate = true;
                    } else {
                        Update(hwnd);
                    }
                }
                m_WndSize = lParam;
            }
            break;
        }
        case WM_PAINT: {
            if (m_bUpdate) {
                Update(hwnd);
                m_bUpdate = false;
            }
            break;
        }
        case WM_EXITSIZEMOVE: {
            Update(hwnd);
            break;
        }
        case WM_SHOWWINDOW: {
            if ((m_Status & SS_ENABLED) == SS_ENABLED) {
                LRESULT lResult = 0;
                if (m_hParentWnd && IsWindow(m_hParentWnd) && m_OriParentProc) {
                    lResult = ((WNDPROC)m_OriParentProc)(hwnd, uMsg, wParam, lParam);
                }

                if (!wParam) {
                    ::ShowWindow(m_hWnd, SW_HIDE);
                    m_Status &= ~(SS_VISABLE | SS_PARENTVISIBLE);
                } else if (!IsZoomed(hwnd) && !IsIconic(hwnd)) {
                    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
                    m_Status |= SS_VISABLE | SS_PARENTVISIBLE;
                    Update(hwnd);
                }

                if (m_pManager->PreMessageHandler(uMsg, wParam, lParam, res)) {
                    return res;
                } else {
                    m_pManager->MessageHandler(uMsg, wParam, lParam, res);
                }

                return lResult;
            }
            break;
        }
        case WM_DESTROY: {
            DestroyWindow(m_hWnd);
            break;
        }
        case WM_NCDESTROY: {
            GetShadowMap().erase(hwnd);
            break;
        }
        default:
            break;
    }

#pragma warning(disable : 4312) // temporrarily disable the type_cast warning in Win32
    // Call the default(original) window procedure for other messages or messages processed but not returned
    if (m_hParentWnd && IsWindow(m_hParentWnd) && m_OriParentProc) {
        return ((WNDPROC)m_OriParentProc)(hwnd, uMsg, wParam, lParam);
    } else {
        return 0;
    }
#pragma warning(default : 4312)
}

LRESULT CALLBACK CShadowUI::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _ASSERT(GetShadowMap().find(hwnd) != GetShadowMap().end()); // Shadow must have been attached

    if (hwnd == nullptr || ::IsWindow(hwnd) == false) {
        return 0;
    }
    if (GetShadowMap().find(hwnd) == GetShadowMap().end()) {
        return 0;
    }

    CShadowUI *pThis = GetShadowMap()[hwnd];
    if (!pThis) {
        return 0;
    }
    return pThis->MyParentProc(hwnd, uMsg, wParam, lParam);
}

void CShadowUI::Update(HWND hParent)
{
    if (!(m_Status & SS_VISABLE)) {
        return;
    }

    RECT WndRect;
    GetWindowRect(hParent, &WndRect);
    int nShadWndWid;
    int nShadWndHei;
    if (m_bIsImageMode) {
        if (m_sShadowImage.IsEmpty()) {
            return;
        }
        nShadWndWid = WndRect.right - WndRect.left + m_rcShadowCorner.left + m_rcShadowCorner.right;
        nShadWndHei = WndRect.bottom - WndRect.top + m_rcShadowCorner.top + m_rcShadowCorner.bottom;
    } else {
        nShadWndWid = WndRect.right - WndRect.left + m_nSize * 2;
        nShadWndHei = WndRect.bottom - WndRect.top + m_nSize * 2;
    }

    const int bitCount = 32;
    // Create the alpha blending bitmap
    BITMAPINFO bmi; // bitmap header
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nShadWndWid;
    bmi.bmiHeader.biHeight = nShadWndHei;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = bitCount; // four 8-bit components
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;

    BYTE *pvBits = nullptr; // pointer to DIB section
    HBITMAP hbitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS,
        reinterpret_cast<void **>(&pvBits), nullptr, 0);
    if ((hbitmap == nullptr) || (pvBits == nullptr)) {
        return;
    }
    HDC hMemDC = CreateCompatibleDC(nullptr);
    HBITMAP hOriBmp = (HBITMAP)SelectObject(hMemDC, hbitmap);

    if (m_bIsImageMode) {
        RECT rcPaint = { 0, 0, nShadWndWid, nShadWndHei };

        const TImageInfo *data = m_pManager->GetImageEx((LPCTSTR)m_sShadowImage, nullptr, 0);
        if (data == nullptr) {
            return;
        }

        RECT rcBmpPart = { 0 };
        rcBmpPart.right = data->nX;
        rcBmpPart.bottom = data->nY;

        RECT rcSource = { 0 };
        CRenderEngine::DrawImage(hMemDC, m_pManager, rcPaint, rcPaint, m_sShadowImage, L"",
            rcPaint, rcSource, m_rcShadowCorner, 0, 0xFF, true, false, false);
    } else {
        ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
        MakeShadow(reinterpret_cast<UINT32 *>(pvBits), bmi.bmiHeader.biSizeImage, hParent, &WndRect);
    }

    POINT ptDst;
    if (m_bIsImageMode) {
        ptDst.x = WndRect.left - m_rcShadowCorner.left;
        ptDst.y = WndRect.top - m_rcShadowCorner.top;
    } else {
        ptDst.x = WndRect.left + m_nxOffset - m_nSize;
        ptDst.y = WndRect.top + m_nyOffset - m_nSize;
    }

    const int minPixel = 0;
    const int maxPixel = 255;
    POINT ptSrc = { 0, 0 };
    SIZE WndSize = { nShadWndWid, nShadWndHei };
    BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, minPixel, maxPixel, AC_SRC_ALPHA };

    MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

    BOOL bRet =
        ::UpdateLayeredWindow(m_hWnd, nullptr, &ptDst, &WndSize, hMemDC, &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

    _ASSERT(bRet); // something was wrong....

    // Delete used resources
    SelectObject(hMemDC, hOriBmp);
    DeleteObject(hbitmap);
    DeleteDC(hMemDC);
}

// The shadow algorithm:
// Get the region of parent window,
// Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
// Apply modified (with blur effect) morphologic dilation to make the blurred border
// The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it
void CShadowUI::MakeShadow(UINT32 *pShadBits, UINT32 bitCount, HWND hParent, RECT *rcParent)
{
    // Get the region of parent window,
    HRGN hParentRgn = CreateRectRgn(0, 0, 0, 0);
    GetWindowRgn(hParent, hParentRgn);

    if (rcParent == nullptr) {
        return;
    }
    // Determine the Start and end point of each horizontal scan line
    SIZE szParent = { rcParent->right - rcParent->left, rcParent->bottom - rcParent->top };
    SIZE szShadow = { szParent.cx + 2 * m_nSize, szParent.cy + 2 * m_nSize };
    // Extra 2 lines (set to be empty) in ptAnchors are used in dilation
    int nAnchors = max(szParent.cy, szShadow.cy); // # of anchor points pares
    int(*ptAnchors)[2] = new int[nAnchors + 2][2];
    int(*ptAnchorsOri)[2] = new int[szParent.cy][2]; // anchor points, will not modify during erosion
    ptAnchors[0][0] = szParent.cx;
    ptAnchors[0][1] = 0;
    ptAnchors[nAnchors + 1][0] = szParent.cx;
    ptAnchors[nAnchors + 1][1] = 0;
    if (m_nSize > 0) {
        // Put the parent window anchors at the center
        for (int i = 0; i < m_nSize; i++) {
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchors[i + 1][1] = 0;
            ptAnchors[szShadow.cy - i][0] = szParent.cx;
            ptAnchors[szShadow.cy - i][1] = 0;
        }
        ptAnchors += m_nSize;
    }
    for (int i = 0; i < szParent.cy; i++) {
        // find start point
        int j;
        for (j = 0; j < szParent.cx; j++) {
            if (PtInRegion(hParentRgn, j, i)) {
                ptAnchors[i + 1][0] = j + m_nSize;
                ptAnchorsOri[i][0] = j;
                break;
            }
        }

        if (j >= szParent.cx) { // Start point not found
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchorsOri[i][1] = 0;
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchorsOri[i][1] = 0;
        } else {
            // find end point
            for (j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--) {
                if (PtInRegion(hParentRgn, j, i)) {
                    ptAnchors[i + 1][1] = j + 1 + m_nSize;
                    ptAnchorsOri[i][1] = j + 1;
                    break;
                }
            }
        }
    }

    if (m_nSize > 0) {
        ptAnchors -= m_nSize; // Restore pos of ptAnchors for erosion
    }
    int(*ptAnchorsTmp)[2] = new int[nAnchors + 2][2]; // Store the result of erosion
    // First and last line should be empty
    ptAnchorsTmp[0][0] = szParent.cx;
    ptAnchorsTmp[0][1] = 0;
    ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
    ptAnchorsTmp[nAnchors + 1][1] = 0;
    int nEroTimes = 0;
    // morphologic erosion
    for (int i = 0; i < m_nSharpness - m_nSize; i++) {
        nEroTimes++;
        for (int j = 1; j < nAnchors + 1; j++) {
            ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
            ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
        }
        // Exchange ptAnchors and ptAnchorsTmp;
        int(*ptAnchorsXange)[2] = ptAnchorsTmp;
        ptAnchorsTmp = ptAnchors;
        ptAnchors = ptAnchorsXange;
    }

    // morphologic dilation
    // now coordinates in ptAnchors are same as in shadow window
    ptAnchors += (m_nSize < 0 ? -m_nSize : 0) + 1;
    // Generate the kernel
    int nKernelSize = m_nSize > m_nSharpness ? m_nSize : m_nSharpness;
    int nCenterSize = m_nSize > m_nSharpness ? (m_nSize - m_nSharpness) : 0;
    UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
    UINT32 *pKernelIter = pKernel;
    for (int i = 0; i <= 2 * nKernelSize; i++) {
        for (int j = 0; j <= 2 * nKernelSize; j++) {
            double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) +
                (j - nKernelSize) * static_cast<double>(j - nKernelSize));
            const int getDarkness = 24;
            const int getFactor = 24;
            if (dLength < nCenterSize) {
                *pKernelIter = (m_nDarkness << getDarkness) | PreMultiply(m_Color, m_nDarkness);
            } else if (dLength <= nKernelSize) {
                UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (m_nSharpness + 1)) * m_nDarkness));
                *pKernelIter = (nFactor << getFactor) | PreMultiply(m_Color, nFactor);
            } else {
                *pKernelIter = 0;
            }
            pKernelIter++;
        }
    }
    // Generate blurred border
    for (int i = nKernelSize; i < szShadow.cy - nKernelSize; i++) {
        int j;
        if (ptAnchors[i][0] < ptAnchors[i][1]) {
            // Start of line
            for (j = ptAnchors[i][0]; j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
                j++) {
                bool tobreak = false;
                for (int k = 0; k <= 2 * nKernelSize; k++) {
                    UINT32 offset = (szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
                    if (offset >= bitCount / sizeof(UINT32) - 2 * nKernelSize) {
                        tobreak = true;
                        break;
                    }

                    UINT32 *pPixel = pShadBits + offset;
                    UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
                    for (int l = 0; l <= 2 * nKernelSize; l++) {
                        if (*pPixel < *pKernelPixel) {
                            *pPixel = *pKernelPixel;
                        }
                        pPixel++;
                        pKernelPixel++;
                    }
                }

                if (tobreak) {
                    break;
                }
            } // for() start of line

            // End of line
            for (j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1); j < ptAnchors[i][1]; j++) {
                bool tobreak = false;
                for (int k = 0; k <= 2 * nKernelSize; k++) {
                    UINT32 offset = (szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
                    if (offset >= bitCount / sizeof(UINT32) - 2 * nKernelSize) {
                        tobreak = true;
                        break;
                    }

                    UINT32 *pPixel = pShadBits + offset;
                    UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);
                    for (int l = 0; l <= 2 * nKernelSize; l++) {
                        if (*pPixel < *pKernelPixel) {
                            *pPixel = *pKernelPixel;
                        }
                        pPixel++;
                        pKernelPixel++;
                    }
                }

                if (tobreak) {
                    break;
                }
            } // for() end of line
        }
    } // for() Generate blurred border

    const int getDarkness = 24; // 高8位明暗度，低24位为RGB。
    // Erase unwanted parts and complement missing
    UINT32 clCenter = (m_nDarkness << getDarkness) | PreMultiply(m_Color, m_nDarkness);
    for (int i = min(nKernelSize, max(m_nSize - m_nyOffset, 0));
        i < max(szShadow.cy - nKernelSize, min(szParent.cy + m_nSize - m_nyOffset,
            szParent.cy + 2 * m_nSize)); i++) {
        UINT32 offset = (szShadow.cy - i - 1) * szShadow.cx;
        bool tobreak = false;

        UINT32 *pLine = pShadBits + offset;
        if (i - m_nSize + m_nyOffset < 0 ||
            i - m_nSize + m_nyOffset >= szParent.cy) { // Line is not covered by parent window
            for (int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++) {
                if (offset + j >= bitCount / sizeof(UINT32)) {
                    tobreak = true;
                    break;
                }

                *(pLine + j) = clCenter;
            }
        } else {
            for (int j = ptAnchors[i][0];
                j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset,
                    ptAnchors[i][1]); j++) {
                if (offset + j >= bitCount / sizeof(UINT32)) {
                    tobreak = true;
                    break;
                }

                *(pLine + j) = clCenter;
            }
            for (int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, 0);
                j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, szShadow.cx); j++) {
                if (offset + j >= bitCount / sizeof(UINT32)) {
                    tobreak = true;
                    break;
                }

                *(pLine + j) = 0;
            }
            for (int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset,
                ptAnchors[i][0]); j < ptAnchors[i][1]; j++) {
                if (offset + j >= bitCount / sizeof(UINT32)) {
                    tobreak = true;
                    break;
                }

                *(pLine + j) = clCenter;
            }
        }

        if (tobreak) {
            break;
        }
    }

    // Delete used resources
    delete[](ptAnchors - (m_nSize < 0 ? -m_nSize : 0) - 1);
    delete[] ptAnchorsTmp;
    ptAnchorsTmp = nullptr;
    delete[] ptAnchorsOri;
    ptAnchorsOri = nullptr;
    delete[] pKernel;
    pKernel = nullptr;
    DeleteObject(hParentRgn);
}

void CShadowUI::ShowShadow(bool bShow)
{
    m_bIsShowShadow = bShow;
    if (bShow) {
        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    } else {
        ::ShowWindow(m_hWnd, SW_HIDE);
    }
}

bool CShadowUI::IsShowShadow() const
{
    return m_bIsShowShadow;
}

bool CShadowUI::SetSize(int NewSize)
{
    const int minShadowSize = -20; // 窗口设置阴影所需要尺寸的下限
    if (NewSize < minShadowSize) {
        return false;
    }

    m_nSize = (signed char)NewSize;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }
    return true;
}

bool CShadowUI::SetSharpness(unsigned int NewSharpness)
{
    const int maxShadowSharpness = 20; // 窗口设置阴影所需要锐利度的上限
    if (NewSharpness > maxShadowSharpness) {
        return false;
    }

    m_nSharpness = (unsigned char)NewSharpness;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }
    return true;
}

bool CShadowUI::SetDarkness(unsigned int NewDarkness)
{
    const int shadowDarkness = 255; // 窗口阴影暗度系数255
    if (NewDarkness > shadowDarkness) {
        return false;
    }

    m_nDarkness = (unsigned char)NewDarkness;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }
    return true;
}

bool CShadowUI::SetPosition(int NewXOffset, int NewYOffset)
{
    // 偏移量向上和向下的极限值
    const int positiveOffset = 20;
    const int negativeOffset = -20;
    if ((NewXOffset > positiveOffset) || (NewXOffset < negativeOffset) ||
        (NewYOffset > positiveOffset) || (NewYOffset < negativeOffset)) {
        return false;
    }

    m_nxOffset = (signed char)NewXOffset;
    m_nyOffset = (signed char)NewYOffset;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }
    return true;
}

bool CShadowUI::SetColor(COLORREF NewColor)
{
    m_Color = NewColor;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }
    return true;
}

bool CShadowUI::SetImage(LPCTSTR szImage)
{
    if (szImage == nullptr) {
        return false;
    }

    m_bIsImageMode = true;
    m_sShadowImage = szImage;
    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }

    return true;
}

bool CShadowUI::SetShadowCorner(RECT rcCorner)
{
    if (rcCorner.left < 0 || rcCorner.top < 0 || rcCorner.right < 0 || rcCorner.bottom < 0) {
        return false;
    }

    m_rcShadowCorner = rcCorner;

    if (m_hWnd != nullptr && (SS_VISABLE & m_Status)) {
        Update(GetParent(m_hWnd));
    }

    return true;
}

bool CShadowUI::CopyShadow(CShadowUI *pShadow)
{
    if (pShadow == nullptr) {
        return false;
    }
    pShadow->SetSize(static_cast<int>(m_nSize));
    pShadow->SetSharpness(static_cast<unsigned int>(m_nSharpness));
    pShadow->SetDarkness(static_cast<unsigned int>(m_nDarkness));
    pShadow->SetColor(m_Color);
    pShadow->SetPosition(static_cast<int>(m_nxOffset), static_cast<int>(m_nyOffset));

    if (m_bIsImageMode) {
        pShadow->SetSize(m_nSize);
        pShadow->SetImage(m_sShadowImage);
        pShadow->SetShadowCorner(m_rcShadowCorner);
    }

    return true;
}
} // namespace DuiLib
