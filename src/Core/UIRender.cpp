/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include <sstream>

#include "unicode/utypes.h"
#include "unicode/uchar.h"
#include "unicode/localpointer.h"
#include "unicode/ubidi.h"
#include <unicode/unistr.h>
using namespace icu;
using icu::UnicodeString;
#define STB_IMAGE_IMPLEMENTATION
#include "../Utils/stb_image.h"

#ifdef USE_XIMAGE_EFFECT
#include "../../3rd/CxImage/ximage.h"
#include "../../3rd/CxImage/ximage.cpp"
#include "../../3rd/CxImage/ximaenc.cpp"
#include "../../3rd/CxImage/ximagif.cpp"
#include "../../3rd/CxImage/ximainfo.cpp"
#include "../../3rd/CxImage/ximalpha.cpp"
#include "../../3rd/CxImage/ximapal.cpp"
#include "../../3rd/CxImage/ximatran.cpp"
#include "../../3rd/CxImage/ximawnd.cpp"
#include "../../3rd/CxImage/xmemfile.cpp"
#endif

DECLARE_HANDLE(HZIP); // An HZIP identifies a zip file that has been opened
using ZRESULT = DWORD;
using ZIPENTRY = struct {
    int index;                    // index of this file within the zip
    char name[MAX_PATH];          // filename within the zip
    DWORD attr;                   // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size;               // sizes of item, compressed and uncompressed. These
    long unc_size;                // may be -1 if not yet known (e.g. being streamed in)
};
using ZIPENTRYW = struct {
    int index;                    // index of this file within the zip
    TCHAR name[MAX_PATH];         // filename within the zip
    DWORD attr;                   // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size;               // sizes of item, compressed and uncompressed. These
    long unc_size;                // may be -1 if not yet known (e.g. being streamed in)
};

#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);

namespace DuiLib {
constexpr int RGB_MAX_VALUE = 255; // RGB 最大数值为255
constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10
constexpr int RADIX_TYPE_TWO_VALUE = 16; // set radix type two value 16
constexpr int ARRAY_LENGTH = 10;
constexpr int INDEX_ZERO = 0;
constexpr int INDEX_ONE = 1;
constexpr int INDEX_TWO = 2;
constexpr int INDEX_THREE = 3;
constexpr int INDEX_FOUR = 4;
constexpr int INDEX_FIVE = 5;
constexpr int TWO_UNIT = 2;
constexpr int THREE_UNIT = 3;
constexpr int FOUR_UNIT = 4;
constexpr int FIVE_UNIT = 5;
constexpr int SIX_UNIT = 6;
constexpr int DEGREES_ONE = 90; // 90度
constexpr int DEGREES_TWO = 180; // 180度
constexpr int DEGREES_THREE = 270; // 270度
constexpr int DEGREES_FOUR = 360; // 360度
constexpr int ONE_BYTE = 8; // 1个字节
constexpr int HSL_VALUE = 100; // 饱和度H:360，色相值S:100，亮度L:100
constexpr int MAX_RECTF_WIDTH = 9999; // 矩形框最大宽度
constexpr int MAX_RECTF_HEIGTH = 9999; // 矩形框最大高度
constexpr int BIT_COUNT = 32; // 用于描述RGB , 透明度 + RGB 共32bit
constexpr int GET_HIGHT_BIT = 24; // 移位24bit，获取高八位

static int g_iFontID = MAX_FONT_ID;
// 150% dpi keep original file name, so it's empty here.
static CDuiString g_strDpi[] = { _T("100"), _T("125"), _T(""), _T("175"), _T("200"), _T("225") };
static int g_nDpi[] = {96, 120, 144, 168, 192, 216}; // DPI 的6种值
int Dpi2Index(int dpi)
{
    switch (dpi) {
        case 100: // set DPI 100
            return INDEX_ZERO;
        case 125: // set DPI 125
            return INDEX_ONE;
        case 150: // set DPI 150
            return INDEX_TWO;
        case 175: // set DPI 175
            return INDEX_THREE;
        case 200: // set DPI 200
            return INDEX_FOUR;
        case 225: // set DPI 225
            return INDEX_FIVE;
        default:
            return INDEX_TWO;
    }
}

void ScaleBitmap(BYTE *pData, int dwSize, int nDataDpi, HBITMAP &hBitmap, int &nWidth, int &nHeight,
    bool &bAlpha, bool bRev = false, bool highQuality = true);

CRenderClip::~CRenderClip()
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    ASSERT(::GetObjectType(hRgn) == OBJ_REGION);
    ASSERT(::GetObjectType(hOldRgn) == OBJ_REGION);
    ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);
}

void CRenderClip::GenerateClip(HDC hDC, RECT rc, CRenderClip &clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
    clip.rcItem = rc;
}

void CRenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip &clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1,
        rcItem.bottom + 1, width, height);
    ::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
    ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
    clip.hDC = hDC;
    clip.rcItem = rc;
    ::DeleteObject(hRgnItem);
}

void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip &clip)
{
    ::SelectClipRgn(hDC, clip.hOldRgn);
}

void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip &clip)
{
    ::SelectClipRgn(hDC, clip.hRgn);
}

static const float ONE_THIRD = 1.0f / THREE_UNIT; // 此处需要取三分之一

static void RGBtoHSL(DWORD argb, float* h, float* s, float* l)
{
    const float
        r = static_cast<float>(GetRValue(argb)),
        g = static_cast<float>(GetGValue(argb)),
        b = static_cast<float>(GetBValue(argb)),
        nR = (r < 0 ? 0 : (r > RGB_MAX_VALUE ? RGB_MAX_VALUE : r)) / RGB_MAX_VALUE,
        nG = (g < 0 ? 0 : (g > RGB_MAX_VALUE ? RGB_MAX_VALUE : g)) / RGB_MAX_VALUE,
        nB = (b < 0 ? 0 : (b > RGB_MAX_VALUE ? RGB_MAX_VALUE : b)) / RGB_MAX_VALUE,
        mn = min(min(nR, nG), nB),
        mx = max(max(nR, nG), nB);
    *l = (mn + mx) / TWO_UNIT;
    if (mx == mn) {
        *h = *s = 0;
    } else {
        const float
            f = (nR == mn) ? (nG - nB) : ((nG == mn) ? (nB - nR) : (nR - nG)),
            i = (nR == mn) ? 3.0f : ((nG == mn) ? 5.0f : 1.0f);
        *h = (i - f / (mx - mn));
        if (*h >= SIX_UNIT) {
            *h -= SIX_UNIT;
        }
        *h *= 60; // 色相60度理论，每相差60代表下一个纯色
        *s = (TWO_UNIT * (*l) <= 1) ? ((mx - mn) / (mx + mn)) : ((mx - mn) / (TWO_UNIT - mx - mn));
    }
}

static void HSLtoRGB(DWORD* argb, float H, float S, float L)
{
    const float
        q = TWO_UNIT * L < 1 ? L * (1 + S) : (L + S - L * S),
        p = TWO_UNIT * L - q,
        h = H / DEGREES_FOUR, // 色相最大值为360
        tr = h + ONE_THIRD,
        tg = h,
        tb = h - ONE_THIRD,
        ntr = tr < 0 ? tr + 1 : (tr > 1 ? tr - 1 : tr),
        ntg = tg < 0 ? tg + 1 : (tg > 1 ? tg - 1 : tg),
        ntb = tb < 0 ? tb + 1 : (tb > 1 ? tb - 1 : tb),
        B = RGB_MAX_VALUE * (SIX_UNIT * ntr < 1 ? p + (q - p) * SIX_UNIT * ntr : (TWO_UNIT * ntr < 1 ? q :
            (THREE_UNIT * ntr < TWO_UNIT ? p + (q - p) * SIX_UNIT * (2.0f * ONE_THIRD - ntr) : p))),
        G = RGB_MAX_VALUE * (SIX_UNIT * ntg < 1 ? p + (q - p) * SIX_UNIT * ntg : (TWO_UNIT * ntg < 1 ? q :
            (THREE_UNIT * ntg < TWO_UNIT ? p + (q - p) * SIX_UNIT * (2.0f * ONE_THIRD - ntg) : p))),
        R = RGB_MAX_VALUE * (SIX_UNIT * ntb < 1 ? p + (q - p) * SIX_UNIT * ntb : (TWO_UNIT * ntb < 1 ? q :
            (THREE_UNIT * ntb < TWO_UNIT ? p + (q - p) * SIX_UNIT * (2.0f * ONE_THIRD - ntb) : p)));
    *argb &= 0xFF000000;
    *argb |= RGB((BYTE)(R < 0 ? 0 : (R > RGB_MAX_VALUE ? RGB_MAX_VALUE : R)),
        (BYTE)(G < 0 ? 0 : (G > RGB_MAX_VALUE ? RGB_MAX_VALUE : G)),
        (BYTE)(B < 0 ? 0 : (B > RGB_MAX_VALUE ? RGB_MAX_VALUE : B)));
}

static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
    return RGB(GetRValue(clrSrc) * src_darken + GetRValue(clrDest) * dest_darken,
        GetGValue(clrSrc) * src_darken + GetGValue(clrDest) * dest_darken,
        GetBValue(clrSrc) * src_darken + GetBValue(clrDest) * dest_darken);
}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight,
    HDC hSrcDC, int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
    HDC hTempDC = ::CreateCompatibleDC(hDC);
    if (hTempDC == nullptr) {
        return FALSE;
    }

    // Creates Source DIB
    LPBITMAPINFO lpbiSrc = nullptr;
    // Fill in the BITMAPINFOHEADER
    lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
    if (lpbiSrc == nullptr) {
        ::DeleteDC(hTempDC);
        return FALSE;
    }
    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = dwWidth;
    lpbiSrc->bmiHeader.biHeight = dwHeight;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    COLORREF *pSrcBits = nullptr;
    HBITMAP hSrcDib = CreateDIBSection(hSrcDC, lpbiSrc, DIB_RGB_COLORS,
        reinterpret_cast<void **>(&pSrcBits), nullptr, NULL);

    if ((hSrcDib == nullptr) || (pSrcBits == nullptr)) {
        delete[] lpbiSrc;
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    HBITMAP hOldTempBmp = (HBITMAP)::SelectObject(hTempDC, hSrcDib);
    ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    // Creates Destination DIB
    LPBITMAPINFO lpbiDest = nullptr;
    // Fill in the BITMAPINFOHEADER
    lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
    if (lpbiDest == nullptr) {
        delete[] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiDest->bmiHeader.biWidth = dwWidth;
    lpbiDest->bmiHeader.biHeight = dwHeight;
    lpbiDest->bmiHeader.biPlanes = 1;
    lpbiDest->bmiHeader.biBitCount = 32; // 32bit
    lpbiDest->bmiHeader.biCompression = BI_RGB;
    lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiDest->bmiHeader.biXPelsPerMeter = 0;
    lpbiDest->bmiHeader.biYPelsPerMeter = 0;
    lpbiDest->bmiHeader.biClrUsed = 0;
    lpbiDest->bmiHeader.biClrImportant = 0;

    COLORREF *pDestBits = nullptr;
    HBITMAP hDestDib = CreateDIBSection(hDC, lpbiDest, DIB_RGB_COLORS,
        reinterpret_cast<void **>(&pDestBits), nullptr, NULL);

    if ((hDestDib == nullptr) || (pDestBits == nullptr)) {
        delete[] lpbiSrc;
        delete[] lpbiDest;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    ::SelectObject(hTempDC, hDestDib);
    ::BitBlt(hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    double src_darken;
    BYTE nAlpha;

    for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++) {
        nAlpha = LOBYTE(*pSrcBits >> GET_HIGHT_BIT);
        src_darken = static_cast<double>(nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
        if (src_darken < 0.0) {
            src_darken = 0.0;
        }
        *pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
    } // for

    ::SelectObject(hTempDC, hDestDib);
    ::BitBlt(hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    delete[] lpbiDest;
    ::DeleteObject(hDestDib);

    delete[] lpbiSrc;
    ::DeleteObject(hSrcDib);

    ::DeleteDC(hTempDC);
    return TRUE;
}

bool CRenderEngine::DrawImage(HDC hDC, CPaintManagerUI *pManager, const RECT &rc, const RECT &rcPaint,
    const CDuiString &sImageName, const CDuiString &sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner,
    DWORD dwMask, BYTE bFade, bool bHole, bool bTiledX, bool bTiledY, HINSTANCE instance, bool bRev, bool highQuality)
{
    if (sImageName.IsEmpty()) {
        return false;
    }
    const TImageInfo *data = nullptr;
    if (sImageResType.IsEmpty()) {
        data = pManager->GetImageEx((LPCTSTR)sImageName, nullptr, dwMask, false, instance, bRev);
    } else {
        data = pManager->GetImageEx((LPCTSTR)sImageName, (LPCTSTR)sImageResType, dwMask, false, instance, bRev);
    }
    if (!data) {
        return false;
    }


    bool bCorner = false;
    if (rcCorner.left || rcCorner.top || rcCorner.right || rcCorner.bottom) {
        bCorner = true;
    }

    bool bSrc = false;
    if (rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0) {
        bSrc = true;
    }

    // add binfei, dynamic scale image by the real rect
    auto width = std::abs(rcItem.right - rcItem.left);
    auto height = std::abs(rcItem.top - rcItem.bottom);
    if ((bCorner == false) && (data->pSrcBits != nullptr && data->isrcsize > 0) &&
        ((bSrc && (width != data->nX || height != data->nY) ||
        (UICulture::GetInstance()->getDirection() == DIRECTION_RTL && bRev) && !data->isRes) ||
        (!bSrc && height != data->nY))) {
        // 如果source无值（使用原图），则宽高同时缩放
        // 如果source有值，则仅按照高度进行等比缩放
        TImageInfo *scaleddata = new (std::nothrow) TImageInfo;
        if (scaleddata == nullptr) {
            return false;
        }
        scaleddata->pBits = nullptr;
        scaleddata->pSrcBits = nullptr;
        scaleddata->isrcsize = data->isrcsize;
        scaleddata->hBitmap = nullptr;
        if (bSrc) {
            scaleddata->nX = width;
            scaleddata->nY = height;
        } else {
            scaleddata->nX = data->nX * height / data->nY;
            scaleddata->nY = height; // 按照高度进行等比缩放
        }

        scaleddata->nAddType = data->nAddType;
        ScaleBitmap(data->pSrcBits, data->isrcsize, g_nDpi[TWO_UNIT], scaleddata->hBitmap,
            scaleddata->nX, scaleddata->nY, scaleddata->bAlpha, bRev, highQuality);
        // 目前方案不合适同一个页面公用同一个image的
        std::swap(reinterpret_cast<BYTE *>(scaleddata->pSrcBits), (BYTE *)data->pSrcBits);
        pManager->ReplaceImage(sImageName.GetData(), scaleddata, false);
        data = scaleddata;
        // 如果已经翻转，就不再需要翻转了，记录一个标记
        TImageInfo *tmpData = const_cast<TImageInfo *>(data);
        if (tmpData != nullptr) {
            tmpData->isRes = true;
        }
    }

    if (rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0) {
        rcBmpPart.right = data->nX;
        rcBmpPart.bottom = data->nY;
    }
    if (rcBmpPart.right > data->nX) {
        rcBmpPart.right = data->nX;
    }
    if (rcBmpPart.bottom > data->nY) {
        rcBmpPart.bottom = data->nY;
    }

    RECT rcTemp;
    if (!::IntersectRect(&rcTemp, &rcItem, &rc)) {
        return true;
    }
    if (!::IntersectRect(&rcTemp, &rcItem, &rcPaint)) {
        return true;
    }
    if (data->hBitmap == nullptr) {
        return false;
    }
    CRenderEngine::DrawImage(hDC, data->hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner,
        pManager->IsLayered() ? true : data->bAlpha, bFade, bHole, bTiledX, bTiledY);

    return true;
}

DWORD CRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
{
    // 设置色相H:180 饱和度S:100 亮度L:100
    if (H == DEGREES_TWO && S == HSL_VALUE && L == HSL_VALUE) {
        return dwColor;
    }
    float fH;
    float fS;
    float fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;
    RGBtoHSL(dwColor, &fH, &fS, &fL);
    fH += (H - DEGREES_TWO);
    fH = fH > 0 ? fH : fH + DEGREES_FOUR; // 色相最大值为360
    fS *= S1;
    fL *= L1;
    HSLtoRGB(&dwColor, fH, fS, fL);
    return dwColor;
}

HBITMAP ConvertIconToBitmap(HICON hIcon, int *x, int *y)
{
    ICONINFO csII;
    SecureZeroMemory(&csII, sizeof(csII));
    if (!::GetIconInfo(hIcon, &csII) || x == nullptr || y == nullptr) {
        return nullptr;
    }
    DWORD dwWidth = csII.xHotspot * TWO_UNIT;
    DWORD dwHeight = csII.yHotspot * TWO_UNIT;
    *x = dwWidth;
    *y = dwHeight;
    HDC hDc = ::GetDC(nullptr);
    HDC hMemDc = ::CreateCompatibleDC(hDc);
    HBITMAP hBitMap = ::CreateCompatibleBitmap(hDc, dwWidth, dwHeight);
    if (!hBitMap) {
        return nullptr;
    }
    ::SelectObject(hMemDc, hBitMap);
    ::DrawIconEx(hMemDc, 0, 0, hIcon, dwWidth, dwHeight, 0, nullptr, DI_NORMAL);
    ::DeleteDC(hMemDc);
    ::ReleaseDC(nullptr, hDc);
    ::DestroyIcon(hIcon);
    return hBitMap;
}

void GetNextDpiIndex(const int dpi_array_index, const int dpi_array_max_index, const int round,
    int &dpi_array_try_index, int &step)
{
    if (dpi_array_index + step <= dpi_array_max_index && dpi_array_index - step >= 0) {
        if (round % TWO_UNIT != 0) { // 判断奇偶数
            dpi_array_try_index = dpi_array_index + step;
        } else {
            dpi_array_try_index = dpi_array_index - step;
            step++;
        }
    } else if (dpi_array_index + step <= dpi_array_max_index) {
        dpi_array_try_index = dpi_array_index + step;
        step++;
    } else {
        dpi_array_try_index = dpi_array_index - step;
        step++;
    }
}
CDuiString GetDpiImage(const CDuiString &sRawImage, int type, int &dpi)
{
    CDuiString sDpiImage = sRawImage;
    dpi = g_nDpi[TWO_UNIT]; // default picture is 150%
    int nDotIndex = sRawImage.ReverseFind(_T('.'));
    if (nDotIndex < 0) {
        return sDpiImage;
    }
    int dpi_array_max_index = (sizeof(g_strDpi) / sizeof(CDuiString)) - 1;
    int dpi_array_index = Dpi2Index(CResourceManager::GetInstance()->GetScale());
    int dpi_array_try_index = dpi_array_index;
    int step = 1;
    int round = 0;
    while (dpi_array_try_index >= 0 && dpi_array_try_index <= dpi_array_max_index) {
        round++;
        CDuiString strDpi = g_strDpi[dpi_array_try_index];
        if (strDpi.IsEmpty() == false) {
            sDpiImage = sRawImage.Left(nDotIndex);
            sDpiImage += _T("@");
            sDpiImage += strDpi;
            sDpiImage += sRawImage.Mid(nDotIndex);
        } else {
            sDpiImage = sRawImage;
        }
        CDuiString strDpiImageFullPath = (type == 0 ? CPaintManagerUI::GetResourcePath() + sDpiImage : sDpiImage);
        HANDLE hFile = ::CreateFile(strDpiImageFullPath.GetData(), GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            dpi = g_nDpi[dpi_array_try_index];
            break;
        }
        GetNextDpiIndex(dpi_array_index, dpi_array_max_index, round, dpi_array_try_index, step);
    }
    return sDpiImage;
}
Gdiplus::Bitmap *GetGdiPlusBitmap(BYTE *pData, int dwSize, IStream *&pStream, HGLOBAL &hGlobal)
{
    if (pData == nullptr || dwSize <= 0) {
        return nullptr;
    }

    pStream = nullptr;
    Gdiplus::Bitmap *gdiBitmap = nullptr;
    hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, dwSize);
    if (hGlobal) {
        LPVOID pvData = GlobalLock(hGlobal);
        if (pvData) {
            errno_t ret = memcpy_s(pvData, dwSize, pData, dwSize);
            if (ret != 0) {
            }
            if (SUCCEEDED(CreateStreamOnHGlobal(hGlobal, FALSE, &pStream))) {
                gdiBitmap = new Gdiplus::Bitmap(pStream);
            }
        }
    }
    return gdiBitmap;
}
Gdiplus::Bitmap *GetScaledGdiplusBitmap(Gdiplus::Bitmap *gdiBitmap, int nWidth, int nHeight, bool highQuality)
{
    if (gdiBitmap == nullptr) {
        return nullptr;
    }

    Gdiplus::Bitmap *pScaledBmp = new Gdiplus::Bitmap(nWidth, nHeight, gdiBitmap->GetPixelFormat());
    if (pScaledBmp) {
        Gdiplus::Graphics *graphics = Gdiplus::Graphics::FromImage(pScaledBmp);
        if (graphics) {
            Gdiplus::ImageAttributes attr;
            attr.SetWrapMode(Gdiplus::WrapModeTile);
            graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            // 使用其他插值模式会使图片出现杂质
            graphics->SetInterpolationMode(highQuality ? Gdiplus::InterpolationModeHighQualityBicubic :
                Gdiplus::InterpolationModeLowQuality);
            graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
            Gdiplus::RectF dstRect(0, 0, static_cast<Gdiplus::REAL>(nWidth), static_cast<Gdiplus::REAL>(nHeight));
            graphics->DrawImage(gdiBitmap, dstRect, 0, 0, static_cast<Gdiplus::REAL>(gdiBitmap->GetWidth()),
                static_cast<Gdiplus::REAL>(gdiBitmap->GetHeight()), Gdiplus::UnitPixel, &attr);
            delete graphics;
            graphics = nullptr;
        } else {
            delete pScaledBmp;
            pScaledBmp = nullptr;
        }
    }
    return pScaledBmp;
}
void GdiPlusBmp2HBitmap(Gdiplus::Bitmap *pScaledBmp, int nWidth, int nHeight, HBITMAP &hBitmap, bool &bAlpha,
    bool bRev = false)
{
    if (pScaledBmp == nullptr) {
        return;
    }

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    bmi.bmiHeader.biHeight = -nHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = BIT_COUNT;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = nWidth * nHeight * FOUR_UNIT;

    LPBYTE pDest = nullptr;
    hBitmap = ::CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, reinterpret_cast<void **>(&pDest), nullptr, 0);
    if (hBitmap && pDest) {
        Gdiplus::BitmapData bitmapData { 0 };
        Gdiplus::Rect rect(0, 0, nWidth, nHeight);

        Gdiplus::Status s;
        s = pScaledBmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &bitmapData);
        if (s != Gdiplus::Ok) {
            DeleteObject(hBitmap);
            hBitmap = nullptr;
            return;
        }

        if (bitmapData.Stride != nWidth * FOUR_UNIT) {
            pScaledBmp->UnlockBits(&bitmapData);
            DeleteObject(hBitmap);
            hBitmap = nullptr;
            return;
        }

        memcpy_s(pDest, bmi.bmiHeader.biSizeImage, bitmapData.Scan0, bmi.bmiHeader.biSizeImage);
        bAlpha = false;
        pScaledBmp->UnlockBits(&bitmapData);

        for (int i = 0; i < nWidth * nHeight; i++) {
            if (pDest[i * FOUR_UNIT + THREE_UNIT] < RGB_MAX_VALUE) {
                bAlpha = true;
                break;
            }
        }

        if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL && bRev) {
            for (int i = 0; i < nHeight; i++) {
                for (int j = 0; j < nWidth / TWO_UNIT; j++) {
                    UINT uTemp = *(reinterpret_cast<UINT *>(
                        &(pDest[i * nWidth * FOUR_UNIT + j * FOUR_UNIT])));
                    *(reinterpret_cast<UINT *>(&(pDest[i * nWidth * FOUR_UNIT + j * FOUR_UNIT]))) =
                        *(reinterpret_cast<UINT *>(
                            &(pDest[i * nWidth * FOUR_UNIT + (nWidth - j - 1) * FOUR_UNIT])));
                    *(reinterpret_cast<UINT *>(
                        &(pDest[i * nWidth * FOUR_UNIT + (nWidth - j - 1) * FOUR_UNIT]))) = uTemp;
                }
            }
        }
    }
}
void ScaleBitmap(BYTE *pData, int dwSize, int nDataDpi, HBITMAP &hBitmap, int &nWidth, int &nHeight, bool &bAlpha,
    bool bRev, bool highQuality)
{
    hBitmap = nullptr;
    bAlpha = true;

    IStream *pStream = nullptr;
    HGLOBAL hGlobal = nullptr;
    Gdiplus::Bitmap *pOriginalBmp = GetGdiPlusBitmap(pData, dwSize, pStream, hGlobal);
    if (pOriginalBmp) {
        Gdiplus::Bitmap *pScaledBmp = nullptr;
        pScaledBmp = GetScaledGdiplusBitmap(pOriginalBmp, nWidth, nHeight, highQuality);
        if (pScaledBmp) {
            delete pOriginalBmp;
            pOriginalBmp = nullptr;
        }
        if (pScaledBmp == nullptr) {
            pScaledBmp = pOriginalBmp;
            nWidth = static_cast<int>(pOriginalBmp->GetWidth());
            nHeight = static_cast<int>(pOriginalBmp->GetHeight());
        }
        GdiPlusBmp2HBitmap(pScaledBmp, nWidth, nHeight, hBitmap, bAlpha, bRev);
        delete pScaledBmp;
    }
    if (pStream) {
        pStream->Release();
    }
    if (hGlobal) {
        GlobalUnlock(hGlobal);
        GlobalFree(hGlobal);
    }
}
TImageInfo *CRenderEngine::LoadImage(STRINGorID bitmap, LPCTSTR type, DWORD mask, HINSTANCE instance, bool bRev,
    bool absPath)
{
    DuiLib::CDuiString str(bitmap.m_lpstr);

    if (_tcscmp(_T(".exe"), str.Right(FOUR_UNIT)) == 0) {
        SHFILEINFO FileInfo;
        SecureZeroMemory(&FileInfo, sizeof(FileInfo));
        (void)::SHGetFileInfo(str.GetData(), 0, &FileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
        HICON hIcon = FileInfo.hIcon;
        if (hIcon == nullptr) {
            wostringstream streamImagePath;
            streamImagePath << str.Left(str.ReverseFind('.')).GetData() << ".ico";
            WIN32_FIND_DATA FindFileData;
            HANDLE hFind = FindFirstFile(streamImagePath.str().c_str(), &FindFileData);
            if (hFind == INVALID_HANDLE_VALUE) {
                hIcon = (HICON)::LoadImage(nullptr, _T("res\\DefaultIcon.ico"), IMAGE_ICON, 0, 0,
                    LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
            } else {
                if (!FindClose(hFind)) {
                    ASSERT(FALSE);
                }
                hIcon = (HICON)::LoadImage(nullptr, streamImagePath.str().c_str(), IMAGE_ICON, 0, 0,
                    LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
            }
        }


        if (hIcon != nullptr) {
            TImageInfo *data = new TImageInfo;
            data->pBits = nullptr;
            data->pSrcBits = nullptr;
            data->hBitmap = ConvertIconToBitmap(hIcon, &(data->nX), &(data->nY));
            data->bAlpha = true;
            data->dwMask = mask;

            ::DeleteObject(hIcon);

            return data;
        } else {
            return nullptr;
        }
    }

    int nLoadedImageDpi = g_nDpi[TWO_UNIT]; // 150% image
    LPBYTE pData = nullptr;
    DWORD dwSize = 0;
    do {
        if (type == nullptr) {
            CDuiString sFile = CPaintManagerUI::GetResourcePath();
            sFile += GetDpiImage(str, 0, nLoadedImageDpi);
            if (absPath) {
                sFile = str;
            }
            HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile == INVALID_HANDLE_VALUE) {
                break;
            }
            dwSize = ::GetFileSize(hFile, nullptr);
            if (dwSize == 0) {
                ::CloseHandle(hFile);
                hFile = nullptr;
                break;
            }

            DWORD dwRead = 0;
            pData = new BYTE[dwSize];
            ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
            ::CloseHandle(hFile);

            if (dwRead != dwSize) {
                delete[] pData;
                pData = nullptr;
                break;
            }
        } else {
            HINSTANCE dllinstance = nullptr;
            if (instance) {
                dllinstance = instance;
            } else {
                dllinstance = CPaintManagerUI::GetResourceDll();
            }
            HRSRC hResource = ::FindResource(dllinstance, bitmap.m_lpstr, type);
            if (hResource == nullptr) {
                break;
            }
            HGLOBAL hGlobal = ::LoadResource(dllinstance, hResource);
            if (hGlobal == nullptr) {
                FreeResource(hResource);
                break;
            }

            dwSize = ::SizeofResource(dllinstance, hResource);
            if (dwSize == 0) {
                break;
            }
            pData = new BYTE[dwSize];
            if (pData != nullptr) { // add for AR000BKK5K
                LPBYTE pRes = (LPBYTE)::LockResource(hGlobal);
                if (pRes != nullptr) {
                    ::memcpy_s(pData, dwSize, pRes, dwSize);
                }
            }
            ::FreeResource(hResource);
        }
    } while (0);

    while (!pData) {
        // 读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(GetDpiImage(str, 1, nLoadedImageDpi), GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            break;
        }
        dwSize = ::GetFileSize(hFile, nullptr);
        if (dwSize == 0) {
            ::CloseHandle(hFile);
            break;
        }

        DWORD dwRead = 0;
        pData = new BYTE[dwSize];
        ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
        ::CloseHandle(hFile);

        if (dwRead != dwSize) {
            delete[] pData;
            pData = nullptr;
        }
        break;
    }
    if (!pData) {
        return nullptr;
    }

    if (1) {
        LPBYTE pImage = nullptr;
        int x, y, n;
        pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, FOUR_UNIT);
        if (!pImage) {
            if (pData) {
                delete[] pData;
                pData = nullptr;
            }
            return nullptr;
        }

        BITMAPINFO bmi;
        ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = x;
        bmi.bmiHeader.biHeight = -y;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = BIT_COUNT;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = x * y * FOUR_UNIT;
        bool rtl = (UICulture::GetInstance()->getDirection() == DIRECTION_RTL);
        bool bAlphaChannel = false;
        LPBYTE pDest = nullptr;
        HBITMAP hBitmap = ::CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS,
            reinterpret_cast<void **>(&pDest), nullptr, 0);
        if (!hBitmap || !pDest) {
            if (pData) {
                delete[] pData;
                pData = nullptr;
            }
            stbi_image_free(pImage);
            return nullptr;
        }

        for (int i = 0; i < x * y; i++) {
            pDest[i * FOUR_UNIT + THREE_UNIT] = pImage[i * FOUR_UNIT + THREE_UNIT];
            if (pDest[i * FOUR_UNIT + THREE_UNIT] < RGB_MAX_VALUE) {
                pDest[i * FOUR_UNIT] = (BYTE)(DWORD(pImage[
                    i * FOUR_UNIT + TWO_UNIT]) * pImage[i * FOUR_UNIT + THREE_UNIT] / RGB_MAX_VALUE);
                pDest[i * FOUR_UNIT + 1] = (BYTE)(DWORD(pImage[
                    i * FOUR_UNIT + 1]) * pImage[i * FOUR_UNIT + THREE_UNIT] / RGB_MAX_VALUE);
                pDest[i * FOUR_UNIT + TWO_UNIT] = (BYTE)(DWORD(pImage[
                    i * FOUR_UNIT]) * pImage[i * FOUR_UNIT + THREE_UNIT] / RGB_MAX_VALUE);
                bAlphaChannel = true;
            } else {
                pDest[i * FOUR_UNIT] = pImage[i * FOUR_UNIT + TWO_UNIT];
                pDest[i * FOUR_UNIT + 1] = pImage[i * FOUR_UNIT + 1];
                pDest[i * FOUR_UNIT + TWO_UNIT] = pImage[i * FOUR_UNIT];
            }
            int xRemain = i % x;
            int yPos = i / x * x + x - 1 - xRemain;
            if (rtl && bRev) {
                pDest[i * FOUR_UNIT] = pImage[yPos * FOUR_UNIT + TWO_UNIT];
                pDest[i * FOUR_UNIT + 1] = pImage[yPos * FOUR_UNIT + 1];
                pDest[i * FOUR_UNIT + TWO_UNIT] = pImage[yPos * FOUR_UNIT];
            }
            if (*(reinterpret_cast<DWORD *>(&pDest[i * FOUR_UNIT])) == mask) {
                pDest[i * FOUR_UNIT] = (BYTE)0;
                pDest[i * FOUR_UNIT + 1] = (BYTE)0;
                pDest[i * FOUR_UNIT + TWO_UNIT] = (BYTE)0;
                pDest[i * FOUR_UNIT + THREE_UNIT] = (BYTE)0;
                bAlphaChannel = true;
            }
        }

        stbi_image_free(pImage);

        TImageInfo *data = new (std::nothrow) TImageInfo;
        if (data) {
            data->pBits = nullptr;
            data->pSrcBits = pData;
            data->isrcsize = dwSize;
            data->hBitmap = hBitmap;
            data->nX = x;
            data->nY = y;
            data->bAlpha = bAlphaChannel;
        }
        return data;
    }
}

bool CRenderEngine::LoadRes(STRINGorID bitmap, LPBYTE *pOutData, DWORD &size, LPCTSTR type, DWORD mask,
    HINSTANCE instance)
{
    DuiLib::CDuiString str(bitmap.m_lpstr);
    *pOutData = nullptr;

    if (_tcscmp(_T(".exe"), str.Right(FOUR_UNIT)) == 0) {
        SHFILEINFO FileInfo;
        SecureZeroMemory(&FileInfo, sizeof(FileInfo));
        (void)::SHGetFileInfo(str.GetData(), 0, &FileInfo, sizeof(SHFILEINFO), SHGFI_ICON);
        HICON hIcon = FileInfo.hIcon;
        if (hIcon == nullptr) {
            wostringstream streamImagePath;
            streamImagePath << str.Left(str.ReverseFind('.')).GetData() << ".ico";
            WIN32_FIND_DATA FindFileData;
            HANDLE hFind = FindFirstFile(streamImagePath.str().c_str(), &FindFileData);
            if (hFind == INVALID_HANDLE_VALUE) {
                hIcon = (HICON)::LoadImage(nullptr, _T("res\\DefaultIcon.ico"), IMAGE_ICON, 0, 0,
                    LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
            } else {
                if (!FindClose(hFind)) {
                    ASSERT(FALSE);
                }
                hIcon = (HICON)::LoadImage(nullptr, streamImagePath.str().c_str(), IMAGE_ICON, 0, 0,
                    LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
            }
        }


        if (hIcon != nullptr) {
            ::DeleteObject(hIcon);
            return TRUE;
        } else {
            return FALSE;
        }
    }

    int nLoadedDpi = g_nDpi[TWO_UNIT];
    LPBYTE pData = nullptr;
    DWORD dwSize = 0;
    do {
        if (type == nullptr) {
            CDuiString sFile = CPaintManagerUI::GetResourcePath();

            sFile += GetDpiImage(str, 0, nLoadedDpi);
            HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile == INVALID_HANDLE_VALUE)
                break;
            dwSize = ::GetFileSize(hFile, nullptr);
            if (dwSize == 0) {
                ::CloseHandle(hFile);
                hFile = nullptr;
                break;
            }

            DWORD dwRead = 0;
            pData = new BYTE[dwSize];
            ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
            ::CloseHandle(hFile);

            if (dwRead != dwSize) {
                delete[] pData;
                pData = nullptr;
                break;
            }
        } else {
            HINSTANCE dllinstance = nullptr;
            if (instance) {
                dllinstance = instance;
            } else {
                dllinstance = CPaintManagerUI::GetResourceDll();
            }
            HRSRC hResource = ::FindResource(dllinstance, bitmap.m_lpstr, type);
            if (hResource == nullptr)
                break;
            HGLOBAL hGlobal = ::LoadResource(dllinstance, hResource);
            if (hGlobal == nullptr) {
                FreeResource(hResource);
                break;
            }

            dwSize = ::SizeofResource(dllinstance, hResource);
            if (dwSize == 0)
                break;
            pData = new BYTE[dwSize];
            LPBYTE hData = (LPBYTE)::LockResource(hGlobal);
            if (hData == nullptr) {
                ::FreeResource(hResource);
                break;
            }
            ::memcpy_s(pData, dwSize, hData, dwSize);
            ::FreeResource(hResource);
            *pOutData = pData;
        }
    } while (0);

    while (!pData) {
        // 读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(GetDpiImage(str, 1, nLoadedDpi), GENERIC_READ, FILE_SHARE_READ, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            break;
        }
        dwSize = ::GetFileSize(hFile, nullptr);
        if (dwSize == 0) {
            ::CloseHandle(hFile);
            hFile = nullptr;
            break;
        }

        DWORD dwRead = 0;
        pData = new BYTE[dwSize];
        ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
        ::CloseHandle(hFile);

        if (dwRead != dwSize) {
            delete[] pData;
            pData = nullptr;
        }
        break;
    }

    *pOutData = pData;
    size = dwSize;

    return (*pOutData != nullptr);
}

#ifdef USE_XIMAGE_EFFECT
static DWORD LoadImage2Memory(const STRINGorID &bitmap, LPCTSTR type, LPBYTE &pData)
{
    assert(pData == nullptr);
    pData = nullptr;
    DWORD dwSize(0U);
    do {
        if (type == nullptr) {
            CDuiString sFile = CPaintManagerUI::GetResourcePath();
            if (CPaintManagerUI::GetResourceZip().IsEmpty()) {
                sFile += bitmap.m_lpstr;
                HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, nullptr);
                if (hFile == INVALID_HANDLE_VALUE) {
                    break;
                }
                dwSize = ::GetFileSize(hFile, nullptr);
                if (dwSize == 0) {
                    break;
                }

                DWORD dwRead = 0;
                pData = new BYTE[dwSize + 1];
                SecureZeroMemory(pData, dwSize + 1);
                ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
                ::CloseHandle(hFile);

                if (dwRead != dwSize) {
                    delete[] pData;
                    pData = nullptr;
                    dwSize = 0U;
                    break;
                }
            } else {
                sFile += CPaintManagerUI::GetResourceZip();
                HZIP hz = nullptr;
                if (CPaintManagerUI::IsCachedResourceZip()) {
                    hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
                } else {
                    hz = OpenZip(reinterpret_cast<void *>(sFile.GetData()), 0, TWO_UNIT);
                }
                if (hz == nullptr) {
                    break;
                }
                ZIPENTRY ze;
                int i;
                if (FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0)
                    break;
                dwSize = ze.unc_size;
                if (dwSize == 0) {
                    break;
                }
                pData = new BYTE[dwSize];
                int res = UnzipItem(hz, i, pData, dwSize, THREE_UNIT);
                if (res != 0x00000000 && res != 0x00000600) {
                    delete[] pData;
                    pData = nullptr;
                    dwSize = 0U;
                    if (!CPaintManagerUI::IsCachedResourceZip()) {
                        CloseZip(hz);
                    }
                    break;
                }
                if (!CPaintManagerUI::IsCachedResourceZip()) {
                    CloseZip(hz);
                }
            }
        } else {
            HINSTANCE hDll = CPaintManagerUI::GetResourceDll();
            HRSRC hResource = ::FindResource(hDll, bitmap.m_lpstr, type);
            if (hResource == nullptr)
                break;
            HGLOBAL hGlobal = ::LoadResource(hDll, hResource);
            if (hGlobal == nullptr) {
                FreeResource(hResource);
                break;
            }

            dwSize = ::SizeofResource(hDll, hResource);
            if (dwSize == 0) {
                break;
            }
            pData = new BYTE[dwSize];
            ::memcpy_s(pData, dwSize, (LPBYTE)::LockResource(hGlobal), dwSize);
            ::FreeResource(hResource);
        }
    } while (0);

    while (!pData) {
        // 读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            break;
        }
        dwSize = ::GetFileSize(hFile, nullptr);
        if (dwSize == 0) {
            break;
        }

        DWORD dwRead = 0;
        pData = new BYTE[dwSize];
        ::ReadFile(hFile, pData, dwSize, &dwRead, nullptr);
        ::CloseHandle(hFile);

        if (dwRead != dwSize) {
            delete[] pData;
            pData = nullptr;
            dwSize = 0U;
        }
        break;
    }
    return dwSize;
}
CxImage *CRenderEngine::LoadGifImageX(STRINGorID bitmap, LPCTSTR type, DWORD mask)
{
    LPBYTE pData = nullptr;
    DWORD dwSize = LoadImage2Memory(bitmap, type, pData);
    if (dwSize == 0U || !pData) {
        return nullptr;
    }
    CxImage *pImg = nullptr;
    if (pImg = new CxImage()) {
        pImg->SetRetreiveAllFrames(TRUE);
        if (!pImg->Decode(pData, dwSize, CXIMAGE_FORMAT_GIF)) {
            delete pImg;
            pImg = nullptr;
        }
    }
    delete[] pData;
    pData = nullptr;
    return pImg;
}
#endif // USE_XIMAGE_EFFECT
void CRenderEngine::FreeImage(TImageInfo *bitmap, bool bDelete)
{
    if (bitmap == nullptr) {
        return;
    }
    if (bitmap->hBitmap) {
        ::DeleteObject(bitmap->hBitmap);
    }
    bitmap->hBitmap = nullptr;
    if (bitmap->pBits) {
        delete[] bitmap->pBits;
    }
    bitmap->pBits = nullptr;
    if (bitmap->pSrcBits) {
        delete[] bitmap->pSrcBits;
    }
    bitmap->pSrcBits = nullptr;
    if (bDelete) {
        delete bitmap;
        bitmap = nullptr;
    }
}

bool CRenderEngine::DrawIconImageString(HDC hDC, CPaintManagerUI *pManager, const RECT &rc,
    const RECT &rcPaint, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    if ((pManager == nullptr) || (hDC == nullptr)) {
        return false;
    }

    // 1、aaa.jpg
    // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0'
    // mask='#FF0000' fade='255' hole='FALSE' xtiled='FALSE' ytiled='FALSE'
    CDuiString sImageName = pStrImage;
    CDuiString sImageResType;
    RECT rcItem = rc;
    RECT rcBmpPart = { 0 };
    RECT rcCorner = { 0 };
    DWORD dwMask = 0;
    BYTE bFade = 0xFF;
    bool bHole = false;
    bool bTiledX = true;
    bool bTiledY = true;
    CDuiSize szIcon(0, 0);

    int image_count = 0;

    CDuiString sItem;
    CDuiString sValue;
    LPTSTR pstr = nullptr;

    for (int i = 0; i < TWO_UNIT; ++i, image_count = 0) {
        if (i == 1) {
            pStrImage = pStrModify;
        }
        if (!pStrImage) {
            continue;
        }
        while (*pStrImage != _T('\0')) {
            sItem.Empty();
            sValue.Empty();
            while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage < pstrTemp) {
                    sItem += *pStrImage++;
                }
            }
            while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (*pStrImage++ != _T('=')) {
                break;
            }
            while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (*pStrImage++ != _T('\'')) {
                break;
            }
            while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage < pstrTemp) {
                    sValue += *pStrImage++;
                }
            }
            if (*pStrImage++ != _T('\'')) {
                break;
            }

            if (!sValue.IsEmpty()) {
                if (sItem == _T("file") || sItem == _T("res")) {
                    if (image_count > 0) {
                        CRenderEngine::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType, rcItem,
                            rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);
                    }

                    sImageName = sValue;
                    ++image_count;
                } else if (sItem == _T("restype")) {
                    if (image_count > 0) {
                        CRenderEngine::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType, rcItem,
                            rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);
                    }

                    sImageResType = sValue;
                    ++image_count;
                } else if (sItem == _T("dest")) {
                    rcItem.left = rc.left + _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);

                    if (rcItem.right > rc.right) {
                        rcItem.right = rc.right;
                    }

                    rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    if (rcItem.bottom > rc.bottom) {
                        rcItem.bottom = rc.bottom;
                    }
                } else if (sItem == _T("source")) {
                    rcBmpPart.left = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcBmpPart.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcBmpPart.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                } else if (sItem == _T("corner")) {
                    rcCorner.left = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                } else if (sItem == _T("mask")) {
                    if (sValue[0] == _T('#')) {
                        dwMask = _tcstoul(sValue.GetData() + 1, &pstr, RADIX_TYPE_TWO_VALUE);
                    } else {
                        dwMask = _tcstoul(sValue.GetData(), &pstr, RADIX_TYPE_TWO_VALUE);
                    }
                } else if (sItem == _T("fade")) {
                    bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                } else if (sItem == _T("hole")) {
                    bHole = (_tcsicmp(sValue.GetData(), _T("TRUE")) == 0);
                } else if (sItem == _T("xtiled")) {
                    bTiledX = (_tcsicmp(sValue.GetData(), _T("TRUE")) == 0);
                } else if (sItem == _T("ytiled")) {
                    bTiledY = (_tcsicmp(sValue.GetData(), _T("TRUE")) == 0);
                } else if (sItem == _T("iconsize")) {
                    szIcon.cx = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    szIcon.cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                } else if (sItem == _T("iconalign")) {
                    MakeFitIconDest(rcItem, szIcon, sValue, rcItem);
                }
            }
            if (*pStrImage++ != _T(' '))
                break;
        }
    }

    CRenderEngine::DrawImage(hDC, pManager, rc, rcPaint, sImageName, sImageResType, rcItem, rcBmpPart, rcCorner, dwMask,
        bFade, bHole, bTiledX, bTiledY);

    return true;
}

bool CRenderEngine::MakeFitIconDest(const RECT &rcControl, const CDuiSize &szIcon, const CDuiString &sAlign,
    RECT &rcDest)
{
    ASSERT(!sAlign.IsEmpty());
    if (sAlign == _T("left")) {
        rcDest.left = rcControl.left;
        rcDest.top = rcControl.top;
        rcDest.right = rcDest.left + szIcon.cx;
        rcDest.bottom = rcDest.top + szIcon.cy;
    } else if (sAlign == _T("center")) {
        rcDest.left = rcControl.left + ((rcControl.right - rcControl.left) - szIcon.cx) / TWO_UNIT;
        rcDest.top = rcControl.top + ((rcControl.bottom - rcControl.top) - szIcon.cy) / TWO_UNIT;
        rcDest.right = rcDest.left + szIcon.cx;
        rcDest.bottom = rcDest.top + szIcon.cy;
    } else if (sAlign == _T("vcenter")) {
        rcDest.left = rcControl.left;
        rcDest.top = rcControl.top + ((rcControl.bottom - rcControl.top) - szIcon.cy) / TWO_UNIT;
        rcDest.right = rcDest.left + szIcon.cx;
        rcDest.bottom = rcDest.top + szIcon.cy;
    } else if (sAlign == _T("hcenter")) {
        rcDest.left = rcControl.left + ((rcControl.right - rcControl.left) - szIcon.cx) / TWO_UNIT;
        rcDest.top = rcControl.top;
        rcDest.right = rcDest.left + szIcon.cx;
        rcDest.bottom = rcDest.top + szIcon.cy;
    }

    if (rcDest.right > rcControl.right) {
        rcDest.right = rcControl.right;
    }

    if (rcDest.bottom > rcControl.bottom) {
        rcDest.bottom = rcControl.bottom;
    }

    return true;
}

TImageInfo *CRenderEngine::LoadImage(LPCTSTR pStrImage, LPCTSTR type, DWORD mask, HINSTANCE instance, bool bRev,
    bool absPath)
{
    if (pStrImage == nullptr) {
        return nullptr;
    }

    CDuiString sStrPath = pStrImage;
    if (type == nullptr) {
        sStrPath = CResourceManager::GetInstance()->GetImagePath(pStrImage);
        if (sStrPath.IsEmpty()) {
            sStrPath = pStrImage;
        }
    }
    return LoadImage(STRINGorID(sStrPath.GetData()), type, mask, instance, bRev, absPath);
}

TImageInfo *CRenderEngine::LoadImage(UINT nID, LPCTSTR type, DWORD mask, HINSTANCE instance)
{
    return LoadImage(STRINGorID(nID), type, mask, instance);
}

void CRenderEngine::DrawText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText, DWORD dwTextColor,
    int iFont, UINT uStyle, DWORD dwTextBKColor, BOOL bTransparent)
{
    if (pstrText == nullptr || pManager == nullptr) {
        return;
    }
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        uStyle = uStyle | DT_RTLREADING;
    }
    if (bTransparent) {
        ::SetBkMode(hDC, TRANSPARENT);
    } else {
        ::SetBkMode(hDC, OPAQUE);
    }
    ::SetBkColor(hDC, RGB(GetBValue(dwTextBKColor), GetGValue(dwTextBKColor), GetRValue(dwTextBKColor)));
    ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
    HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
    ::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
    ::SelectObject(hDC, hOldFont);
}

bool CRenderEngine::DrawImage(HDC hDC, CPaintManagerUI *pManager, const RECT &rcItem, const RECT &rcPaint,
    TDrawInfo &drawInfo)
{
    if (pManager == nullptr) {
        return false;
    }
    const TImageInfo *data = pManager->GetImageEx((LPCTSTR)drawInfo.sImageName, 0, 0, false, 0, true);
    if (data == nullptr) {
        return false;
    }
    RECT rcDest = { 0, 0, data->nX, data->nY };
    RECT rcTemp = { 0 };
    if (data->hBitmap == nullptr) {
        return false;
    }
    DrawImage(hDC, data->hBitmap, rcItem, rcPaint, rcDest, rcTemp, false, drawInfo.uFade, drawInfo.bHole,
        drawInfo.bTiledX, drawInfo.bTiledY);
    return true;
}

void CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT &rc, const RECT &rcPaint, const RECT &rcBmpPart,
    const RECT &rcCorners, bool bAlpha, BYTE uFade, bool hole, bool xtiled, bool ytiled)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    using LPALPHABLEND = BOOL(WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend =
        (LPALPHABLEND)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

    if (lpAlphaBlend == nullptr) {
        lpAlphaBlend = AlphaBitBlt;
    }
    if (hBitmap == nullptr) {
        return;
    }

    HDC hCloneDC = ::CreateCompatibleDC(hDC);
    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, hBitmap);
    ::SetStretchBltMode(hDC, HALFTONE);

    RECT rcTemp = { 0 };
    RECT rcDest = { 0 };
    if (lpAlphaBlend && (bAlpha || uFade < RGB_MAX_VALUE)) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
        // middle
        if (!hole) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                if (!xtiled && !ytiled) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right,
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                } else if (xtiled && ytiled) {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                    if (lWidth > 0 && lHeight > 0) {
                        int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for (int j = 0; j < iTimesY; ++j) {
                            LONG lDestTop = rcDest.top + lHeight * j;
                            LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;
                            if (lDestBottom > rcDest.bottom) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            for (int i = 0; i < iTimesX; ++i) {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if (lDestRight > rcDest.right) {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }
                                lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j,
                                    lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC,
                                    rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth,
                                    lDrawHeight, bf);
                            }
                        }
                    }
                } else if (xtiled) {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                    if (lWidth > 0) {
                        int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        for (int i = 0; i < iTimes; ++i) {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if (lDestRight > rcDest.right) {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }
                            lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, hCloneDC,
                                rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth,
                                rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                        }
                    }
                } else { // ytiled
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                    if (lHeight > 0) {
                        int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for (int i = 0; i < iTimes; ++i) {
                            LONG lDestTop = rcDest.top + lHeight * i;
                            LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;
                            if (lDestBottom > rcDest.bottom) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right,
                                lDestBottom - lDestTop, hCloneDC, rcBmpPart.left + rcCorners.left,
                                rcBmpPart.top + rcCorners.top,
                                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);
                        }
                    }
                }
            }
        }

        // left-top
        if (rcCorners.left > 0 && rcCorners.top > 0) {
            rcDest.left = rc.left;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                    rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
            }
        }
        // top
        if (rcCorners.top > 0) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.top;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                    rcBmpPart.left + rcCorners.left, rcBmpPart.top,
                    rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.top, bf);
            }
        }
        // right-top
        if (rcCorners.right > 0 && rcCorners.top > 0) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                    rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
            }
        }
        // left
        if (rcCorners.left > 0) {
            rcDest.left = rc.left;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                    rcBmpPart.top + rcCorners.top, rcCorners.left,
                    rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
            }
        }
        // right
        if (rcCorners.right > 0) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.top + rcCorners.top;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                    rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right,
                    rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
            }
        }
        // left-bottom
        if (rcCorners.left > 0 && rcCorners.bottom > 0) {
            rcDest.left = rc.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.left;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                    rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
            }
        }
        // bottom
        if (rcCorners.bottom > 0) {
            rcDest.left = rc.left + rcCorners.left;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                    rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom,
                    rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
            }
        }
        // right-bottom
        if (rcCorners.right > 0 && rcCorners.bottom > 0) {
            rcDest.left = rc.right - rcCorners.right;
            rcDest.top = rc.bottom - rcCorners.bottom;
            rcDest.right = rcCorners.right;
            rcDest.bottom = rcCorners.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;
            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                    rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right,
                    rcCorners.bottom, bf);
            }
        }
    } else {
        if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left &&
            rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top && rcCorners.left == 0 && rcCorners.right == 0 &&
            rcCorners.top == 0 && rcCorners.bottom == 0) {
            if (::IntersectRect(&rcTemp, &rcPaint, &rc)) {
                ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, hCloneDC,
                    rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
            }
        } else {
            // middle
            if (!hole) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    if (!xtiled && !ytiled) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                            rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
                            rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right,
                            rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                    } else if (xtiled && ytiled) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for (int j = 0; j < iTimesY; ++j) {
                            LONG lDestTop = rcDest.top + lHeight * j;
                            LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;
                            if (lDestBottom > rcDest.bottom) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            for (int i = 0; i < iTimesX; ++i) {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if (lDestRight > rcDest.right) {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }
                                ::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j,
                                    lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC,
                                    rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
                            }
                        }
                    } else if (xtiled) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        for (int i = 0; i < iTimes; ++i) {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if (lDestRight > rcDest.right) {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }
                            ::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, hCloneDC,
                                rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth,
                                rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                        }
                    } else { // ytiled
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for (int i = 0; i < iTimes; ++i) {
                            LONG lDestTop = rcDest.top + lHeight * i;
                            LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;
                            if (lDestBottom > rcDest.bottom) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            ::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right,
                                lDestBottom - lDestTop, hCloneDC, rcBmpPart.left + rcCorners.left,
                                rcBmpPart.top + rcCorners.top,
                                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight,
                                SRCCOPY);
                        }
                    }
                }
            }

            // left-top
            if (rcCorners.left > 0 && rcCorners.top > 0) {
                rcDest.left = rc.left;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                        rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
                }
            }
            // top
            if (rcCorners.top > 0) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top,
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
                }
            }
            // right-top
            if (rcCorners.right > 0 && rcCorners.top > 0) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
                }
            }
            // left
            if (rcCorners.left > 0) {
                rcDest.left = rc.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                        rcBmpPart.top + rcCorners.top, rcCorners.left,
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                }
            }
            // right
            if (rcCorners.right > 0) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right,
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                }
            }
            // left-bottom
            if (rcCorners.left > 0 && rcCorners.bottom > 0) {
                rcDest.left = rc.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, rcBmpPart.left,
                        rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
                }
            }
            // bottom
            if (rcCorners.bottom > 0) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom,
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
                }
            }
            // right-bottom
            if (rcCorners.right > 0 && rcCorners.bottom > 0) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest)) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                        rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right,
                        rcCorners.bottom, SRCCOPY);
                }
            }
        }
    }

    ::SelectObject(hCloneDC, hOldBitmap);
    ::DeleteDC(hCloneDC);
}

bool CRenderEngine::DrawImageInfo(HDC hDC, CPaintManagerUI *pManager, const RECT &rcItem, const RECT &rcPaint,
    const TDrawInfo *pDrawInfo, HINSTANCE instance, bool highQuality)
{
    if (pManager == nullptr || hDC == nullptr || pDrawInfo == nullptr)
        return false;

    RECT rcDest = rcItem;
    if (pDrawInfo->rcDest.left != 0 || pDrawInfo->rcDest.top != 0 || pDrawInfo->rcDest.right != 0 ||
        pDrawInfo->rcDest.bottom != 0) {
        if (UICulture::GetInstance()->getDirection()) {
            LONG DirectionLeft = (rcItem.right - rcItem.left) - (pDrawInfo->rcDest.right - pDrawInfo->rcDest.left) -
                pDrawInfo->rcDest.left;
            LONG DirectionRight = DirectionLeft + (pDrawInfo->rcDest.right - pDrawInfo->rcDest.left);
            rcDest.left = rcItem.left + DirectionLeft;
            rcDest.top = rcItem.top + pDrawInfo->rcDest.top;
            rcDest.right = rcItem.left + DirectionRight;
            if (rcDest.right > rcItem.right) {
                rcDest.right = rcItem.right;
            }
            rcDest.bottom = rcItem.top + pDrawInfo->rcDest.bottom;
            if (rcDest.bottom > rcItem.bottom) {
                rcDest.bottom = rcItem.bottom;
            }
        } else {
            rcDest.left = rcItem.left + pDrawInfo->rcDest.left;
            rcDest.top = rcItem.top + pDrawInfo->rcDest.top;
            rcDest.right = rcItem.left + pDrawInfo->rcDest.right;
            if (rcDest.right > rcItem.right) {
                rcDest.right = rcItem.right;
            }
            rcDest.bottom = rcItem.top + pDrawInfo->rcDest.bottom;
            if (rcDest.bottom > rcItem.bottom) {
                rcDest.bottom = rcItem.bottom;
            }
        }
    }
    return CRenderEngine::DrawImage(hDC, pManager, rcItem, rcPaint, pDrawInfo->sImageName, pDrawInfo->sResType, rcDest,
        pDrawInfo->rcSource, pDrawInfo->rcCorner, pDrawInfo->dwMask, pDrawInfo->uFade, pDrawInfo->bHole,
        pDrawInfo->bTiledX, pDrawInfo->bTiledY, instance, pDrawInfo->bReverse, highQuality);
}

bool CRenderEngine::DrawImageString(HDC hDC, CPaintManagerUI *pManager, const RECT &rcItem, const RECT &rcPaint,
    LPCTSTR pStrImage, LPCTSTR pStrModify, HINSTANCE instance, bool highQuality)
{
    if ((pManager == nullptr) || (hDC == nullptr)) {
        return false;
    }
    const TDrawInfo *pDrawInfo = pManager->GetDrawInfo(pStrImage, pStrModify);
    return DrawImageInfo(hDC, pManager, rcItem, rcPaint, pDrawInfo, instance, highQuality);
}

void CRenderEngine::DrawColor(HDC hDC, const RECT &rc, DWORD color, int rounded, bool roundEnhance)
{
    if (color <= 0x00FFFFFF) {
        return;
    }

    Gdiplus::Graphics graphics(hDC);
    Gdiplus::SolidBrush brush(
        Gdiplus::Color((LOBYTE((color) >> GET_HIGHT_BIT)), GetBValue(color), GetGValue(color), GetRValue(color)));
    if (rounded <= 0) {
        graphics.FillRectangle(&brush, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    } else {
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        RECT rcItem = rc;
        Gdiplus::Rect rcItemGDIP = Utils::FromWin32RECT(rcItem);
        if (rcItemGDIP.Width == 0 || rcItemGDIP.Height == 0) {
            return;
        }
        // 裁剪区域不能作画，导致边框有时不全，往里收缩一个像素
        constexpr INT offsetPx = 2;
        if (roundEnhance) {
            ++rcItemGDIP.X;
            ++rcItemGDIP.Y;
            rcItemGDIP.Width -= offsetPx;
            rcItemGDIP.Height -= offsetPx;
        }
        int width = rcItemGDIP.Width;
        int height = rcItemGDIP.Height;

        int arcWidth = (rounded > height / TWO_UNIT || rounded == -1) ? height : rounded * TWO_UNIT;
        Gdiplus::Point ptTopLeft;
        rcItemGDIP.GetLocation(&ptTopLeft);

        Gdiplus::Rect rcArcLeft(ptTopLeft.X, ptTopLeft.Y, arcWidth, arcWidth);
        Gdiplus::Rect rcArcRight(rcItemGDIP.GetRight() - arcWidth, ptTopLeft.Y, arcWidth, arcWidth);

        Gdiplus::GraphicsPath path;

        // top left arc
        path.AddArc(rcArcLeft, DEGREES_TWO, DEGREES_ONE);
        // top line
        if (arcWidth < width) {
            path.AddLine(rcItemGDIP.GetLeft() + arcWidth / TWO_UNIT, rcItemGDIP.GetTop(),
                rcItemGDIP.GetRight() - arcWidth / TWO_UNIT, rcItemGDIP.GetTop());
        }
        // top right arc
        path.AddArc(rcArcRight, -90, DEGREES_ONE);
        if (arcWidth < height) {
            // right line
            path.AddLine(rcItemGDIP.GetRight(), rcItemGDIP.GetTop() + arcWidth / TWO_UNIT,
                rcItemGDIP.GetRight(), rcItemGDIP.GetBottom() - arcWidth / TWO_UNIT);
        }
        // bottom right arc
        rcArcRight.Offset(0, rcItemGDIP.Height - arcWidth);
        path.AddArc(rcArcRight, 0, DEGREES_ONE);
        // bottom line
        path.AddLine(rcItemGDIP.GetRight() - arcWidth / TWO_UNIT, rcItemGDIP.GetBottom(),
            rcItemGDIP.GetLeft() + arcWidth / TWO_UNIT, rcItemGDIP.GetBottom());
        // bottom left
        rcArcLeft.Offset(0, rcItemGDIP.Height - arcWidth);
        path.AddArc(rcArcLeft, DEGREES_ONE, DEGREES_ONE);
        // left line
        if (arcWidth < height) {
            // left line
            path.AddLine(rcItemGDIP.GetLeft(), rcItemGDIP.GetTop() + arcWidth / TWO_UNIT,
                rcItemGDIP.GetLeft(), rcItemGDIP.GetBottom() - arcWidth / TWO_UNIT);
        }

        Gdiplus::SolidBrush brushBk(0);
        Gdiplus::Pen penBorder(Gdiplus::Color(0), 1);

        brushBk.SetColor(color);
        penBorder.SetColor(color);

        graphics.FillPath(&brushBk, &path);
    }
}

void CRenderEngine::DrawGradient(HDC hDC, const RECT &rc, DWORD dwFirst,
    DWORD dwSecond, bool bVertical, int nSteps)
{
    using LPALPHABLEND = BOOL(WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
    static LPALPHABLEND lpAlphaBlend =
        (LPALPHABLEND)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
    if (lpAlphaBlend == nullptr) {
        lpAlphaBlend = AlphaBitBlt;
    }
    using PGradientFill = BOOL(WINAPI *)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
    static PGradientFill lpGradientFill =
        (PGradientFill)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

    BYTE bAlpha = (BYTE)(((dwFirst >> GET_HIGHT_BIT) + (dwSecond >> GET_HIGHT_BIT)) >> 1);
    if (bAlpha == 0) {
        return;
    }
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    RECT rcPaint = rc;
    HDC hPaintDC = hDC;
    HBITMAP hPaintBitmap = nullptr;
    HBITMAP hOldPaintBitmap = nullptr;
    if (bAlpha < RGB_MAX_VALUE) {
        rcPaint.left = rcPaint.top = 0;
        rcPaint.right = cx;
        rcPaint.bottom = cy;
        hPaintDC = ::CreateCompatibleDC(hDC);
        hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
        ASSERT(hPaintDC);
        ASSERT(hPaintBitmap);
        hOldPaintBitmap = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);
    }
    if (lpGradientFill != nullptr) {
        TRIVERTEX triv[TWO_UNIT] = {{
                rcPaint.left,
                rcPaint.top,
                static_cast<COLOR16>(GetBValue(dwFirst) << ONE_BYTE),
                static_cast<COLOR16>(GetGValue(dwFirst) << ONE_BYTE),
                static_cast<COLOR16>(GetRValue(dwFirst) << ONE_BYTE),
                0xFF00
            }, {rcPaint.right,
                rcPaint.bottom,
                static_cast<COLOR16>(GetBValue(dwSecond) << ONE_BYTE),
                static_cast<COLOR16>(GetGValue(dwSecond) << ONE_BYTE),
                static_cast<COLOR16>(GetRValue(dwSecond) << ONE_BYTE),
                0xFF00
            }
        };
        GRADIENT_RECT grc = { 0, 1 };
        lpGradientFill(hPaintDC, triv, TWO_UNIT, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V :
            GRADIENT_FILL_RECT_H);
    } else {
        // Determine how many shades
        DWORD nShift = 1;
        if (nSteps >= ONE_BYTE * ONE_BYTE) {
            nShift = SIX_UNIT;
        } else if (nSteps >= ONE_BYTE * FOUR_UNIT) {
            nShift = FIVE_UNIT;
        } else if (nSteps >= ONE_BYTE * TWO_UNIT) {
            nShift = FOUR_UNIT;
        } else if (nSteps >= ONE_BYTE) {
            nShift = THREE_UNIT;
        } else if (nSteps >= FOUR_UNIT) {
            nShift = TWO_UNIT;
        }
        DWORD nLines = 1 << nShift;
        for (DWORD i = 0; i < nLines; i++) {
            // Do a little alpha blending
            BYTE bR = (BYTE)((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
            BYTE bG = (BYTE)((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
            BYTE bB = (BYTE)((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
            // ... then paint with the resulting color
            HBRUSH hBrush = ::CreateSolidBrush(RGB(bR, bG, bB));
            RECT r2 = rcPaint;
            if (bVertical) {
                r2.bottom = rc.bottom - ((i * (DWORD)(rc.bottom - rc.top)) >> nShift);
                r2.top = rc.bottom - (((i + 1) * (DWORD)(rc.bottom - rc.top)) >> nShift);
                if ((r2.bottom - r2.top) > 0) {
                    ::FillRect(hDC, &r2, hBrush);
                }
            } else {
                r2.left = rc.right - (((DWORD)(i + 1) * (DWORD)(rc.right - rc.left)) >> (DWORD)nShift);
                r2.right = rc.right - (((DWORD)i * ((DWORD)(rc.right - rc.left))) >> ((DWORD)nShift));
                if ((r2.right - r2.left) > 0) {
                    ::FillRect(hPaintDC, &r2, hBrush);
                }
            }
            ::DeleteObject(hBrush);
        }
    }
    if (bAlpha < RGB_MAX_VALUE) {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
        lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
        ::SelectObject(hPaintDC, hOldPaintBitmap);
        ::DeleteObject(hPaintBitmap);
        ::DeleteDC(hPaintDC);
    }
}

void CRenderEngine::DrawLine(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    LOGPEN lg;
    lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
    lg.lopnStyle = nStyle;
    lg.lopnWidth.x = nSize;
    HPEN hPen = CreatePenIndirect(&lg);
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    POINT ptTemp = { 0 };
    ::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
    ::LineTo(hDC, rc.right, rc.bottom);
    ::SelectObject(hDC, hOldPen);
    ::DeleteObject(hPen);
}

void CRenderEngine::DrawRect(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    if (nStyle == HSL_VALUE) {
        Gdiplus::Graphics graphics(hDC);
        Gdiplus::Pen pen(Gdiplus::Color((LOBYTE((dwPenColor) >> GET_HIGHT_BIT)),
            GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
        Gdiplus::Rect rect(rc.left, rc.top, (rc.right - rc.left - 1), (rc.bottom - rc.top - 1));

        graphics.DrawRectangle(&pen, rect);
        return;
    }

    HPEN hPen = ::CreatePen(nStyle, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
    ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
    ::SelectObject(hDC, hOldPen);
    ::DeleteObject(hPen);
}

void GetRoundedRectPath(Gdiplus::GraphicsPath &path, const RECT &rc, int arcWidth, int arcHeight)
{
    // ↖
    path.AddArc(rc.left, rc.top, arcWidth * TWO_UNIT, arcHeight * TWO_UNIT, DEGREES_TWO, DEGREES_ONE);
    // ↑
    if (rc.left + arcWidth < rc.right - arcWidth) {
        path.AddLine(rc.left + arcWidth, rc.top, rc.right - arcWidth, rc.top);
    }
    // ↗
    path.AddArc(rc.right - arcWidth * TWO_UNIT, rc.top, arcWidth * TWO_UNIT,
        arcHeight * TWO_UNIT, DEGREES_THREE, DEGREES_ONE);
    // →
    if (rc.top + arcHeight < rc.bottom - arcHeight) {
        path.AddLine(rc.right, rc.top + arcHeight, rc.right, rc.bottom - arcHeight);
    }
    // ↘
    path.AddArc(rc.right - arcWidth * TWO_UNIT, rc.bottom - arcHeight * TWO_UNIT,
        arcWidth * TWO_UNIT, arcHeight * TWO_UNIT, 0, DEGREES_ONE);
    // ↓
    if (rc.left + arcWidth < rc.right - arcWidth) {
        path.AddLine(rc.right - arcWidth, rc.bottom, rc.left + arcWidth, rc.bottom);
    }
    // ↙
    path.AddArc(rc.left, rc.bottom - arcHeight * TWO_UNIT, arcWidth * TWO_UNIT,
        arcHeight * TWO_UNIT, DEGREES_ONE, DEGREES_ONE);
    // ←
    if (rc.top + arcHeight < rc.bottom - arcHeight) {
        path.AddLine(rc.left, rc.bottom - arcHeight, rc.left, rc.top + arcHeight);
    }
}

void CRenderEngine::DrawRoundRect(HDC hDC, const RECT &rc, int nSize, int width, int height, DWORD dwPenColor,
    DWORD dwFillColor, int nStyle)
{
    if (width == 0 || height == 0) {
        DrawRect(hDC, rc, nSize, dwPenColor, nStyle);
        return;
    }

    int rcWidth = rc.right - rc.left;
    int rcHeight = rc.bottom - rc.top;

    int arcWidth = min(rcWidth / TWO_UNIT, width / TWO_UNIT);
    int arcHeight = min(rcHeight / TWO_UNIT, height / TWO_UNIT);

    if (arcWidth == 0 || arcHeight == 0) {
        DrawRect(hDC, rc, nSize, dwPenColor, nStyle);
        return;
    }

    Gdiplus::Graphics graphics(hDC);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    if (dwFillColor) {
        Gdiplus::SolidBrush brush(dwFillColor);
        RECT rc_ = rc;
        Gdiplus::GraphicsPath path;
        GetRoundedRectPath(path, rc_, arcWidth, arcHeight);
        graphics.FillPath(&brush, &path);
    }
    if (dwPenColor) {
        Gdiplus::Pen pen(dwPenColor);
        pen.SetWidth(static_cast<Gdiplus::REAL>(nSize));
        Gdiplus::GraphicsPath path;
        GetRoundedRectPath(path, rc, arcWidth, arcHeight);
        graphics.DrawPath(&pen, &path);
    }
}

void CRenderEngine::DrawText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText, DWORD dwTextColor,
    int iFont, UINT uStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    if (pstrText == nullptr || pManager == nullptr) {
        return;
    }

#ifdef SWITCH_LANGUAGE_TEST
    wstring language = UICulture::GetInstance()->GetLocaleLanguage();
    std::transform(language.begin(), language.end(), language.begin(), ::toupper);
    wstring oldStr = L"_";
    size_t pos = language.find(oldStr);
    if (pos != std::wstring::npos) {
        language.replace(pos, oldStr.size(), L"-");
    }
    wstring debug = L"duilib: ";
    debug += language;
    OutputDebugString(debug.c_str());
    CDuiString text(pstrText);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    if (valueList.size() == TWO_UNIT) { // 确认容器的大小
        pstrText = valueList[0]; // 获取容器的第一个元素
        text = valueList[1];
        vector<DuiLib::CDuiString> vecDuiStr = text.Split(L"$UI$");
        CDuiString translatorValue;
        for (int i = 0; i < vecDuiStr.size(); ++i) {
            DuiLib::CDuiString strVal = vecDuiStr[i];
            vector<DuiLib::CDuiString> endlist = strVal.Split(L"&UI&");
            if (endlist.size() == TWO_UNIT) {                             // 确认容器的大小
                if (_tcsicmp(language.c_str(), endlist[0]) == 0) { // 获取容器第一个元素
                    pstrText = endlist[1];                         // 获取容器的第二个元素
                    break;
                }
            }
        }
    }
#endif // SWITCH_LANGUAGE_TEST

    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        uStyle = uStyle | DT_RTLREADING;
    }

    HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
    Gdiplus::Graphics graphics(hDC);
    Gdiplus::Font font(hDC, pManager->GetFont(iFont));
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left),
        (Gdiplus::REAL)(rc.bottom - rc.top));
    BYTE af = 0xFF;
    if (pManager->IsLayered())
        af = (dwTextColor & 0xFF000000) >> GET_HIGHT_BIT;
    Gdiplus::SolidBrush brush(
        Gdiplus::Color(af, GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

    Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

    if ((uStyle & DT_END_ELLIPSIS) != 0) {
        stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    }

    if ((uStyle & DT_WORD_ELLIPSIS) != 0) {
        stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);
    }

    if ((uStyle & DT_PATH_ELLIPSIS) != 0) {
        stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);
    }

    unsigned int flags = 0;
    if ((uStyle & DT_NOCLIP) != 0) {
        flags |= Gdiplus::StringFormatFlagsNoClip;
    }
    if ((uStyle & DT_SINGLELINE) != 0) {
        flags |= Gdiplus::StringFormatFlagsNoWrap;
    }
    if ((uStyle & DT_RTLREADING) != 0) {
        flags |= Gdiplus::StringFormatFlagsDirectionRightToLeft;
    }

    int formatFlags = flags;
    stringFormat.SetFormatFlags(formatFlags);

    if ((uStyle & DT_LEFT) != 0) {
        stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    } else if ((uStyle & DT_CENTER) != 0) {
        stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    } else if ((uStyle & DT_RIGHT) != 0) {
        stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
    } else {
        stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    }
    stringFormat.GenericTypographic();
    if ((uStyle & DT_TOP) != 0) {
        stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
    } else if ((uStyle & DT_VCENTER) != 0) {
        stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    } else if ((uStyle & DT_BOTTOM) != 0) {
        stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
    } else {
        stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
    }
#ifdef UNICODE
    if ((uStyle & DT_CALCRECT) != 0) {
        Gdiplus::RectF bounds;

        graphics.MeasureString(pstrText, -1, &font, rectF, &stringFormat, &bounds);

        // MeasureString存在计算误差，这里加一像素
        rc.bottom = rc.top + (long)bounds.Height + 1;
        rc.right = rc.left + (long)bounds.Width + 1;
    } else {
        graphics.DrawString(pstrText, -1, &font, rectF, &stringFormat, &brush);
    }
#else
    DWORD dwSize = MultiByteToWideChar(CP_ACP, 0, pstrText, -1, nullptr, 0);
    if (dwSize + 1 <= 0) {
        return;
    }
    WCHAR *pcwszDest = new WCHAR[dwSize + 1];
    SecureZeroMemory(pcwszDest, (dwSize + 1) * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, nullptr, pstrText, -1, pcwszDest, dwSize);
    if (pcwszDest != nullptr) {
        if ((uStyle & DT_CALCRECT) != 0) {
            Gdiplus::RectF bounds;
            graphics.MeasureString(pcwszDest, -1, &font, rectF, &stringFormat, &bounds);
            rc.bottom = rc.top + (long)(bounds.Height * 1.06);
            rc.right = rc.left + (long)(bounds.Width * 1.06);
        } else {
            graphics.DrawString(pcwszDest, -1, &font, rectF, &stringFormat, &brush);
        }
        delete[] pcwszDest;
    }
#endif
    ::SelectObject(hDC, hOldFont);
}

bool CompleteWordSplitChar(wchar_t p)
{
    // 分割单词常见符号
    if (p == ' ' || p == '\t' || p == '\r' || p == '\n' || p == '-') {
        return true;
    }
    // 日文、笔画、汉字
    if (p >= 0x3040 && p <= 0x9FFF) {
        return true;
    }

    return false;
}

void CRenderEngine::DrawHtmlText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText, DWORD dwTextColor,
    int iFont, RECT *prcLinks, CDuiString *sLinks, int &nLinkRects, UINT uStyle, int *piLinkIndex)
{
#ifdef SWITCH_LANGUAGE_TEST
    wstring language = UICulture::GetInstance()->GetLocaleLanguage();
    std::transform(language.begin(), language.end(), language.begin(), ::toupper);
    wstring oldStr = L"_";
    size_t pos = language.find(oldStr);
    if (pos != std::wstring::npos) {
        language.replace(pos, oldStr.size(), L"-");
    }
    wstring debug = L"duilib: ";
    debug += language;
    OutputDebugString(debug.c_str());
    CDuiString text(pstrText);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    if (valueList.size() == TWO_UNIT) { // 确认容器的大小
        pstrText = valueList[0]; // 获取容器的第一个元素
        text = valueList[1];
        vector<DuiLib::CDuiString> vecDuiStr = text.Split(L"$UI$");
        CDuiString translatorValue;
        for (int i = 0; i < vecDuiStr.size(); ++i) {
            DuiLib::CDuiString strVal = vecDuiStr[i];
            vector<DuiLib::CDuiString> endlist = strVal.Split(L"&UI&");
            if (endlist.size() == TWO_UNIT) {                             // 确认容器的大小
                if (_tcsicmp(language.c_str(), endlist[0]) == 0) { // 获取容器第一个元素
                    pstrText = endlist[1];                         // 获取容器的第二个元素
                    break;
                }
            }
        }
    }
#endif // SWITCH_LANGUAGE_TEST


    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    if (pstrText == nullptr || pManager == nullptr) {
        return;
    }
    if (::IsRectEmpty(&rc)) {
        return;
    }

    bool bDraw = (uStyle & DT_CALCRECT) == 0;
    bool rtl = false;
    bool bosiLnk = false;
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        rtl = true;
        if (UICulture::GetInstance()->GetLocaleLanguage() != L"ar_sa") { // 解决波斯语等管家不支持语言bug
            rtl = false;
            bosiLnk = true;
        }
    }

    CStdPtrArray aFontArray(ARRAY_LENGTH);
    CStdPtrArray aColorArray(ARRAY_LENGTH);
    CStdPtrArray aPIndentArray(ARRAY_LENGTH);

    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    HRGN hRgn = ::CreateRectRgnIndirect(&rc);
    if (bDraw) {
        ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);
    }

    auto fontinfo = pManager->GetFontInfo(iFont);
    TEXTMETRIC *pTm = &(fontinfo->tm);
    HFONT hOldFont = (HFONT)::SelectObject(hDC, fontinfo->hFont);
    ::SetBkMode(hDC, TRANSPARENT);
    ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
    DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
    ::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

    // If the drawstyle include a alignment, we'll need to first determine the text-size so
    // we can draw it at the correct position...
    if (((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0 || (uStyle & DT_VCENTER) != 0 ||
        (uStyle & DT_BOTTOM) != 0) && (uStyle & DT_CALCRECT) == 0) {
        RECT rcText = { 0, 0, 9999, 100 };
        int nLinks = 0;
        DrawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, iFont, nullptr, nullptr,
            nLinks, uStyle | DT_CALCRECT);
        if ((uStyle & DT_SINGLELINE) != 0) {
            if ((uStyle & DT_CENTER) != 0) {
                rc.left =
                    rc.left + ((rc.right - rc.left) / TWO_UNIT) - ((rcText.right - rcText.left) / TWO_UNIT);
                rc.right = rc.left + (rcText.right - rcText.left);
            }
            if ((uStyle & DT_RIGHT) != 0) {
                rc.left = rc.right - (rcText.right - rcText.left);
            }
        }
        if ((uStyle & DT_VCENTER) != 0) {
            rc.top = rc.top + ((rc.bottom - rc.top) / TWO_UNIT) - ((rcText.bottom - rcText.top) / TWO_UNIT);
            rc.bottom = rc.top + (rcText.bottom - rcText.top);
        }
        if ((uStyle & DT_BOTTOM) != 0) {
            rc.top = rc.bottom - (rcText.bottom - rcText.top);
        }
    }

    bool bHoverLink = false;
    int hoverLinkIndex = -1; // 默认没有hover的link
    CDuiString sHoverLink;
    POINT ptMouse = pManager->GetMousePos();
    for (int i = 0; !bHoverLink && i < nLinkRects; i++) {
        if (::PtInRect(prcLinks + i, ptMouse)) {
            sHoverLink = *(reinterpret_cast<CDuiString *>(sLinks + i));
            bHoverLink = true;
            hoverLinkIndex = i;
        }
    }

    POINT pt = { rc.left, rc.top };
    int iLinkIndex = 0;
    int cyLine = static_cast<int>(pTm->tmHeight + pTm->tmExternalLeading +
        reinterpret_cast<uintptr_t>(aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
    int cyMinHeight = 0;
    int cxMaxWidth = 0;
    POINT ptLinkStart = { 0 };
    bool bLineEnd = false;
    bool bInRaw = false;
    bool bInLink = false;
    bool bInSelected = false;
    int iLineLinkIndex = 0;
    bool bClear = false;

    // 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
    CStdPtrArray aLineFontArray;
    CStdPtrArray aLineColorArray;
    CStdPtrArray aLinePIndentArray;
    std::vector<std::wstring> aOrgTextArray;
    std::vector<int> aOrgTextLengthArray;
    LPCTSTR pstrLineBegin = pstrText;
    bool bLineInRaw = false;
    bool bLineInLink = false;
    bool bLineInSelected = false;
    int cyLineHeight = 0;
    bool bLineDraw = false; // 行的第二阶段：绘制
    while (*pstrText != _T('\0')) {
        if (pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd) {
            if (*pstrText == _T('\n')) {
                pstrText++;
            }
            if (bLineEnd) {
                bLineEnd = false;
            }
            if (!bLineDraw) {
                if (bInLink && iLinkIndex < nLinkRects) {
                    if (rtl) {
                        RECT rcRTL;
                        rcRTL.right = rc.right - (ptLinkStart.x - rc.left);
                        rcRTL.left = rcRTL.right - (MIN(pt.x, rc.right) - ptLinkStart.x);
                        ::SetRect(&prcLinks[iLinkIndex++], rcRTL.left, ptLinkStart.y, rcRTL.right, pt.y + cyLine);
                    } else {
                        ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right),
                            pt.y + cyLine);
                    }
                    CDuiString *pStr1 = reinterpret_cast<CDuiString *>(sLinks + iLinkIndex - 1);
                    CDuiString *pStr2 = reinterpret_cast<CDuiString *>(sLinks + iLinkIndex);
                    *pStr2 = *pStr1;
                }
                for (int i = iLineLinkIndex; i < iLinkIndex; i++) {
                    prcLinks[i].bottom = pt.y + cyLine;
                }
                if (bDraw) {
                    bInLink = bLineInLink;
                    iLinkIndex = iLineLinkIndex;
                }
            } else {
                if (bInLink && iLinkIndex < nLinkRects) {
                    iLinkIndex++;
                }
                bLineInLink = bInLink;
                iLineLinkIndex = iLinkIndex;
            }
            if ((uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw)) {
                break;
            }
            if (bDraw)
                bLineDraw = !bLineDraw; // !
            pt.x = rc.left;
            if (!bLineDraw) {
                pt.y += cyLine;
            }
            if (pt.y > rc.bottom && bDraw) {
                break;
            }
            ptLinkStart = pt;
            cyLine = static_cast<int>(pTm->tmHeight + pTm->tmExternalLeading +
                reinterpret_cast<uintptr_t>(aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
            if (pt.x >= rc.right) {
                break;
            }
        } else if (!bInRaw && (*pstrText == _T('<') || *pstrText == _T('{')) &&
            (pstrText[1] >= _T('a') && pstrText[1] <= _T('z')) &&
            (pstrText[TWO_UNIT] == _T(' ') || pstrText[TWO_UNIT] == _T('>') || pstrText[TWO_UNIT] == _T('}'))) {
            pstrText++;
            LPCTSTR pstrNextStart = nullptr;
            switch (*pstrText) {
                case _T('a'): { // Link
                    pstrText++;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    if (iLinkIndex < nLinkRects && !bLineDraw) {
                        CDuiString *pStr = reinterpret_cast<CDuiString *>(sLinks + iLinkIndex);
                        pStr->Empty();
                        while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}')) {
                            LPCTSTR pstrTemp = ::CharNext(pstrText);
                            while (pstrText < pstrTemp) {
                                *pStr += *pstrText++;
                            }
                        }
                    }

                    DWORD clrColor = dwTextColor;
                    if (clrColor == 0) {
                        pManager->GetDefaultLinkFontColor();
                    }
                    if (bHoverLink && iLinkIndex < nLinkRects) {
                        CDuiString *pStr = reinterpret_cast<CDuiString *>(sLinks + iLinkIndex);
                        if (sHoverLink == *pStr) {
                            clrColor = pManager->GetDefaultLinkHoverFontColor();
                        }
                    }
                    aColorArray.Add((LPVOID)((uintptr_t)(clrColor)));
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    TFontInfo *pFontInfo = pManager->GetDefaultFontInfo();
                    if (aFontArray.GetSize() > 0) {
                        pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                    }
                    if (pFontInfo != nullptr) {
                        if (pFontInfo->bUnderline == false) {
                            bool underlined = (iLinkIndex == hoverLinkIndex); // 当前渲染的link被选中，增加下划线属性
                            HFONT hFont = pManager->GetFont(pFontInfo->sFontName,
                                pFontInfo->iSize, pFontInfo->bBold, underlined, pFontInfo->bItalic);
                            if (hFont == nullptr) {
                                hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                    pFontInfo->bBold, underlined, pFontInfo->bItalic);
                            }
                            pFontInfo = pManager->GetFontInfo(hFont);
                            aFontArray.Add(pFontInfo);
                            pTm = &pFontInfo->tm;
                            if (pFontInfo->hFont != nullptr) {
                                ::SelectObject(hDC, pFontInfo->hFont);
                            }
                            cyLine = static_cast<int>(MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                reinterpret_cast<uintptr_t>(aPIndentArray.GetAt(aPIndentArray.GetSize() - 1))));
                        }
                        ptLinkStart = pt;
                        bInLink = true;
                    }
                } break;
                case _T('b'): { // Bold
                    pstrText++;
                    TFontInfo *pFontInfo = pManager->GetDefaultFontInfo();
                    if (aFontArray.GetSize() > 0) {
                        pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                    }
                    if (pFontInfo != nullptr && pFontInfo->bBold == false) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, true,
                            pFontInfo->bUnderline, pFontInfo->bItalic);
                        if (hFont == nullptr) {
                            hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize, true,
                                pFontInfo->bUnderline, pFontInfo->bItalic);
                        }
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        if (pFontInfo->hFont != nullptr) {
                            ::SelectObject(hDC, pFontInfo->hFont);
                        }
                        cyLine = static_cast<int>(MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                            (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                    }
                } break;
                case _T('c'): { // Color
                    pstrText++;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    if (*pstrText == _T('#')) {
                        pstrText++;
                    }
                    DWORD clrColor = static_cast<DWORD>(_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText),
                        RADIX_TYPE_TWO_VALUE));
                    aColorArray.Add((LPVOID)((uintptr_t)clrColor));
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                } break;
                case _T('f'): { // Font
                    pstrText++;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    LPCTSTR pstrTemp = pstrText;
                    int iFont = static_cast<int>(_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText),
                        RADIX_TYPE_ONE_VALUE));
                    if (pstrTemp != pstrText) {
                        TFontInfo *pFontInfo = pManager->GetFontInfo(iFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        if (pFontInfo->hFont != nullptr) {
                            ::SelectObject(hDC, pFontInfo->hFont);
                        }
                    } else {
                        CDuiString sFontName;
                        int iFontSize = 10; // 字体大小size为10
                        CDuiString sFontAttr;
                        bool bBold = false;
                        bool bUnderline = false;
                        bool bItalic = false;
                        while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') &&
                            *pstrText != _T(' ')) {
                            pstrTemp = ::CharNext(pstrText);
                            while (pstrText < pstrTemp) {
                                sFontName += *pstrText++;
                            }
                        }
                        while (*pstrText > _T('\0') && *pstrText <= _T(' '))
                            pstrText = ::CharNext(pstrText);
                        if (isdigit(*pstrText)) {
                            iFontSize = static_cast<int>(_tcstol(pstrText,
                                const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                        }
                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                            pstrText = ::CharNext(pstrText);
                        }
                        while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}')) {
                            pstrTemp = ::CharNext(pstrText);
                            while (pstrText < pstrTemp) {
                                sFontAttr += *pstrText++;
                            }
                        }
                        sFontAttr.MakeLower();
                        if (sFontAttr.Find(_T("bold")) >= 0) {
                            bBold = true;
                        }
                        if (sFontAttr.Find(_T("underline")) >= 0) {
                            bUnderline = true;
                        }
                        if (sFontAttr.Find(_T("italic")) >= 0) {
                            bItalic = true;
                        }
                        HFONT hFont = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);
                        if (hFont == nullptr) {
                            hFont = pManager->AddFont(g_iFontID, sFontName, iFontSize, bBold, bUnderline, bItalic);
                        }
                        TFontInfo *pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        if (pFontInfo->hFont != nullptr) {
                            ::SelectObject(hDC, pFontInfo->hFont);
                        }
                    }
                    cyLine = static_cast<int>(MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                        (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                } break;
                case _T('i'): { // Italic or Image
                    pstrNextStart = pstrText - 1;
                    pstrText++;
                    CDuiString sImageString = pstrText;
                    int iWidth = 0;
                    int iHeight = 0;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    const TImageInfo *pImageInfo = nullptr;
                    CDuiString sName;
                    while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') &&
                        *pstrText != _T(' ')) {
                        LPCTSTR pstrTemp = ::CharNext(pstrText);
                        while (pstrText < pstrTemp) {
                            sName += *pstrText++;
                        }
                    }
                    if (sName.IsEmpty()) { // Italic
                        pstrNextStart = nullptr;
                        TFontInfo *pFontInfo = pManager->GetDefaultFontInfo();
                        if (aFontArray.GetSize() > 0) {
                            pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                        }
                        if (pFontInfo != nullptr && pFontInfo->bItalic == false) {
                            HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold,
                                pFontInfo->bUnderline, true);
                            if (hFont == nullptr) {
                                hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                    pFontInfo->bBold, pFontInfo->bUnderline, true);
                            }
                            pFontInfo = pManager->GetFontInfo(hFont);
                            aFontArray.Add(pFontInfo);
                            pTm = &pFontInfo->tm;
                            if (pFontInfo->hFont != nullptr) {
                                ::SelectObject(hDC, pFontInfo->hFont);
                            }
                            cyLine = MAX(cyLine, static_cast<int>(pTm->tmHeight + pTm->tmExternalLeading +
                                (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                        }
                    } else {
                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                            pstrText = ::CharNext(pstrText);
                        }
                        int iImageListNum = static_cast<int>(_tcstol(pstrText,
                            const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                        if (iImageListNum <= 0) {
                            iImageListNum = 1;
                        }
                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                            pstrText = ::CharNext(pstrText);
                        }
                        int iImageListIndex = static_cast<int>(_tcstol(pstrText,
                            const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                        if (iImageListIndex < 0 || iImageListIndex >= iImageListNum) {
                            iImageListIndex = 0;
                        }

                        if (_tcsstr(sImageString.GetData(), _T("file=\'")) != nullptr ||
                            _tcsstr(sImageString.GetData(), _T("res=\'")) != nullptr) {
                            CDuiString sImageResType;
                            CDuiString sImageName;
                            LPCTSTR pStrImage = sImageString.GetData();
                            CDuiString sItem;
                            CDuiString sValue;
                            while (*pStrImage != _T('\0')) {
                                sItem.Empty();
                                sValue.Empty();
                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                                    pStrImage = ::CharNext(pStrImage);
                                }
                                while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
                                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                                    while (pStrImage < pstrTemp) {
                                        sItem += *pStrImage++;
                                    }
                                }
                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                                    pStrImage = ::CharNext(pStrImage);
                                }
                                if (*pStrImage++ != _T('=')) {
                                    break;
                                }
                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                                    pStrImage = ::CharNext(pStrImage);
                                }
                                if (*pStrImage++ != _T('\'')) {
                                    break;
                                }
                                while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
                                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                                    while (pStrImage < pstrTemp) {
                                        sValue += *pStrImage++;
                                    }
                                }
                                if (*pStrImage++ != _T('\'')) {
                                    break;
                                }
                                if (!sValue.IsEmpty()) {
                                    if (sItem == _T("file") || sItem == _T("res")) {
                                        sImageName = sValue;
                                    } else if (sItem == _T("restype")) {
                                        sImageResType = sValue;
                                    }
                                }
                                if (*pStrImage++ != _T(' ')) {
                                    break;
                                }
                            }

                            pImageInfo = pManager->GetImageEx((LPCTSTR)sImageName, sImageResType);
                        } else {
                            pImageInfo = pManager->GetImageEx((LPCTSTR)sName);
                        }

                        if (pImageInfo) {
                            iWidth = pImageInfo->nX;
                            iHeight = pImageInfo->nY;
                            if (iImageListNum > 1) {
                                iWidth /= iImageListNum;
                            }

                            if (pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0) {
                                bLineEnd = true;
                            } else {
                                pstrNextStart = nullptr;
                                if (bDraw && bLineDraw) {
                                    CDuiRect rcImage(pt.x, pt.y + cyLineHeight - iHeight, pt.x + iWidth,
                                        pt.y + cyLineHeight);
                                    if (iHeight < cyLineHeight) {
                                        rcImage.bottom -= (cyLineHeight - iHeight) / TWO_UNIT;
                                        rcImage.top = rcImage.bottom - iHeight;
                                    }
                                    CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
                                    rcBmpPart.left = iWidth * iImageListIndex;
                                    rcBmpPart.right = iWidth * (iImageListIndex + 1);
                                    CDuiRect rcCorner(0, 0, 0, 0);
                                    if (pImageInfo->hBitmap != nullptr) {
                                        DrawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner,
                                            pImageInfo->bAlpha, RGB_MAX_VALUE);
                                    }
                                }

                                cyLine = MAX(iHeight, cyLine);
                                pt.x += iWidth;
                                cyMinHeight = pt.y + iHeight;
                                cxMaxWidth = MAX(cxMaxWidth, pt.x);
                            }
                        } else {
                            pstrNextStart = nullptr;
                        }
                    }
                } break;
                case _T('n'): { // Newline
                    pstrText++;
                    if ((uStyle & DT_SINGLELINE) != 0) {
                        break;
                    }
                    bLineEnd = true;
                    bClear = true;
                } break;
                case _T('p'): { // Paragraph
                    pstrText++;
                    if (pt.x > rc.left) {
                        bLineEnd = true;
                    }
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    uintptr_t cyLineExtra = static_cast<uintptr_t>(_tcstol(pstrText,
                        const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                    aPIndentArray.Add((LPVOID)cyLineExtra);
                    cyLine = static_cast<int>(MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra));
                } break;
                case _T('r'): { // Raw Text
                    pstrText++;
                    bInRaw = true;
                } break;
                case _T('s'): { // Selected text background color
                    pstrText++;
                    bInSelected = !bInSelected;
                    if (bDraw && bLineDraw) {
                        if (bInSelected) {
                            ::SetBkMode(hDC, OPAQUE);
                        } else {
                            ::SetBkMode(hDC, TRANSPARENT);
                        }
                    }
                } break;
                case _T('u'): { // Underline text
                    pstrText++;
                    TFontInfo *pFontInfo = pManager->GetDefaultFontInfo();
                    if (aFontArray.GetSize() > 0) {
                        pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                    }
                    if (pFontInfo != nullptr && pFontInfo->bUnderline == false) {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize, pFontInfo->bBold, true,
                            pFontInfo->bItalic);
                        if (hFont == nullptr) {
                            hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                pFontInfo->bBold, true, pFontInfo->bItalic);
                        }
                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        if (pFontInfo->hFont != nullptr) {
                            ::SelectObject(hDC, pFontInfo->hFont);
                        }
                        cyLine = static_cast<int>(MAX(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                            (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                    }
                } break;
                case _T('x'): { // X Indent
                    pstrText++;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    int iWidth = static_cast<int>(_tcstol(pstrText,
                        const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                    pt.x += iWidth;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                } break;
                case _T('y'): { // Y Indent
                    pstrText++;
                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    cyLine = static_cast<int>(_tcstol(pstrText,
                        const_cast<LPTSTR *>(&pstrText), RADIX_TYPE_ONE_VALUE));
                } break;
            }
            if (pstrNextStart != nullptr) {
                pstrText = pstrNextStart;
            } else {
                while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}')) {
                    pstrText = ::CharNext(pstrText);
                }
                pstrText = ::CharNext(pstrText);
            }
        } else if (!bInRaw && (*pstrText == _T('<') || *pstrText == _T('{')) && pstrText[1] == _T('/')) {
            pstrText++;
            pstrText++;
            switch (*pstrText) {
                case _T('c'): {
                    pstrText++;
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if (aColorArray.GetSize() > 0) {
                        clrColor = static_cast<DWORD>(reinterpret_cast<uintptr_t>(
                            aColorArray.GetAt(aColorArray.GetSize() - 1)));
                    }
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                } break;
                case _T('p'):
                    pstrText++;
                    if (pt.x > rc.left) {
                        bLineEnd = true;
                    }
                    aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
                    cyLine = MAX(cyLine, static_cast<int>(pTm->tmHeight + pTm->tmExternalLeading +
                        (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                    break;
                case _T('s'): {
                    pstrText++;
                    bInSelected = !bInSelected;
                    if (bDraw && bLineDraw) {
                        if (bInSelected) {
                            ::SetBkMode(hDC, OPAQUE);
                        } else {
                            ::SetBkMode(hDC, TRANSPARENT);
                        }
                    }
                } break;
                case _T('a'): {
                    if (iLinkIndex < nLinkRects) {
                        if (!bLineDraw) {
                            if (rtl) {
                                RECT rcRTL;
                                rcRTL.right = rc.right - (ptLinkStart.x - rc.left);
                                rcRTL.left = rcRTL.right - (MIN(pt.x, rc.right) - ptLinkStart.x);
                                ::SetRect(&prcLinks[iLinkIndex], rcRTL.left, ptLinkStart.y, rcRTL.right,
                                    pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                                if (piLinkIndex) {
                                    piLinkIndex[iLinkIndex] = 1;
                                }
                            } else {
                                ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y, MIN(pt.x, rc.right),
                                    pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                                if (piLinkIndex) {
                                    piLinkIndex[iLinkIndex] = 1;
                                }
                            }
                        } else {
                            if (bosiLnk) {
                                LONG totalLength = GetVecTotalLength(aOrgTextLengthArray);
                                LONG xt = (rc.right - rc.left) - totalLength;
                                RECT rcRTL;
                                rcRTL.left = ptLinkStart.x + xt;
                                rcRTL.right = rcRTL.left + (pt.x - ptLinkStart.x);
                                ::SetRect(&prcLinks[iLinkIndex], rcRTL.left, ptLinkStart.y, rcRTL.right,
                                    pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                            }
                        }
                        iLinkIndex++;
                    }
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;
                    if (aColorArray.GetSize() > 0) {
                        clrColor = static_cast<DWORD>(
                            reinterpret_cast<uintptr_t>(aColorArray.GetAt(aColorArray.GetSize() - 1)));
                    }
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    bInLink = false;
                }
                case _T('b'):
                case _T('f'):
                case _T('i'):
                case _T('u'): {
                    pstrText++;
                    if (aFontArray.GetSize() - 1 != 0) {
                        aFontArray.Remove(aFontArray.GetSize() - 1);
                    }
                    TFontInfo *pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                    if (pFontInfo == nullptr) {
                        pFontInfo = pManager->GetDefaultFontInfo();
                    }
                    if (pTm->tmItalic && pFontInfo->bItalic == false) {
                        ABC abc;
                        ::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
                        // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
                        pt.x += abc.abcC / TWO_UNIT;
                    }
                    pTm = &pFontInfo->tm;
                    if (pFontInfo->hFont != nullptr) {
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    cyLine = MAX(cyLine, static_cast<int>(pTm->tmHeight + pTm->tmExternalLeading +
                        (uintptr_t)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1)));
                } break;
            }
            while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
                pstrText = ::CharNext(pstrText);
            pstrText = ::CharNext(pstrText);
        } else if (!bInRaw && *pstrText == _T('<') && pstrText[TWO_UNIT] == _T('>') &&
            (pstrText[1] == _T('{') || pstrText[1] == _T('}'))) {
            SIZE szSpace = { 0 };
            GetTextExtentPoint32Ex(hDC, pManager, &pstrText[1], 1, &szSpace);
            if (bDraw && bLineDraw) {
                ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            }
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;
            pstrText++;
            pstrText++;
        } else if (!bInRaw && *pstrText == _T('{') && pstrText[TWO_UNIT] == _T('}') &&
            (pstrText[1] == _T('<') || pstrText[1] == _T('>'))) {
            SIZE szSpace = { 0 };
            GetTextExtentPoint32Ex(hDC, pManager, &pstrText[1], 1, &szSpace);
            if (bDraw && bLineDraw) {
                ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, &pstrText[1], 1);
            }
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;
            pstrText++;
            pstrText++;
        } else if (!bInRaw && *pstrText == _T(' ')) {
            SIZE szSpace = { 0 };
            GetTextExtentPoint32Ex(hDC, pManager, _T(" "), 1, &szSpace);
            // Still need to paint the space because the font might have
            // underline formatting.
            if (bDraw && bLineDraw) {
                ::TextOut(hDC, pt.x, pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading, _T(" "), 1);
            }
            if (!(bDraw && bLineDraw)) {
                std::wstring orgStr = pstrText;
                std::wstring cutStr = orgStr.substr(0, 1);
                aOrgTextArray.push_back(cutStr);
                aOrgTextLengthArray.push_back(szSpace.cx);
            }
            pt.x += szSpace.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            pstrText++;
        } else {
            POINT ptPos = pt;
            int cchChars = 0;
            int cchSize = 0;
            int cchLastGoodWord = 0;
            int cchLastGoodSize = 0;
            LPCTSTR p = pstrText;
            LPCTSTR pstrNext = nullptr;
            SIZE szText = { 0 };
            if (!bInRaw && *p == _T('<') || *p == _T('{')) {
                p++, cchChars++, cchSize++;
            }
            while (*p != _T('\0') && *p != _T('\n')) {
                // This part makes sure that we're word-wrapping if needed or providing support
                // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
                // slow when repeated so often.
                if (bInRaw) {
                    if ((*p == _T('<') || *p == _T('{')) && p[1] == _T('/') && p[TWO_UNIT] == _T('r') &&
                        (p[THREE_UNIT] == _T('>') || p[THREE_UNIT] == _T('}'))) {
                        p += FOUR_UNIT;
                        bInRaw = false;
                        break;
                    }
                } else {
                    if (*p == _T('<') || *p == _T('{')) {
                        break;
                    }
                }
                pstrNext = ::CharNext(p);
                cchChars++;
                cchSize += static_cast<int>(pstrNext - p);
                szText.cx = cchChars * pTm->tmMaxCharWidth;
                if (pt.x + szText.cx >= rc.right) {
                    GetTextExtentPoint32Ex(hDC, pManager, pstrText, cchSize, &szText);
                }
                if (pt.x + szText.cx > rc.right) {
                    if (pt.x + szText.cx > rc.right && pt.x != rc.left) {
                        cchChars--;
                        cchSize -= static_cast<int>(pstrNext - p);
                    }
                    if ((uStyle & DT_WORDBREAK) != 0 && (cchLastGoodWord > 0 || pt.x > rc.left)) {
                        cchChars = cchLastGoodWord;
                        cchSize = cchLastGoodSize;
                    }
                    if ((uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0) {
                        cchChars -= 1;
                        LPCTSTR pstrPrev = ::CharPrev(pstrText, p);
                        if (cchChars > 0) {
                            cchChars -= 1;
                            pstrPrev = ::CharPrev(pstrText, pstrPrev);
                            cchSize -= static_cast<int>(p - pstrPrev);
                        } else {
                            cchSize -= static_cast<int>(p - pstrPrev);
                        }
                        pt.x = rc.right;
                    }
                    bLineEnd = true;
                    cxMaxWidth = MAX(cxMaxWidth, pt.x);
                    break;
                }

                if (CompleteWordSplitChar(p[0])) {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }
                p = ::CharNext(p);
            }

            GetTextExtentPoint32Ex(hDC, pManager, pstrText, cchSize, &szText);
            if (bDraw && bLineDraw) {
                if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0) {
                    ptPos.x += (rc.right - rc.left - szText.cx) / TWO_UNIT;
                } else if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0) {
                    LONG totalLength = GetVecTotalLength(aOrgTextLengthArray);
                    LONG xt = (rc.right - rc.left) - totalLength;
                    ptPos.x += xt;
                }
                if (rtl) {
                    LONG totalLength = GetVecTotalLength(aOrgTextLengthArray);
                    LONG xt = (rc.right - rc.left) - totalLength;
                    int posRight = rc.right - (ptPos.x - rc.left);
                    int posLeft = posRight - szText.cx;
                    int posTop = ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading;
                    if (cyLineHeight == szText.cy) {
                        posTop = ptPos.y;
                    }
                    int posBottom = posTop + cyLine;
                    RECT rcRtl = { posLeft, posTop, posRight, posBottom };
                    wstring strW(pstrText, cchSize);
                    LayeredTextOut(hDC, pManager, rcRtl, strW.c_str(), uStyle | DT_RTLREADING, cchSize);

                    if (pt.x >= rcRtl.right && (uStyle & DT_END_ELLIPSIS) != 0) {
                        SIZE pointSize = { 0 };
                        int pointNumber = THREE_UNIT;
                        GetTextExtentPoint32Ex(hDC, pManager, _T("..."), pointNumber, &pointSize);
                        if (rcRtl.left >= pointSize.cx) {
                            RECT tempRec2 = { rcRtl.left - pointSize.cx, rcRtl.top, rcRtl.left,
                                              rcRtl.top + (rcRtl.bottom - rcRtl.top) };
                            LayeredTextOut(hDC, pManager, tempRec2, _T("..."), uStyle, pointNumber);
                        }
                    }
                } else {
                    if (bosiLnk) {
                        LONG totalLength = GetVecTotalLength(aOrgTextLengthArray);
                        LONG xt = (rc.right - rc.left) - totalLength;
                        ptPos.x += xt;
                    }
                    wstring layeredWstr(pstrText, cchSize);
                    int pLeft = ptPos.x;
                    int pTop = ptPos.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading;
                    if (cyLineHeight == szText.cy) {
                        pTop = ptPos.y;
                    }
                    int pRight = pLeft + szText.cx;
                    int pBottom = pTop + cyLine;
                    RECT tempRec = { pLeft, pTop, pRight, pBottom };

                    LayeredTextOut(hDC, pManager, tempRec, layeredWstr.c_str(), uStyle, cchSize);
                    if (pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0) {
                        RECT tempRec2 = { tempRec.right, tempRec.top, rc.right,
                                          tempRec.top + (tempRec.bottom - tempRec.top) };
                        LayeredTextOut(hDC, pManager, tempRec2, _T("..."), uStyle, THREE_UNIT);
                    }
                }

                if (bLineEnd) {
                    aOrgTextArray.clear();
                    aOrgTextLengthArray.clear();
                }
                if (bClear) {
                    aOrgTextArray.clear();
                    aOrgTextLengthArray.clear();
                    bClear = false;
                }
            }
            if (!(bDraw && bLineDraw)) {
                std::wstring orgStr = pstrText;
                std::wstring cutStr = orgStr.substr(0, cchSize);
                aOrgTextArray.push_back(cutStr);
                aOrgTextLengthArray.push_back(szText.cx);
            }

            pt.x += szText.cx;
            cxMaxWidth = MAX(cxMaxWidth, pt.x);
            cyMinHeight = MAX(cyMinHeight, szText.cy);
            cyLine = MAX(cyLine, szText.cy);
            pstrText += cchSize;
        }

        if (pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0')) {
            bLineEnd = true;
            if (bDraw && bLineDraw && bosiLnk) {
                aOrgTextArray.clear();
                aOrgTextLengthArray.clear();
            }
        }
        if (bDraw && bLineEnd) {
            if (!bLineDraw) {
                aFontArray.Resize(aLineFontArray.GetSize());
                ::memcpy_s(aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID),
                    aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID));
                aColorArray.Resize(aLineColorArray.GetSize());
                ::memcpy_s(aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID),
                    aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID));
                aPIndentArray.Resize(aLinePIndentArray.GetSize());
                ::memcpy_s(aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID),
                    aLinePIndentArray.GetData(), aLinePIndentArray.GetSize() * sizeof(LPVOID));

                cyLineHeight = cyLine;
                pstrText = pstrLineBegin;
                bInRaw = bLineInRaw;
                bInSelected = bLineInSelected;

                DWORD clrColor = dwTextColor;
                if (aColorArray.GetSize() > 0) {
                    clrColor = reinterpret_cast<DWORD>(aColorArray.GetAt(aColorArray.GetSize() - 1));
                }
                ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                TFontInfo *pFontInfo = reinterpret_cast<TFontInfo *>(aFontArray.GetAt(aFontArray.GetSize() - 1));
                if (pFontInfo == nullptr) {
                    pFontInfo = pManager->GetFontInfo(iFont);
                }
                pTm = &pFontInfo->tm;
                if (pFontInfo->hFont == nullptr) {
                    return;
                }
                if (pFontInfo->hFont != nullptr) {
                    ::SelectObject(hDC, pFontInfo->hFont);
                }
                if (bInSelected) {
                    ::SetBkMode(hDC, OPAQUE);
                }
            } else {
                aLineFontArray.Resize(aFontArray.GetSize());
                ::memcpy_s(aLineFontArray.GetData(), aLineFontArray.GetSize() * sizeof(LPVOID),
                    aFontArray.GetData(), aFontArray.GetSize() * sizeof(LPVOID));
                aLineColorArray.Resize(aColorArray.GetSize());
                ::memcpy_s(aLineColorArray.GetData(), aLineColorArray.GetSize() * sizeof(LPVOID),
                    aColorArray.GetData(), aColorArray.GetSize() * sizeof(LPVOID));
                ::memcpy_s(aLinePIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID),
                    aPIndentArray.GetData(), aPIndentArray.GetSize() * sizeof(LPVOID));
                pstrLineBegin = pstrText;
                bLineInSelected = bInSelected;
                bLineInRaw = bInRaw;
            }
        }

        ASSERT(iLinkIndex <= nLinkRects);
    }

    nLinkRects = iLinkIndex;

    // Return size of text when requested
    if ((uStyle & DT_CALCRECT) != 0) {
        rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
        rc.right = MIN(rc.right, cxMaxWidth);
    }

    if (bDraw) {
        ::SelectClipRgn(hDC, hOldRgn);
    }
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);

    ::SelectObject(hDC, hOldFont);
}

HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI *pManager, CControlUI *pControl, RECT rc)
{
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
    ASSERT(hPaintDC);
    ASSERT(hPaintBitmap);
    HBITMAP hOldPaintBitmap = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);
    pControl->DoPaint(hPaintDC, rc);

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = BIT_COUNT;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = static_cast<DWORD>(cx * cy * sizeof(DWORD));
    LPDWORD pDest = nullptr;
    HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS,
        reinterpret_cast<LPVOID *>(&pDest), nullptr, 0);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);
    if (hBitmap != nullptr) {
        HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
        ::GdiFlush();
    }

    // Cleanup
    ::SelectObject(hPaintDC, hOldPaintBitmap);
    ::DeleteObject(hPaintBitmap);
    ::DeleteDC(hPaintDC);

    return hBitmap;
}

SIZE CRenderEngine::GetTextSize(HDC hDC, CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont, UINT uStyle)
{
    SIZE size = { 0, 0 };
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    if (pstrText == nullptr || pManager == nullptr) {
        return size;
    }
    ::SetBkMode(hDC, TRANSPARENT);
    HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
    GetTextExtentPoint32Ex(hDC, pManager, pstrText, static_cast<int>(_tcslen(pstrText)), &size);
    ::SelectObject(hDC, hOldFont);
    return size;
}

void CRenderEngine::CheckAlphaColor(DWORD &dwColor)
{
    // RestoreAlphaColor认为0x00000000是真正的透明，其它都是GDI绘制导致的
    // 所以在GDI绘制中不能用0xFF000000这个颜色值，现在处理是让它变成RGB(0,0,1)
    // RGB(0,0,1)与RGB(0,0,0)很难分出来
    if ((0x00FFFFFF & dwColor) == 0) {
        dwColor += 1;
    }
}

HBITMAP CRenderEngine::CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE **pBits)
{
    LPBITMAPINFO lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
    if (lpbiSrc == nullptr) {
        return nullptr;
    }

    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = cx;
    lpbiSrc->bmiHeader.biHeight = cy;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = cx * cy;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    HBITMAP hBitmap = CreateDIBSection(hDC, lpbiSrc, DIB_RGB_COLORS,
        reinterpret_cast<void **>(pBits), nullptr, NULL);
    delete[] lpbiSrc;
    lpbiSrc = nullptr;
    return hBitmap;
}

void CRenderEngine::AdjustImage(bool bUseHSL, TImageInfo *imageInfo, short H, short S, short L)
{
    if (imageInfo == nullptr || imageInfo->bUseHSL == false || imageInfo->hBitmap == nullptr
        || imageInfo->pBits == nullptr || imageInfo->pSrcBits == nullptr) {
        return;
    }
    // 设置色相H:180 饱和度S:100 亮度L:100
    if (bUseHSL == false || (H == DEGREES_TWO && S == HSL_VALUE && L == HSL_VALUE)) {
        ::memcpy_s(imageInfo->pBits, imageInfo->nX * imageInfo->nY * FOUR_UNIT,
            imageInfo->pSrcBits, imageInfo->nX * imageInfo->nY * FOUR_UNIT);
        return;
    }

    float fH;
    float fS;
    float fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;
    for (int i = 0; i < imageInfo->nX * imageInfo->nY; i++) {
        RGBtoHSL(*(reinterpret_cast<DWORD *>(imageInfo->pSrcBits + i * FOUR_UNIT)), &fH, &fS, &fL);
        fH += (H - DEGREES_TWO);
        fH = fH > 0 ? fH : fH + DEGREES_FOUR; // 色相最大值为360
        fS *= S1;
        fL *= L1;
        HSLtoRGB(reinterpret_cast<DWORD *>(imageInfo->pBits + i * FOUR_UNIT), fH, fS, fL);
    }
}

SIZE CRenderEngine::GetVecStrLength(std::vector<std::wstring> &vec, HDC hDC, CPaintManagerUI *pManager)
{
    std::wstring str;
    for (size_t i = 0; i < vec.size(); ++i) {
        str += vec[i];
    }
    SIZE szText = { 0 };
    GetTextExtentPoint32Ex(hDC, pManager, str.c_str(), static_cast<int>(str.length()), &szText);

    return szText;
}

LONG CRenderEngine::GetVecTotalLength(std::vector<int> &vec)
{
    int total = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        total += vec[i];
    }

    return total;
}


void CRenderEngine::LayeredTextOut(HDC hDC, CPaintManagerUI *pManager, RECT &rc,
    LPCTSTR pstrText, UINT uStyle, int cchSize)
{
    Gdiplus::Graphics graphics(hDC);
    Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

    unsigned int flags = 0;
    flags |= Gdiplus::StringFormatFlagsNoWrap;

    if ((uStyle & DT_RTLREADING) != 0) {
        flags |= Gdiplus::StringFormatFlagsDirectionRightToLeft;
    }
    flags |= Gdiplus::StringFormatFlagsMeasureTrailingSpaces;
    int formatFlags = flags;

    stringFormat.SetFormatFlags(formatFlags);
    stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    stringFormat.GenericTypographic();
    stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);

    wstring texttemp(pstrText, cchSize);
    HFONT hf = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);
    Gdiplus::Font fnt(hDC, hf);
    COLORREF clr = GetTextColor(hDC);
    COLORREF curcolor = RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr));
    if ((curcolor & 0xFF000000) == 0) {
        curcolor = (curcolor | 0xFF000000);
    }
    Gdiplus::SolidBrush fntBrush(curcolor);
    // Gdiplus::Pen pen(0xffff0000);
    Gdiplus::Rect gdiRc = Utils::FromWin32RECT(rc);
    // graphics.DrawRectangle(&pen, Utils::FromWin32RECT(rc));
    Gdiplus::RectF gdiRcf(static_cast<Gdiplus::REAL>(gdiRc.X), static_cast<Gdiplus::REAL>(gdiRc.Y),
        static_cast<Gdiplus::REAL>(gdiRc.Width), static_cast<Gdiplus::REAL>(gdiRc.Height));


    // GDI+在使用Cleartype渲染大字号字体时存在缺陷，会有锯齿，这里设置字号大于20时强制使用图形反锯齿，小于等于20号使用默认设置。
    Gdiplus::TextRenderingHint trh = (Gdiplus::TextRenderingHint)pManager->GetGdiplusTextRenderingHint();
    graphics.SetTextRenderingHint(fnt.GetSize() > 20 ? Gdiplus::TextRenderingHintAntiAliasGridFit : trh);
    Gdiplus::RectF bounds;
    graphics.MeasureString(texttemp.c_str(), -1, &fnt, gdiRcf, &stringFormat, &bounds);

    // MeasureString存在计算误差，这里加一像素
    rc.bottom = rc.top + (long)bounds.Height + 1;
    rc.right = rc.left + (long)bounds.Width + 1;

    Gdiplus::RectF rcText(static_cast<Gdiplus::REAL>(rc.left), static_cast<Gdiplus::REAL>(rc.top),
        static_cast<Gdiplus::REAL>(rc.right - rc.left), static_cast<Gdiplus::REAL>(rc.bottom - rc.top));
    graphics.DrawString(texttemp.c_str(), cchSize, &fnt, rcText, &stringFormat, &fntBrush);
}


void CRenderEngine::GetTextExtentPoint32Ex(HDC hDC, CPaintManagerUI *pManager, LPCTSTR pstrText, int cchSize,
    SIZE *szText)
{
    if (!pManager) {
        return;
    }
    if (!szText) {
        return;
    }

    {
        Gdiplus::RectF bounds;

        Gdiplus::Graphics graphics(hDC);
        Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();

        unsigned int formatFlags = 0;
        formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
        formatFlags |= Gdiplus::StringFormatFlagsMeasureTrailingSpaces;
        if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
            formatFlags |= Gdiplus::StringFormatFlagsDirectionRightToLeft;
        }

        stringFormat.SetFormatFlags(formatFlags);

        stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);

        stringFormat.GenericTypographic();


        wstring texttemp(pstrText, cchSize);
        HFONT hf = (HFONT)::GetCurrentObject(hDC, OBJ_FONT);
        Gdiplus::Font font(hDC, hf);

        // GDI+在使用Cleartype渲染大字号字体时存在缺陷，会有锯齿，这里设置字号大于20时强制使用图形反锯齿
        // 小于等于20号使用默认设置。
        Gdiplus::TextRenderingHint trh = (Gdiplus::TextRenderingHint)pManager->GetGdiplusTextRenderingHint();
        graphics.SetTextRenderingHint(font.GetSize() > 20 ? Gdiplus::TextRenderingHintAntiAliasGridFit : trh);
        Gdiplus::RectF rectF(0, 0, MAX_RECTF_WIDTH, MAX_RECTF_HEIGTH);
        graphics.MeasureString(texttemp.c_str(), -1, &font, rectF, &stringFormat, &bounds);

        // MeasureString存在计算误差，这里加一像素
        szText->cy = (long)bounds.Height + 1;
        szText->cx = (long)bounds.Width + 1;
    }
}
} // namespace DuiLib
