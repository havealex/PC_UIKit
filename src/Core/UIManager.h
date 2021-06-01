/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__
#include "Core/win32blur.h"

#pragma once

namespace DuiLib {
class CControlUI;
class CRichEditUI;
class CIDropTarget;
class DuiDropTarget;

enum UILIB_RESTYPE {
    UILIB_FILE = 1,    // 来自磁盘文件
    UILIB_ZIP,         // 来自磁盘zip压缩包
    UILIB_RESOURCE,    // 来自资源
    UILIB_ZIPRESOURCE, // 来自资源的zip压缩包
};

enum EVENTTYPE_UI {
    UIEVENT__FIRST = 1,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
    UIEVENT_RBUTTONUP,
    UIEVENT_MBUTTONDOWN,
    UIEVENT_MBUTTONUP,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT_POINTERUPDATE,
    UIEVENT_POINTERUP,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_POINTERDOWN,
    UIEVENT__LAST,
};

enum MSGTYPE_UI {
    // 内部保留消息
    UIMSG_TRAYICON = WM_USER + 1,
    // 用于子線程中notify主線程更新UI的消息
    UIMSG_UPDATE_ON_MAIN = WM_USER + 2,
    // 程序自定义消息
    UIMSG_USER = WM_USER + 100,
};

#define UIFLAG_TABSTOP 0x00000001
#define UIFLAG_SETCURSOR 0x00000002
#define UIFLAG_WANTRETURN 0x00000004

#define UIFIND_ALL 0x00000000
#define UIFIND_VISIBLE 0x00000001
#define UIFIND_ENABLED 0x00000002
#define UIFIND_HITTEST 0x00000004
#define UIFIND_UPDATETEST 0x00000008
#define UIFIND_TOP_FIRST 0x00000010
#define UIFIND_ME_FIRST 0x80000000

// Flags used for controlling the paint
#define UISTATE_FOCUSED 0x00000001
#define UISTATE_SELECTED 0x00000002
#define UISTATE_DISABLED 0x00000004
#define UISTATE_HOT 0x00000008
#define UISTATE_PUSHED 0x00000010
#define UISTATE_READONLY 0x00000020
#define UISTATE_CAPTURED 0x00000040

enum class TaskbarState {
    NoProgress,    // 图标原始状态
    Indeterminate, // 等待状态（进度条从左往右不断滚动）
    Normal,        // 配合SetTaskbarProgress显示进度条
    Error,         // 错误（图标变为红色背景）
    Paused         // 暂停（没试过，可能是黄色）
};

enum class MessageType {
    Paint,
    UserAction,
    AllMessage,
    Dispatch
};

struct UILIB_API tagTFontInfo {
    HFONT hFont;
    CDuiString sFontName;
    int iSize;
    bool bBold;
    bool bUnderline;
    bool bItalic;
    TEXTMETRIC tm;
};
using TFontInfo = UILIB_API tagTFontInfo;

struct UILIB_API tagTImageInfo {
    HBITMAP hBitmap;
    LPBYTE pBits;
    LPBYTE pSrcBits;
    int isrcsize;
    int nX;
    int nY;
    bool bAlpha;
    bool bUseHSL;
    int nAddType;
    CDuiString sResType;
    DWORD dwMask;
    bool isRes;

    tagTImageInfo()
    {
        hBitmap = nullptr;
        pBits = nullptr;
        pSrcBits = nullptr;
        isrcsize = 0;
        nX = 0;
        nY = 0;
        bAlpha = false;
        bUseHSL = false;
        nAddType = 0;
        dwMask = 0;
        isRes = false;
    }

    ~tagTImageInfo()
    {
        if (pBits) {
            delete[] pBits;
            pBits = nullptr;
        }
        if (pSrcBits) {
            delete[] pSrcBits;
            pSrcBits = nullptr;
        }
    }
};
using TImageInfo = UILIB_API tagTImageInfo;

struct UILIB_API tagTDrawInfo {
    tagTDrawInfo();
    void Parse(LPCTSTR pStrImage, LPCTSTR pStrModify);
    void Clear();

    CDuiString sDrawString;
    CDuiString sDrawModify;
    CDuiString sImageName;
    CDuiString sResType;
    RECT rcDest;
    RECT rcSource;
    RECT rcCorner;
    DWORD dwMask;
    BYTE uFade;
    bool bHole;
    bool bTiledX;
    bool bTiledY;
    bool bHSL;
    bool bReverse;
};
using TDrawInfo = UILIB_API tagTDrawInfo;

struct UILIB_API tagTPercentInfo {
    double left;
    double top;
    double right;
    double bottom;
};
using TPercentInfo = UILIB_API tagTPercentInfo;

struct UILIB_API tagTResInfo {
    DWORD m_dwDefaultDisabledColor;
    DWORD m_dwDefaultFontColor;
    DWORD m_dwDefaultLinkFontColor;
    DWORD m_dwDefaultLinkHoverFontColor;
    DWORD m_dwDefaultSelectedBkColor;
    TFontInfo m_DefaultFontInfo;
    CStdStringPtrMap m_CustomFonts;
    CStdStringPtrMap m_ImageHash;
    CStdStringPtrMap m_AttrHash;
    CStdStringPtrMap m_StyleHash;
    CStdStringPtrMap m_DrawInfoHash;
};
using TResInfo = UILIB_API tagTResInfo;

// Structure for notifications from the system
// to the control implementation.
struct UILIB_API tagTEventUI {
    int Type;
    CControlUI *pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
};
using TEventUI = UILIB_API tagTEventUI;

// Drag&Drop control
const TCHAR * const CF_MOVECONTROL = _T("CF_MOVECONTROL");

struct UILIB_API tagTCFMoveUI {
    CControlUI *pControl;
};
using TCFMoveUI = UILIB_API tagTCFMoveUI;

// Listener interface
class INotifyUI {
public:
    virtual void Notify(TNotifyUI &msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI {
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) = 0;
};

class ITranslateAccelerator {
public:
    virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};

using LPCREATECONTROL = CControlUI *(*)(LPCTSTR pstrType);

class UILIB_API CPaintManagerUI : public CIDropTarget, public DuiDropTarget {
public:
    CPaintManagerUI();
    ~CPaintManagerUI();

public:
    void Init(HWND hWnd, LPCTSTR pstrName = nullptr);
    bool IsUpdateNeeded() const;
    static bool IsMainThread();
    static void CheckMainThread();
    void NeedUpdate();
    void Invalidate();
    void Invalidate(RECT &rcItem);

    LPCTSTR GetName() const;
    HDC GetPaintDC() const;
    HDC GetOffscreenPaintDC() const;
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;
    int GetTooltipWindowWidth() const;
    void SetTooltipWindowWidth(int iWidth);
    int GetHoverTime() const;
    void SetHoverTime(int iTime);

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT &GetSizeBox();
    void SetSizeBox(RECT &rcSizeBox);
    RECT &GetCaptionRect();
    void SetCaptionRect(RECT &rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int cx, int cy);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);

    BYTE GetOpacity() const;
    void SetOpacity(BYTE nOpacity);

    bool IsLayered();
    void SetLayered(bool bLayered);
    RECT &GetLayeredInset();
    void SetLayeredInset(RECT &rcLayeredInset);
    BYTE GetLayeredOpacity();
    void SetLayeredOpacity(BYTE nOpacity);

    CShadowUI *GetShadow();
    // 光标
    bool ShowCaret(bool bShow);
    bool SetCaretPos(CRichEditUI *obj, int x, int y);
    CRichEditUI *GetCurrentCaretObject();
    bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
    void DrawCaret(HDC hDC, const RECT &rcPaint);

    void SetUseGdiplusText(bool bUse);
    bool IsUseGdiplusText() const;
    void SetGdiplusTextRenderingHint(int trh);
    int GetGdiplusTextRenderingHint() const;

    static HINSTANCE GetInstance();
    static CDuiString GetInstancePath();
    static CDuiString GetCurrentPath();
    static HINSTANCE GetResourceDll();
    static const CDuiString &GetResourcePath();
    static const CDuiString &GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetResourcePath(LPCTSTR pStrPath);

    static void SetResourceType(int nType);
    static int GetResourceType();
    static bool GetHSL(short *H, short *S, short *L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200
    static void ReloadSkin();
    static void ReloadPaintText(LPCTSTR language);
    static CPaintManagerUI *GetPaintManager(LPCTSTR pstrName);
    static CStdPtrArray *GetPaintManagers();
    static bool LoadPlugin(LPCTSTR pstrModuleName);
    static CStdPtrArray *GetPlugins();
    static int GetSystemDPIScale();

    bool IsForceUseSharedRes() const;
    void SetForceUseSharedRes(bool bForce);

    void DeletePtr(void *ptr);

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor, bool bShared = false);
    DWORD GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(DWORD dwColor, bool bShared = false);
    TFontInfo *GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic,
        bool bShared = false);
    DWORD GetCustomFontCount(bool bShared = false) const;
    int AddPtFont(FontType ft, int nSize, bool bBold = false, bool bUnderline = false);
    HFONT AddFont(int id, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic,
        bool bShared = false, bool usePx = false);
    HFONT GetFont(int id);
    HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    int GetFontIndex(HFONT hFont, bool bShared = false);
    int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bShared = false);
    void RemoveFont(HFONT hFont, bool bShared = false);
    void RemoveFont(int id, bool bShared = false);
    void RemoveAllFonts(bool bShared = false);
    TFontInfo *GetFontInfo(int id);
    TFontInfo *GetFontInfo(HFONT hFont);

    const TImageInfo *GetImage(LPCTSTR bitmap);
    const TImageInfo *GetImageEx(LPCTSTR bitmap, LPCTSTR type = nullptr, DWORD mask = 0, bool bUseHSL = false,
        HINSTANCE instance = nullptr, bool bRev = false);
    const TImageInfo *AddImage(LPCTSTR bitmap, LPCTSTR type = nullptr, DWORD mask = 0, bool bUseHSL = false,
        bool bShared = false, HINSTANCE instance = nullptr, bool bRev = false, bool absPath = false);
    const TImageInfo *AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha,
        bool bShared = false);
    void RemoveImage(LPCTSTR bitmap, bool bShared = false);
    void ReplaceImage(LPCTSTR bitmap, TImageInfo *newimage, bool bShared = false);
    void RemoveAllImages(bool bShared = false);
    static void ReloadSharedImages();
    void ReloadImages();

    const TDrawInfo *GetDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
    void RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
    void RemoveAllDrawInfos();

    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, bool bShared = false);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName, bool bShared = false);
    void RemoveAllDefaultAttributeList(bool bShared = false);

    void AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
    LPCTSTR GetWindowCustomAttribute(LPCTSTR pstrName) const;
    bool RemoveWindowCustomAttribute(LPCTSTR pstrName);
    void RemoveAllWindowCustomAttribute();

    // 样式管理
    void AddStyle(LPCTSTR pName, LPCTSTR pStyle, bool bShared = false);
    LPCTSTR GetStyle(LPCTSTR pName) const;
    BOOL RemoveStyle(LPCTSTR pName, bool bShared = false);
    const CStdStringPtrMap &GetStyles(bool bShared = false) const;
    void RemoveAllStyle(bool bShared = false);

    const TImageInfo *GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify = nullptr);

    // 初始化拖拽
    bool InitDragDrop();
    bool InitHicastDragDrop(HicastDragCallback *callback);
    bool InitDragDropFeature();
    void UninitDragDropFeature();
    bool DragDropInit();
    virtual bool OnDrop(FORMATETC *pFmtEtc, STGMEDIUM &medium, DWORD *pdwEffect);
    CControlUI *eventDropControl = nullptr;

    HRESULT ShareDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
    HRESULT ShareDragOver(DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
    HRESULT ShareDragLeave() override;
    HRESULT ShareDrop(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) override;
    IDataObject *curDataObject = nullptr;
    CControlUI *curEventDrop = nullptr;

    bool AttachDialog(CControlUI *pControl);
    bool InitControls(CControlUI *pControl, CControlUI *pParent = nullptr);
    void ReapObjects(CControlUI *pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI *pControl);
    CStdPtrArray *GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI *pControl);
    void RemoveAllOptionGroups();

    CControlUI *GetFocus() const;
    void SetFocus(CControlUI *pControl);
    void SetFocusNeeded(CControlUI *pControl);

    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(CControlUI *pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(CControlUI *pControl, UINT nTimerID);
    void KillTimer(CControlUI *pControl);
    void RemoveAllTimers();

    void SetCapture();
    void ReleaseCapture();
    bool IsCaptured();

    bool IsPainting();
    void SetPainting(bool bIsPainting);

    bool AddNotifier(INotifyUI *pControl);
    bool RemoveNotifier(INotifyUI *pControl);
    void SendNotify(TNotifyUI &Msg, bool bAsync = false);
    void SendNotify(CControlUI *pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0,
        bool bAsync = false);

    bool AddPreMessageFilter(IMessageFilterUI *pFilter);
    bool RemovePreMessageFilter(IMessageFilterUI *pFilter);

    bool AddMessageFilter(IMessageFilterUI *pFilter);
    bool RemoveMessageFilter(IMessageFilterUI *pFilter);

    int GetPostPaintCount();
    bool IsPostPaint(CControlUI *pControl);
    bool AddPostPaint(CControlUI *pControl);
    bool RemovePostPaint(CControlUI *pControl);
    bool SetPostPaintIndex(CControlUI *pControl, int iIndex);

    int GetPaintChildWndCount();
    bool AddPaintChildWnd(HWND hChildWnd);
    bool RemovePaintChildWnd(HWND hChildWnd);

    void AddDelayedCleanup(CControlUI *pControl);

    bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
    bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
    bool TranslateAccelerator(LPMSG pMsg);

    CControlUI *GetRoot() const;
    CControlUI *FindControl(POINT pt) const;
    CControlUI *FindControl(LPCTSTR pstrName) const;
    CControlUI *FindSubControlByPoint(CControlUI *pParent, POINT pt) const;
    CControlUI *FindSubControlByName(CControlUI *pParent, LPCTSTR pstrName) const;
    CControlUI *FindSubControlByClass(CControlUI *pParent, LPCTSTR pstrClass, int iIndex = 0);
    CStdPtrArray *FindSubControlsByClass(CControlUI *pParent, LPCTSTR pstrClass);

    static void MessageLoop();
    static void ProcessMessage(MessageType t = MessageType::AllMessage, bool *quit = nullptr);
    static bool TranslateMessage(const LPMSG pMsg);

    void ResetDPIAssets();
    void RebuildFont(TFontInfo *pFontInfo);
    void SetDPI(int iDPI);
    static void SetAllDPI(int iDPI, bool isMainScreen);
    static void UpdateDPIByMonitor(HWND hWnd);
    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
    bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
    void UsedVirtualWnd(bool bUsed);

    void OffscreenPaint(RECT &rcPaint, RECT &rcClient, DWORD &dwWidth, DWORD &dwHeight);
    void UpdateLayered(RECT &rcPaint, RECT &rcClient, DWORD &dwWidth, DWORD &dwHeight);
    void PaintUpdate(RECT &rcClient, bool &bNeedSizeMsg);
    void TextPaintUpdate(LPCTSTR language);
    void SendPointerUpEvent(POINT pt, WPARAM wParam, LPARAM lParam);
    void SendPointerDownEvent(POINT pt, WPARAM wParam, LPARAM lParam);
    bool OnWM_PAINT(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
    void EnableBlurRect();
    void EnableBlurRect(const RECT &rc);
    static void SetBlurMode(HWND hwnd, ACCENT_STATE mode, DWORD color);
    void SetTaskbarState(TaskbarState state);
    void SetTaskbarProgress(int percent);

    static void AddUserValue(const DuiLib::CDuiString &key, const DuiLib::CDuiString &val);

    static DuiLib::CDuiString GetUserValue(const DuiLib::CDuiString &key);
    CControlUI *GetOptionGroupSelected(LPCTSTR pStrGroupName);
    void RemoveAllImagesForDPIChange(bool bShared = false);
    SIZE GetOrgiSize();
    void SetOrgiSize(int cx, int cy);
    SIZE GetFixOrgiSize();

    template <typename T>
    void GetControlByName(T **ctrl, LPCTSTR name_)
    {
        (*ctrl) = dynamic_cast<T *>(FindControl(name_));
    }

    template <typename T>
    void GetSubControlByName(DuiLib::CControlUI *parent, T **ctrl, LPCTSTR name_)
    {
        (*ctrl) = dynamic_cast<T *>(FindSubControlByName(parent, name_));
    }

    void CaptureMouse(DuiLib::CControlUI *ctrl, bool capture);
    // 获取当前快捷控件
    DuiLib::CControlUI *GetShortCutCtr() const
    {
        return m_shortCutCtr;
    }
    // 设置当前快捷控件，设置快捷控件，解决点击enter键触发任意按钮操作的问题
    void SetShortCutCtr(DuiLib::CControlUI *val)
    {
        m_shortCutCtr = val;
    }

private:
    CStdPtrArray *GetFoundControls();
    static CControlUI *CALLBACK __FindControlFromNameHash(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromCount(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromPoint(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromTab(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromShortcut(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromName(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlFromClass(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlsFromClass(CControlUI *pThis, LPVOID pData);
    static CControlUI *CALLBACK __FindControlsFromUpdate(CControlUI *pThis, LPVOID pData);

    static void AdjustSharedImagesHSL();
    void AdjustImagesHSL();
    void KillTimerWhenDestory(CControlUI *pControl);

public:
    HWND m_hWndPaint;   // 所附加的窗体的句柄
    HWND m_hwndTooltip; // 提示信息
    RECT m_rcSizeBox;
    TResInfo m_ResInfo;
    HDC m_hDcOffscreen;
    HDC m_hDcPaint;
    CRichEditUI *m_currentCaretObject;
    CControlUI *m_pRoot;
    CControlUI *m_pFocus;
    // 窗口阴影
    CShadowUI m_shadow;
    RECT m_rcLayeredInset;
    CStdPtrArray m_aFoundControls;
    RECT m_rcCaption;

private:
    DuiLib::CControlUI *controlCaptureMouse = nullptr;
    CDuiString m_sName;
    HDC m_hDcBackground;
    HBITMAP m_hbmpOffscreen;
    BYTE *m_pOffscreenBits;
    HBITMAP m_hbmpBackground;
    COLORREF *m_pBackgroundBits;
    RECT m_rcBlur;
    bool m_bBlur = false;

    bool m_bShowUpdateRect;
    // 是否开启Gdiplus
    bool m_bUseGdiplusText;
    int m_trh;
    ULONG_PTR m_gdiplusToken;
    Gdiplus::GdiplusStartupInput *m_pGdiplusStartupInput;

    TOOLINFO m_ToolTip;

    // RichEdit光标
    RECT m_rtCaret;
    bool m_bCaretActive;
    bool m_bCaretShowing;

    bool TranspostMouseEvent(int Type, POINT ptMouse, WORD wKeyState, WPARAM wParam, LPARAM lParam);

    CControlUI *m_pEventKey;
    CControlUI *m_pEventPointer;
    POINT m_ptLastMousePos;
    POINT m_ptLastPointerPos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    SIZE m_szRoundCorner;
    UINT m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bOffscreenPaint;

    BYTE m_nOpacity;
    bool m_bLayered;
    bool m_bLayeredChanged;
    RECT m_rcLayeredUpdate;

    bool m_bMouseTracking;
    bool m_bMouseCapture;
    bool m_bIsPainting = false;
    bool m_bUsedVirtualWnd;
    bool m_bPointerCapture;

    CStdPtrArray m_aNotifiers;
    CStdPtrArray m_aTimers;
    CStdPtrArray m_aTranslateAccelerator;
    CStdPtrArray m_aPreMessageFilters;
    CStdPtrArray m_aMessageFilters;
    CStdPtrArray m_aPostPaintControls;
    CStdPtrArray m_aChildWnds;
    CStdPtrArray m_aDelayedCleanup;
    CStdPtrArray m_aAsyncNotify;
    CStdStringPtrMap m_mNameHash;
    CStdStringPtrMap m_mWindowCustomAttrHash;
    CStdStringPtrMap m_mOptionGroup;
    static std::thread::id uithreadID;
    static bool uithreadIDUpdated;
    static std::map<DuiLib::CDuiString, DuiLib::CDuiString> userValueMap;
    bool m_bForceUseSharedRes;

    // 拖拽
    bool m_bDragMode;
    HBITMAP m_hDragBitmap;

    int m_curScale;

    SIZE m_szOrigWindowSize;

    // 添加快捷控件
    CControlUI *m_shortCutCtr = nullptr;
    //
    static HINSTANCE m_hInstance;
    static HINSTANCE m_hResourceInstance;
    static CDuiString m_pStrResourcePath;
    static CDuiString m_pStrResourceZip;
    static HANDLE m_hResourceZip;
    static bool m_bCachedResourceZip;
    static int m_nResType;
    static TResInfo m_SharedResInfo;
    static bool m_bUseHSL;
    static short m_H;
    static short m_S;
    static short m_L;
    static CStdPtrArray m_aPreMessages;
    static CStdPtrArray m_aPlugins;
    HMODULE m_mod = nullptr;
};
} // namespace DuiLib

#endif // __UIMANAGER_H__
