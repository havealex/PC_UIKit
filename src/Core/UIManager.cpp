/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include <zmouse.h>
#include <shcore.h>
#include <atlcomcli.h>
#include <Shobjidl.h>
#include "UIMainQueue.h"
#include "../Debug/LogOut.h"
#include "../Core/win32blur.h"
#include "../Utils/VersionHelpers.h"
#include "UIManager.h"

#pragma comment(lib, "dwmapi.lib")

#define UISTR_UI_DEFAULT_FONTNAME _T("微软雅黑")
#define UISTR_UI_VALUE_FONTNAME _T("Calibri")
#define WHETHER_RETURN_DEFUALT(expression) {if (expression) {return;}}
#define CHECK_RESULT_WITH_LOG(expression) { if (expression) {LOGGER_ERROR("[DUILIB]: safe fun ret fail");}}

DECLARE_HANDLE(HZIP); // An HZIP identifies a zip file that has been opened
using ZRESULT = DWORD;

namespace DuiLib {
    constexpr int AREA_EXPAND_FOUR_TIME = 4;
    constexpr int CHAR_LENGTH_TYPE_ONE = 16; // 用于字符长度
    constexpr int CHAR_LENGTH_TYPE_TWO = 32; // 用于字符长度
    constexpr int DPI_TYPE_ONE = 96; // DPI set 96
    constexpr int SCALE_TAYO_ONE = 100; // scale set 100%
    constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10
    constexpr int RADIX_TYPE_TWO_VALUE = 16; // set radix type two value 16
    constexpr size_t STR_END_SIZE = 1;
    const DWORD MAX_MUL_VALUE_DWORD = static_cast<DWORD>(sqrt(ULONG_MAX));
    using LPGetDpiForMonitor = HRESULT(WINAPI *)(_In_ HMONITOR moniter, _In_ MONITOR_DPI_TYPE value,
        _Out_ UINT *dpix, _Out_ UINT *dpiy);

static void GetChildWndRect(HWND hWnd, HWND hChildWnd, RECT &rcChildWnd)
{
    ::GetWindowRect(hChildWnd, &rcChildWnd);

    POINT pt;
    pt.x = rcChildWnd.left;
    pt.y = rcChildWnd.top;
    ::ScreenToClient(hWnd, &pt);
    rcChildWnd.left = pt.x;
    rcChildWnd.top = pt.y;

    pt.x = rcChildWnd.right;
    pt.y = rcChildWnd.bottom;
    ::ScreenToClient(hWnd, &pt);
    rcChildWnd.right = pt.x;
    rcChildWnd.bottom = pt.y;
}

static UINT MapKeyState()
{
    UINT uState = 0;
    if (::GetKeyState(VK_CONTROL) < 0) {
        uState |= MK_CONTROL;
    }
    if (::GetKeyState(VK_RBUTTON) < 0) {
        uState |= MK_LBUTTON;
    }
    if (::GetKeyState(VK_LBUTTON) < 0) {
        uState |= MK_RBUTTON;
    }
    if (::GetKeyState(VK_SHIFT) < 0) {
        uState |= MK_SHIFT;
    }
    if (::GetKeyState(VK_MENU) < 0) {
        uState |= MK_ALT;
    }
    return uState;
}

struct TagFindTabInfo {
    CControlUI *pFocus;
    CControlUI *pLast;
    bool bForward;
    bool bNextIsIt;
};
using FINDTABINFO = TagFindTabInfo;

struct TagFindShortCut {
    TCHAR ch;
    bool bPickNext;
};
using FINDSHORTCUT = TagFindShortCut;

struct TagTimerInfo {
    CControlUI *pSender;
    UINT nLocalID;
    HWND hWnd;
    UINT uWinTimer;
    bool bKilled;
};
using TIMERINFO = TagTimerInfo;

tagTDrawInfo::tagTDrawInfo()
{
    Clear();
}

void tagTDrawInfo::Parse(LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    // 1、aaa.jpg
    // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0'
    // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'
    if (pStrImage == nullptr) {
        return;
    }
    sDrawString = pStrImage;
    sDrawModify = pStrModify;
    sImageName = pStrImage;

    CDuiString sItem;
    CDuiString sValue;
    LPTSTR pstr = nullptr;
    for (int i = 0; i < 2; ++i) { // 循环2次
        if (i == 1) {
            pStrImage = pStrModify;
        }
        if (!pStrImage) {
            continue;
        }
        while (pStrImage != nullptr && *pStrImage != _T('\0')) {
            sItem.Empty();
            sValue.Empty();
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            while (pStrImage != nullptr && *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage < pstrTemp) {
                    sItem += *pStrImage++;
                }
            }
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (pStrImage != nullptr && *pStrImage++ != _T('=')) {
                break;
            }
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (pStrImage != nullptr && *pStrImage++ != _T('\'')) {
                break;
            }
            while (pStrImage != nullptr && *pStrImage != _T('\0') && *pStrImage != _T('\'')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage != nullptr && pStrImage < pstrTemp) {
                    sValue += *pStrImage++;
                }
            }
            if (pStrImage != nullptr && *pStrImage++ != _T('\'')) {
                break;
            }
            if (!sValue.IsEmpty()) {
                if (sItem == _T("file") || sItem == _T("res")) {
                    sImageName = sValue;
                } else if (sItem == _T("restype")) {
                    sResType = sValue;
                } else if (sItem == _T("dest")) {
                    rcDest.left = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcDest.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcDest.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcDest.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    CResourceManager::GetInstance()->Scale(&rcDest);
                } else if (sItem == _T("source")) {
                    rcSource.left = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcSource.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcSource.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcSource.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    CResourceManager::GetInstance()->Scale(&rcSource);
                } else if (sItem == _T("corner")) {
                    rcCorner.left = _tcstol(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    rcCorner.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                    ASSERT(pstr);
                    CResourceManager::GetInstance()->Scale(&rcCorner);
                } else if (sItem == _T("mask")) {
                    if (sValue[0] == _T('#')) {
                        dwMask = _tcstoul(sValue.GetData() + 1, &pstr, RADIX_TYPE_TWO_VALUE);
                    } else {
                        dwMask = _tcstoul(sValue.GetData(), &pstr, RADIX_TYPE_TWO_VALUE);
                    }
                } else if (sItem == _T("fade")) {
                    uFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, RADIX_TYPE_ONE_VALUE);
                } else if (sItem == _T("hole")) {
                    bHole = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
                } else if (sItem == _T("xtiled")) {
                    bTiledX = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
                } else if (sItem == _T("ytiled")) {
                    bTiledY = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
                } else if (sItem == _T("hsl")) {
                    bHSL = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
                } else if (sItem == _T("reverse")) {
                    bReverse = (_tcsicmp(sValue.GetData(), _T("true")) == 0);
                }
            }
            if (pStrImage != nullptr && *pStrImage++ != _T(' ')) {
                break;
            }
        }
    }
}
void tagTDrawInfo::Clear()
{
    sDrawString.Empty();
    sDrawModify.Empty();
    sImageName.Empty();

    SecureZeroMemory(&rcDest, sizeof(RECT));
    SecureZeroMemory(&rcSource, sizeof(RECT));
    SecureZeroMemory(&rcCorner, sizeof(RECT));
    dwMask = 0;
    uFade = 255; // 淡出设置为255
    bHole = false;
    bTiledX = false;
    bTiledY = false;
    bHSL = false;
    bReverse = false;
}

using PFUNCUPDATELAYEREDWINDOW = BOOL(__stdcall *)(HWND, HDC, POINT *, SIZE *, HDC,
    POINT *, COLORREF, BLENDFUNCTION *, DWORD);
PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = nullptr;
HPEN m_hUpdateRectPen = nullptr;
HINSTANCE CPaintManagerUI::m_hResourceInstance = nullptr;
CDuiString CPaintManagerUI::m_pStrResourcePath;
CDuiString CPaintManagerUI::m_pStrResourceZip;
HANDLE CPaintManagerUI::m_hResourceZip = nullptr;
bool CPaintManagerUI::m_bCachedResourceZip = true;
int CPaintManagerUI::m_nResType = UILIB_FILE;
TResInfo CPaintManagerUI::m_SharedResInfo;

std::thread::id CPaintManagerUI::uithreadID;
bool CPaintManagerUI::uithreadIDUpdated = false;

std::map<DuiLib::CDuiString, DuiLib::CDuiString> CPaintManagerUI::userValueMap;

HINSTANCE CPaintManagerUI::m_hInstance = nullptr;
bool CPaintManagerUI::m_bUseHSL = false;
short CPaintManagerUI::m_H = 180; // 设置色相值180
short CPaintManagerUI::m_S = 100; // 设置饱和度值100
short CPaintManagerUI::m_L = 100; // 设置亮度值100
CStdPtrArray CPaintManagerUI::m_aPreMessages;
CStdPtrArray CPaintManagerUI::m_aPlugins;

CPaintManagerUI::CPaintManagerUI()
    : m_hWndPaint(nullptr),
      m_hDcPaint(nullptr),
      m_hDcOffscreen(nullptr),
      m_hDcBackground(nullptr),
      m_bOffscreenPaint(true),
      m_hbmpOffscreen(nullptr),
      m_pOffscreenBits(nullptr),
      m_hbmpBackground(nullptr),
      m_pBackgroundBits(nullptr),
      m_hwndTooltip(nullptr),
      m_uTimerID(0x1000),
      m_pRoot(nullptr),
      m_pFocus(nullptr),
      m_pEventKey(nullptr),
      m_bFirstLayout(true),
      m_bFocusNeeded(false),
      m_bUpdateNeeded(false),
      m_bMouseTracking(false),
      m_bMouseCapture(false),
      m_bUsedVirtualWnd(false),
      m_bPointerCapture(false),
      m_bForceUseSharedRes(false),
      m_nOpacity(0xFF),
      m_bLayered(false),
      m_bLayeredChanged(false),
      m_bShowUpdateRect(false),
      m_bCaretActive(false),
      m_bCaretShowing(false),
      m_currentCaretObject(nullptr),
      m_bUseGdiplusText(false),
      m_trh(Gdiplus::TextRenderingHintClearTypeGridFit),
      m_bDragMode(false),
      m_hDragBitmap(nullptr),
      m_bIsPainting(false),
      m_pEventPointer(nullptr),
      m_rtCaret { 0 },
      m_ToolTip { 0 },
      m_rcBlur { 0 }
{
    if (!uithreadIDUpdated) {
        uithreadID = std::this_thread::get_id();
        uithreadIDUpdated = true;

        LOGGER_DEBUG("[DUILIB]: con CPaintManagerUI uithreadID first " << this);
    }

    bool isMain = IsMainThread();
    if (isMain == false) {
        LOGGER_ERROR("[DUILIB]: con CPaintManagerUI. outside ui thread , this:" << this);
    } else {
        LOGGER_ERROR("[DUILIB]: con CPaintManagerUI. isMainThread:" << isMain ? 1 : 0 << ", this:%" << this);
    }

    if (m_SharedResInfo.m_DefaultFontInfo.sFontName.IsEmpty()) {
        m_SharedResInfo.m_dwDefaultDisabledColor = 0xFFA7A6AA;
        m_SharedResInfo.m_dwDefaultFontColor = 0xFF000000;
        m_SharedResInfo.m_dwDefaultLinkFontColor = 0xFF0000FF;
        m_SharedResInfo.m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
        m_SharedResInfo.m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        lf.lfCharSet = DEFAULT_CHARSET;
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            lf.lfCharSet = SHIFTJIS_CHARSET;
        }
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            LOGFONT lfDef;
            if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfDef, 0) != 0) {
                auto ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, lfDef.lfFaceName, LF_FACESIZE - STR_END_SIZE);
                CHECK_RESULT_WITH_LOG(ret != S_OK);
            }
        }
        HFONT hDefaultFont = ::CreateFontIndirect(&lf);
        m_SharedResInfo.m_DefaultFontInfo.hFont = hDefaultFont;
        m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
        m_SharedResInfo.m_DefaultFontInfo.iSize = -lf.lfHeight;
        m_SharedResInfo.m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
        m_SharedResInfo.m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
        m_SharedResInfo.m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
        ::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
    }

    m_ResInfo.m_dwDefaultDisabledColor = m_SharedResInfo.m_dwDefaultDisabledColor;
    m_ResInfo.m_dwDefaultFontColor = m_SharedResInfo.m_dwDefaultFontColor;
    m_ResInfo.m_dwDefaultLinkFontColor = m_SharedResInfo.m_dwDefaultLinkFontColor;
    m_ResInfo.m_dwDefaultLinkHoverFontColor = m_SharedResInfo.m_dwDefaultLinkHoverFontColor;
    m_ResInfo.m_dwDefaultSelectedBkColor = m_SharedResInfo.m_dwDefaultSelectedBkColor;

    if (m_hUpdateRectPen == nullptr) {
        m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0)); // R:220 G:0 B:0
        // Boot Windows Common Controls (for the ToolTip control)
        ::InitCommonControls();
        CDuiString dllFullPath = GetSystemDllPath(_T("msimg32.dll"));
        m_mod = ::LoadLibrary(dllFullPath.GetData());
    }

    m_szMinWindow.cx = 0;
    m_szMinWindow.cy = 0;
    m_szMaxWindow.cx = 0;
    m_szMaxWindow.cy = 0;
    m_szInitWindowSize.cx = 0;
    m_szInitWindowSize.cy = 0;
    m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
    ::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
    ::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
    ::ZeroMemory(&m_rcLayeredInset, sizeof(m_rcLayeredInset));
    ::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
    m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
    m_szOrigWindowSize.cx = 0;
    m_szOrigWindowSize.cy = 0;
    m_ptLastPointerPos.x = 0;
    m_ptLastPointerPos.y = 0;
    m_pGdiplusStartupInput = new Gdiplus::GdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, m_pGdiplusStartupInput, nullptr); // 加载GDI接口
    m_curScale = static_cast<int>(CResourceManager::GetInstance()->GetScale());
    CShadowUI::Initialize(m_hInstance);
}

CPaintManagerUI::~CPaintManagerUI()
{
    bool isMain = IsMainThread();
    int printValue = isMain ? 1 : 0;
    LOG_DEBUG("DUILIB ~CPaintManagerUI %0x isMainThread %0x", this, printValue);

    // Delete the control-tree structures
    for (int i = 0; i < m_aDelayedCleanup.GetSize(); i++) {
        CControlUI *item1 = static_cast<CControlUI *>(m_aDelayedCleanup.GetAt(i));
        if (item1 == nullptr) {
            continue;
        }
        delete item1;
    }
    for (int i = 0; i < m_aAsyncNotify.GetSize(); i++) {
        TNotifyUI *item1 = static_cast<TNotifyUI *>(m_aAsyncNotify.GetAt(i));
        if (item1 == nullptr) {
            continue;
        }
        delete item1;
    }
    m_mNameHash.Resize(0);
    if (m_pRoot != nullptr) {
        delete m_pRoot;
    }
    m_pRoot = nullptr;

    ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
    RemoveAllFonts();
    RemoveAllImages();
    RemoveAllStyle();
    RemoveAllDefaultAttributeList();
    RemoveAllWindowCustomAttribute();
    RemoveAllOptionGroups();
    RemoveAllTimers();
    RemoveAllDrawInfos();

    if (m_hwndTooltip != nullptr) {
        ::DestroyWindow(m_hwndTooltip);
        m_hwndTooltip = nullptr;
    }
    if (m_hDcOffscreen != nullptr) {
        ::DeleteDC(m_hDcOffscreen);
    }
    if (m_hDcBackground != nullptr) {
        ::DeleteDC(m_hDcBackground);
    }
    if (m_hbmpOffscreen != nullptr) {
        ::DeleteObject(m_hbmpOffscreen);
    }
    if (m_hbmpBackground != nullptr) {
        ::DeleteObject(m_hbmpBackground);
    }
    if (m_hDcPaint != nullptr) {
        ::ReleaseDC(m_hWndPaint, m_hDcPaint);
    }
    m_aPreMessages.Remove(m_aPreMessages.Find(this));
    // 销毁拖拽图片
    if (m_hDragBitmap != nullptr) {
        ::DeleteObject(m_hDragBitmap);
    }
    // 卸载GDIPlus
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
    delete m_pGdiplusStartupInput;
    m_pGdiplusStartupInput = nullptr;
    if (m_mod != nullptr) {
        (void)FreeLibrary(m_mod);
    }
}

void CPaintManagerUI::Init(HWND hWnd, LPCTSTR pstrName)
{
    ASSERT(::IsWindow(hWnd));

    m_mNameHash.Resize();
    RemoveAllFonts();
    RemoveAllImages();
    RemoveAllStyle();
    RemoveAllDefaultAttributeList();
    RemoveAllWindowCustomAttribute();
    RemoveAllOptionGroups();
    RemoveAllTimers();

    m_sName.Empty();
    if (pstrName != nullptr) {
        m_sName = pstrName;
    }

    if (m_hWndPaint != hWnd) {
        m_hWndPaint = hWnd;
        m_hDcPaint = ::GetDC(hWnd);
        m_aPreMessages.Add(this);
    }
    LOG_DEBUG("[DUILIB]: this:%0x, Init m_hDcPaint:%0x ,hWnd:%0x ", this, m_hDcPaint, hWnd);
    SetTargetWnd(hWnd);
    InitDragDrop();
}

void CPaintManagerUI::DeletePtr(void *ptr)
{
    if (ptr) {
        delete ptr;
        ptr = nullptr;
    }
}

HINSTANCE CPaintManagerUI::GetInstance()
{
    return m_hInstance;
}

CDuiString CPaintManagerUI::GetInstancePath()
{
    if (m_hInstance == nullptr) {
        return _T('\0');
    }

    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
    CDuiString sInstancePath = tszModule;
    int pos = sInstancePath.ReverseFind(_T('\\'));
    if (pos >= 0) {
        sInstancePath = sInstancePath.Left(pos + 1);
    }
    return sInstancePath;
}

CDuiString CPaintManagerUI::GetCurrentPath()
{
    TCHAR tszModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
    CDuiString sInstancePath = tszModule;
    int pos = sInstancePath.ReverseFind(_T('\\'));
    if (pos >= 0) {
        sInstancePath = sInstancePath.Left(pos + 1);
    }
    return sInstancePath;
}

HINSTANCE CPaintManagerUI::GetResourceDll()
{
    if (m_hResourceInstance == nullptr) {
        return m_hInstance;
    }
    return m_hResourceInstance;
}

const CDuiString &CPaintManagerUI::GetResourcePath()
{
    return m_pStrResourcePath;
}

const CDuiString &CPaintManagerUI::GetResourceZip()
{
    return m_pStrResourceZip;
}

bool CPaintManagerUI::IsCachedResourceZip()
{
    return m_bCachedResourceZip;
}

HANDLE CPaintManagerUI::GetResourceZipHandle()
{
    return m_hResourceZip;
}

void CPaintManagerUI::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}

void CPaintManagerUI::SetCurrentPath(LPCTSTR pStrPath)
{
    ::SetCurrentDirectory(pStrPath);
}

void CPaintManagerUI::SetResourceDll(HINSTANCE hInst)
{
    m_hResourceInstance = hInst;
}

void CPaintManagerUI::SetResourcePath(LPCTSTR pStrPath)
{
    m_pStrResourcePath = pStrPath;
    if (m_pStrResourcePath.IsEmpty()) {
        return;
    }
    TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
    if (cEnd != _T('\\') && cEnd != _T('/')) {
        m_pStrResourcePath += _T('\\');
    }
}

void CPaintManagerUI::SetResourceType(int nType)
{
    m_nResType = nType;
}

int CPaintManagerUI::GetResourceType()
{
    return m_nResType;
}

bool CPaintManagerUI::GetHSL(short *H, short *S, short *L)
{
    *H = m_H;
    *S = m_S;
    *L = m_L;
    return m_bUseHSL;
}

void CPaintManagerUI::SetHSL(bool bUseHSL, short H, short S, short L)
{
    if (m_bUseHSL || m_bUseHSL != bUseHSL) {
        m_bUseHSL = bUseHSL;
        if (m_H == H && m_S == S && m_L == L) {
            return;
        }
        m_H = CLAMP(H, 0, 360); // 设置色相 Hue 360
        m_S = CLAMP(S, 0, 200); // 设置饱和度 Saturation 200
        m_L = CLAMP(L, 0, 200); // 设置亮度 Lightness 200
        AdjustSharedImagesHSL();
        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
            if (pManager != nullptr) {
                pManager->AdjustImagesHSL();
            }
        }
    }
}

void CPaintManagerUI::ReloadSkin()
{
    ReloadSharedImages();
    for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
        CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
        if (pManager == nullptr) {
            continue;
        }
        pManager->ReloadImages();
    }
}

void CPaintManagerUI::ReloadPaintText(LPCTSTR language)
{
#ifdef SWITCH_LANGUAGE_TEST
    for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
        CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
        if (pManager == nullptr) {
            continue;
        }
        pManager->TextPaintUpdate(language);
    }
#endif
}

CPaintManagerUI *CPaintManagerUI::GetPaintManager(LPCTSTR pstrName)
{
    if (pstrName == nullptr) {
        return nullptr;
    }
    CDuiString sName = pstrName;
    if (sName.IsEmpty()) {
        return nullptr;
    }
    for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
        CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
        if (pManager == nullptr) {
            continue;
        }
        if (pManager != nullptr && sName == pManager->GetName()) {
            return pManager;
        }
    }
    return nullptr;
}

CStdPtrArray *CPaintManagerUI::GetPaintManagers()
{
    return &m_aPreMessages;
}

bool CPaintManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
{
    ASSERT(!::IsBadStringPtr(pstrModuleName, -1) || pstrModuleName == nullptr);
    if (pstrModuleName == nullptr) {
        return false;
    }
    HMODULE hModule = ::LoadLibrary(pstrModuleName);
    if (hModule != nullptr) {
        LPCREATECONTROL lpCreateControl = (LPCREATECONTROL)::GetProcAddress(hModule, "CreateControl");
        if (lpCreateControl != nullptr) {
            if (m_aPlugins.Find(lpCreateControl) >= 0) {
                (void)FreeLibrary(hModule);
                return true;
            }
            m_aPlugins.Add(lpCreateControl);
            (void)FreeLibrary(hModule);
            return true;
        }
        (void)FreeLibrary(hModule);
    }
    return false;
}

CStdPtrArray *CPaintManagerUI::GetPlugins()
{
    return &m_aPlugins;
}

HWND CPaintManagerUI::GetPaintWindow() const
{
    return m_hWndPaint;
}

HWND CPaintManagerUI::GetTooltipWindow() const
{
    return m_hwndTooltip;
}

LPCTSTR CPaintManagerUI::GetName() const
{
    return m_sName;
}

HDC CPaintManagerUI::GetPaintDC() const
{
    return m_hDcPaint;
}

HDC CPaintManagerUI::GetOffscreenPaintDC() const
{
    return m_hDcOffscreen;
}

POINT CPaintManagerUI::GetMousePos() const
{
    return m_ptLastMousePos;
}

SIZE CPaintManagerUI::GetClientSize() const
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    return CDuiSize(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
}

SIZE CPaintManagerUI::GetInitSize()
{
    return m_szInitWindowSize;
}

void CPaintManagerUI::SetInitSize(int cx, int cy)
{
    m_szInitWindowSize.cx = cx;
    m_szInitWindowSize.cy = cy;
    if (m_pRoot == nullptr && m_hWndPaint != nullptr) {
        ::SetWindowPos(m_hWndPaint, nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }
}

RECT &CPaintManagerUI::GetSizeBox()
{
    return m_rcSizeBox;
}

void CPaintManagerUI::SetSizeBox(RECT &rcSizeBox)
{
    m_rcSizeBox = rcSizeBox;
}

RECT &CPaintManagerUI::GetCaptionRect()
{
    return m_rcCaption;
}

void CPaintManagerUI::SetCaptionRect(RECT &rcCaption)
{
    m_rcCaption = rcCaption;
}

SIZE CPaintManagerUI::GetRoundCorner() const
{
    return m_szRoundCorner;
}

void CPaintManagerUI::SetRoundCorner(int cx, int cy)
{
    m_szRoundCorner.cx = cx;
    m_szRoundCorner.cy = cy;
}

SIZE CPaintManagerUI::GetMinInfo() const
{
    return m_szMinWindow;
}

void CPaintManagerUI::SetMinInfo(int cx, int cy)
{
    ASSERT(cx >= 0 && cy >= 0);
    m_szMinWindow.cx = cx;
    m_szMinWindow.cy = cy;
}

SIZE CPaintManagerUI::GetMaxInfo() const
{
    return m_szMaxWindow;
}

void CPaintManagerUI::SetMaxInfo(int cx, int cy)
{
    ASSERT(cx >= 0 && cy >= 0);
    m_szMaxWindow.cx = cx;
    m_szMaxWindow.cy = cy;
}

bool CPaintManagerUI::IsShowUpdateRect() const
{
    return m_bShowUpdateRect;
}

void CPaintManagerUI::SetShowUpdateRect(bool show)
{
    m_bShowUpdateRect = show;
}

BYTE CPaintManagerUI::GetOpacity() const
{
    return m_nOpacity;
}

void CPaintManagerUI::SetOpacity(BYTE nOpacity)
{
    m_nOpacity = nOpacity;
    if (m_hWndPaint != nullptr) {
        using PFUNCSETLAYEREDWINDOWATTR = BOOL(__stdcall *)(HWND, COLORREF, BYTE, DWORD);
        PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = nullptr;

        HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
        if (hUser32) {
            fSetLayeredWindowAttributes =
                (PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
        }
        if (fSetLayeredWindowAttributes == nullptr) {
            return;
        }

        DWORD dwStyle = static_cast<DWORD>(::GetWindowLong(m_hWndPaint, GWL_EXSTYLE));
        DWORD dwNewStyle = dwStyle;
        if (nOpacity >= 0 && nOpacity < 256) { // 32字节
            dwNewStyle |= WS_EX_LAYERED;
        } else {
            dwNewStyle &= ~WS_EX_LAYERED;
        }
        if (dwStyle != dwNewStyle) {
            ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
        }
        fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA | LWA_COLORKEY);
    }
}

bool CPaintManagerUI::IsLayered()
{
    return m_bLayered;
}

void CPaintManagerUI::SetLayered(bool bLayered)
{
    if (m_hWndPaint != nullptr && bLayered != m_bLayered) {
        UINT uStyle = static_cast<UINT>(GetWindowStyle(m_hWndPaint));
        if ((uStyle & WS_CHILD) != 0) {
            return;
        }
        if (g_fUpdateLayeredWindow == nullptr) {
            HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
            if (hUser32) {
                g_fUpdateLayeredWindow = (PFUNCUPDATELAYEREDWINDOW)::GetProcAddress(hUser32, "UpdateLayeredWindow");
                if (g_fUpdateLayeredWindow == nullptr) {
                    return;
                }
            }
        }
        m_bLayered = bLayered;
        if (m_pRoot != nullptr) {
            m_pRoot->NeedUpdate();
        }
        Invalidate();
    }
}

RECT &CPaintManagerUI::GetLayeredInset()
{
    return m_rcLayeredInset;
}

void CPaintManagerUI::SetLayeredInset(RECT &rcLayeredInset)
{
    m_rcLayeredInset = rcLayeredInset;
    m_bLayeredChanged = true;
    Invalidate();
}

BYTE CPaintManagerUI::GetLayeredOpacity()
{
    return m_nOpacity;
}

void CPaintManagerUI::SetLayeredOpacity(BYTE nOpacity)
{
    m_nOpacity = nOpacity;
    m_bLayeredChanged = true;
    Invalidate();
}

bool CPaintManagerUI::ShowCaret(bool bShow)
{
    if (m_bCaretShowing == bShow) {
        return true;
    }

    m_bCaretShowing = bShow;
    if (!bShow) {
        ::KillTimer(m_hWndPaint, CARET_TIMERID);
        if (m_bCaretActive) {
            Invalidate(m_rtCaret);
        }
        m_bCaretActive = false;
    } else {
        ::SetCoalescableTimer(m_hWndPaint, CARET_TIMERID, ::GetCaretBlinkTime(), nullptr, TIMERV_DEFAULT_COALESCING);
        if (!m_bCaretActive) {
            Invalidate(m_rtCaret);
            m_bCaretActive = true;
        }
    }

    return true;
}

bool CPaintManagerUI::SetCaretPos(CRichEditUI *obj, int x, int y)
{
    if (!::SetCaretPos(x, y)) {
        return false;
    }

    m_currentCaretObject = obj;
    RECT tempRt = m_rtCaret;
    int w = m_rtCaret.right - m_rtCaret.left;
    int h = m_rtCaret.bottom - m_rtCaret.top;
    m_rtCaret.left = x;
    m_rtCaret.top = y;
    m_rtCaret.right = x + w;
    m_rtCaret.bottom = y + h;
    Invalidate(tempRt);
    Invalidate(m_rtCaret);

    return true;
}

CRichEditUI *CPaintManagerUI::GetCurrentCaretObject()
{
    return m_currentCaretObject;
}

bool CPaintManagerUI::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
    ::CreateCaret(m_hWndPaint, hBmp, nWidth, nHeight);
    // hBmp处理位图光标
    m_rtCaret.right = m_rtCaret.left + nWidth;
    m_rtCaret.bottom = m_rtCaret.top + nHeight;
    return true;
}

void CPaintManagerUI::DrawCaret(HDC hDC, const RECT &rcPaint)
{
    if (m_currentCaretObject && (!m_currentCaretObject->IsFocused() || m_hWndPaint != ::GetFocus())) {
        ::KillTimer(m_hWndPaint, CARET_TIMERID);
        if (m_bCaretActive) {
            Invalidate(m_rtCaret);
        }
        m_bCaretActive = false;
        return;
    }

    if (m_bCaretActive && m_bCaretShowing && m_currentCaretObject) {
        RECT temp = {};
        if (::IntersectRect(&temp, &rcPaint, &m_rtCaret)) {
            DWORD dwColor = m_currentCaretObject->GetTextColor();
            if (dwColor == 0) {
                dwColor = m_ResInfo.m_dwDefaultFontColor;
            }
            CRenderEngine::DrawColor(hDC, temp, dwColor);
        }
    }
}

CShadowUI *CPaintManagerUI::GetShadow()
{
    return &m_shadow;
}

void CPaintManagerUI::SetUseGdiplusText(bool bUse)
{
    m_bUseGdiplusText = bUse;
}

bool CPaintManagerUI::IsUseGdiplusText() const
{
    return m_bUseGdiplusText;
}

void CPaintManagerUI::SetGdiplusTextRenderingHint(int trh)
{
    m_trh = trh;
}

int CPaintManagerUI::GetGdiplusTextRenderingHint() const
{
    return m_trh;
}

bool CPaintManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & /* lRes */)
{
    for (int i = 0; i < m_aPreMessageFilters.GetSize(); i++) {
        bool bHandled = false;
        IMessageFilterUI *item1 = static_cast<IMessageFilterUI *>(m_aPreMessageFilters.GetAt(i));
        if (item1 == nullptr) {
            continue;
        }
        LRESULT lResult = item1->MessageHandler(uMsg, wParam, lParam, bHandled);
        if (bHandled) {
            return true;
        }
    }
    switch (uMsg) {
        case WM_KEYDOWN: {
            // Tabbing between controls
            if (wParam == VK_TAB) {
                if (m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && m_pFocus->GetClass() &&
                    _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != nullptr) {
                    if (static_cast<CRichEditUI *>(m_pFocus)->IsWantTab()) {
                        return false;
                    }
                }
                SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
                return true;
            }
        } break;
        case WM_SYSCHAR: {
            // Handle ALT-shortcut key-combinations
            FINDSHORTCUT fs = { 0 };
            fs.ch = static_cast<TCHAR>(toupper(static_cast<int>(wParam)));
            CControlUI *pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs,
                UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
            if (pControl != nullptr) {
                pControl->SetFocus();
                pControl->Activate();
                return true;
            }
        } break;
        case WM_SYSKEYDOWN: {
            if (m_pFocus != nullptr) {
                TEventUI event = { 0 };
                event.Type = UIEVENT_SYSKEY;
                event.chKey = (TCHAR)wParam;
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            }
        } break;
    }
    return false;
}

bool CPaintManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
{
    if (m_hWndPaint == nullptr || !m_pRoot) {
        return false;
    }
    // Cycle through listeners
    for (int i = 0; i < m_aMessageFilters.GetSize(); i++) {
        bool bHandled = false;
        IMessageFilterUI *item1 = static_cast<IMessageFilterUI *>(m_aMessageFilters.GetAt(i));
        if (item1 == nullptr) {
            continue;
        }
        LRESULT lResult = item1->MessageHandler(uMsg, wParam, lParam, bHandled);
        if (bHandled) {
            lRes = lResult;
            return true;
        }
    }
    // Custom handling of events
    switch (uMsg) {
        case WM_APP + 1: {
            for (int i = 0; i < m_aDelayedCleanup.GetSize(); i++) {
                CControlUI *item1 = static_cast<CControlUI *>(m_aDelayedCleanup.GetAt(i));
                if (item1 == nullptr) {
                    continue;
                }
                delete item1;
            }
            m_aDelayedCleanup.Empty();

            TNotifyUI *pMsg = nullptr;
            while (pMsg = static_cast<TNotifyUI *>(m_aAsyncNotify.GetAt(0))) {
                m_aAsyncNotify.Remove(0);
                if (pMsg->pSender != nullptr) {
                    if (pMsg->pSender->OnNotify) {
                        pMsg->pSender->OnNotify(pMsg);
                    }
                }
                for (int j = 0; j < m_aNotifiers.GetSize(); j++) {
                    INotifyUI *item1 = static_cast<INotifyUI *>(m_aNotifiers.GetAt(j));
                    if (item1 == nullptr) {
                        continue;
                    }
                    item1->Notify(*pMsg);
                }
                delete pMsg;
            }
        } break;
        case WM_CLOSE: {
            SetFocus(nullptr);

            if (::GetActiveWindow() == m_hWndPaint) {
                HWND hwndParent = GetWindowOwner(m_hWndPaint);
                if (hwndParent != nullptr) {
                    HWND hWnd = ::SetFocus(hwndParent);
                    if (hWnd == nullptr) {
                        LOGGER_ERROR("[DUILIB] WM_CLOSE SetFocus return null " << GetLastError());
                    }
                }
            }

            if (m_hwndTooltip != nullptr) {
                ::DestroyWindow(m_hwndTooltip);
                m_hwndTooltip = nullptr;
            }


            m_bMouseTracking = false;
        } break;
        case WM_ERASEBKGND: {
            // We'll do the painting here...
            lRes = 1;
        }
            return true;
        case WM_PAINT: {
            return OnWM_PAINT(uMsg, wParam, lParam, lRes);
        }
        case WM_PRINTCLIENT: {
            if (m_pRoot == nullptr) {
                break;
            }
            RECT rcClient;
            ::GetClientRect(m_hWndPaint, &rcClient);
            HDC hDC = (HDC)wParam;
            int save = ::SaveDC(hDC);
            m_pRoot->DoPaint(hDC, rcClient);
            if (((DWORD)lParam & PRF_CHILDREN) != 0) {
                HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                while (hWndChild != nullptr) {
                    RECT rcPos = { 0 };
                    const UINT cPoints = 2;
                    ::GetWindowRect(hWndChild, &rcPos);
                    ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), cPoints);
                    ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, nullptr);
                    ::SendMessage(hWndChild, WM_PRINT, wParam, ((DWORD)lParam) | PRF_NONCLIENT);
                    hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                }
            }
            ::RestoreDC(hDC, save);
        } break;
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            if (m_szMinWindow.cx > 0) {
                lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
            }
            if (m_szMinWindow.cy > 0) {
                lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
            }
            if (m_szMaxWindow.cx > 0) {
                lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
            }
            if (m_szMaxWindow.cy > 0) {
                lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
            }
        } break;
        case WM_SIZE: {
            if (m_pFocus != nullptr) {
                TEventUI event = { 0 };
                event.Type = UIEVENT_WINDOWSIZE;
                event.pSender = m_pFocus;
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            }
            if (m_pRoot != nullptr) {
                m_pRoot->NeedUpdate();
            }
            if (wParam == SIZE_RESTORED) {
                UpdateDPIByMonitor(GetPaintWindow());
            }
        }
            return true;
        case WM_TIMER: {
            if (CARET_TIMERID == LOWORD(wParam)) {
                Invalidate(m_rtCaret);
                m_bCaretActive = !m_bCaretActive;
            } else {
                for (int i = 0; i < m_aTimers.GetSize(); i++) {
                    const TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i));
                    if (pTimer == nullptr) {
                        continue;
                    }
                    if (pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) &&
                        pTimer->bKilled == false) {
                        TEventUI event = { 0 };
                        event.Type = UIEVENT_TIMER;
                        event.pSender = pTimer->pSender;
                        event.dwTimestamp = ::GetTickCount();
                        event.ptMouse = m_ptLastMousePos;
                        event.wKeyState = MapKeyState();
                        event.wParam = pTimer->nLocalID;
                        event.lParam = lParam;
                        pTimer->pSender->Event(event);
                        break;
                    }
                }
            }
        } break;
        case WM_MOUSEHOVER: {
#ifdef TEST_TIPS_BUILD
            break;
#endif // TEST_TIPS_BUILD
            m_bMouseTracking = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            CControlUI *pHover = FindControl(pt);
            if (pHover == nullptr) {
                break;
            }
            // Generate mouse hover event
            TranspostMouseEvent(UIEVENT_MOUSEHOVER, pt, (WORD)MapKeyState(), wParam, lParam);

            // Create tooltip information
            static int s_maxLength = 255;
            CDuiString sToolTip = pHover->GetToolTip();
            int strLength = sToolTip.GetLength();
            if (strLength > s_maxLength) {
                s_maxLength = strLength;
            }
            ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
            m_ToolTip.cbSize = sizeof(TOOLINFO);
            m_ToolTip.uFlags = TTF_IDISHWND;
            m_ToolTip.hwnd = m_hWndPaint;
            m_ToolTip.uId = (UINT_PTR)m_hWndPaint;
            m_ToolTip.hinst = m_hInstance;
            m_ToolTip.lpszText = const_cast<LPTSTR>((LPCTSTR)sToolTip);
            m_ToolTip.rect = pHover->GetPos();
            if (m_hwndTooltip == nullptr) {
                m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, nullptr,
                    m_hInstance, nullptr);
                ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ToolTip);
                ::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->GetToolTipWidth());
                if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
                    LONG lStyle = ::GetWindowLong(m_hwndTooltip, GWL_EXSTYLE);
                    ::SetWindowLong(m_hwndTooltip, GWL_EXSTYLE, ((unsigned long)lStyle) | WS_EX_LAYOUTRTL);
                }
            }

            if (!::IsWindowVisible(m_hwndTooltip)) {
                ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
                ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTip);
            } else {
                TOOLINFO ti = { 0 };
                ti.cbSize = sizeof(TOOLINFO);
                ti.hwnd = m_hWndPaint;
                ti.uId = (UINT_PTR)m_hWndPaint;

                std::unique_ptr<WCHAR[]> buf(new WCHAR[s_maxLength]);
                if (buf.get()) {
                    SecureZeroMemory(buf.get(), s_maxLength);
                }
                ti.lpszText = buf.get();

                // Get the current tooltip definition.
                if (SendMessage(m_hwndTooltip, TTM_GETTOOLINFO, 0, (LPARAM)&ti)) {
                    wstring strGet;
                    wstring strNow;
                    if (ti.lpszText != nullptr) {
                        strGet = ti.lpszText;
                    }

                    if (m_ToolTip.lpszText != nullptr) {
                        strNow = m_ToolTip.lpszText;
                    }

                    RECT rectGet = ti.rect;
                    RECT rectNow = m_ToolTip.rect;

                    if (strGet != strNow) {
                        ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
                    }
                    if (!EqualRect(&rectGet, &rectNow)) {
                        ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
                    }
                }
            }
        }
            return true;
        case WM_MOUSELEAVE: {
            if (m_hwndTooltip != nullptr) {
                ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_ToolTip);
            }
            if (m_bMouseTracking) {
                POINT pt = { 0 };
                RECT rcWnd = { 0 };
                ::GetCursorPos(&pt);
                ::GetWindowRect(m_hWndPaint, &rcWnd);
                if (!::IsIconic(m_hWndPaint) && ::GetActiveWindow() == m_hWndPaint && ::PtInRect(&rcWnd, pt)) {
                    if (::SendMessage(m_hWndPaint, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT) {
                        ::ScreenToClient(m_hWndPaint, &pt);
                        ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
                    } else {
                        ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
                    }
                } else {
                    ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
                }
            }
            m_bMouseTracking = false;
        } break;
        case WM_MOUSEMOVE: {
            // Start tracking this entire window again...
            if (!m_bMouseTracking) {
                TRACKMOUSEEVENT tme = { 0 };
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_HOVER | TME_LEAVE;
                tme.hwndTrack = m_hWndPaint;
                tme.dwHoverTime = m_hwndTooltip == nullptr ?
                    400UL :
                    (DWORD)::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                _TrackMouseEvent(&tme);
                m_bMouseTracking = true;
            }

            // Generate the appropriate mouse messages
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            // 是否移动
            bool bNeedDrag = true;
            if (m_ptLastMousePos.x == pt.x && m_ptLastMousePos.y == pt.y) {
                bNeedDrag = false;
            }

            CControlUI *pControl = FindControl(pt);
            if (pControl != nullptr && pControl->GetManager() != this) {
                break;
            }

            // 拖拽事件
            if (bNeedDrag && m_bDragMode && wParam == MK_LBUTTON) {
                ::ReleaseCapture();
                CIDropSource *pdsrc = new CIDropSource;
                if (pdsrc == nullptr) {
                    return 0;
                }
                pdsrc->AddRef();

                CIDataObject *pdobj = new CIDataObject(pdsrc);
                if (pdobj == nullptr) {
                    return 0;
                }
                pdobj->AddRef();

                FORMATETC fmtetc = { 0 };
                STGMEDIUM medium = { 0 };
                fmtetc.dwAspect = DVASPECT_CONTENT;
                fmtetc.lindex = -1;
                fmtetc.cfFormat = CF_BITMAP;
                fmtetc.tymed = TYMED_GDI;
                medium.tymed = TYMED_GDI;
                HBITMAP hBitmap = (HBITMAP)OleDuplicateData(m_hDragBitmap, fmtetc.cfFormat, NULL);
                medium.hBitmap = hBitmap;
                pdobj->SetData(&fmtetc, &medium, FALSE);
                BITMAP bmap;
                GetObject(hBitmap, sizeof(BITMAP), &bmap);
                RECT rc = { 0, 0, bmap.bmWidth, bmap.bmHeight };
                fmtetc.cfFormat = CF_ENHMETAFILE;
                fmtetc.tymed = TYMED_ENHMF;
                HDC hMetaDC = CreateEnhMetaFile(m_hDcPaint, nullptr, nullptr, nullptr);
                HDC hdcMem = CreateCompatibleDC(m_hDcPaint);
                HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBitmap);
                ::BitBlt(hMetaDC, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
                ::SelectObject(hdcMem, hOldBmp);
                medium.hEnhMetaFile = CloseEnhMetaFile(hMetaDC);
                DeleteDC(hdcMem);
                medium.tymed = TYMED_ENHMF;
                pdobj->SetData(&fmtetc, &medium, TRUE);
                CDragSourceHelper dragSrcHelper;
                POINT ptDrag = { 0 };
                const int multiple = 2;
                ptDrag.x = bmap.bmWidth / multiple;
                ptDrag.y = bmap.bmHeight / multiple;
                dragSrcHelper.InitializeFromBitmap(hBitmap, ptDrag, rc, pdobj); // will own the bmp
                DWORD dwEffect;
                (void)::DoDragDrop(pdobj, pdsrc, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
                pdsrc->Release();
                pdobj->Release();
                m_bDragMode = false;
                break;
            }

            if (TranspostMouseEvent(UIEVENT_MOUSELEAVE, pt, 0, 0, 0)) {
                if (m_hwndTooltip != nullptr) {
                    ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_ToolTip);
                }
            }
            TranspostMouseEvent(UIEVENT_MOUSEENTER, pt, 0, 0, 0);
            TranspostMouseEvent(UIEVENT_MOUSEMOVE, pt, 0, 0, 0);

            // 记录鼠标位置
            m_ptLastMousePos = pt;
        } break;
        case WM_LBUTTONDOWN: {
            // We alway set focus back to our app (this helps
            // when Win32 child windows are placed on the dialog
            // and we need to remove them on focus change).
            HWND hWnd = ::SetFocus(m_hWndPaint);
            if (hWnd == nullptr) {
                LOGGER_ERROR("[DUILIB] WM_LBUTTONDOWN SetFocus return null " << GetLastError());
            }
            // 查找控件
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }

            // 准备拖拽
            if (pControl->IsDragEnabled()) {
                m_bDragMode = true;
                if (m_hDragBitmap != nullptr) {
                    ::DeleteObject(m_hDragBitmap);
                    m_hDragBitmap = nullptr;
                }
                m_hDragBitmap = CRenderEngine::GenerateBitmap(this, pControl, pControl->GetPos());
            }

            // 开启捕获
            SetCapture();
            // 事件处理
            pControl->SetFocus();

            TEventUI event = { 0 };
            event.Type = UIEVENT_BUTTONDOWN;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = (WORD)wParam;
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        } break;
        case WM_LBUTTONDBLCLK: {
            HWND hWnd = ::SetFocus(m_hWndPaint);
            if (hWnd == nullptr) {
                LOGGER_ERROR("[DUILIB] WM_LBUTTONDBLCLK SetFocus return null " << GetLastError());
            }
            SetCapture();
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }

            TranspostMouseEvent(UIEVENT_DBLCLICK, pt, (WORD)wParam, 0, 0);
        } break;
        case WM_LBUTTONUP: {
            ReleaseCapture();
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;

            TranspostMouseEvent(UIEVENT_BUTTONUP, pt, (WORD)wParam, wParam, lParam);
        } break;
        case WM_RBUTTONDOWN: {
            HWND hWnd = ::SetFocus(m_hWndPaint);
            if (hWnd == nullptr) {
                LOGGER_ERROR("[DUILIB] WM_RBUTTONDOWN SetFocus return null " << GetLastError());
            }
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }
            pControl->SetFocus();
            SetCapture();

            TranspostMouseEvent(UIEVENT_RBUTTONDOWN, pt, (WORD)wParam, wParam, lParam);
        } break;
        case WM_RBUTTONUP: {
            ReleaseCapture();

            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;

            TranspostMouseEvent(UIEVENT_RBUTTONUP, pt, (WORD)wParam, wParam, lParam);
        } break;
        case WM_MBUTTONDOWN: {
            HWND hWnd = ::SetFocus(m_hWndPaint);
            if (hWnd == nullptr) {
                LOGGER_ERROR("[DUILIB] WM_MBUTTONDOWN SetFocus return null " << GetLastError());
            }
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }
            pControl->SetFocus();
            SetCapture();

            TranspostMouseEvent(UIEVENT_MBUTTONDOWN, pt, (WORD)wParam, wParam, lParam);
        } break;
        case WM_MBUTTONUP: {
            ReleaseCapture();

            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            m_ptLastMousePos = pt;

            TranspostMouseEvent(UIEVENT_MBUTTONUP, pt, (WORD)wParam, wParam, lParam);
        } break;
        case 0x0246 /* WM_POINTERDOWN */: {
            HWND hWnd = ::SetFocus(m_hWndPaint);
            if (hWnd == nullptr) {
                LOGGER_ERROR("[DUILIB] WM_POINTERDOWN SetFocus return null " << GetLastError());
            }
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastPointerPos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }
            pControl->SetFocus();
            SetCapture();
            m_pEventPointer = pControl;
            SendPointerDownEvent(pt, wParam, lParam);
        } break;
        case 0x0247 /* WM_POINTERUP */: {
            ReleaseCapture();
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            CControlUI *currentControl = FindControl(pt);
            if (currentControl == nullptr) {
                break;
            }
            if (currentControl->GetManager() != this) {
                break;
            }
            m_pEventPointer = currentControl;
            m_ptLastPointerPos = pt;
            SendPointerUpEvent(pt, lParam, wParam);
        } break;
        case 0x0245 /* WM_POINTERUPDATE */: {
            if (m_pEventPointer == nullptr || !m_bPointerCapture) {
                break;
            }
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);

            CControlUI *currentControl = FindControl(pt);
            if (currentControl == nullptr || currentControl->GetManager() != this) {
                break;
            }
            m_pEventPointer = currentControl;

            int zDelta = static_cast<int>(pt.y - m_ptLastPointerPos.y);
            m_ptLastPointerPos = pt;

            if (zDelta == 0) {
                break;
            }

            TEventUI event = { 0 };
            event.Type = UIEVENT_POINTERUPDATE;
            event.pSender = m_pEventPointer;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            event.ptMouse = pt;

            m_pEventPointer->Event(event);
        } break;
        case WM_CONTEXTMENU: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            ReleaseCapture();

            TranspostMouseEvent(UIEVENT_CONTEXTMENU, pt, (WORD)wParam, 0, 0);
        } break;
        case WM_MOUSEWHEEL: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ScreenToClient(m_hWndPaint, &pt);
            m_ptLastMousePos = pt;
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if (pControl->GetManager() != this) {
                break;
            }
            int zDelta = static_cast<int short>HIWORD(wParam);
            TEventUI event = { 0 };
            event.Type = UIEVENT_SCROLLWHEEL;
            event.pSender = pControl;
            event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
            event.lParam = lParam;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);

            // Let's make sure that the scroll item below the cursor is the same as before...
            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
        } break;
        case WM_CHAR: {
            if (m_pFocus == nullptr) {
                break;
            }
            TEventUI event = { 0 };
            event.Type = UIEVENT_CHAR;
            event.pSender = m_pFocus;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
        } break;
        case WM_KEYDOWN: {
            if (m_pFocus == nullptr) {
                break;
            }
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYDOWN;
            event.pSender = m_pFocus;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            if (nullptr != m_shortCutCtr) {
                m_shortCutCtr->Event(event);
            } else {
                m_pFocus->Event(event);
            }
            m_pEventKey = m_pFocus;
        } break;
        case WM_KEYUP: {
            if (m_pEventKey == nullptr) {
                break;
            }
            TEventUI event = { 0 };
            event.Type = UIEVENT_KEYUP;
            event.pSender = m_pEventKey;
            event.wParam = wParam;
            event.lParam = lParam;
            event.chKey = (TCHAR)wParam;
            event.ptMouse = m_ptLastMousePos;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            m_pEventKey->Event(event);
            m_pEventKey = nullptr;
        } break;
        case WM_SETCURSOR: {
            if (LOWORD(lParam) != HTCLIENT) {
                break;
            }
            if (m_bMouseCapture) {
                return true;
            }

            POINT pt = { 0 };
            ::GetCursorPos(&pt);
            ::ScreenToClient(m_hWndPaint, &pt);
            CControlUI *pControl = FindControl(pt);
            if (pControl == nullptr) {
                break;
            }
            if ((pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0) {
                break;
            }
            TEventUI event = { 0 };
            event.Type = UIEVENT_SETCURSOR;
            event.pSender = pControl;
            event.wParam = wParam;
            event.lParam = lParam;
            event.ptMouse = pt;
            event.wKeyState = MapKeyState();
            event.dwTimestamp = ::GetTickCount();
            pControl->Event(event);
        }
            return true;
        case WM_SETFOCUS: {
            if (m_pFocus != nullptr) {
                TEventUI event = { 0 };
                event.Type = UIEVENT_SETFOCUS;
                event.wParam = wParam;
                event.lParam = lParam;
                event.pSender = m_pFocus;
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            }
            break;
        }
        case WM_NOTIFY: {
            LPNMHDR lpNMHDR = (LPNMHDR)lParam;
            if (lpNMHDR != nullptr) {
                if (lpNMHDR->hwndFrom == nullptr) {
                    return false;
                }
                lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
            }
            return true;
        } break;
        case WM_COMMAND: {
            if (lParam == 0) {
                break;
            }
            HWND hWndChild = (HWND)lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            if (lRes != 0) {
                return true;
            }
        } break;
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC: {
            // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
            // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the
            // WM_CTLCOLORSTATIC message.
            if (lParam == 0) {
                break;
            }
            HWND hWndChild = (HWND)lParam;
            lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
            return true;
        } break;
        default:
            break;
    }
    return false;
}

// 给鼠标所在的控件发送一个Event
bool CPaintManagerUI::TranspostMouseEvent(int Type, POINT ptMouse, WORD wKeyState, WPARAM wParam, LPARAM lParam)
{
    TEventUI event = { 0 };
    event.Type = Type;
    event.ptMouse = ptMouse;
    event.wKeyState = wKeyState;
    event.wParam = wParam;
    event.lParam = lParam;
    event.dwTimestamp = ::GetTickCount();

    CControlUI *old_ctl = nullptr;
    CControlUI *new_ctl = nullptr;

    // 鼠标下面的控件
    if (ptMouse.x == m_ptLastMousePos.x && ptMouse.y == m_ptLastMousePos.y) {
        old_ctl = new_ctl = FindControl(ptMouse);
    } else {
        old_ctl = FindControl(m_ptLastMousePos);
        new_ctl = FindControl(ptMouse);
    }

    // 特殊参数
    if (Type == UIEVENT_CONTEXTMENU) {
        event.lParam = (LPARAM)new_ctl;
    }

    // 特殊事件
    if (Type == UIEVENT_MOUSELEAVE) {
        if (new_ctl == old_ctl) {
            return false;
        }
        event.pSender = old_ctl;
    } else if (Type == UIEVENT_MOUSEENTER) {
        if (new_ctl == old_ctl) {
            return false;
        }
        event.pSender = new_ctl;
    } else {
        event.pSender = new_ctl;
    }

    if (event.pSender == nullptr) {
        if (controlCaptureMouse) {
            controlCaptureMouse->Event(event);
        }
        return false;
    }

    if (controlCaptureMouse) {
        controlCaptureMouse->Event(event);
    } else {
        event.pSender->Event(event);
    }
    return true;
}

bool CPaintManagerUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

bool CPaintManagerUI::IsMainThread()
{
    return std::this_thread::get_id() == uithreadID;
}

void CPaintManagerUI::CheckMainThread()
{
    if (!IsMainThread()) {
        LOG_ERROR("[DUILIB]: Cannot invoke control methods outside ui thread.");
    }
}

void CPaintManagerUI::NeedUpdate()
{
    m_bUpdateNeeded = true;
}

void CPaintManagerUI::Invalidate()
{
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    ::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcClient);
    ::InvalidateRect(m_hWndPaint, nullptr, FALSE);
}

void CPaintManagerUI::Invalidate(RECT &rcItem)
{
    if (rcItem.left < 0) {
        rcItem.left = 0;
    }
    if (rcItem.top < 0) {
        rcItem.top = 0;
    }
    if (rcItem.right < rcItem.left) {
        rcItem.right = rcItem.left;
    }
    if (rcItem.bottom < rcItem.top) {
        rcItem.bottom = rcItem.top;
    }
    ::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcItem);
    ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

bool CPaintManagerUI::AttachDialog(CControlUI *pControl)
{
    ASSERT(::IsWindow(m_hWndPaint));
    // 创建阴影窗口
    m_shadow.Create(this);

    // Reset any previous attachment
    SetFocus(nullptr);
    m_pEventKey = nullptr;

    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if (m_pRoot != nullptr) {
        m_aPostPaintControls.Empty();
        AddDelayedCleanup(m_pRoot);
    }
    // Set the dialog root element
    m_pRoot = pControl;
    // Go ahead...
    m_bUpdateNeeded = true;
    m_bFirstLayout = true;
    m_bFocusNeeded = true;
    // Initiate all control
    return InitControls(pControl);
}

bool CPaintManagerUI::InitControls(CControlUI *pControl, CControlUI *pParent /* = nullptr */)
{
    ASSERT(pControl);
    if (pControl == nullptr) {
        return false;
    }
    pControl->SetManager(this, pParent != nullptr ? pParent : pControl->GetParent(), true);
    pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
    return true;
}

void CPaintManagerUI::ReapObjects(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return;
    }
    if (pControl == m_pEventKey) {
        m_pEventKey = nullptr;
    }
    if (pControl == m_pFocus) {
        m_pFocus = nullptr;
    }
    KillTimerWhenDestory(pControl);
    const CDuiString &sName = pControl->GetName();
    if (!sName.IsEmpty()) {
        if (pControl == FindControl(sName)) {
            m_mNameHash.Remove(sName);
        }
    }
    for (int i = 0; i < m_aAsyncNotify.GetSize(); i++) {
        TNotifyUI *pMsg = static_cast<TNotifyUI *>(m_aAsyncNotify.GetAt(i));
        if (pMsg == nullptr) {
            continue;
        }
        if (pMsg->pSender == pControl) {
            pMsg->pSender = nullptr;
        }
    }
    if (m_pEventPointer == pControl) {
        m_pEventPointer = nullptr;
    }
}

bool CPaintManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI *pControl)
{
    const int iPreallocSize = 6;
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if (lp) {
        CStdPtrArray *aOptionGroup = static_cast<CStdPtrArray *>(lp);
        for (int i = 0; i < aOptionGroup->GetSize(); i++) {
            if (static_cast<CControlUI *>(aOptionGroup->GetAt(i)) == pControl) {
                return false;
            }
        }
        aOptionGroup->Add(pControl);
    } else {
        CStdPtrArray *aOptionGroup = new CStdPtrArray(iPreallocSize);
        aOptionGroup->Add(pControl);
        m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
    }
    return true;
}

CStdPtrArray *CPaintManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if (lp) {
        return static_cast<CStdPtrArray *>(lp);
    }
    return nullptr;
}

void CPaintManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI *pControl)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if (lp) {
        CStdPtrArray *aOptionGroup = static_cast<CStdPtrArray *>(lp);
        if (aOptionGroup == nullptr) {
            return;
        }
        for (int i = 0; i < aOptionGroup->GetSize(); i++) {
            if (static_cast<CControlUI *>(aOptionGroup->GetAt(i)) == pControl) {
                aOptionGroup->Remove(i);
                break;
            }
        }
        if (aOptionGroup->IsEmpty()) {
            delete aOptionGroup;
            m_mOptionGroup.Remove(pStrGroupName);
        }
    }
}

void CPaintManagerUI::RemoveAllOptionGroups()
{
    CStdPtrArray *aOptionGroup;
    for (int i = 0; i < m_mOptionGroup.GetSize(); i++) {
        if (LPCTSTR key = m_mOptionGroup.GetAt(i)) {
            aOptionGroup = static_cast<CStdPtrArray *>(m_mOptionGroup.Find(key));
            delete aOptionGroup;
        }
    }
    m_mOptionGroup.RemoveAll();
}

void CPaintManagerUI::MessageLoop()
{
    MSG msg = { 0 };
    int ret;
    while (ret = ::GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_QUIT) {
            break;
        } else if (msg.message == UIMSG_UPDATE_ON_MAIN) {
            UIMainQueue::getInstance().finish();
        }
        if (!CPaintManagerUI::TranslateMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void CPaintManagerUI::ProcessMessage(MessageType t, bool *quit)
{
    MSG msg = { 0 };
    int ret;
    UINT msgMin = 0;
    UINT msgMax = 0;

    if (quit) {
        *quit = false;
    }

    if (t == DuiLib::MessageType::Paint) {
        msgMin = WM_PAINT;
        msgMax = WM_PAINT;
    } else if (t == DuiLib::MessageType::Dispatch) {
        msgMin = UIMSG_UPDATE_ON_MAIN;
        msgMax = UIMSG_UPDATE_ON_MAIN;
    }

    while (ret = ::PeekMessage(&msg, nullptr, msgMin, msgMax, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            if (quit) {
                *quit = true;
                ::PostQuitMessage(static_cast<int>(msg.wParam));
                return;
            }
        }
        if (!CPaintManagerUI::TranslateMessage(&msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void DuiLib::CPaintManagerUI::SetDPI(int iDPI)
{
    int scale1 = m_curScale;
    CResourceManager::GetInstance()->SetScale(iDPI);
    int scale2 = static_cast<int>(CResourceManager::GetInstance()->GetScale());
    m_curScale = scale2;
    // dpi两次改变后避免重复刷新
    if (scale1 == scale2) {
        return;
    }
    ResetDPIAssets();
    RECT rcWnd = { 0 };
    RECT rc = { 0 };
    ::GetWindowRect(GetPaintWindow(), &rcWnd);
    RECT *prcNewWindow = &rcWnd;
    if (!::IsZoomed(GetPaintWindow())) {
        SIZE sizeT = GetFixOrgiSize();
        LONG originX = (rcWnd.left + rcWnd.right) / 2; // 缩放前后窗口横轴方向中心不偏移
        rc.left = originX - sizeT.cx / 2; // 缩放前后窗口横轴方向中心不偏移
        rc.top = rcWnd.top;
        rc.right = originX + sizeT.cx / 2; // 缩放前后窗口横轴方向中心不偏移
        rc.bottom = rcWnd.top + sizeT.cy;
        prcNewWindow = &rc;
    }
    SetWindowPos(GetPaintWindow(), nullptr, prcNewWindow->left, prcNewWindow->top,
        prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top,
        SWP_NOZORDER | SWP_NOACTIVATE);
    if (GetRoot() != nullptr) {
        GetRoot()->NeedUpdate();
    }
    ::PostMessage(GetPaintWindow(), WM_PAINT, 0, 0);
}

void DuiLib::CPaintManagerUI::SetAllDPI(int iDPI, bool isMainScreen)
{
    for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
        CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
        if (pManager == nullptr) {
            continue;
        }

        if (FALSE == ::IsWindow(pManager->GetPaintWindow())) {
            continue;
        }

        if (::IsIconic(pManager->GetPaintWindow())) {
            continue;
        }
        HMONITOR hMonitor = MonitorFromWindow(pManager->GetPaintWindow(), MONITOR_DEFAULTTONEAREST);
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        GetMonitorInfo(hMonitor, &oMonitor);
        // 区分主屏和扩展屏
        bool curWndMainScreen = true;
        int width = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);
        if (oMonitor.rcMonitor.right <= 0 || oMonitor.rcMonitor.bottom <= 0 || oMonitor.rcMonitor.left >= width ||
            oMonitor.rcMonitor.top >= height) {
            curWndMainScreen = false;
        }
        if (isMainScreen != curWndMainScreen) {
            // 主屏DPI改变，扩展屏窗口不动，反之亦然
            continue;
        }

        pManager->SetDPI(iDPI);
    }
}

void DuiLib::CPaintManagerUI::UpdateDPIByMonitor(HWND hWnd)
{
    if (hWnd == nullptr || (!IsWindows7OrGreater())) {
        return;
    }
    HWND parentHwnd = GetParent(hWnd);
    TCHAR className[MAX_PATH + 1] = { 0 };
    int getResult = GetClassName(hWnd, className, MAX_PATH);
    if (_tcsicmp(className, _T("DuiMenuWnd")) == 0 || getResult == 0) {
        return;
    }
    if (::IsWindow(hWnd) && !IsWindowVisible(hWnd) && parentHwnd != nullptr) {
        return;
    }

    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    GetMonitorInfo(hMonitor, &oMonitor);
    // 区分主屏和扩展屏
    bool isMainScreen = true;
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    if (oMonitor.rcMonitor.right <= 0 || oMonitor.rcMonitor.bottom <= 0 || oMonitor.rcMonitor.left >= width ||
        oMonitor.rcMonitor.top >= height) {
        isMainScreen = false;
    }
    UINT dpix = DPI_TYPE_ONE; // 设置DPI x
    UINT dpiy = DPI_TYPE_ONE; // 设置DPI y
    CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
    HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
    if (hModule != nullptr) {
        LPGetDpiForMonitor getDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
        if (getDpiForMonitor != nullptr) {
            getDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
        }
        FreeLibrary(hModule);
    }
    int scale = MulDiv(dpix, SCALE_TAYO_ONE, DPI_TYPE_ONE);
    RECT rc { 0 };
    GetWindowRect(hWnd, &rc);
    /*
     * 主窗口因加载插件导致创建慢会在左上角闪，需要Init时设置在9999不可见位置
     * 避免该场景下误判为窗口处于第二屏
     */
    const int invisibleLocat = 9999;
    if (rc.top == invisibleLocat) {
        scale = GetSystemDPIScale();
        isMainScreen = true;
    }
    SetAllDPI(dpix, isMainScreen);
}

void DuiLib::CPaintManagerUI::ResetDPIAssets()
{
    RemoveAllDrawInfos();
    RemoveAllImagesForDPIChange();

    for (int it = 0; it < m_ResInfo.m_CustomFonts.GetSize(); it++) {
        TFontInfo *pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(m_ResInfo.m_CustomFonts[it]));
        if (pFontInfo == nullptr) {
            continue;
        }
        RebuildFont(pFontInfo);
    }
    RebuildFont(&m_ResInfo.m_DefaultFontInfo);

    for (int it = 0; it < m_SharedResInfo.m_CustomFonts.GetSize(); it++) {
        auto fontCfg = m_SharedResInfo.m_CustomFonts[it];
        if (fontCfg == nullptr) {
            continue;
        }
        TFontInfo *pFontInfo =
            static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(fontCfg));
        if (pFontInfo == nullptr) {
            continue;
        }
        RebuildFont(pFontInfo);
    }
    RebuildFont(&m_SharedResInfo.m_DefaultFontInfo);
}

void DuiLib::CPaintManagerUI::RebuildFont(TFontInfo *pFontInfo)
{
    if (pFontInfo == nullptr) {
        return;
    }
    if (pFontInfo->hFont != nullptr) {
        ::DeleteObject(pFontInfo->hFont);
    }
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    if (pFontInfo->sFontName == CDuiString(L"微软雅黑")) {
        auto ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, L"Microsoft YaHei", LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
    } else {
        auto ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, pFontInfo->sFontName, LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
    }
    if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
        LOGFONT lfDef;
        if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfDef, 0) != 0) {
            auto ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, lfDef.lfFaceName, LF_FACESIZE - STR_END_SIZE);
            CHECK_RESULT_WITH_LOG(ret != S_OK)
        }
    }
    lf.lfCharSet = DEFAULT_CHARSET;
    if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
        lf.lfCharSet = SHIFTJIS_CHARSET;
    }
    lf.lfHeight = -CResourceManager::GetInstance()->Scale(pFontInfo->iSize);
    lf.lfQuality = CLEARTYPE_QUALITY;
    if (pFontInfo->bBold) {
        lf.lfWeight += FW_BOLD;
    }
    if (pFontInfo->bUnderline) {
        lf.lfUnderline = TRUE;
    }
    if (pFontInfo->bItalic) {
        lf.lfItalic = TRUE;
    }
    HFONT hFont = ::CreateFontIndirect(&lf);
    pFontInfo->hFont = hFont;
    ::ZeroMemory(&(pFontInfo->tm), sizeof(pFontInfo->tm));
    if (m_hDcPaint) {
        HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
}

CControlUI *CPaintManagerUI::GetFocus() const
{
    return m_pFocus;
}

void CPaintManagerUI::SetFocus(CControlUI *pControl)
{
    // Paint manager window has focus?
    HWND hFocusWnd = ::GetFocus();
    // Already has focus?
    if (pControl == m_pFocus) {
        return;
    }
    // Remove focus from old control
    if (m_pFocus != nullptr) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = m_pFocus;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
        m_pFocus = nullptr;
    }
    if (pControl == nullptr) {
        return;
    }
    // Set focus to new control
    if (pControl != nullptr && pControl->GetManager() == this && pControl->IsVisible() && pControl->IsEnabled()) {
        m_pFocus = pControl;
        TEventUI event = { 0 };
        event.Type = UIEVENT_SETFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        if (m_pFocus == nullptr) {
            return;
        }
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_SETFOCUS);
    }
}

void CPaintManagerUI::SetFocusNeeded(CControlUI *pControl)
{
    HWND hWnd = ::SetFocus(m_hWndPaint);
    if (hWnd == nullptr) {
        LOGGER_ERROR("[DUILIB] SetFocusNeeded return null " << GetLastError());
    }
    if (pControl == nullptr) {
        return;
    }
    if (m_pFocus != nullptr) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_KILLFOCUS;
        event.pSender = pControl;
        event.dwTimestamp = ::GetTickCount();
        m_pFocus->Event(event);
        SendNotify(m_pFocus, DUI_MSGTYPE_KILLFOCUS);
        m_pFocus = nullptr;
    }
    FINDTABINFO info = { 0 };
    info.pFocus = pControl;
    info.bForward = false;
    if (m_pRoot != nullptr) {
        m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    }
    m_bFocusNeeded = true;
    if (m_pRoot != nullptr) {
        m_pRoot->NeedUpdate();
    }
}

bool CPaintManagerUI::SetTimer(CControlUI *pControl, UINT nTimerID, UINT uElapse)
{
    if (pControl == nullptr) {
        return false;
    }
    ASSERT(uElapse > 0);
    bool isMain = CPaintManagerUI::IsMainThread();
    if (isMain == false) {
        wstring str;
        if (pControl && (pControl->GetName().IsEmpty() == false)) {
            str = GetName();
        }
        LOG_ERROR((L"[DUILIB]: SetTimer. outside ui thread , name:" + str + L" timerID:" +
            to_wstring(nTimerID)).c_str());
    }

    if (pControl->GetManager() != this) {
        wstring str;
        if (pControl && (pControl->GetName().IsEmpty() == false)) {
            str = GetName();
        }
        LOG_ERROR((L"[DUILIB]: SetTimer. manager not cur manager , name:" + str + L" timerID:"
            + to_wstring(nTimerID)).c_str());
        return false;
    }

    for (int i = 0; i < m_aTimers.GetSize(); i++) {
        TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i));
        if (pTimer == nullptr) {
            continue;
        }
        if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint && pTimer->nLocalID == nTimerID) {
            if (pTimer->bKilled == true) {
                if (::SetCoalescableTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, nullptr,
                    TIMERV_DEFAULT_COALESCING)) {
                    pTimer->bKilled = false;
                    return true;
                }
                return false;
            }
            return false;
        }
    }

    m_uTimerID = (++m_uTimerID) % 0xFFFFFFF0; // 0xf1-0xfe特殊用途
    if (!::SetCoalescableTimer(m_hWndPaint, m_uTimerID, uElapse, nullptr, TIMERV_DEFAULT_COALESCING)) {
        return FALSE;
    }
    TIMERINFO *pTimer = new TIMERINFO;
    if (pTimer == nullptr) {
        return FALSE;
    }
    pTimer->hWnd = m_hWndPaint;
    pTimer->pSender = pControl;
    pTimer->nLocalID = nTimerID;
    pTimer->uWinTimer = m_uTimerID;
    pTimer->bKilled = false;
    return m_aTimers.Add(pTimer);
}

bool CPaintManagerUI::KillTimer(CControlUI *pControl, UINT nTimerID)
{
    ASSERT(pControl != nullptr);

    bool isMain = CPaintManagerUI::IsMainThread();
    if (isMain == false) {
        wstring str;
        if (pControl && (pControl->GetName().IsEmpty() == false)) {
            str = GetName();
        }
        LOG_ERROR(
            (L"[DUILIB]: KillTimer. outside ui thread , name:" + str + L" timerID:" + to_wstring(nTimerID)).c_str());
    }

    for (int i = 0; i < m_aTimers.GetSize(); i++) {
        TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i));
        if (pTimer == nullptr) {
            continue;
        }
        if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint && pTimer->nLocalID == nTimerID) {
            if (pTimer->bKilled == false) {
                if (::IsWindow(m_hWndPaint)) {
                    ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                }
                pTimer->bKilled = true;
                return true;
            }
        }
    }
    return false;
}

void CPaintManagerUI::KillTimer(CControlUI *pControl)
{
    ASSERT(pControl != nullptr);
    int count = m_aTimers.GetSize();
    int j = 0;
    for (int i = 0; i < count; i++) {
        TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i - j));
        if (pTimer == nullptr) {
            continue;
        }
        if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint) {
            if (pTimer->bKilled == false) {
                ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
            }
            delete pTimer;
            m_aTimers.Remove(i - j);
            j++;
        }
    }
}

void CPaintManagerUI::KillTimerWhenDestory(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return;
    }
    int count = m_aTimers.GetSize();
    int j = 0;
    for (int i = 0; i < count; i++) {
        TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i - j));
        if (pTimer == nullptr) {
            continue;
        }
        if (pTimer->pSender == pControl) {
            if (pTimer->bKilled == false) {
                if (pTimer->hWnd == nullptr) {
                    return;
                }
                ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
            }
            delete pTimer;
            m_aTimers.Remove(i - j);
            j++;
        }
    }
}

void CPaintManagerUI::RemoveAllTimers()
{
    for (int i = 0; i < m_aTimers.GetSize(); i++) {
        TIMERINFO *pTimer = static_cast<TIMERINFO *>(m_aTimers.GetAt(i));
        if (pTimer == nullptr) {
            continue;
        }
        if (pTimer->hWnd == m_hWndPaint) {
            if (pTimer->bKilled == false) {
                if (::IsWindow(m_hWndPaint)) {
                    ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
                }
            }
            delete pTimer;
        }
    }

    m_aTimers.Empty();
}

void CPaintManagerUI::SetCapture()
{
    ::SetCapture(m_hWndPaint);
    m_bMouseCapture = true;
    m_bPointerCapture = true;
}

void CPaintManagerUI::ReleaseCapture()
{
    ::ReleaseCapture();
    m_bMouseCapture = false;
    m_bDragMode = false;
    m_bPointerCapture = false;
}

bool CPaintManagerUI::IsCaptured()
{
    return m_bMouseCapture;
}

bool CPaintManagerUI::IsPainting()
{
    return m_bIsPainting;
}

void CPaintManagerUI::SetPainting(bool bIsPainting)
{
    m_bIsPainting = bIsPainting;
}

bool CPaintManagerUI::SetNextTabControl(bool bForward)
{
    // If we're in the process of restructuring the layout we can delay the
    // focus calulation until the next repaint.
    if (m_bUpdateNeeded && bForward) {
        m_bFocusNeeded = true;
        ::InvalidateRect(m_hWndPaint, nullptr, FALSE);
        return true;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.pFocus = m_pFocus;
    info1.bForward = bForward;
    CControlUI *pControl =
        m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if (pControl == nullptr) {
        if (bForward) {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.pFocus = bForward ? nullptr : info1.pLast;
            info2.bForward = bForward;
            pControl =
                m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        } else {
            pControl = info1.pLast;
        }
    }
    if (pControl != nullptr) {
        SetFocus(pControl);
    }
    m_bFocusNeeded = false;
    return true;
}

bool CPaintManagerUI::AddNotifier(INotifyUI *pNotifier)
{
    ASSERT(m_aNotifiers.Find(pNotifier) < 0);
    return m_aNotifiers.Add(pNotifier);
}

bool CPaintManagerUI::RemoveNotifier(INotifyUI *pNotifier)
{
    for (int i = 0; i < m_aNotifiers.GetSize(); i++) {
        if (static_cast<INotifyUI *>(m_aNotifiers.GetAt(i)) == pNotifier) {
            return m_aNotifiers.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::AddPreMessageFilter(IMessageFilterUI *pFilter)
{
    if (!IsMainThread()) {
        LOG_ERROR("[DUILIB]Cannot AddPreMessageFilter outside uithread");
    }
    if (m_aPreMessageFilters.Find(pFilter) < 0) {
        return m_aPreMessageFilters.Add(pFilter);
    } else {
        return false;
    }
}

bool CPaintManagerUI::RemovePreMessageFilter(IMessageFilterUI *pFilter)
{
    if (!IsMainThread()) {
        LOG_ERROR("[DUILIB]Cannot RemovePreMessageFilter outside uithread");
    }
    for (int i = 0; i < m_aPreMessageFilters.GetSize(); i++) {
        if (static_cast<IMessageFilterUI *>(m_aPreMessageFilters.GetAt(i)) == pFilter) {
            return m_aPreMessageFilters.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::AddMessageFilter(IMessageFilterUI *pFilter)
{
    if (!IsMainThread())
        LOG_ERROR("[DUILIB]Cannot AddMessageFilter outside uithread");
    ASSERT(m_aMessageFilters.Find(pFilter) < 0);
    return m_aMessageFilters.Add(pFilter);
}

bool CPaintManagerUI::RemoveMessageFilter(IMessageFilterUI *pFilter)
{
    if (!IsMainThread()) {
        LOG_ERROR("[DUILIB]Cannot RemoveMessageFilter outside uithread");
    }
    for (int i = 0; i < m_aMessageFilters.GetSize(); i++) {
        if (static_cast<IMessageFilterUI *>(m_aMessageFilters.GetAt(i)) == pFilter) {
            return m_aMessageFilters.Remove(i);
        }
    }
    return false;
}

int CPaintManagerUI::GetPostPaintCount()
{
    return m_aPostPaintControls.GetSize();
}

bool CPaintManagerUI::IsPostPaint(CControlUI *pControl)
{
    return m_aPostPaintControls.Find(pControl) >= 0;
}

bool CPaintManagerUI::AddPostPaint(CControlUI *pControl)
{
    ASSERT(m_aPostPaintControls.Find(pControl) < 0);
    return m_aPostPaintControls.Add(pControl);
}

bool CPaintManagerUI::RemovePostPaint(CControlUI *pControl)
{
    for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
        if (static_cast<CControlUI *>(m_aPostPaintControls.GetAt(i)) == pControl) {
            return m_aPostPaintControls.Remove(i);
        }
    }
    return false;
}

bool CPaintManagerUI::SetPostPaintIndex(CControlUI *pControl, int iIndex)
{
    RemovePostPaint(pControl);
    return m_aPostPaintControls.InsertAt(iIndex, pControl);
}

int CPaintManagerUI::GetPaintChildWndCount()
{
    return m_aChildWnds.GetSize();
}

bool CPaintManagerUI::AddPaintChildWnd(HWND hChildWnd)
{
    RECT rcChildWnd;
    GetChildWndRect(m_hWndPaint, hChildWnd, rcChildWnd);
    Invalidate(rcChildWnd);

    if (m_aChildWnds.Find(hChildWnd) >= 0) {
        return false;
    }
    return m_aChildWnds.Add(hChildWnd);
}

bool CPaintManagerUI::RemovePaintChildWnd(HWND hChildWnd)
{
    for (int i = 0; i < m_aChildWnds.GetSize(); i++) {
        if (static_cast<HWND>(m_aChildWnds.GetAt(i)) == hChildWnd) {
            return m_aChildWnds.Remove(i);
        }
    }
    return false;
}

void CPaintManagerUI::AddDelayedCleanup(CControlUI *pControl)
{
    if (pControl == nullptr) {
        return;
    }
    pControl->SetManager(this, nullptr, false);
    m_aDelayedCleanup.Add(pControl);
    ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
}

void CPaintManagerUI::SendNotify(CControlUI *pControl, LPCTSTR pstrMessage, WPARAM wParam,
    LPARAM lParam, bool bAsync)
{
    TNotifyUI Msg;
    Msg.pSender = pControl;
    Msg.sType = pstrMessage;
    Msg.wParam = wParam;
    Msg.lParam = lParam;
    SendNotify(Msg, bAsync);
}

void CPaintManagerUI::SendNotify(TNotifyUI &Msg, bool bAsync)
{
    Msg.ptMouse = m_ptLastMousePos;
    Msg.dwTimestamp = ::GetTickCount();
    if (m_bUsedVirtualWnd) {
        if (Msg.pSender != nullptr) {
            Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
        }
    }

    if (!bAsync) {
        // Send to all listeners
        if (Msg.pSender != nullptr) {
            if (Msg.pSender->OnNotify) {
                Msg.pSender->OnNotify(&Msg);
            }
        }
        for (int i = 0; i < m_aNotifiers.GetSize(); i++) {
            INotifyUI *item1 = static_cast<INotifyUI *>(m_aNotifiers.GetAt(i));
            if (item1 == nullptr) {
                continue;
            }
            item1->Notify(Msg);
        }
    } else {
        TNotifyUI *pMsg = new TNotifyUI;
        pMsg->pSender = Msg.pSender;
        pMsg->sType = Msg.sType;
        pMsg->wParam = Msg.wParam;
        pMsg->lParam = Msg.lParam;
        pMsg->ptMouse = Msg.ptMouse;
        pMsg->dwTimestamp = Msg.dwTimestamp;
        m_aAsyncNotify.Add(pMsg);
        ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
    }
}

bool CPaintManagerUI::IsForceUseSharedRes() const
{
    return m_bForceUseSharedRes;
}

void CPaintManagerUI::SetForceUseSharedRes(bool bForce)
{
    m_bForceUseSharedRes = bForce;
}

DWORD CPaintManagerUI::GetDefaultDisabledColor() const
{
    return m_ResInfo.m_dwDefaultDisabledColor;
}

void CPaintManagerUI::SetDefaultDisabledColor(DWORD dwColor, bool bShared)
{
    if (bShared) {
        if (m_ResInfo.m_dwDefaultDisabledColor == m_SharedResInfo.m_dwDefaultDisabledColor) {
            m_ResInfo.m_dwDefaultDisabledColor = dwColor;
        }
        m_SharedResInfo.m_dwDefaultDisabledColor = dwColor;
    } else {
        m_ResInfo.m_dwDefaultDisabledColor = dwColor;
    }
}

DWORD CPaintManagerUI::GetDefaultFontColor() const
{
    return m_ResInfo.m_dwDefaultFontColor;
}

void CPaintManagerUI::SetDefaultFontColor(DWORD dwColor, bool bShared)
{
    if (bShared) {
        if (m_ResInfo.m_dwDefaultFontColor == m_SharedResInfo.m_dwDefaultFontColor)
            m_ResInfo.m_dwDefaultFontColor = dwColor;
        m_SharedResInfo.m_dwDefaultFontColor = dwColor;
    } else {
        m_ResInfo.m_dwDefaultFontColor = dwColor;
    }
}

DWORD CPaintManagerUI::GetDefaultLinkFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkFontColor;
}

void CPaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor, bool bShared)
{
    if (bShared) {
        if (m_ResInfo.m_dwDefaultLinkFontColor == m_SharedResInfo.m_dwDefaultLinkFontColor) {
            m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
        }
        m_SharedResInfo.m_dwDefaultLinkFontColor = dwColor;
    } else {
        m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
    }
}

DWORD CPaintManagerUI::GetDefaultLinkHoverFontColor() const
{
    return m_ResInfo.m_dwDefaultLinkHoverFontColor;
}

void CPaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared)
{
    if (bShared) {
        if (m_ResInfo.m_dwDefaultLinkHoverFontColor == m_SharedResInfo.m_dwDefaultLinkHoverFontColor) {
            m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
        }
        m_SharedResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
    } else {
        m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
    }
}

DWORD CPaintManagerUI::GetDefaultSelectedBkColor() const
{
    return m_ResInfo.m_dwDefaultSelectedBkColor;
}

void CPaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor, bool bShared)
{
    if (bShared) {
        if (m_ResInfo.m_dwDefaultSelectedBkColor == m_SharedResInfo.m_dwDefaultSelectedBkColor) {
            m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
        }
        m_SharedResInfo.m_dwDefaultSelectedBkColor = dwColor;
    } else {
        m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
    }
}

TFontInfo *CPaintManagerUI::GetDefaultFontInfo()
{
    if (m_ResInfo.m_DefaultFontInfo.sFontName.IsEmpty()) {
        if (m_SharedResInfo.m_DefaultFontInfo.tm.tmHeight == 0) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, m_SharedResInfo.m_DefaultFontInfo.hFont);
            ::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
        return &m_SharedResInfo.m_DefaultFontInfo;
    } else {
        if (m_ResInfo.m_DefaultFontInfo.tm.tmHeight == 0) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, m_ResInfo.m_DefaultFontInfo.hFont);
            ::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
        return &m_ResInfo.m_DefaultFontInfo;
    }
}

void CPaintManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic,
    bool bShared)
{
    if (pStrFontName == nullptr) {
        return;
    }
    if (wstring(pStrFontName) == L"微软雅黑") {
        pStrFontName = L"Microsoft YaHei";
    }
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    if (pStrFontName != nullptr && lstrlen(pStrFontName) > 0) {
        TCHAR szFaceName[CHAR_LENGTH_TYPE_TWO] = {0};
        auto ret = _tcsncat_s(szFaceName, LF_FACESIZE, pStrFontName, LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
        ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, szFaceName, LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            LOGFONT lfDef;
            if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfDef, 0) != 0) {
                ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, lfDef.lfFaceName, LF_FACESIZE - STR_END_SIZE);
                CHECK_RESULT_WITH_LOG(ret != S_OK)
            }
        }
    }
    lf.lfCharSet = DEFAULT_CHARSET;
    if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
        lf.lfCharSet = SHIFTJIS_CHARSET;
    }
    lf.lfHeight = -nSize;
    if (bBold) {
        lf.lfWeight += FW_BOLD;
    }
    if (bUnderline) {
        lf.lfUnderline = TRUE;
    }
    if (bItalic) {
        lf.lfItalic = TRUE;
    }

    HFONT hFont = ::CreateFontIndirect(&lf);
    if (hFont == nullptr) {
        return;
    }

    if (bShared) {
        ::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
        m_SharedResInfo.m_DefaultFontInfo.hFont = hFont;
        m_SharedResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
        m_SharedResInfo.m_DefaultFontInfo.iSize = CResourceManager::GetInstance()->Scale(nSize);
        m_SharedResInfo.m_DefaultFontInfo.bBold = bBold;
        m_SharedResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
        m_SharedResInfo.m_DefaultFontInfo.bItalic = bItalic;
        ::ZeroMemory(&m_SharedResInfo.m_DefaultFontInfo.tm, sizeof(m_SharedResInfo.m_DefaultFontInfo.tm));
        if (m_hDcPaint) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
            ::GetTextMetrics(m_hDcPaint, &m_SharedResInfo.m_DefaultFontInfo.tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
    } else {
        ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
        m_ResInfo.m_DefaultFontInfo.hFont = hFont;
        m_ResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
        m_ResInfo.m_DefaultFontInfo.iSize = CResourceManager::GetInstance()->Scale(nSize);
        m_ResInfo.m_DefaultFontInfo.bBold = bBold;
        m_ResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
        m_ResInfo.m_DefaultFontInfo.bItalic = bItalic;
        ::ZeroMemory(&m_ResInfo.m_DefaultFontInfo.tm, sizeof(m_ResInfo.m_DefaultFontInfo.tm));
        if (m_hDcPaint) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
            ::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
    }
}

DWORD CPaintManagerUI::GetCustomFontCount(bool bShared) const
{
    if (bShared) {
        return (DWORD)(m_SharedResInfo.m_CustomFonts.GetSize());
    } else {
        return (DWORD)(m_ResInfo.m_CustomFonts.GetSize());
    }
}

int CPaintManagerUI::AddPtFont(FontType ft, int nSize, bool bBold, bool bUnderline)
{
    int fontIDBase = bBold ? FONT_ID_TEXT_BOLD_SIZE_ : FONT_ID_TEXT_SIZE_;
    fontIDBase = bUnderline ? FONT_ID_TEXT_UNDERLINE_SIZE_ : fontIDBase;
    CDuiString fontName;
    switch (ft) {
        case DuiLib::FontType::Text:
            fontIDBase = bBold ? FONT_ID_TEXT_BOLD_SIZE_ : FONT_ID_TEXT_SIZE_;
            fontIDBase = bUnderline ? FONT_ID_TEXT_UNDERLINE_SIZE_ : fontIDBase;
            fontName = UISTR_UI_DEFAULT_FONTNAME;
            break;
        case DuiLib::FontType::Value:
            fontIDBase = bBold ? FONT_ID_VALUE_BOLD_SIZE_ : FONT_ID_VALUE_SIZE_;
            fontIDBase = bUnderline ? FONT_ID_VALUE_UNDERLINE_SIZE_ : fontIDBase;
            fontName = UISTR_UI_VALUE_FONTNAME;
            break;
        default:
            fontIDBase = bBold ? FONT_ID_TEXT_BOLD_SIZE_ : FONT_ID_TEXT_SIZE_;
            fontIDBase = bUnderline ? FONT_ID_TEXT_UNDERLINE_SIZE_ : fontIDBase;
            break;
    }

    int ptFontID = fontIDBase + nSize;
    HFONT existedFont = GetFont(ptFontID);
    HFONT defaultFont = nullptr;
    if (TFontInfo *fi = GetDefaultFontInfo()) {
        defaultFont = fi->hFont;
    }

    if (existedFont == defaultFont || existedFont == nullptr) {
        AddFont(ptFontID, fontName.GetData(), nSize, bBold, bUnderline, false, true);
    }

    return ptFontID;
}

HFONT CPaintManagerUI::AddFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic,
    bool bShared, bool usePx)
{
    if (wstring(pStrFontName) == L"微软雅黑") {
        pStrFontName = L"Microsoft YaHei";
    }
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    if (pStrFontName != nullptr && lstrlen(pStrFontName) > 0) {
        TCHAR szFaceName[CHAR_LENGTH_TYPE_TWO] = { 0 };
        auto ret = _tcsncat_s(szFaceName, LF_FACESIZE, pStrFontName, LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
        ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, szFaceName, LF_FACESIZE - STR_END_SIZE);
        CHECK_RESULT_WITH_LOG(ret != S_OK)
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            LOGFONT lfDef;
            if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfDef, 0) != 0) {
                ret = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, lfDef.lfFaceName, LF_FACESIZE - STR_END_SIZE);
                CHECK_RESULT_WITH_LOG(ret != S_OK)
            }
        }
    }
    lf.lfCharSet = DEFAULT_CHARSET;
    if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
        lf.lfCharSet = SHIFTJIS_CHARSET;
    }
    int heightMul = usePx ? 1 : -1;
    if (abs(CResourceManager::GetInstance()->Scale(nSize)) > (INT_MAX / abs(heightMul))) {
        lf.lfHeight = FW_NORMAL;
    } else {
        lf.lfHeight = CResourceManager::GetInstance()->Scale(nSize) * heightMul;
    }

    if (bBold) {
        lf.lfWeight = FW_BOLD;
    }
    if (bUnderline) {
        lf.lfUnderline = TRUE;
    }
    if (bItalic) {
        lf.lfItalic = TRUE;
    }
    HFONT hFont = ::CreateFontIndirect(&lf);
    if (hFont == nullptr) {
        return nullptr;
    }

    TFontInfo *pFontInfo = new TFontInfo;
    if (!pFontInfo) {
        return nullptr;
    }
    ::ZeroMemory(pFontInfo, sizeof(TFontInfo));
    pFontInfo->hFont = hFont;
    pFontInfo->sFontName = lf.lfFaceName;
    pFontInfo->iSize = nSize;
    pFontInfo->bBold = bBold;
    pFontInfo->bUnderline = bUnderline;
    pFontInfo->bItalic = bItalic;
    if (m_hDcPaint) {
        HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    TCHAR idBuffer[CHAR_LENGTH_TYPE_ONE];
    ::ZeroMemory(idBuffer, sizeof(idBuffer));
    _itot(id, idBuffer, RADIX_TYPE_ONE_VALUE);
    if (bShared || m_bForceUseSharedRes) {
        TFontInfo *pOldFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
        if (pOldFontInfo) {
            if ((pOldFontInfo->hFont) != nullptr) {
                ::DeleteObject(pOldFontInfo->hFont);
            }
            delete pOldFontInfo;
            m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
        }

        if (!m_SharedResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo)) {
            ::DeleteObject(hFont);
            delete pFontInfo;
            return nullptr;
        }
    } else {
        TFontInfo *pOldFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(idBuffer));
        if (pOldFontInfo) {
            if ((pOldFontInfo->hFont) != nullptr) {
                ::DeleteObject(pOldFontInfo->hFont);
            }
            delete pOldFontInfo;
            m_ResInfo.m_CustomFonts.Remove(idBuffer);
        }

        if (!m_ResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo)) {
            ::DeleteObject(hFont);
            delete pFontInfo;
            return nullptr;
        }
    }

    return hFont;
}

HFONT CPaintManagerUI::GetFont(int id)
{
    if (id < 0) {
        return GetDefaultFontInfo()->hFont;
    }

    TCHAR idBuffer[CHAR_LENGTH_TYPE_ONE];
    ::ZeroMemory(idBuffer, sizeof(idBuffer));
    _itot(id, idBuffer, RADIX_TYPE_ONE_VALUE);
    TFontInfo *pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(idBuffer));
    if (!pFontInfo) {
        pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
    }
    if (!pFontInfo) {
        return GetDefaultFontInfo()->hFont;
    }
    return pFontInfo->hFont;
}

HFONT CPaintManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    TFontInfo *pFontInfo = nullptr;
    for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
        if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
            pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key));
            if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) {
                return pFontInfo->hFont;
            }
        }
    }
    for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
        if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
            pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key));
            if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic) {
                return pFontInfo->hFont;
            }
        }
    }

    return nullptr;
}

int CPaintManagerUI::GetFontIndex(HFONT hFont, bool bShared)
{
    TFontInfo *pFontInfo = nullptr;
    if (bShared) {
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    return _ttoi(key);
                }
            }
        }
    } else {
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    return _ttoi(key);
                }
            }
        }
    }

    return -1;
}

int CPaintManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic,
    bool bShared)
{
    TFontInfo *pFontInfo = nullptr;
    if (bShared) {
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize
                    == nSize && pFontInfo->bBold == bBold && pFontInfo->bUnderline
                    == bUnderline && pFontInfo->bItalic == bItalic) {
                    return _ttoi(key);
                }
            }
        }
    } else {
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize
                    == nSize && pFontInfo->bBold == bBold && pFontInfo->bUnderline
                    == bUnderline && pFontInfo->bItalic == bItalic) {
                    return _ttoi(key);
                }
            }
        }
    }

    return -1;
}

void CPaintManagerUI::RemoveFont(HFONT hFont, bool bShared)
{
    TFontInfo *pFontInfo = nullptr;
    if (bShared) {
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    ::DeleteObject(pFontInfo->hFont);
                    delete pFontInfo;
                    m_SharedResInfo.m_CustomFonts.Remove(key);
                    return;
                }
            }
        }
    } else {
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    WHETHER_RETURN_DEFUALT(pFontInfo->hFont == nullptr);
                    ::DeleteObject(pFontInfo->hFont);
                    delete pFontInfo;
                    m_ResInfo.m_CustomFonts.Remove(key);
                    return;
                }
            }
        }
    }
}

void CPaintManagerUI::RemoveFont(int id, bool bShared)
{
    TCHAR idBuffer[CHAR_LENGTH_TYPE_ONE];
    ::ZeroMemory(idBuffer, sizeof(idBuffer));
    _itot(id, idBuffer, RADIX_TYPE_ONE_VALUE);

    TFontInfo *pFontInfo = nullptr;
    if (bShared) {
        pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
        if (pFontInfo) {
            WHETHER_RETURN_DEFUALT(pFontInfo->hFont == nullptr);
            ::DeleteObject(pFontInfo->hFont);
            delete pFontInfo;
            m_SharedResInfo.m_CustomFonts.Remove(idBuffer);
        }
    } else {
        pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(idBuffer));
        if (pFontInfo) {
            WHETHER_RETURN_DEFUALT(pFontInfo->hFont == nullptr);
            ::DeleteObject(pFontInfo->hFont);
            delete pFontInfo;
            m_ResInfo.m_CustomFonts.Remove(idBuffer);
        }
    }
}

void CPaintManagerUI::RemoveAllFonts(bool bShared)
{
    TFontInfo *pFontInfo;
    if (bShared) {
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key, false));
                if (pFontInfo) {
                    WHETHER_RETURN_DEFUALT(pFontInfo->hFont == nullptr);
                    ::DeleteObject(pFontInfo->hFont);
                    delete pFontInfo;
                }
            }
        }
        m_SharedResInfo.m_CustomFonts.RemoveAll();
    } else {
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key, false));
                if (pFontInfo) {
                    WHETHER_RETURN_DEFUALT(pFontInfo->hFont == nullptr);
                    ::DeleteObject(pFontInfo->hFont);
                    delete pFontInfo;
                }
            }
        }
        m_ResInfo.m_CustomFonts.RemoveAll();
    }
}

TFontInfo *CPaintManagerUI::GetFontInfo(int id)
{
    if (id < 0) {
        return GetDefaultFontInfo();
    }

    TCHAR idBuffer[CHAR_LENGTH_TYPE_ONE];
    ::ZeroMemory(idBuffer, sizeof(idBuffer));
    _itot(id, idBuffer, RADIX_TYPE_ONE_VALUE);
    TFontInfo *pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(idBuffer));
    if (!pFontInfo) {
        pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(idBuffer));
    }
    if (!pFontInfo) {
        pFontInfo = GetDefaultFontInfo();
    }
    if (pFontInfo->tm.tmHeight == 0) {
        HFONT hOldFont = nullptr;
        if (pFontInfo->hFont != nullptr) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, pFontInfo->hFont);
        }
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        if (hOldFont != nullptr) {
            ::SelectObject(m_hDcPaint, hOldFont);
        }
    }
    return pFontInfo;
}

TFontInfo *CPaintManagerUI::GetFontInfo(HFONT hFont)
{
    TFontInfo *pFontInfo = nullptr;
    for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
        if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
            pFontInfo = static_cast<TFontInfo *>(m_ResInfo.m_CustomFonts.Find(key));
            if (pFontInfo && pFontInfo->hFont == hFont) {
                break;
            }
        }
    }
    if (!pFontInfo) {
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFontInfo *>(m_SharedResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    break;
                }
            }
        }
    }
    if (!pFontInfo) {
        pFontInfo = GetDefaultFontInfo();
    }
    if (pFontInfo->tm.tmHeight == 0) {
        HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, pFontInfo->hFont);
        ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
        ::SelectObject(m_hDcPaint, hOldFont);
    }
    return pFontInfo;
}

const TImageInfo *CPaintManagerUI::GetImage(LPCTSTR bitmap)
{
    TImageInfo *data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
    if (!data) {
        data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
    }
    return data;
}

const TImageInfo *CPaintManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL,
    HINSTANCE instance, bool bRev)
{
    const TImageInfo *data = GetImage(bitmap);
    if (!data) {
        if (AddImage(bitmap, type, mask, bUseHSL, false, instance, bRev)) {
            if (m_bForceUseSharedRes) {
                data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
            } else {
                data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
            }
        }
    }

    return data;
}

const TImageInfo *CPaintManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask, bool bUseHSL, bool bShared,
    HINSTANCE instance, bool bRev, bool absPath)
{
    if (bitmap == nullptr || bitmap[0] == _T('\0')) {
        return nullptr;
    }

    TImageInfo *data = nullptr;
    if (type != nullptr) {
        if (isdigit(*bitmap)) {
            LPTSTR pstr = nullptr;
            int iIndex = _tcstol(bitmap, &pstr, RADIX_TYPE_ONE_VALUE);
            data = CRenderEngine::LoadImage(iIndex, type, mask, instance, bRev);
        }
    } else {
        data = CRenderEngine::LoadImage(bitmap, nullptr, mask, instance, bRev, absPath);
    }

    if (data == nullptr) {
        return nullptr;
    }

    if (absPath) {
        data->nAddType = 1;
    }
    data->bUseHSL = bUseHSL;
    if (type != nullptr) {
        data->sResType = type;
    }
    data->dwMask = mask;
    if (data->bUseHSL) {
        data->pBits = new BYTE[data->nX * data->nY * AREA_EXPAND_FOUR_TIME];
    }
    if (m_bUseHSL) {
        CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);
    }
    if (data) {
        if (bShared || m_bForceUseSharedRes) {
            TImageInfo *pOldImageInfo = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo) {
                CRenderEngine::FreeImage(pOldImageInfo);
                m_SharedResInfo.m_ImageHash.Remove(bitmap);
            }

            if (!m_SharedResInfo.m_ImageHash.Insert(bitmap, data)) {
                CRenderEngine::FreeImage(data);
                data = nullptr;
            }
        } else {
            TImageInfo *pOldImageInfo = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
            if (pOldImageInfo) {
                CRenderEngine::FreeImage(pOldImageInfo);
                m_ResInfo.m_ImageHash.Remove(bitmap);
            }

            if (!m_ResInfo.m_ImageHash.Insert(bitmap, data)) {
                CRenderEngine::FreeImage(data);
                data = nullptr;
            }
        }
    }

    return data;
}

const TImageInfo *CPaintManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha,
    bool bShared)
{
    // 因无法确定外部HBITMAP格式，不能使用hsl调整
    if (bitmap == nullptr || bitmap[0] == _T('\0')) {
        return nullptr;
    }
    if (hBitmap == nullptr || iWidth <= 0 || iHeight <= 0) {
        return nullptr;
    }

    TImageInfo *data = new TImageInfo;
    data->pBits = nullptr;
    data->pSrcBits = nullptr;
    data->hBitmap = hBitmap;
    data->pBits = nullptr;
    data->nX = iWidth;
    data->nY = iHeight;
    data->bAlpha = bAlpha;
    data->bUseHSL = false;
    data->pSrcBits = nullptr;
    data->dwMask = 0;

    if (bShared || m_bForceUseSharedRes) {
        if (!m_SharedResInfo.m_ImageHash.Insert(bitmap, data)) {
            CRenderEngine::FreeImage(data);
            data = nullptr;
        }
    } else {
        if (!m_ResInfo.m_ImageHash.Insert(bitmap, data)) {
            CRenderEngine::FreeImage(data);
            data = nullptr;
        }
    }

    return data;
}

void CPaintManagerUI::RemoveImage(LPCTSTR bitmap, bool bShared)
{
    TImageInfo *data = nullptr;
    if (bShared) {
        data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
        if (data) {
            CRenderEngine::FreeImage(data);
            m_SharedResInfo.m_ImageHash.Remove(bitmap);
        }
    } else {
        data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
        if (data) {
            CRenderEngine::FreeImage(data);
            m_ResInfo.m_ImageHash.Remove(bitmap);
        }
    }
}

void CPaintManagerUI::ReplaceImage(LPCTSTR bitmap, TImageInfo *newimage, bool bShared)
{
    TImageInfo *data = nullptr;
    if (bShared) {
        data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
        if (data) {
            CRenderEngine::FreeImage(data);
            m_SharedResInfo.m_ImageHash.Remove(bitmap);
        }
        m_SharedResInfo.m_ImageHash.Insert(bitmap, newimage);
    } else {
        data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
        if (data) {
            CRenderEngine::FreeImage(data);
            m_ResInfo.m_ImageHash.Remove(bitmap);
        }
        m_ResInfo.m_ImageHash.Insert(bitmap, newimage);
    }
}

void CPaintManagerUI::RemoveAllImages(bool bShared)
{
    if (bShared) {
        TImageInfo *data;
        for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(key, false));
                if (data) {
                    CRenderEngine::FreeImage(data);
                }
            }
        }
        m_SharedResInfo.m_ImageHash.RemoveAll();
    } else {
        TImageInfo *data;
        for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(key, false));
                if (data) {
                    CRenderEngine::FreeImage(data);
                }
            }
        }
        m_ResInfo.m_ImageHash.RemoveAll();
    }
}

void CPaintManagerUI::AdjustSharedImagesHSL()
{
    TImageInfo *data;
    for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); i++) {
        if (LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
            data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(key));
            if (data && data->bUseHSL) {
                CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
            }
        }
    }
}

void CPaintManagerUI::AdjustImagesHSL()
{
    TImageInfo *data;
    for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
        if (LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
            data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(key));
            if (data && data->bUseHSL) {
                CRenderEngine::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
            }
        }
    }
    Invalidate();
}

void CPaintManagerUI::ReloadSharedImages()
{
    TImageInfo *data = nullptr;
    TImageInfo *pNewData = nullptr;
    for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); i++) {
        if (LPCTSTR bitmap = m_SharedResInfo.m_ImageHash.GetAt(i)) {
            data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(bitmap));
            if (data != nullptr) {
                if (!data->sResType.IsEmpty()) {
                    if (isdigit(*bitmap)) {
                        LPTSTR pstr = nullptr;
                        int iIndex = _tcstol(bitmap, &pstr, RADIX_TYPE_ONE_VALUE);
                        pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
                    }
                } else {
                    pNewData = CRenderEngine::LoadImage(bitmap, nullptr, data->dwMask);
                }
                if (pNewData == nullptr)
                    continue;

                CRenderEngine::FreeImage(data, false);
                data->hBitmap = pNewData->hBitmap;
                data->pBits = pNewData->pBits;
                data->nX = pNewData->nX;
                data->nY = pNewData->nY;
                data->bAlpha = pNewData->bAlpha;
                data->pSrcBits = nullptr;
                if (data->bUseHSL) {
                    data->pSrcBits = new BYTE[data->nX * data->nY * AREA_EXPAND_FOUR_TIME];
                    ::memcpy_s(data->pSrcBits, data->nX * data->nY * AREA_EXPAND_FOUR_TIME,
                        data->pBits, data->nX * data->nY * AREA_EXPAND_FOUR_TIME);
                } else {
                    data->pSrcBits = nullptr;
                }
                if (m_bUseHSL) {
                    CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);
                }

                delete pNewData;
                pNewData = nullptr;
            }
        }
    }
}

void CPaintManagerUI::ReloadImages()
{
    RemoveAllDrawInfos();

    TImageInfo *data = nullptr;
    TImageInfo *pNewData = nullptr;
    for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
        if (LPCTSTR bitmap = m_ResInfo.m_ImageHash.GetAt(i)) {
            data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(bitmap));
            if (data != nullptr) {
                if (!data->sResType.IsEmpty()) {
                    if (isdigit(*bitmap)) {
                        LPTSTR pstr = nullptr;
                        int iIndex = _tcstol(bitmap, &pstr, RADIX_TYPE_ONE_VALUE);
                        pNewData = CRenderEngine::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
                    }
                } else {
                    pNewData = CRenderEngine::LoadImage(bitmap, nullptr, data->dwMask);
                }

                CRenderEngine::FreeImage(data, false);
                if (pNewData == nullptr) {
                    m_ResInfo.m_ImageHash.Remove(bitmap);
                    continue;
                }
                data->hBitmap = pNewData->hBitmap;
                data->pBits = pNewData->pBits;
                data->nX = pNewData->nX;
                data->nY = pNewData->nY;
                data->bAlpha = pNewData->bAlpha;
                data->pSrcBits = nullptr;
                if (data->bUseHSL) {
                    data->pSrcBits = new BYTE[data->nX * data->nY * AREA_EXPAND_FOUR_TIME];
                    ::memcpy_s(data->pSrcBits, data->nX * data->nY * AREA_EXPAND_FOUR_TIME,
                        data->pBits, data->nX * data->nY * AREA_EXPAND_FOUR_TIME);
                } else {
                    data->pSrcBits = nullptr;
                }
                if (m_bUseHSL) {
                    CRenderEngine::AdjustImage(true, data, m_H, m_S, m_L);
                }

                delete pNewData;
                pNewData = nullptr;
            }
        }
    }

    if (m_pRoot)
        m_pRoot->Invalidate();
}

const TDrawInfo *CPaintManagerUI::GetDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    CDuiString sStrImage = pStrImage;
    CDuiString sStrModify = pStrModify;
    CDuiString sKey = sStrImage + sStrModify;
    TDrawInfo *pDrawInfo = static_cast<TDrawInfo *>(m_ResInfo.m_DrawInfoHash.Find(sKey));
    if (pDrawInfo == nullptr && !sKey.IsEmpty()) {
        pDrawInfo = new TDrawInfo();
        pDrawInfo->Parse(pStrImage, pStrModify);
        m_ResInfo.m_DrawInfoHash.Insert(sKey, pDrawInfo);
    }
    return pDrawInfo;
}

void CPaintManagerUI::RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    CDuiString sStrImage = pStrImage;
    CDuiString sStrModify = pStrModify;
    CDuiString sKey = sStrImage + sStrModify;
    TDrawInfo *pDrawInfo = static_cast<TDrawInfo *>(m_ResInfo.m_DrawInfoHash.Find(sKey));
    if (pDrawInfo != nullptr) {
        m_ResInfo.m_DrawInfoHash.Remove(sKey);
        delete pDrawInfo;
        pDrawInfo = nullptr;
    }
}

void CPaintManagerUI::RemoveAllDrawInfos()
{
    TDrawInfo *pDrawInfo = nullptr;
    for (int i = 0; i < m_ResInfo.m_DrawInfoHash.GetSize(); i++) {
        LPCTSTR key = m_ResInfo.m_DrawInfoHash.GetAt(i);
        if (key != nullptr) {
            pDrawInfo = static_cast<TDrawInfo *>(m_ResInfo.m_DrawInfoHash.Find(key, false));
            if (pDrawInfo) {
                delete pDrawInfo;
                pDrawInfo = nullptr;
            }
        }
    }
    m_ResInfo.m_DrawInfoHash.RemoveAll();
}

void CPaintManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, bool bShared)
{
    if (bShared || m_bForceUseSharedRes) {
        CDuiString *pDefaultAttr = new CDuiString(pStrControlAttrList);
        if (pDefaultAttr != nullptr) {
            CDuiString *pOldDefaultAttr =
                static_cast<CDuiString *>(m_SharedResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
            if (pOldDefaultAttr)
                delete pOldDefaultAttr;
        }
    } else {
        CDuiString *pDefaultAttr = new CDuiString(pStrControlAttrList);
        if (pDefaultAttr != nullptr) {
            CDuiString *pOldDefaultAttr =
                static_cast<CDuiString *>(m_ResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
            if (pOldDefaultAttr) {
                delete pOldDefaultAttr;
            }
        }
    }
}

LPCTSTR CPaintManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
{
    CDuiString *pDefaultAttr = static_cast<CDuiString *>(m_ResInfo.m_AttrHash.Find(pStrControlName));
    if (!pDefaultAttr) {
        pDefaultAttr = static_cast<CDuiString *>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
    }
    if (pDefaultAttr) {
        return pDefaultAttr->GetData();
    }
    return nullptr;
}

bool CPaintManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared)
{
    if (bShared) {
        CDuiString *pDefaultAttr = static_cast<CDuiString *>(m_SharedResInfo.m_AttrHash.Find(pStrControlName));
        if (!pDefaultAttr) {
            return false;
        }

        delete pDefaultAttr;
        return m_SharedResInfo.m_AttrHash.Remove(pStrControlName);
    } else {
        CDuiString *pDefaultAttr = static_cast<CDuiString *>(m_ResInfo.m_AttrHash.Find(pStrControlName));
        if (!pDefaultAttr) {
            return false;
        }

        delete pDefaultAttr;
        return m_ResInfo.m_AttrHash.Remove(pStrControlName);
    }
}

void CPaintManagerUI::RemoveAllDefaultAttributeList(bool bShared)
{
    if (bShared) {
        CDuiString *pDefaultAttr;
        for (int i = 0; i < m_SharedResInfo.m_AttrHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_AttrHash.GetAt(i)) {
                pDefaultAttr = static_cast<CDuiString *>(m_SharedResInfo.m_AttrHash.Find(key));
                if (pDefaultAttr) {
                    delete pDefaultAttr;
                }
            }
        }
        m_SharedResInfo.m_AttrHash.RemoveAll();
    } else {
        CDuiString *pDefaultAttr;
        for (int i = 0; i < m_ResInfo.m_AttrHash.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_AttrHash.GetAt(i)) {
                pDefaultAttr = static_cast<CDuiString *>(m_ResInfo.m_AttrHash.Find(key));
                if (pDefaultAttr) {
                    delete pDefaultAttr;
                }
            }
        }
        m_ResInfo.m_AttrHash.RemoveAll();
    }
}

void CPaintManagerUI::AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
{
    if (pstrName == nullptr || pstrName[0] == _T('\0') || pstrAttr == nullptr || pstrAttr[0] == _T('\0')) {
        return;
    }
    CDuiString *pCostomAttr = new CDuiString(pstrAttr);
    if (pCostomAttr != nullptr) {
        if (m_mWindowCustomAttrHash.Find(pstrName) == nullptr) {
            m_mWindowCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
        } else {
            delete pCostomAttr;
        }
    }
}

LPCTSTR CPaintManagerUI::GetWindowCustomAttribute(LPCTSTR pstrName) const
{
    if (pstrName == nullptr || pstrName[0] == _T('\0')) {
        return nullptr;
    }
    CDuiString *pCostomAttr = static_cast<CDuiString *>(m_mWindowCustomAttrHash.Find(pstrName));
    if (pCostomAttr) {
        return pCostomAttr->GetData();
    }
    return nullptr;
}

bool CPaintManagerUI::RemoveWindowCustomAttribute(LPCTSTR pstrName)
{
    if (pstrName == nullptr || pstrName[0] == _T('\0')) {
        return nullptr;
    }
    CDuiString *pCostomAttr = static_cast<CDuiString *>(m_mWindowCustomAttrHash.Find(pstrName));
    if (!pCostomAttr) {
        return false;
    }

    delete pCostomAttr;
    return m_mWindowCustomAttrHash.Remove(pstrName);
}

void CPaintManagerUI::RemoveAllWindowCustomAttribute()
{
    CDuiString *pCostomAttr;
    for (int i = 0; i < m_mWindowCustomAttrHash.GetSize(); i++) {
        if (LPCTSTR key = m_mWindowCustomAttrHash.GetAt(i)) {
            pCostomAttr = static_cast<CDuiString *>(m_mWindowCustomAttrHash.Find(key));
            delete pCostomAttr;
        }
    }
    m_mWindowCustomAttrHash.Resize();
}

CControlUI *CPaintManagerUI::GetRoot() const
{
    ASSERT(m_pRoot);
    return m_pRoot;
}

CControlUI *CPaintManagerUI::FindControl(POINT pt) const
{
    ASSERT(m_pRoot);
    return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI *CPaintManagerUI::FindControl(LPCTSTR pstrName) const
{
    ASSERT(m_pRoot);
    return static_cast<CControlUI *>(m_mNameHash.Find(pstrName));
}

CControlUI *CPaintManagerUI::FindSubControlByPoint(CControlUI *pParent, POINT pt) const
{
    if (pParent == nullptr) {
        pParent = GetRoot();
    }
    ASSERT(pParent);
    if (pParent == nullptr) {
        return nullptr;
    }
    return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
}

CControlUI *CPaintManagerUI::FindSubControlByName(CControlUI *pParent, LPCTSTR pstrName) const
{
    if (pParent == nullptr) {
        pParent = GetRoot();
    }
    ASSERT(pParent);
    if (pParent == nullptr) {
        return nullptr;
    }
    return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
}

CControlUI *CPaintManagerUI::FindSubControlByClass(CControlUI *pParent, LPCTSTR pstrClass, int iIndex)
{
    if (pParent == nullptr) {
        pParent = GetRoot();
    }
    if (pParent == nullptr) {
        return nullptr;
    }
    m_aFoundControls.Resize(iIndex + 1);
    return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
}

CStdPtrArray *CPaintManagerUI::FindSubControlsByClass(CControlUI *pParent, LPCTSTR pstrClass)
{
    if (pParent == nullptr) {
        pParent = GetRoot();
    }
    if (pParent == nullptr) {
        return nullptr;
    }
    m_aFoundControls.Empty();
    pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
    return &m_aFoundControls;
}

void CPaintManagerUI::CaptureMouse(DuiLib::CControlUI *ctrl, bool capture)
{
    if (!ctrl) {
        return;
    }
    if (capture) {
        controlCaptureMouse = ctrl;
    } else if (controlCaptureMouse == ctrl) {
        controlCaptureMouse = nullptr;
    }
}
CStdPtrArray *CPaintManagerUI::GetFoundControls()
{
    return &m_aFoundControls;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromNameHash(CControlUI *pThis, LPVOID pData)
{
    CPaintManagerUI *pManager = static_cast<CPaintManagerUI *>(pData);
    const CDuiString &sName = pThis->GetName();
    if (sName.IsEmpty())
        return nullptr;
    // Add this control to the hash list
    pManager->m_mNameHash.Set(sName, pThis);
    return nullptr; // Attempt to add all controls
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromCount(CControlUI * /* pThis */, LPVOID pData)
{
    int *pnCount = static_cast<int *>(pData);
    (*pnCount)++;
    return nullptr; // Count all controls
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromPoint(CControlUI *pThis, LPVOID pData)
{
    LPPOINT pPoint = static_cast<LPPOINT>(pData);
    return ::PtInRect(&pThis->GetPos(), *pPoint) ? pThis : nullptr;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromTab(CControlUI *pThis, LPVOID pData)
{
    FINDTABINFO *pInfo = static_cast<FINDTABINFO *>(pData);
    if (pInfo->pFocus == pThis) {
        if (pInfo->bForward) {
            pInfo->bNextIsIt = true;
        }
        return pInfo->bForward ? nullptr : pInfo->pLast;
    }
    if ((pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0) {
        return nullptr;
    }
    pInfo->pLast = pThis;
    if (pInfo->bNextIsIt) {
        return pThis;
    }
    if (pInfo->pFocus == nullptr) {
        return pThis;
    }
    return nullptr; // Examine all controls
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromShortcut(CControlUI *pThis, LPVOID pData)
{
    if (!pThis->IsVisible()) {
        return nullptr;
    }
    FINDSHORTCUT *pFS = static_cast<FINDSHORTCUT *>(pData);
    if (pFS->ch == toupper(pThis->GetShortcut())) {
        pFS->bPickNext = true;
    }
    if (pThis->GetClass() == nullptr) {
        return nullptr;
    }
    if (_tcsstr(pThis->GetClass(), _T("LabelUI")) != nullptr){
        return nullptr; // Labels never get focus!
    }
    return pFS->bPickNext ? pThis : nullptr;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromName(CControlUI *pThis, LPVOID pData)
{
    LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
    const CDuiString &sName = pThis->GetName();
    if (sName.IsEmpty()) {
        return nullptr;
    }
    return (_tcsicmp(sName, pstrName) == 0) ? pThis : nullptr;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlFromClass(CControlUI *pThis, LPVOID pData)
{
    if (pThis == nullptr) {
        return nullptr;
    }
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    CStdPtrArray *pFoundControls = pThis->GetManager()->GetFoundControls();
    if (pstrType == nullptr || pType == nullptr || pFoundControls == nullptr) {
        return nullptr;
    }
    if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0) {
        int iIndex = -1;
        while (pFoundControls->GetAt(++iIndex) != nullptr)
            ;
        if (iIndex < pFoundControls->GetSize()) {
            pFoundControls->SetAt(iIndex, pThis);
        }
    }
    if (pFoundControls->GetAt(pFoundControls->GetSize() - 1) != nullptr) {
        return pThis;
    }
    return nullptr;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlsFromClass(CControlUI *pThis, LPVOID pData)
{
    if (pThis == nullptr || pData == nullptr) {
        return nullptr;
    }
    LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
    LPCTSTR pType = pThis->GetClass();
    if (pstrType == nullptr || pType == nullptr) {
        return nullptr;
    }
    if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0) {
        pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
    }
    return nullptr;
}

CControlUI *CALLBACK CPaintManagerUI::__FindControlsFromUpdate(CControlUI *pThis, LPVOID pData)
{
    if (pThis->IsUpdateNeeded()) {
        pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
        return pThis;
    }
    return nullptr;
}

bool CPaintManagerUI::TranslateAccelerator(LPMSG pMsg)
{
    for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++) {
        ITranslateAccelerator *item1 = static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator.GetAt(i));
        if (item1 == nullptr) {
            continue;
        }
        LRESULT lResult = item1->TranslateAccelerator(pMsg);
        if (lResult == S_OK) {
            return true;
        }
    }
    return false;
}

bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{
    // Pretranslate Message takes care of system-wide messages, such as
    // tabbing and shortcut key-combos. We'll look for all messages for
    // each window and any child control attached.
    UINT uStyle = static_cast<UINT>(GetWindowStyle(pMsg->hwnd));
    UINT uChildRes = uStyle & WS_CHILD;
    LRESULT lRes = 0;
    if (uChildRes != 0) {
        HWND hWndParent = ::GetParent(pMsg->hwnd);

        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CPaintManagerUI *pT = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
            if (pT == nullptr) {
                continue;
            }
            HWND hTempParent = hWndParent;
            while (hTempParent) {
                if (pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow()) {
                    if (pT->TranslateAccelerator(pMsg)) {
                        return true;
                    }

                    pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
                }
                hTempParent = GetParent(hTempParent);
            }
        }
    } else {
        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CPaintManagerUI *pT = static_cast<CPaintManagerUI *>(m_aPreMessages.GetAt(i));
            if (pT == nullptr) {
                continue;
            }
            if (pMsg->hwnd == pT->GetPaintWindow()) {
                if (pT->TranslateAccelerator(pMsg)) {
                    return true;
                }

                if (pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes)) {
                    return true;
                }

                return false;
            }
        }
    }
    return false;
}

bool CPaintManagerUI::AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
    ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
    return m_aTranslateAccelerator.Add(pTranslateAccelerator);
}

bool CPaintManagerUI::RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator)
{
    for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++) {
        if (static_cast<ITranslateAccelerator *>(m_aTranslateAccelerator.GetAt(i)) == pTranslateAccelerator) {
            return m_aTranslateAccelerator.Remove(i);
        }
    }
    return false;
}

void CPaintManagerUI::UsedVirtualWnd(bool bUsed)
{
    m_bUsedVirtualWnd = bUsed;
}

// 样式管理
void CPaintManagerUI::AddStyle(LPCTSTR pName, LPCTSTR pDeclarationList, bool bShared)
{
    CDuiString *pStyle = new CDuiString(pDeclarationList);

    if (bShared || m_bForceUseSharedRes) {
        if (!m_SharedResInfo.m_StyleHash.Insert(pName, pStyle)) {
            delete pStyle;
        }
    } else {
        if (!m_ResInfo.m_StyleHash.Insert(pName, pStyle)) {
            delete pStyle;
        }
    }
}

LPCTSTR CPaintManagerUI::GetStyle(LPCTSTR pName) const
{
    CDuiString *pStyle;
    if (this != nullptr) {
        pStyle = static_cast<CDuiString *>(m_ResInfo.m_StyleHash.Find(pName));
    } else {
        pStyle = nullptr;
    }
    if (!pStyle) {
        pStyle = static_cast<CDuiString *>(m_SharedResInfo.m_StyleHash.Find(pName));
    }
    if (pStyle) {
        return pStyle->GetData();
    } else {
    return nullptr;
    }
}

BOOL CPaintManagerUI::RemoveStyle(LPCTSTR pName, bool bShared)
{
    CDuiString *pStyle = nullptr;
    if (bShared) {
        pStyle = static_cast<CDuiString *>(m_SharedResInfo.m_StyleHash.Find(pName));
        if (pStyle) {
            delete pStyle;
            m_SharedResInfo.m_StyleHash.Remove(pName);
        }
    } else {
        pStyle = static_cast<CDuiString *>(m_ResInfo.m_StyleHash.Find(pName));
        if (pStyle) {
            delete pStyle;
            m_ResInfo.m_StyleHash.Remove(pName);
        }
    }
    return true;
}

const CStdStringPtrMap &CPaintManagerUI::GetStyles(bool bShared) const
{
    if (bShared) {
        return m_SharedResInfo.m_StyleHash;
    } else {
        return m_ResInfo.m_StyleHash;
    }
}

void CPaintManagerUI::RemoveAllStyle(bool bShared)
{
    if (bShared) {
        CDuiString *pStyle;
        for (int i = 0; i < m_SharedResInfo.m_StyleHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_StyleHash.GetAt(i)) {
                pStyle = static_cast<CDuiString *>(m_SharedResInfo.m_StyleHash.Find(key));
                delete pStyle;
            }
        }
        m_SharedResInfo.m_StyleHash.RemoveAll();
    } else {
        CDuiString *pStyle;
        for (int i = 0; i < m_ResInfo.m_StyleHash.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_StyleHash.GetAt(i)) {
                pStyle = static_cast<CDuiString *>(m_ResInfo.m_StyleHash.Find(key));
                delete pStyle;
            }
        }
        m_ResInfo.m_StyleHash.RemoveAll();
    }
}

const TImageInfo *CPaintManagerUI::GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    CDuiString sImageName = pStrImage;
    CDuiString sImageResType = _T("");
    DWORD dwMask = 0;
    CDuiString sItem;
    CDuiString sValue;
    LPTSTR pstr = nullptr;
    const int len = 2;
    for (int i = 0; i < len; ++i) {
        if (i == 1) {
            pStrImage = pStrModify;
        }

        if (!pStrImage) {
            continue;
        }

        while (*pStrImage != _T('\0')) {
            sItem.Empty();
            sValue.Empty();
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            while (pStrImage != nullptr && *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage < pstrTemp) {
                    sItem += *pStrImage++;
                }
            }
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (pStrImage == nullptr || *pStrImage++ != _T('=')) {
                break;
            }
            while (pStrImage != nullptr && *pStrImage > _T('\0') && *pStrImage <= _T(' ')) {
                pStrImage = ::CharNext(pStrImage);
            }
            if (pStrImage == nullptr || *pStrImage++ != _T('\'')) {
                break;
            }
            while (pStrImage != nullptr && *pStrImage != _T('\0') && *pStrImage != _T('\'')) {
                LPTSTR pstrTemp = ::CharNext(pStrImage);
                while (pStrImage < pstrTemp) {
                    sValue += *pStrImage++;
                }
            }
            if (pStrImage == nullptr || *pStrImage++ != _T('\'')) {
                break;
            }
            if (!sValue.IsEmpty()) {
                if (sItem == _T("file") || sItem == _T("res")) {
                    sImageName = sValue;
                } else if (sItem == _T("restype")) {
                    sImageResType = sValue;
                } else if (sItem == _T("mask")) {
                    if (sValue[0] == _T('#')) {
                        dwMask = _tcstoul(sValue.GetData() + 1, &pstr, RADIX_TYPE_TWO_VALUE);
                    } else {
                        dwMask = _tcstoul(sValue.GetData(), &pstr, RADIX_TYPE_TWO_VALUE);
                    }
                }
            }
            if (pStrImage == nullptr || *pStrImage++ != _T(' ')) {
                break;
            }
        }
    }
    return GetImageEx(sImageName, sImageResType, dwMask);
}

bool CPaintManagerUI::InitHicastDragDrop(HicastDragCallback *callback)
{
    if (!DragDropInit()) {
        return false;
    }

    this->RegisterHicastCallback(callback);

    return true;
}

bool CPaintManagerUI::InitDragDropFeature()
{
    if (!DragDropInit()) {
        return false;
    }
    if (DragDropRegister(this, m_hWndPaint)) {
        return true;
    }
    return false;
}

void CPaintManagerUI::UninitDragDropFeature()
{
    DragDropRegister(nullptr, m_hWndPaint);
    if (IsWindow(m_hWndPaint)) {
        RevokeDragDrop(m_hWndPaint);
    }
}

bool CPaintManagerUI::DragDropInit()
{
    OleInitialize(nullptr);
    AddRef();

    auto ret = RegisterDragDrop(m_hWndPaint, this);
    // calls addref
    if (FAILED(ret)) {
        if (ret != DRAGDROP_E_ALREADYREGISTERED) {
            return false;
        }
    } else {
        Release(); // i decided to AddRef explicitly after new
    }

    FORMATETC ftetc = { 0 };
    ftetc.cfFormat = CF_BITMAP;
    ftetc.dwAspect = DVASPECT_CONTENT;
    ftetc.lindex = -1;
    ftetc.tymed = TYMED_GDI;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_DIB;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_HDROP;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_ENHMETAFILE;
    ftetc.tymed = TYMED_ENHMF;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_TEXT;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_UNICODETEXT;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    return true;
}

bool CPaintManagerUI::InitDragDrop()
{
    AddRef();

    if (FAILED(RegisterDragDrop(m_hWndPaint, this))) { // calls addref
        return false;
    } else {
        Release(); // i decided to AddRef explicitly after new
    }

    FORMATETC ftetc = { 0 };
    ftetc.cfFormat = CF_BITMAP;
    ftetc.dwAspect = DVASPECT_CONTENT;
    ftetc.lindex = -1;
    ftetc.tymed = TYMED_GDI;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_DIB;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_HDROP;
    ftetc.tymed = TYMED_HGLOBAL;
    AddSuportedFormat(ftetc);
    ftetc.cfFormat = CF_ENHMETAFILE;
    ftetc.tymed = TYMED_ENHMF;
    AddSuportedFormat(ftetc);

    return true;
}
static WORD DIBNumColors(void *pv)
{
    int bits;
    LPBITMAPINFOHEADER lpbi;
    LPBITMAPCOREHEADER lpbc;
    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);
    /*  With the BITMAPINFO format headers, the size of the palette
     * is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     * is dependent on the bits per pixel ( = 2 raised to the power of
     * bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER)) {
        if (lpbi->biClrUsed != 0) {
            return (WORD)lpbi->biClrUsed;
        }
        bits = lpbi->biBitCount;
    } else {
        bits = lpbc->bcBitCount;
    }
    switch (bits) {
        case 1: // set 2bits
            return 2; // set 2bits
        case 4: // set 16bits
            return 16; // set 16bits
        case 8: // set 64bits
            return 256; // set 256bits
        default: // set 256bits
            /* A 24 bitcount DIB has no color table */
            return 0; // 返回值
    }
}
// code taken from SEEDIB MSDN sample
static WORD ColorTableSize(LPVOID lpv)
{
    LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpv;

    if (lpbih->biSize != sizeof(BITMAPCOREHEADER)) {
        if (((LPBITMAPINFOHEADER)(lpbih))->biCompression == BI_BITFIELDS)
            /* Remember that 16/32bpp dibs can still have a color table */
            return static_cast<WORD>((sizeof(DWORD) * 3) + (DIBNumColors(lpbih) * sizeof(RGBQUAD)));
        else
            return (WORD)(DIBNumColors(lpbih) * sizeof(RGBQUAD));
    } else
        return (WORD)(DIBNumColors(lpbih) * sizeof(RGBTRIPLE));
}

bool CPaintManagerUI::OnDrop(FORMATETC *pFmtEtc, STGMEDIUM &medium, DWORD *pdwEffect)
{
    POINT ptMouse = { 0 };
    GetCursorPos(&ptMouse);
    ::SendMessage(m_hTargetWnd, WM_LBUTTONUP, NULL, MAKELPARAM(ptMouse.x, ptMouse.y));

    if (pFmtEtc == nullptr) {
        return true;
    }

    if (pFmtEtc->cfFormat == CF_DIB && medium.tymed == TYMED_HGLOBAL) {
        if (medium.hGlobal != nullptr) {
            LPBITMAPINFOHEADER lpbi = reinterpret_cast<BITMAPINFOHEADER *>(GlobalLock(medium.hGlobal));
            if (lpbi != nullptr) {
                HBITMAP hbm = nullptr;
                HDC hdc = GetDC(nullptr);
                if (hdc != nullptr) {
                    int i = (reinterpret_cast<BITMAPFILEHEADER *>(lpbi))->bfOffBits;
                    hbm = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)lpbi, (LONG)CBM_INIT,
                        (LPSTR)lpbi + lpbi->biSize + ColorTableSize(lpbi), (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

                    ::ReleaseDC(nullptr, hdc);
                }
                GlobalUnlock(medium.hGlobal);
                if (hbm != nullptr) {
                    hbm = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);
                }
                if (hbm != nullptr) {
                    DeleteObject(hbm);
                }
                return true; // release the medium
            }
        }
    }
    if (pFmtEtc->cfFormat == CF_BITMAP && medium.tymed == TYMED_GDI) {
        if (medium.hBitmap != nullptr) {
            HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)medium.hBitmap);
            if (hBmp != nullptr) {
                DeleteObject(hBmp);
            }
            return false; // don't free the bitmap
        }
    }
    if (pFmtEtc->cfFormat == CF_ENHMETAFILE && medium.tymed == TYMED_ENHMF) {
        ENHMETAHEADER emh;
        GetEnhMetaFileHeader(medium.hEnhMetaFile, sizeof(ENHMETAHEADER), &emh);
        RECT rc;
        HDC hDC = GetDC(m_hTargetWnd);
        // start code: taken from ENHMETA.EXE MSDN Sample
        // *ALSO NEED to GET the pallete (select and RealizePalette it, but i was too lazy*
        // Get the characteristics of the output device
        float PixelsX = static_cast<float>(GetDeviceCaps(hDC, HORZRES));
        float PixelsY = static_cast<float>(GetDeviceCaps(hDC, VERTRES));
        float MMX = static_cast<float>(GetDeviceCaps(hDC, HORZSIZE));
        float MMY = static_cast<float>(GetDeviceCaps(hDC, VERTSIZE));
        // Calculate the rect in which to draw the metafile based on the
        // intended size and the current output device resolution
        // Remember that the intended size is given in 0.01mm units, so
        // convert those to device units on the target device
        rc.top = static_cast<int>(static_cast<float>(emh.rclFrame.top) * PixelsY / (MMY * 100.0f));
        rc.left = static_cast<int>(static_cast<float>(emh.rclFrame.left) * PixelsX / (MMX * 100.0f));
        rc.right = static_cast<int>(static_cast<float>(emh.rclFrame.right) * PixelsX / (MMX * 100.0f));
        rc.bottom = static_cast<int>(static_cast<float>(emh.rclFrame.bottom) * PixelsY / (MMY * 100.0f));

        HDC hdcMem = CreateCompatibleDC(hDC);
        HGDIOBJ hBmpMem = CreateCompatibleBitmap(hDC, emh.rclBounds.right, emh.rclBounds.bottom);
        HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBmpMem);
        PlayEnhMetaFile(hdcMem, medium.hEnhMetaFile, &rc);
        HBITMAP hBmp = (HBITMAP)::SelectObject(hdcMem, hOldBmp);
        DeleteDC(hdcMem);
        ReleaseDC(m_hTargetWnd, hDC);
        hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
        if (hBmp != nullptr) {
            DeleteObject(hBmp);
        }
        return true;
    }
    if (pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL) {
        HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
        if (hDrop != nullptr) {
            TCHAR szFileName[MAX_PATH];
            UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
            if (cFiles > 0) {
                DragQueryFile(hDrop, 0, szFileName, MAX_PATH);
                HBITMAP hBitmap =
                    (HBITMAP)LoadImage(nullptr, szFileName, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
                if (hBitmap) {
                    HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    if (hBmp != nullptr) {
                        DeleteObject(hBmp);
                    }
                }
            }
        }
        GlobalUnlock(medium.hGlobal);
    }
    return true; // let base free the medium
}

HRESULT CPaintManagerUI::ShareDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
    if (pDataObj == nullptr || pdwEffect == nullptr) {
        return S_FALSE;
    }
    curDataObject = pDataObj;
    POINT pt = { ptl.x, ptl.y };
    ::ScreenToClient(m_hWndPaint, &pt);
    CControlUI *hoverControl = FindControl(pt);
    if (hoverControl == nullptr) {
        *pdwEffect = DROPEFFECT_NONE;
        return S_OK;
    }

    // Generate mouse hover event
    hoverControl->OnDragEnter(pDataObj, grfKeyState, pt, pdwEffect);
    curEventDrop = hoverControl;
    return S_OK;
}

HRESULT CPaintManagerUI::ShareDragOver(DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
    POINT pt = { ptl.x, ptl.y };
    ::ScreenToClient(m_hWndPaint, &pt);
    m_ptLastMousePos = pt;
    CControlUI *pNewHover = FindControl(pt);
    if (pNewHover == nullptr && pdwEffect != nullptr) {
        *pdwEffect = DROPEFFECT_NONE;
        return S_OK;
    }

    if (pNewHover != nullptr && pdwEffect != nullptr && pNewHover->GetManager() != this) {
        *pdwEffect = DROPEFFECT_NONE;
        return S_OK;
    }

    if (pNewHover != curEventDrop && pNewHover != nullptr) {
        pNewHover->OnDragEnter(curDataObject, grfKeyState, pt, pdwEffect);
        curEventDrop = pNewHover;
    }

    if (pNewHover != nullptr) {
        pNewHover->OnDragOver(grfKeyState, pt, pdwEffect);
    }
    return S_OK;
}

HRESULT CPaintManagerUI::ShareDragLeave()
{
    curDataObject = nullptr;
    if (curEventDrop != nullptr) {
        curEventDrop->OnDragLeave();
        curEventDrop = nullptr;
    }
    return S_OK;
}

HRESULT CPaintManagerUI::ShareDrop(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
    if (pDataObj == nullptr || pdwEffect == nullptr) {
        return S_FALSE;
    }
    POINT pt = { ptl.x, ptl.y };
    ::ScreenToClient(m_hWndPaint, &pt);
    if (curEventDrop != nullptr) {
        curEventDrop->OnDrop(pDataObj, grfKeyState, pt, pdwEffect);
    } else {
        *pdwEffect = DROPEFFECT_NONE;
        return S_OK;
    }
    return S_OK;
}

bool CPaintManagerUI::OnWM_PAINT(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
{
    RECT rcPaint = { 0 };
    if (!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)) {
        if (IsLayered()) {
            GetWindowRect(m_hWndPaint, &rcPaint);
            ::OffsetRect(&rcPaint, -rcPaint.left, -rcPaint.top);
        } else {
            return true;
        }
    }

    if (m_pRoot == nullptr) {
        PAINTSTRUCT ps = { 0 };
        ::BeginPaint(m_hWndPaint, &ps);
        ::EndPaint(m_hWndPaint, &ps);
        return true;
    }

    bool bNeedSizeMsg = false;
    RECT rcClient = { 0 };
    ::GetClientRect(m_hWndPaint, &rcClient);
    DWORD dwWidth = rcClient.right - rcClient.left;
    DWORD dwHeight = rcClient.bottom - rcClient.top;

    SetPainting(true);
    if (m_bUpdateNeeded) {
        m_bUpdateNeeded = false;
        PaintUpdate(rcClient, bNeedSizeMsg);
    }

    // Set focus to first control?
    if (m_bFocusNeeded) {
        SetNextTabControl();
    }

    if (m_hbmpOffscreen) {
        BITMAP bmp { 0 };
        GetObject(m_hbmpOffscreen, sizeof(BITMAP), (LPBYTE)&bmp);
        if (bmp.bmWidth != dwWidth || bmp.bmHeight != dwHeight) {
            if (m_hDcOffscreen != nullptr) {
                ::DeleteDC(m_hDcOffscreen);
            }
            if (m_hbmpOffscreen != nullptr) {
                ::DeleteObject(m_hbmpOffscreen);
            }
            m_hDcOffscreen = nullptr;
            m_hbmpOffscreen = nullptr;

            LOG_ERROR(
                "[DUILIB]: this:%0x, m_hDcPaint:%0x  bitmap size bmWidth %d, bmHeight %d,dwWidth %d, dwHeight %d ",
                this, m_hDcPaint, bmp.bmWidth, bmp.bmHeight, dwWidth, dwHeight);
        }
    }

    if (m_bLayered) {
        DWORD dwExStyle = static_cast<DWORD>(::GetWindowLong(m_hWndPaint, GWL_EXSTYLE));
        DWORD dwNewExStyle = dwExStyle | WS_EX_LAYERED;
        if (dwExStyle != dwNewExStyle) {
            ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewExStyle);
        }
        m_bOffscreenPaint = true;
        UnionRect(&rcPaint, &rcPaint, &m_rcLayeredUpdate);
        if (rcPaint.right > rcClient.right) {
            rcPaint.right = rcClient.right;
        }
        if (rcPaint.bottom > rcClient.bottom) {
            rcPaint.bottom = rcClient.bottom;
        }
        ::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
    }

    if (m_bOffscreenPaint && m_hbmpOffscreen == nullptr) {
        m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
        if (m_hDcOffscreen == nullptr) {
            LOG_DEBUG("[DUILIB]: this:%0x, m_hDcPaint:%0x CreateCompatibleDC is nullptr. getlasterror:%0x", this,
                m_hDcPaint, GetLastError());
            m_hDcPaint = ::GetDC(m_hWndPaint);
            m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
            LOG_DEBUG("[DUILIB]: this:%0x, m_hWndPaint:%0x, m_hDcPaint:%0x, m_hDcOffscreen:%0x, CreateCompatibleDC "
                      "Again. getlasterror:%0x",
                this, m_hWndPaint, m_hDcPaint, m_hDcOffscreen, GetLastError());
        }
        m_hbmpOffscreen = CRenderEngine::CreateARGB32Bitmap(m_hDcPaint, dwWidth, dwHeight,
            reinterpret_cast<LPBYTE *>(&m_pOffscreenBits));
        ASSERT(m_hDcOffscreen);
        if (m_hDcOffscreen == nullptr) {
            return true;
        }
        ASSERT(m_hbmpOffscreen);
        if (m_hbmpOffscreen == nullptr) {
            return true;
        }
    }
    // Begin Windows paint
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWndPaint, &ps);
    if (m_bOffscreenPaint) {
        OffscreenPaint(rcPaint, rcClient, dwWidth, dwHeight);
    } else {
        // A standard paint job
        int iSaveDC = ::SaveDC(m_hDcPaint);
        m_pRoot->DoPaint(m_hDcPaint, rcPaint);
        for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
            CControlUI *pPostPaintControl = static_cast<CControlUI *>(m_aPostPaintControls.GetAt(i));
            if (pPostPaintControl == nullptr) {
                continue;
            }
            pPostPaintControl->DoPostPaint(m_hDcPaint, rcPaint);
        }
        ::RestoreDC(m_hDcPaint, iSaveDC);
    }
    // All Done!
    ::EndPaint(m_hWndPaint, &ps);
    if (m_bUpdateNeeded)
        Invalidate();
    SetPainting(false);
    // 发送窗口大小改变消息
    if (bNeedSizeMsg) {
        this->SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWSIZE, 0, 0, true);
    }
    return true;
}

void CPaintManagerUI::EnableBlurRect()
{
    EnableBlurRect(RECT { 0, 0, 0, 0 });
}

void CPaintManagerUI::EnableBlurRect(const RECT &rc)
{
    m_bBlur = true;
    m_rcBlur = rc;
    Utils::EnableBlurWindow(m_hWndPaint);
}

void CPaintManagerUI::SetBlurMode(HWND hwnd, ACCENT_STATE mode, DWORD color)
{
    CDuiString dllFullPath = GetSystemDllPath(_T("user32.dll"));
    const HINSTANCE hModule = LoadLibrary(dllFullPath.GetData());
    if (hModule) {
        using pSetWindowCompositionAttribute = BOOL(WINAPI *)(HWND, PWINDOWCOMPOSITIONATTRIBDATA);
        const pSetWindowCompositionAttribute SetWindowCompositionAttribute =
            (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute) {
            ACCENT_POLICY policy = { mode, 0, 0, 0 };
            policy.GradientColor = color;
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
        FreeLibrary(hModule);
    }
}

void CPaintManagerUI::SetTaskbarState(TaskbarState state)
{
    CComPtr<ITaskbarList3> itaskbarList3;
    HRESULT hr = itaskbarList3.CoCreateInstance(CLSID_TaskbarList);
    if (!SUCCEEDED(hr)) {
        return;
    }

    switch (state) {
        case DuiLib::TaskbarState::NoProgress:
            itaskbarList3->SetProgressState(m_hWndPaint, TBPF_NOPROGRESS);
            break;
        case DuiLib::TaskbarState::Indeterminate:
            itaskbarList3->SetProgressState(m_hWndPaint, TBPF_INDETERMINATE);
            break;
        case DuiLib::TaskbarState::Normal:
            itaskbarList3->SetProgressState(m_hWndPaint, TBPF_NORMAL);
            break;
        case DuiLib::TaskbarState::Error:
            itaskbarList3->SetProgressState(m_hWndPaint, TBPF_ERROR);
            break;
        case DuiLib::TaskbarState::Paused:
            itaskbarList3->SetProgressState(m_hWndPaint, TBPF_PAUSED);
            break;
        default:
            break;
    }
}

void CPaintManagerUI::SetTaskbarProgress(int percent)
{
    CComPtr<ITaskbarList3> itaskbarList3;
    HRESULT hr = itaskbarList3.CoCreateInstance(CLSID_TaskbarList);
    if (!SUCCEEDED(hr)) {
        return;
    }

    SetTaskbarState(TaskbarState::Normal);
    itaskbarList3->SetProgressValue(m_hWndPaint, percent, SCALE_TAYO_ONE);
}

void CPaintManagerUI::AddUserValue(const DuiLib::CDuiString &key, const DuiLib::CDuiString &val)
{
    userValueMap[key] = val;
}

DuiLib::CDuiString CPaintManagerUI::GetUserValue(const DuiLib::CDuiString &key)
{
    return userValueMap[key];
}

void CPaintManagerUI::OffscreenPaint(RECT &rcPaint, RECT &rcClient, DWORD &dwWidth, DWORD &dwHeight)
{
    if (m_hDcOffscreen == nullptr || m_pRoot == nullptr || dwWidth > MAX_MUL_VALUE_DWORD ||
        dwHeight > MAX_MUL_VALUE_DWORD) {
        return;
    }
    // UI绘画范围计算
    DWORD maxIndex = static_cast<DWORD>((dwWidth * dwHeight * AREA_EXPAND_FOUR_TIME) - (sizeof(DWORD)));

    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
    int iSaveDC = ::SaveDC(m_hDcOffscreen);
    if (m_bLayered && m_pOffscreenBits) {
        for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
            for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
                if (static_cast<DWORD>(abs(y)) > MAX_MUL_VALUE_DWORD) {
                    break;
                }
                DWORD i = (y * dwWidth + x) * AREA_EXPAND_FOUR_TIME; // UI绘画范围计算
                if (i > maxIndex)
                    break;
                *(reinterpret_cast<DWORD *>(&m_pOffscreenBits[i])) = 0;
            }
        }
    }
    m_pRoot->DoPaint(m_hDcOffscreen, rcPaint);
    DrawCaret(m_hDcOffscreen, rcPaint);
    for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
        CControlUI *pPostPaintControl = static_cast<CControlUI *>(m_aPostPaintControls.GetAt(i));
        if (pPostPaintControl == nullptr) {
            continue;
        }
        pPostPaintControl->DoPostPaint(m_hDcOffscreen, rcPaint);
    }
    if (m_bLayered) {
        UpdateLayered(rcPaint, rcClient, dwWidth, dwHeight);
    }
    ::RestoreDC(m_hDcOffscreen, iSaveDC);

    if (m_bLayered) {
        RECT rcWnd = { 0 };
        ::GetWindowRect(m_hWndPaint, &rcWnd);
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_nOpacity, AC_SRC_ALPHA };
        POINT ptPos = { rcWnd.left, rcWnd.top };
        SIZE sizeWnd = { static_cast<LONG>(dwWidth), static_cast<LONG>(dwHeight) };
        POINT ptSrc = { 0, 0 };
        if (g_fUpdateLayeredWindow != nullptr) {
            g_fUpdateLayeredWindow(m_hWndPaint, m_hDcPaint, &ptPos, &sizeWnd, m_hDcOffscreen, &ptSrc, 0, &bf,
                ULW_ALPHA);
        }
    } else {
        ::BitBlt(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top,
            m_hDcOffscreen, rcPaint.left, rcPaint.top, SRCCOPY);
    }
    ::SelectObject(m_hDcOffscreen, hOldBitmap);

    if (m_bShowUpdateRect) {
        CRenderEngine::DrawRect(m_hDcPaint, rcPaint, 1, 0xFFFF0000);
    }
}

void CPaintManagerUI::UpdateLayered(RECT &rcPaint, RECT &rcClient, DWORD &dwWidth, DWORD &dwHeight)
{
    const int iValue = 3; // 关屏位数
    const int itValue = 2; // 关屏位数
    const int digitNum = 255; // 位数
    if (m_pOffscreenBits) {
        DWORD maxIndex = static_cast<DWORD>(static_cast<DWORD>(dwWidth * dwHeight * AREA_EXPAND_FOUR_TIME) -
            (sizeof(DWORD)));
        for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
            for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
                DWORD i = (y * dwWidth + x) * AREA_EXPAND_FOUR_TIME;
                if (i > maxIndex) {
                    break;
                }
                if ((m_pOffscreenBits[i + iValue] == 0) &&
                    (m_pOffscreenBits[i + 0] != 0 || m_pOffscreenBits[i + 1] != 0 ||
                        m_pOffscreenBits[i + itValue] != 0)) {
                    m_pOffscreenBits[i + iValue] = digitNum; // 255bit
                }
            }
        }
    }

    for (int i = 0; i < m_aChildWnds.GetSize();) {
        HWND hChildWnd = static_cast<HWND>(m_aChildWnds.GetAt(i));
        if (hChildWnd == nullptr) {
            continue;
        }
        if (!::IsWindow(hChildWnd)) {
            m_aChildWnds.Remove(i);
            continue;
        }
        ++i;
        if (!::IsWindowVisible(hChildWnd)) {
            continue;
        }
        RECT rcChildWnd;
        GetChildWndRect(m_hWndPaint, hChildWnd, rcChildWnd);

        RECT rcTemp = { 0 };
        if (!::IntersectRect(&rcTemp, &rcPaint, &rcChildWnd)) {
            continue;
        }

        COLORREF *pChildBitmapBits = nullptr;
        HDC hChildMemDC = ::CreateCompatibleDC(m_hDcOffscreen);
        HBITMAP hChildBitmap = CRenderEngine::CreateARGB32Bitmap(hChildMemDC, rcChildWnd.right - rcChildWnd.left,
            rcChildWnd.bottom - rcChildWnd.top, reinterpret_cast<LPBYTE *>(&pChildBitmapBits));
        ::ZeroMemory(pChildBitmapBits, (rcChildWnd.right - rcChildWnd.left)
            * (rcChildWnd.bottom - rcChildWnd.top) * AREA_EXPAND_FOUR_TIME);
        HBITMAP hOldChildBitmap = (HBITMAP)::SelectObject(hChildMemDC, hChildBitmap);
        ::SendMessage(hChildWnd, WM_PRINT, (WPARAM)hChildMemDC,
            (LPARAM)(PRF_CHECKVISIBLE | PRF_CHILDREN | PRF_CLIENT | PRF_OWNED));
        COLORREF *pChildBitmapBit;
        for (LONG y = 0; y < rcChildWnd.bottom - rcChildWnd.top; y++) {
            for (LONG x = 0; x < rcChildWnd.right - rcChildWnd.left; x++) {
                pChildBitmapBit = pChildBitmapBits + y * (rcChildWnd.right - rcChildWnd.left) + x;
                if (*pChildBitmapBit != 0x00000000) {
                    *pChildBitmapBit |= 0xff000000;
                }
            }
        }
        ::BitBlt(m_hDcOffscreen, rcChildWnd.left, rcChildWnd.top, rcChildWnd.right - rcChildWnd.left,
            rcChildWnd.bottom - rcChildWnd.top, hChildMemDC, 0, 0, SRCCOPY);
        ::SelectObject(hChildMemDC, hOldChildBitmap);
        ::DeleteObject(hChildBitmap);
        ::DeleteDC(hChildMemDC);
    }
}

void CPaintManagerUI::TextPaintUpdate(LPCTSTR language)
{
#ifdef SWITCH_LANGUAGE_TEST
    NeedUpdate();
    CControlUI *pControl = nullptr;
    m_aFoundControls.Empty();
    m_pRoot->FindControl(__FindControlsFromClass, _T("TextUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("LabelUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("ButtonUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("OptionUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("RichEditUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("EditUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("XListUI"), UIFIND_ALL);
    m_pRoot->FindControl(__FindControlsFromClass, _T("ComboUI"), UIFIND_ALL);

    UICulture::GetInstance()->setLocale(language, false);

    for (int i = 0; i < m_aFoundControls.GetSize(); i++) {
        pControl = static_cast<CControlUI *>(m_aFoundControls.GetAt(i));
        if (pControl == nullptr) {
            continue;
        }
        pControl->SetLanguageText(language);
        pControl->NeedParentUpdate();
    }
#endif
}

void CPaintManagerUI::PaintUpdate(RECT &rcClient, bool &bNeedSizeMsg)
{
    if (!::IsRectEmpty(&rcClient) && !::IsIconic(m_hWndPaint)) {
        if (m_pRoot == nullptr) {
            return;
        }
        if (m_pRoot->IsUpdateNeeded()) {
            if (m_hDcOffscreen != nullptr) {
                ::DeleteDC(m_hDcOffscreen);
            }
            if (m_hbmpOffscreen != nullptr) {
                ::DeleteObject(m_hbmpOffscreen);
            }
            m_hDcOffscreen = nullptr;
            m_hbmpOffscreen = nullptr;
            m_pRoot->SetPos(rcClient, true);
            bNeedSizeMsg = true;
        } else {
            CControlUI *pControl = nullptr;
            m_aFoundControls.Empty();
            m_pRoot->FindControl(__FindControlsFromUpdate, nullptr,
                UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
            for (int it = 0; it < m_aFoundControls.GetSize(); it++) {
                pControl = static_cast<CControlUI *>(m_aFoundControls.GetAt(it));
                if (pControl == nullptr) {
                    continue;
                }
                if (!pControl->IsFloat()) {
                    pControl->SetPos(pControl->GetPos(), true);
                } else {
                    pControl->SetPos(pControl->GetFloatRelativePos(), true);
                }
            }
        }
        // We'll want to notify the window when it is first initialized
        // with the correct layout. The window form would take the time
        // to submit swipes/animations.
        if (m_bFirstLayout) {
            m_bFirstLayout = false;
            SendNotify(m_pRoot, DUI_MSGTYPE_WINDOWINIT, 0, 0, false);
            // 更新阴影窗口显示
            m_shadow.Update(m_hWndPaint);
        }
    }
}

void CPaintManagerUI::SendPointerUpEvent(POINT pt, WPARAM wParam, LPARAM lParam)
{
    if (m_pEventPointer != nullptr) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_POINTERUP;
        event.pSender = m_pEventPointer;
        event.wParam = wParam;
        event.lParam = lParam;
        event.ptMouse = pt;
        event.wKeyState = (WORD)wParam;
        event.dwTimestamp = ::GetTickCount();
        m_pEventPointer->Event(event);
        m_pEventPointer = nullptr;
    }
}
void CPaintManagerUI::SendPointerDownEvent(POINT pt, WPARAM wParam, LPARAM lParam)
{
    if (m_pEventPointer != nullptr) {
        TEventUI event = { 0 };
        event.Type = UIEVENT_POINTERDOWN;
        event.pSender = m_pEventPointer;
        event.wParam = wParam;
        event.lParam = lParam;
        event.ptMouse = pt;
        event.wKeyState = (WORD)wParam;
        event.dwTimestamp = ::GetTickCount();
        m_pEventPointer->Event(event);
    }
}


CControlUI *CPaintManagerUI::GetOptionGroupSelected(LPCTSTR pStrGroupName)
{
    LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
    if (lp) {
        CStdPtrArray *aOptionGroup = static_cast<CStdPtrArray *>(lp);
        if (aOptionGroup == nullptr) {
            return nullptr;
        }
        for (int i = 0; i < aOptionGroup->GetSize(); i++) {
            CControlUI *pControl = static_cast<CControlUI *>(aOptionGroup->GetAt(i));
            if (pControl && (CDuiString(pControl->GetClass()) == L"OptionUI")) {
                if ((static_cast<COptionUI *>(pControl))->IsSelected() == true) {
                    return pControl;
                }
            }
        }
    }
    return nullptr;
}

int CPaintManagerUI::GetSystemDPIScale()
{
    HMONITOR hMonitor;
    POINT pt;
    UINT dpix = DPI_TYPE_ONE; // set DPI x
    UINT dpiy = DPI_TYPE_ONE; // set DPI y
    pt.x = 1;
    pt.y = 1;
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (IsWindows7OrGreater()) {
        CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
        HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
        if (hModule != nullptr) {
            LPGetDpiForMonitor getDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
            if (getDpiForMonitor != nullptr) {
                getDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
            }
            FreeLibrary(hModule);
        }
    }

    int Scale = MulDiv(dpix, SCALE_TAYO_ONE, DPI_TYPE_ONE);
    return Scale;
}


void CPaintManagerUI::RemoveAllImagesForDPIChange(bool bShared)
{
    if (bShared) {
        std::map<wstring, TImageInfo *> mapT;
        TImageInfo *data;
        for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TImageInfo *>(m_SharedResInfo.m_ImageHash.Find(key, false));
                if (data) {
                    if (data->nAddType == 0) {
                        CRenderEngine::FreeImage(data);
                    } else {
                        mapT.insert(make_pair(wstring(key), data));
                    }
                }
            }
        }
        m_SharedResInfo.m_ImageHash.RemoveAll();

        for (auto iter = mapT.begin(); iter != mapT.end(); iter++) {
            m_SharedResInfo.m_ImageHash.Insert(iter->first.c_str(), iter->second);
        }
    } else {
        std::map<wstring, TImageInfo *> mapT;
        TImageInfo *data;
        for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TImageInfo *>(m_ResInfo.m_ImageHash.Find(key, false));
                if (data) {
                    if (data->nAddType == 0) {
                        CRenderEngine::FreeImage(data);
                    } else {
                        mapT.insert(make_pair(wstring(key), data));
                    }
                }
            }
        }
        m_ResInfo.m_ImageHash.RemoveAll();

        for (auto iter = mapT.begin(); iter != mapT.end(); iter++) {
            m_ResInfo.m_ImageHash.Insert(iter->first.c_str(), iter->second);
        }
    }
}

SIZE CPaintManagerUI::GetOrgiSize()
{
    return m_szOrigWindowSize;
}

void CPaintManagerUI::SetOrgiSize(int cx, int cy)
{
    m_szOrigWindowSize.cx = cx;
    m_szOrigWindowSize.cy = cy;
}

SIZE CPaintManagerUI::GetFixOrgiSize()
{
    SIZE sizeFix = CResourceManager::GetInstance()->Scale(m_szOrigWindowSize);
    return sizeFix;
}
} // namespace DuiLib
