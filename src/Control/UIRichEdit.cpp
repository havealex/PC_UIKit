/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include <codecvt>

// These constants are for backward compatibility. They are the
// sizes used for initialization and reset in RichEdit 1.0
namespace {
    const int CODE_PAGE = 1200;
    const int NUM_BASE_COLOR = 16;
    const int NUM_BASE_PADDING = 10;
}

namespace DuiLib {
    const LONG cInitTextMax = 32767;


EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    { 0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 } };

EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    { 0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5 }
};

#ifndef LY_PER_INCH
constexpr int LY_PER_INCH = 1440;
#endif

#ifndef HIMETRIC_PER_INCH
constexpr int HIMETRIC_PER_INCH = 2540;
#endif

#include <textserv.h>

class CRTFStreamIn {
public:
    explicit CRTFStreamIn(LPCWSTR pText)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
        m_pText = converter.to_bytes(pText).c_str();
        m_nWritedByteCount = 0;
    }
    ~CRTFStreamIn() {}

    string m_pText;
    int m_nWritedByteCount;
};

class CTxtWinHost : public ITextHost {
public:
    CTxtWinHost();
    BOOL Init(CRichEditUI *re, const CREATESTRUCT *pcs);
    virtual ~CTxtWinHost();

    ITextServices *GetTextServices(void)
    {
        return pserv;
    }
    void SetClientRect(RECT *prc);
    RECT *GetClientRect()
    {
        return &rcClient;
    }
    BOOL GetWordWrap(void)
    {
        return fWordWrap;
    }
    void SetWordWrap(BOOL fWordWrap);
    BOOL GetReadOnly();
    void SetReadOnly(BOOL fReadOnly);
    void SetFont(HFONT hFont);
    void SetColor(DWORD dwColor);
    SIZEL *GetExtent();
    void SetExtent(SIZEL *psizelExtent);
    void LimitText(LONG nChars);
    BOOL IsCaptured();
    BOOL IsShowCaret();
    void NeedFreshCaret();
    INT GetCaretWidth();
    INT GetCaretHeight();

    BOOL GetAllowBeep();
    void SetAllowBeep(BOOL fAllowBeep);
    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);
    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fNew);
    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);
    BOOL SetSaveSelection(BOOL fSaveSelection);
    HRESULT OnTxInPlaceDeactivate();
    HRESULT OnTxInPlaceActivate(LPCRECT prcClient);
    BOOL GetActiveState(void)
    {
        return fInplaceActive;
    }
    BOOL DoSetCursor(RECT *prc, POINT *pt);
    void SetTransparent(BOOL fTransparent);
    void GetControlRect(LPRECT prc);
    LONG SetAccelPos(LONG laccelpos);
    WCHAR SetPasswordChar(WCHAR chPasswordChar);
    void SetDisabled(BOOL fOn);
    LONG SetSelBarWidth(LONG lSelBarWidth);
    BOOL GetTimerState();

    void SetCharFormat(CHARFORMAT2W &c);
    void SetParaFormat(PARAFORMAT2 &p);

    // -----------------------------
    // 	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    // 	ITextHost interface
    // -----------------------------
    virtual HDC TxGetDC();
    virtual INT TxReleaseDC(HDC hdc);
    virtual BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
    virtual BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
    virtual BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
    virtual BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
    virtual void TxInvalidateRect(LPCRECT prc, BOOL fMode);
    virtual void TxViewChange(BOOL fUpdate);
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
    virtual BOOL TxShowCaret(BOOL fShow);
    virtual BOOL TxSetCaretPos(INT x, INT y);
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
    virtual void TxKillTimer(UINT idTimer);
    virtual void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate,
        LPRECT lprcUpdate, UINT fuScroll);
    virtual void TxSetCapture(BOOL fCapture);
    virtual void TxSetFocus();
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText);
    virtual BOOL TxScreenToClient(LPPOINT lppt);
    virtual BOOL TxClientToScreen(LPPOINT lppt);
    virtual HRESULT TxActivate(LONG *plOldState);
    virtual HRESULT TxDeactivate(LONG lNewState);
    virtual HRESULT TxGetClientRect(LPRECT prc);
    virtual HRESULT TxGetViewInset(LPRECT prc);
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
    virtual HRESULT TxGetParaFormat(const PARAFORMAT **ppPF);
    virtual COLORREF TxGetSysColor(int nIndex);
    virtual HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle);
    virtual HRESULT TxGetMaxLength(DWORD *plength);
    virtual HRESULT TxGetScrollBars(DWORD *pdwScrollBar);
    virtual HRESULT TxGetPasswordChar(TCHAR *pch);
    virtual HRESULT TxGetAcceleratorPos(LONG *pcp);
    virtual HRESULT TxGetExtent(LPSIZEL lpExtent);
    virtual HRESULT OnTxCharFormatChange(const CHARFORMATW *pcf);
    virtual HRESULT OnTxParaFormatChange(const PARAFORMAT *ppf);
    virtual HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
    virtual HRESULT TxNotify(DWORD iNotify, void *pv);
    virtual HIMC TxImmGetContext(void);
    virtual void TxImmReleaseContext(HIMC himc);
    virtual HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth);

public:
    RECT rcClient = { 0 };        // Client Rect for this control
    ITextServices *pserv = nullptr; // pointer to Text Services object
    SIZEL sizelExtent = { 0 };    // Extent array

private:
    CRichEditUI *m_re;
    ULONG cRefs; // Reference Count
    // Properties
    DWORD dwStyle; // style bits

    unsigned fEnableAutoWordSel : 1; // enable Word style auto word selection?
    unsigned fWordWrap : 1;          // Whether control should word wrap
    unsigned fAllowBeep : 1;         // Whether beep is allowed
    unsigned fRich : 1;              // Whether control is rich text
    unsigned fSaveSelection : 1;     // Whether to save the selection when inactive
    unsigned fInplaceActive : 1;     // Whether control is inplace active
    unsigned fTransparent : 1;       // Whether control is transparent
    unsigned fTimer : 1;             // A timer is set
    unsigned fCaptured : 1;
    unsigned fShowCaret : 1;
    unsigned fNeedFreshCaret : 1; // 修正改变大小后点击其他位置原来光标不能消除的问题

    INT iCaretWidth = 0;
    INT iCaretHeight = 0;
    INT iCaretLastWidth = 0;
    INT iCaretLastHeight = 0;

    LONG lSelBarWidth = 0; // Width of the selection bar
    LONG cchTextMost = 0;  // maximum text size
    DWORD dwEventMask = 0; // DoEvent mask to pass on to parent window
    LONG icf = 0;
    LONG ipf = 0;
    CHARFORMAT2W cf;      // Default character format
    PARAFORMAT2 pf;       // Default paragraph format
    LONG laccelpos = 0;       // Accelerator position
    WCHAR chPasswordChar = L'*'; // Password character
    HFONT m_hFont = nullptr;
};

// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

HRESULT InitDefaultCharFormat(CRichEditUI *re, CHARFORMAT2W *pcf, HFONT hfont)
{
    SecureZeroMemory(pcf, sizeof(CHARFORMAT2W));
    LOGFONT lf;
    if (re == nullptr) {
        return E_POINTER;
    }
    if (!hfont) {
        hfont = re->GetManager()->GetFont(re->GetFont());
    }
    ::GetObject(hfont, sizeof(LOGFONT), &lf);

    DWORD dwColor = re->GetTextColor();
    if (re->GetManager()->IsLayered()) {
        CRenderEngine::CheckAlphaColor(dwColor);
    }
    if (pcf == nullptr) {
        return S_FALSE;
    }
    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    (void)CResourceManager::GetInstance()->GetTrueDPI();
    pcf->yHeight = -lf.lfHeight * LY_PER_INCH / 96;
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;
    if (lf.lfWeight >= FW_BOLD) {
        pcf->dwEffects |= CFE_BOLD;
    }
    if (lf.lfItalic) {
        pcf->dwEffects |= CFE_ITALIC;
    }
    if (lf.lfUnderline) {
        pcf->dwEffects |= CFE_UNDERLINE;
    }
    pcf->bCharSet = lf.lfCharSet;
    pcf->bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy_s(pcf->szFaceName, lf.lfFaceName);
#else
    // need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE);
#endif

    return S_OK;
}

HRESULT InitDefaultParaFormat(CRichEditUI *re, PARAFORMAT2 *ppf)
{
    SecureZeroMemory(ppf, sizeof(PARAFORMAT2));
    if (ppf == nullptr) {
        return S_FALSE;
    }
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->wAlignment = PFA_LEFT;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

    return S_OK;
}

HRESULT CreateHost(CRichEditUI *re, const CREATESTRUCT *pcs, CTxtWinHost **pptec)
{
    HRESULT hr = E_FAIL;

    auto *phost = new (std::nothrow) CTxtWinHost();
    if (phost) {
        if (phost->Init(re, pcs) && pptec != nullptr) {
            *pptec = phost;
            hr = S_OK;
        }
    }

    if (FAILED(hr)) {
        delete phost;
    }

    return S_OK;
}

CTxtWinHost::CTxtWinHost() : m_re(nullptr), pserv(nullptr)
{
    ::ZeroMemory(&cRefs, sizeof(CTxtWinHost) - offsetof(CTxtWinHost, cRefs));
    cchTextMost = cInitTextMax;
    laccelpos = -1;
    m_hFont = nullptr;
}

CTxtWinHost::~CTxtWinHost()
{
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxInPlaceDeactivate();
    pserv->Release();
}
BOOL CTxtWinHost::Init(CRichEditUI *re, const CREATESTRUCT *pcs)
{
    IUnknown *pUnk = nullptr;
    HRESULT hr;
    CDuiString dllFullPath;
    if (re == nullptr) {
        return FALSE;
    }

    m_re = re;
    // Initialize Reference count
    cRefs = 1;

    // Create and cache CHARFORMAT for this control
    if (FAILED(InitDefaultCharFormat(re, &cf, nullptr))) {
        goto err;
    }

    // Create and cache PARAFORMAT for this control
    if (FAILED(InitDefaultParaFormat(re, &pf))) {
        goto err;
    }

    // edit controls created without a window are multiline by default
    // so that paragraph formats can be
    dwStyle = ES_MULTILINE;

    // edit controls are rich by default
    fRich = static_cast<unsigned>(re->IsRich());

    cchTextMost = re->GetLimitText();

    if (pcs) {
        dwStyle = pcs->style;
        if (!(dwStyle & (ES_AUTOHSCROLL | WS_HSCROLL))) {
            fWordWrap = TRUE;
        }
    }

    if (!(dwStyle & ES_LEFT)) {
        if (dwStyle & ES_CENTER) {
            pf.wAlignment = PFA_CENTER;
        } else if (dwStyle & ES_RIGHT) {
            pf.wAlignment = PFA_RIGHT;
        }
    }

    fInplaceActive = TRUE;

    PCreateTextServices TextServicesProc = nullptr;
#ifdef _UNICODE
    dllFullPath = GetSystemDllPath(_T("Msftedit.dll"));
    HMODULE hmod = LoadLibrary(dllFullPath.GetData());
#else
    dllFullPath = GetSystemDllPath(_T("Riched20.dll"));
    HMODULE hmod = LoadLibrary(dllFullPath.GetData());
#endif
    if (hmod) {
        TextServicesProc = (PCreateTextServices)GetProcAddress(hmod, "CreateTextServices");
    }
    if (TextServicesProc) {
        HRESULT hr = TextServicesProc(nullptr, this, &pUnk);
    }
    if (pUnk == nullptr) {
        FreeLibrary(hmod);
        return FALSE;
    }
    hr = pUnk->QueryInterface(IID_ITextServices, reinterpret_cast<void **>(&pserv));

    // Whether the previous call succeeded or failed we are done
    // with the private interface.
    pUnk->Release();

    if (FAILED(hr)) {
        goto err;
    }

    // Set window text
    if (pcs && pcs->lpszName) {
#ifdef _UNICODE
        if (pserv == nullptr) {
            return FALSE;
        }
        if (FAILED(pserv->TxSetText((TCHAR *)pcs->lpszName))) {
            goto err;
        }
#else
        size_t iLen = _tcslen(pcs->lpszName);
        if (iLen == nullptr) {
            return FALSE;
        }
        LPWSTR lpText = new WCHAR[iLen + 1];
        ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pcs->lpszName, -1, (LPWSTR)lpText, iLen);
        if (FAILED(pserv->TxSetText((LPWSTR)lpText))) {
            delete[] lpText;
            goto err;
        }
        delete[] lpText;
#endif
    }

    return TRUE;

err:
    return FALSE;
}

HRESULT CTxtWinHost::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    if (ppvObject == nullptr) {
        return hr;
    }
    *ppvObject = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextHost)) {
        AddRef();
        *ppvObject = reinterpret_cast<ITextHost *>(this);
        hr = S_OK;
    }

    return hr;
}

ULONG CTxtWinHost::AddRef(void)
{
    return ++cRefs;
}

ULONG CTxtWinHost::Release(void)
{
    ULONG c_Refs = --cRefs;

    if (c_Refs == 0) {
        delete this;
    }

    return c_Refs;
}

HIMC CTxtWinHost::TxImmGetContext(void)
{
    return nullptr;
}

void CTxtWinHost::TxImmReleaseContext(HIMC himc) {}

HDC CTxtWinHost::TxGetDC()
{
    return m_re->GetManager()->GetPaintDC();
}

int CTxtWinHost::TxReleaseDC(HDC hdc)
{
    return 1;
}

BOOL CTxtWinHost::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    CScrollBarUI *pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI *pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if (fnBar == SB_VERT && pVerticalScrollBar) {
        pVerticalScrollBar->SetVisible(fShow == TRUE);
    } else if (fnBar == SB_HORZ && pHorizontalScrollBar) {
        pHorizontalScrollBar->SetVisible(fShow == TRUE);
    } else if (fnBar == SB_BOTH) {
        if (pVerticalScrollBar) {
            pVerticalScrollBar->SetVisible(fShow == TRUE);
        }
        if (pHorizontalScrollBar) {
            pHorizontalScrollBar->SetVisible(fShow == TRUE);
        }
    }
    return TRUE;
}

BOOL CTxtWinHost::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
    if (m_re == nullptr || m_re->GetVerticalScrollBar() == nullptr) {
        return FALSE;
    }
    if (fuSBFlags == SB_VERT) {
        m_re->EnableScrollBar(true, m_re->GetHorizontalScrollBar() != nullptr);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    } else if (fuSBFlags == SB_HORZ) {
        m_re->EnableScrollBar(m_re->GetVerticalScrollBar() != nullptr, true);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    } else if (fuSBFlags == SB_BOTH) {
        m_re->EnableScrollBar(true, true);
        m_re->GetVerticalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
        m_re->GetHorizontalScrollBar()->SetVisible(fuArrowflags != ESB_DISABLE_BOTH);
    }
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
    CScrollBarUI *pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI *pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if (fnBar == SB_VERT && pVerticalScrollBar) {
        if (nMaxPos - nMinPos - rcClient.bottom + rcClient.top <= 0) {
            pVerticalScrollBar->SetVisible(false);
        } else {
            pVerticalScrollBar->SetVisible(true);
            pVerticalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.bottom + rcClient.top);
        }
    } else if (fnBar == SB_HORZ && pHorizontalScrollBar) {
        if (nMaxPos - nMinPos - rcClient.right + rcClient.left <= 0) {
            pHorizontalScrollBar->SetVisible(false);
        } else {
            pHorizontalScrollBar->SetVisible(true);
            pHorizontalScrollBar->SetScrollRange(nMaxPos - nMinPos - rcClient.right + rcClient.left);
        }
    }
    return TRUE;
}

BOOL CTxtWinHost::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
    CScrollBarUI *pVerticalScrollBar = m_re->GetVerticalScrollBar();
    CScrollBarUI *pHorizontalScrollBar = m_re->GetHorizontalScrollBar();
    if (fnBar == SB_VERT && pVerticalScrollBar) {
        pVerticalScrollBar->SetScrollPos(nPos);
    } else if (fnBar == SB_HORZ && pHorizontalScrollBar) {
        pHorizontalScrollBar->SetScrollPos(nPos);
    }
    return TRUE;
}

void CTxtWinHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (prc == nullptr) {
        m_re->GetManager()->Invalidate(rcClient);
        return;
    }
    RECT rc = *prc;
    m_re->GetManager()->Invalidate(rc);
}

void CTxtWinHost::TxViewChange(BOOL fUpdate)
{
    if (m_re == nullptr) {
        return;
    }
    if (m_re->OnTxViewChanged(fUpdate)) {
        m_re->Invalidate();
    }
}

BOOL CTxtWinHost::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    iCaretWidth = xWidth;
    iCaretHeight = yHeight;
    if (m_re->GetManager()->IsLayered()) {
        return m_re->GetManager()->CreateCaret(hbmp, xWidth, yHeight);
    } else {
        return ::CreateCaret(m_re->GetManager()->GetPaintWindow(), hbmp, xWidth, yHeight);
    }
}

BOOL CTxtWinHost::TxShowCaret(BOOL fShow)
{
    fShowCaret = fShow;
    if (m_re == nullptr) {
        return FALSE;
    }
    if (m_re->GetManager()->IsLayered()) {
        if (m_re->GetManager()->GetCurrentCaretObject() == m_re) {
            if ((m_re->IsReadOnly() || !m_re->Activate())) {
                m_re->GetManager()->ShowCaret(false);
                return TRUE;
            }
        }

        return m_re->GetManager()->ShowCaret(fShow == TRUE);
    } else {
        if (fShow) {
            return ::ShowCaret(m_re->GetManager()->GetPaintWindow());
        } else {
            return ::HideCaret(m_re->GetManager()->GetPaintWindow());
        }
    }
}

BOOL CTxtWinHost::TxSetCaretPos(INT x, INT y)
{
    if (m_re->GetManager()->IsLayered()) {
        m_re->GetManager()->SetCaretPos(m_re, x, y);
        return true;
    } else {
        return ::SetCaretPos(x, y);
    }
}

BOOL CTxtWinHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    fTimer = TRUE;
    return m_re->GetManager()->SetTimer(m_re, idTimer, uTimeout) == TRUE;
}

void CTxtWinHost::TxKillTimer(UINT idTimer)
{
    m_re->GetManager()->KillTimer(m_re, idTimer);
    fTimer = FALSE;
}

void CTxtWinHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate,
    LPRECT lprcUpdate, UINT fuScroll)
{
    return;
}

void CTxtWinHost::TxSetCapture(BOOL fCapture)
{
    if (fCapture) {
        m_re->GetManager()->SetCapture();
    } else {
        m_re->GetManager()->ReleaseCapture();
    }
    fCaptured = fCapture;
}

void CTxtWinHost::TxSetFocus()
{
    if (m_re == nullptr) {
        return;
    }
    m_re->SetFocus();
}

void CTxtWinHost::TxSetCursor(HCURSOR hcur, BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL CTxtWinHost::TxScreenToClient(LPPOINT lppt)
{
    return ::ScreenToClient(m_re->GetManager()->GetPaintWindow(), lppt);
}

BOOL CTxtWinHost::TxClientToScreen(LPPOINT lppt)
{
    return ::ClientToScreen(m_re->GetManager()->GetPaintWindow(), lppt);
}

HRESULT CTxtWinHost::TxActivate(LONG *plOldState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetClientRect(LPRECT prc)
{
    *prc = rcClient;
    GetControlRect(prc);
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetViewInset(LPRECT prc)
{
    prc->left = prc->right = prc->top = prc->bottom = 0;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    if (ppCF == nullptr) {
        return S_FALSE;
    }
    *ppCF = &cf;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    if (ppPF == nullptr) {
        return S_FALSE;
    }
    *ppPF = &pf;
    return NOERROR;
}

COLORREF CTxtWinHost::TxGetSysColor(int nIndex)
{
    DWORD dwColor = ::GetSysColor(nIndex);
    CRenderEngine::CheckAlphaColor(dwColor);
    return dwColor;
}

HRESULT CTxtWinHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    if (pstyle == nullptr) {
        return S_FALSE;
    }
    *pstyle = !fTransparent ? TXTBACK_OPAQUE : TXTBACK_TRANSPARENT;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetMaxLength(DWORD *pLength)
{
    if (pLength == nullptr) {
        return S_FALSE;
    }
    *pLength = cchTextMost;
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetScrollBars(DWORD *pdwScrollBar)
{
    if (pdwScrollBar == nullptr) {
        return S_FALSE;
    }
    *pdwScrollBar = dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return NOERROR;
}

HRESULT CTxtWinHost::TxGetPasswordChar(TCHAR *pch)
{
#ifdef _UNICODE
    if (pch == nullptr) {
        return S_FALSE;
    }
    *pch = chPasswordChar;
#else
    ::WideCharToMultiByte(CP_ACP, 0, &chPasswordChar, 1, pch, 1, nullptr, nullptr);
#endif
    return NOERROR;
}

HRESULT CTxtWinHost::TxGetAcceleratorPos(LONG *pcp)
{
    if (pcp == nullptr) {
        return S_FALSE;
    }
    *pcp = laccelpos;
    return S_OK;
}

HRESULT CTxtWinHost::OnTxCharFormatChange(const CHARFORMATW *pcf)
{
    return S_OK;
}

HRESULT CTxtWinHost::OnTxParaFormatChange(const PARAFORMAT *ppf)
{
    return S_OK;
}

HRESULT CTxtWinHost::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
    DWORD dwProperties = 0;

    if (fRich) {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (dwStyle & ES_MULTILINE) {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (dwStyle & ES_READONLY) {
        dwProperties |= TXTBIT_READONLY;
    }

    if (dwStyle & ES_PASSWORD) {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(dwStyle & ES_NOHIDESEL)) {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (fEnableAutoWordSel) {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (fWordWrap) {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (fAllowBeep) {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (fSaveSelection) {
        dwProperties |= TXTBIT_SAVESELECTION;
    }
    if (pdwBits == nullptr) {
        return S_FALSE;
    }
    *pdwBits = dwProperties & dwMask;
    return NOERROR;
}


HRESULT CTxtWinHost::TxNotify(DWORD iNotify, void *pv)
{
    if (iNotify == EN_REQUESTRESIZE) {
        RECT rc;
        REQRESIZE *preqsz = reinterpret_cast<REQRESIZE *>(pv);
        if (preqsz == nullptr) {
            return S_FALSE;
        }

        GetControlRect(&rc);
        rc.bottom = rc.top + preqsz->rc.bottom;
        rc.right = rc.left + preqsz->rc.right;
        SetClientRect(&rc);
    }
    if (m_re == nullptr) {
        return S_FALSE;
    }
    m_re->OnTxNotify(iNotify, pv);
    return S_OK;
}

HRESULT CTxtWinHost::TxGetExtent(LPSIZEL lpExtent)
{
    *lpExtent = sizelExtent;
    return S_OK;
}

HRESULT CTxtWinHost::TxGetSelectionBarWidth(LONG *plSelBarWidth)
{
    if (plSelBarWidth == nullptr) {
        return S_FALSE;
    }
    *plSelBarWidth = lSelBarWidth;
    return S_OK;
}

void CTxtWinHost::SetWordWrap(BOOL fWordWrap)
{
    fWordWrap = fWordWrap;
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL CTxtWinHost::GetReadOnly()
{
    return (dwStyle & ES_READONLY) != 0;
}

void CTxtWinHost::SetReadOnly(BOOL fReadOnly)
{
    if (fReadOnly) {
        dwStyle |= ES_READONLY;
    } else {
        dwStyle &= ~ES_READONLY;
    }
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_READONLY, fReadOnly ? TXTBIT_READONLY : 0);
}

void CTxtWinHost::SetFont(HFONT hFont)
{
    if (hFont == nullptr || hFont == m_hFont) {
        return;
    }
    m_hFont = hFont;
    LOGFONT lf;
    ::GetObject(hFont, sizeof(LOGFONT), &lf);
    // LONG yPixPerInch = ::GetDeviceCaps(m_re->GetManager()->GetPaintDC(), LOGPIXELSY);
    LONG yPixPerInch = static_cast<LONG>(CResourceManager::GetInstance()->GetTrueDPI());

    cf.yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
    if (lf.lfWeight >= FW_BOLD) {
        cf.dwEffects |= CFE_BOLD;
    }
    if (lf.lfItalic) {
        cf.dwEffects |= CFE_ITALIC;
    }
    if (lf.lfUnderline) {
        cf.dwEffects |= CFE_UNDERLINE;
    }
    cf.bCharSet = lf.lfCharSet;
    cf.bPitchAndFamily = lf.lfPitchAndFamily;
#ifdef _UNICODE
    _tcscpy_s(cf.szFaceName, lf.lfFaceName);
#else
    // need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, cf.szFaceName, LF_FACESIZE);
#endif

    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

void CTxtWinHost::SetColor(DWORD dwColor)
{
    CRenderEngine::CheckAlphaColor(dwColor);
    cf.crTextColor = RGB(GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

SIZEL *CTxtWinHost::GetExtent()
{
    return &sizelExtent;
}

void CTxtWinHost::SetExtent(SIZEL *psizelExtent)
{
    if (psizelExtent == nullptr || pserv == nullptr) {
        return;
    }
    sizelExtent = *psizelExtent;
    pserv->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE, TXTBIT_EXTENTCHANGE);
}

void CTxtWinHost::LimitText(LONG nChars)
{
    cchTextMost = nChars;
    if (cchTextMost <= 0) {
        cchTextMost = cInitTextMax;
    }
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

BOOL CTxtWinHost::IsCaptured()
{
    return fCaptured;
}

BOOL CTxtWinHost::IsShowCaret()
{
    return fShowCaret;
}

void CTxtWinHost::NeedFreshCaret()
{
    fNeedFreshCaret = TRUE;
}

INT CTxtWinHost::GetCaretWidth()
{
    return iCaretWidth;
}

INT CTxtWinHost::GetCaretHeight()
{
    return iCaretHeight;
}

BOOL CTxtWinHost::GetAllowBeep()
{
    return fAllowBeep;
}

void CTxtWinHost::SetAllowBeep(BOOL fAllowBeep)
{
    fAllowBeep = fAllowBeep;
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, fAllowBeep ? TXTBIT_ALLOWBEEP : 0);
}

WORD CTxtWinHost::GetDefaultAlign()
{
    return pf.wAlignment;
}

void CTxtWinHost::SetDefaultAlign(WORD wNewAlign)
{
    pf.wAlignment = wNewAlign;
    if (pserv == nullptr) {
        return;
    }
    // Notify control of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL CTxtWinHost::GetRichTextFlag()
{
    return fRich;
}

void CTxtWinHost::SetRichTextFlag(BOOL fNew)
{
    fRich = fNew;
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, fNew ? TXTBIT_RICHTEXT : 0);
}

LONG CTxtWinHost::GetDefaultLeftIndent()
{
    return pf.dxOffset;
}

void CTxtWinHost::SetDefaultLeftIndent(LONG lNewIndent)
{
    pf.dxOffset = lNewIndent;
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

void CTxtWinHost::SetClientRect(RECT *prc)
{
    if (prc == nullptr || pserv == nullptr) {
        return;
    }
    rcClient = *prc;
    LONG xPerInch = static_cast<LONG>(CResourceManager::GetInstance()->GetTrueDPI());
    LONG yPerInch = static_cast<LONG>(CResourceManager::GetInstance()->GetTrueDPI());

    sizelExtent.cx = DXtoHimetricX(rcClient.right - rcClient.left, xPerInch);
    sizelExtent.cy = DYtoHimetricY(rcClient.bottom - rcClient.top, yPerInch);
    pserv->OnTxPropertyBitsChange(TXTBIT_VIEWINSETCHANGE, TXTBIT_VIEWINSETCHANGE);
}

BOOL CTxtWinHost::SetSaveSelection(BOOL f_SaveSelection)
{
    BOOL fResult = f_SaveSelection;
    fSaveSelection = f_SaveSelection;
    if (pserv == nullptr) {
        return FALSE;
    }
    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION, fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;
}

HRESULT CTxtWinHost::OnTxInPlaceDeactivate()
{
    if (pserv == nullptr) {
        return FALSE;
    }
    HRESULT hr = pserv->OnTxInPlaceDeactivate();

    if (SUCCEEDED(hr)) {
        fInplaceActive = FALSE;
    }

    return hr;
}

HRESULT CTxtWinHost::OnTxInPlaceActivate(LPCRECT prcClient)
{
    fInplaceActive = TRUE;
    if (pserv == nullptr) {
        return FALSE;
    }
    HRESULT hr = pserv->OnTxInPlaceActivate(prcClient);

    if (FAILED(hr)) {
        fInplaceActive = FALSE;
    }

    return hr;
}

BOOL CTxtWinHost::DoSetCursor(RECT *prc, POINT *pt)
{
    RECT rc = prc ? *prc : rcClient;
    if (pt == nullptr || pserv == nullptr) {
        return FALSE;
    }
    // Is this in our rectangle?
    if (PtInRect(&rc, *pt)) {
        RECT *prcClient = (!fInplaceActive || prc) ? &rc : nullptr;
        pserv->OnTxSetCursor(DVASPECT_CONTENT, -1, nullptr, nullptr, m_re->GetManager()->GetPaintDC(),
            nullptr, prcClient, pt->x, pt->y);

        return TRUE;
    }

    return FALSE;
}

void CTxtWinHost::GetControlRect(LPRECT prc)
{
    prc->top = rcClient.top;
    prc->bottom = rcClient.bottom;
    prc->left = rcClient.left;
    prc->right = rcClient.right;
}

void CTxtWinHost::SetTransparent(BOOL f_Transparent)
{
    fTransparent = f_Transparent;
    if (pserv == nullptr) {
        return;
    }
    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, 0);
}

LONG CTxtWinHost::SetAccelPos(LONG l_accelpos)
{
    LONG laccelposOld = l_accelpos;

    laccelpos = l_accelpos;
    if (pserv == nullptr) {
        return laccelposOld;
    }
    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_SHOWACCELERATOR, 0);

    return laccelposOld;
}

WCHAR CTxtWinHost::SetPasswordChar(WCHAR ch_PasswordChar)
{
    WCHAR chOldPasswordChar = chPasswordChar;

    chPasswordChar = ch_PasswordChar;
    if (pserv == nullptr) {
        return chOldPasswordChar;
    }
    // notify text services of property change
    pserv->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, (chPasswordChar != 0) ? TXTBIT_USEPASSWORD : 0);

    return chOldPasswordChar;
}

void CTxtWinHost::SetDisabled(BOOL fOn)
{
    cf.dwMask |= CFM_COLOR | CFM_DISABLED;
    cf.dwEffects |= CFE_AUTOCOLOR | CFE_DISABLED;

    if (!fOn) {
        cf.dwEffects &= ~CFE_DISABLED;
    }
    if (pserv == nullptr) {
        return;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
}

LONG CTxtWinHost::SetSelBarWidth(LONG l_SelBarWidth)
{
    LONG lOldSelBarWidth = lSelBarWidth;

    lSelBarWidth = l_SelBarWidth;

    if (lSelBarWidth) {
        dwStyle |= ES_SELECTIONBAR;
    } else {
        dwStyle &= (~ES_SELECTIONBAR);
    }
    if (pserv == nullptr) {
        return lOldSelBarWidth;
    }
    pserv->OnTxPropertyBitsChange(TXTBIT_SELBARCHANGE, TXTBIT_SELBARCHANGE);

    return lOldSelBarWidth;
}

BOOL CTxtWinHost::GetTimerState()
{
    return fTimer;
}

void CTxtWinHost::SetCharFormat(CHARFORMAT2W &c)
{
    cf = c;
}

void CTxtWinHost::SetParaFormat(PARAFORMAT2 &p)
{
    pf = p;
}

IMPLEMENT_DUICONTROL(CRichEditUI)

CRichEditUI::CRichEditUI()
    : m_pTwh(nullptr),
      m_bVScrollBarFixing(false),
      m_bWantTab(true),
      m_bWantReturn(true),
      m_bWantCtrlReturn(true),
      m_bRich(true),
      m_bReadOnly(false),
      m_bWordWrap(false),
      m_dwTextColor(0),
      m_iFont(-1),
      m_iLimitText(cInitTextMax),
      m_lTwhStyle(ES_MULTILINE),
      m_bDrawCaret(true),
      m_bInited(false),
      m_chLeadByte(0),
      m_uButtonState(0),
      m_dwTipValueColor(0xFFBAC0C5),
      m_uTipValueAlign(DT_SINGLELINE | DT_LEFT)
{
#ifndef _UNICODE
    m_fAccumulateDBC = true;
#else
    m_fAccumulateDBC = false;
#endif
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
}

CRichEditUI::~CRichEditUI()
{
    if (m_pManager == nullptr) {
        return;
    }
    if (m_pTwh) {
        m_pTwh->Release();
        m_pManager->RemoveMessageFilter(this);
    }
}

LPCTSTR CRichEditUI::GetClass() const
{
    return _T("RichEditUI");
}

LPVOID CRichEditUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_RICHEDIT) == 0) {
        return static_cast<CRichEditUI *>(this);
    }
    return CContainerUI::GetInterface(pstrName);
}

UINT CRichEditUI::GetControlFlags() const
{
    if (!IsEnabled()) {
        return CControlUI::GetControlFlags();
    }

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

bool CRichEditUI::IsMultiLine()
{
    return (m_lTwhStyle & ES_MULTILINE) == ES_MULTILINE;
}

void CRichEditUI::SetMultiLine(bool bMultiLine)
{
    if (!bMultiLine) {
        m_lTwhStyle &= ~ES_MULTILINE;
    } else {
        m_lTwhStyle |= ES_MULTILINE;
    }
}

bool CRichEditUI::IsWantTab()
{
    return m_bWantTab;
}

void CRichEditUI::SetWantTab(bool bWantTab)
{
    m_bWantTab = bWantTab;
}

bool CRichEditUI::IsWantReturn()
{
    return m_bWantReturn;
}

void CRichEditUI::SetWantReturn(bool bWantReturn)
{
    m_bWantReturn = bWantReturn;
}

bool CRichEditUI::IsWantCtrlReturn()
{
    return m_bWantCtrlReturn;
}

void CRichEditUI::SetWantCtrlReturn(bool bWantCtrlReturn)
{
    m_bWantCtrlReturn = bWantCtrlReturn;
}

bool CRichEditUI::IsRich()
{
    return m_bRich;
}

void CRichEditUI::SetRich(bool bRich)
{
    m_bRich = bRich;
    if (m_pTwh) {
        m_pTwh->SetRichTextFlag(bRich);
    }
}

bool CRichEditUI::IsReadOnly()
{
    return m_bReadOnly;
}

void CRichEditUI::SetReadOnly(bool bReadOnly)
{
    m_bReadOnly = bReadOnly;
    if (m_pTwh) {
        m_pTwh->SetReadOnly(bReadOnly);
    }
}

bool CRichEditUI::GetWordWrap()
{
    return m_bWordWrap;
}

void CRichEditUI::SetWordWrap(bool bWordWrap)
{
    m_bWordWrap = bWordWrap;
    if (m_pTwh) {
        m_pTwh->SetWordWrap(bWordWrap);
    }
}

int CRichEditUI::GetFont()
{
    return m_iFont;
}

void CRichEditUI::SetFont(int index)
{
    m_iFont = index;
    if (m_pTwh) {
        m_pTwh->SetFont(GetManager()->GetFont(m_iFont));
    }
}

void CRichEditUI::SetDefaultAlign(WORD wNewAlign)
{
    if (m_pTwh) {
        m_pTwh->SetDefaultAlign(wNewAlign);
    }
}

void CRichEditUI::SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
{
    if (wstring(pStrFontName) == L"微软雅黑") {
        pStrFontName = L"Microsoft YaHei";
    }
    if (pStrFontName == nullptr) {
        return;
    }
    if (m_pTwh) {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        errno_t res = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, pStrFontName, LF_FACESIZE - 1);
        if (res != 0) {
        }
        // #if defined(JP_BUILD)
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            LOGFONT lfDef;
            if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lfDef, 0) != 0) {
                res = _tcsncpy_s(lf.lfFaceName, LF_FACESIZE, lfDef.lfFaceName, LF_FACESIZE - 1);
                if (res != 0) {
                }
            }
        }
        // #endif
        lf.lfCharSet = DEFAULT_CHARSET;
        // #if defined(JP_BUILD)
        if (UICulture::GetInstance()->GetCustomFont() == L"japan") {
            lf.lfCharSet = SHIFTJIS_CHARSET;
        }
        // #endif
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
        m_pTwh->SetFont(hFont);
        ::DeleteObject(hFont);
    }
}

void CRichEditUI::SetEnabled(bool bEnabled)
{
    if (m_bEnabled == bEnabled) {
        return;
    }
    if (m_pManager == nullptr) {
        return;
    }
    if (m_pTwh) {
        m_pTwh->SetColor(bEnabled ? m_dwTextColor : m_pManager->GetDefaultDisabledColor());
    }

    CContainerUI::SetEnabled(bEnabled);
}

LONG CRichEditUI::GetWinStyle()
{
    return static_cast<LONG>(m_lTwhStyle);
}

void CRichEditUI::SetWinStyle(LONG lStyle)
{
    m_lTwhStyle = static_cast<ULONG>(lStyle);
}

DWORD CRichEditUI::GetTextColor()
{
    return m_dwTextColor;
}

void CRichEditUI::SetTextColor(DWORD dwTextColor)
{
    m_dwTextColor = dwTextColor;
    if (m_pTwh) {
        m_pTwh->SetColor(dwTextColor);
    }
}

int CRichEditUI::GetLimitText()
{
    return m_iLimitText;
}

void CRichEditUI::SetLimitText(int iChars)
{
    m_iLimitText = iChars;
    if (m_pTwh) {
        m_pTwh->LimitText(m_iLimitText);
    }
}

long CRichEditUI::GetTextLength(DWORD dwFlags) const
{
    GETTEXTLENGTHEX textLenEx;
    textLenEx.flags = dwFlags;
#ifdef _UNICODE
    textLenEx.codepage = CODE_PAGE;
#else
    textLenEx.codepage = CP_ACP;
#endif
    LRESULT lResult = 0;
    TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&textLenEx, 0, &lResult);
    return (long)lResult;
}

CDuiString CRichEditUI::GetText() const
{
    long lLen = GetTextLength(GTL_DEFAULT);

    // 如果长度太大或者太小, 返回个空值
    const int minLen = 1;
    const int maxLen = 10485760; // 10*1024*1024
    if (lLen < minLen || lLen > maxLen) {
        CDuiString sText;
        return sText;
    }

    LPTSTR lpText = nullptr;
    GETTEXTEX gt;
    gt.flags = GT_DEFAULT;
#ifdef _UNICODE
    gt.cb = static_cast<DWORD>(sizeof(TCHAR) * (lLen + 1));
    gt.codepage = CODE_PAGE;
    lpText = new TCHAR[lLen + 1];
    ::ZeroMemory(lpText, (lLen + 1) * sizeof(TCHAR));
#else
    const int multiples = 2;
    const int endLen = 1;
    gt.cb = sizeof(TCHAR) * lLen * multiples + endLen;
    gt.codepage = CP_ACP;
    lpText = new TCHAR[lLen * multiples + endLen];
    ::ZeroMemory(lpText, (lLen * multiples + endLen) * sizeof(TCHAR));
#endif
    gt.lpDefaultChar = nullptr;
    gt.lpUsedDefChar = nullptr;
    TxSendMessage(EM_GETTEXTEX, reinterpret_cast<WPARAM>(&gt), reinterpret_cast<LPARAM>(lpText), 0);
    CDuiString sText(lpText);
    delete[] lpText;
    return sText;
}

DWORD CALLBACK EditStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    CRTFStreamIn *rtf = reinterpret_cast<CRTFStreamIn *>(dwCookie);
    if (pcb == nullptr || pbBuff == nullptr || rtf == nullptr) {
        return 0;
    }
    if (rtf->m_pText.length() - rtf->m_nWritedByteCount + 1 <= cb) {
        *pcb = static_cast<LONG>(rtf->m_pText.length() - rtf->m_nWritedByteCount + 1);
    } else {
        *pcb = cb;
    }
    memcpy_s(pbBuff, cb, rtf->m_pText.c_str() + rtf->m_nWritedByteCount, *pcb);
    rtf->m_nWritedByteCount += *pcb;

    return 0;
}

void CRichEditUI::SetText(LPCTSTR pstrText)
{
    m_sText = pstrText;
    if (!m_pTwh) {
        return;
    }

    if (m_sText.Find(L"{\\rtf") == 0) {
        CRTFStreamIn rtf(pstrText);

        EDITSTREAM es = { 0 };
        es.dwCookie = (DWORD_PTR)&rtf;
        es.pfnCallback = &EditStreamInCallback;

        TxSendMessage(EM_STREAMIN, (WPARAM)SF_RTF, (LPARAM)&es, 0);
    } else {
        m_pTwh->SetColor(m_dwTextColor);
        SetSel(0, -1);
        ReplaceSel(pstrText, FALSE);
    }
}

bool CRichEditUI::GetModify() const
{
    if (!m_pTwh) {
        return false;
    }
    LRESULT lResult;
    TxSendMessage(EM_GETMODIFY, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void CRichEditUI::SetModify(bool bModified) const
{
    TxSendMessage(EM_SETMODIFY, bModified, 0, 0);
}

void CRichEditUI::GetSel(CHARRANGE &cr) const
{
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
}

void CRichEditUI::GetSel(long &nStartChar, long &nEndChar) const
{
    CHARRANGE cr;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    nStartChar = cr.cpMin;
    nEndChar = cr.cpMax;
}

int CRichEditUI::SetSel(CHARRANGE &cr)
{
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);
    return static_cast<int>(lResult);
}

int CRichEditUI::SetSel(long nStartChar, long nEndChar)
{
    CHARRANGE cr;
    cr.cpMin = nStartChar;
    cr.cpMax = nEndChar;
    LRESULT lResult;
    TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr, &lResult);
    return static_cast<int>(lResult);
}

void CRichEditUI::ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo)
{
#ifdef _UNICODE
    TxSendMessage(EM_REPLACESEL, static_cast<WPARAM>(bCanUndo), reinterpret_cast<LPARAM>(lpszNewText), 0);
#else
    int iLen = _tcslen(lpszNewText);
    if ((iLen + 1) == 0 || (iLen + 1) > BUFSIZ) {
        return;
    }
    LPWSTR lpText = new WCHAR[iLen + 1];
    if (lpText == nullptr) {
        return;
    }
    ::ZeroMemory(lpText, (iLen + 1) * sizeof(WCHAR));
    ::MultiByteToWideChar(CP_ACP, 0, lpszNewText, -1, (LPWSTR)lpText, iLen);
    TxSendMessage(EM_REPLACESEL, static_cast<WPARAM>(bCanUndo), reinterpret_cast<LPARAM>(lpText), 0);
    delete[] lpText;
#endif
}

void CRichEditUI::ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo)
{
    TxSendMessage(EM_REPLACESEL, static_cast<WPARAM>(bCanUndo), reinterpret_cast<LPARAM>(lpszNewText), 0);
}

CDuiString CRichEditUI::GetSelText() const
{
    if (!m_pTwh) {
        return CDuiString();
    }
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr, 0);
    LPWSTR lpText = nullptr;
    lpText = new WCHAR[cr.cpMax - cr.cpMin + 1];
    ::ZeroMemory(lpText, (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR));
    TxSendMessage(EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(lpText), 0);
    CDuiString sText;
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

int CRichEditUI::SetSelAll()
{
    return SetSel(0, -1);
}

int CRichEditUI::SetSelNone()
{
    return SetSel(-1, 0);
}

bool CRichEditUI::GetZoom(int &nNum, int &nDen) const
{
    LRESULT lResult;
    TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetZoom(int nNum, int nDen)
{
    const int minNum = 0;
    const int maxNum = 64;
    if (nNum < minNum || nNum > maxNum) {
        return false;
    }
    if (nDen < minNum || nDen > maxNum) {
        return false;
    }
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, nNum, nDen, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetZoomOff()
{
    LRESULT lResult;
    TxSendMessage(EM_SETZOOM, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

WORD CRichEditUI::GetSelectionType() const
{
    LRESULT lResult;
    TxSendMessage(EM_SELECTIONTYPE, 0, 0, &lResult);
    return (WORD)lResult;
}

bool CRichEditUI::GetAutoURLDetect() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETAUTOURLDETECT, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetAutoURLDetect(bool bAutoDetect)
{
    LRESULT lResult;
    TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0, &lResult);
    return (BOOL)lResult == FALSE;
}

DWORD CRichEditUI::GetEventMask() const
{
    LRESULT lResult;
    TxSendMessage(EM_GETEVENTMASK, 0, 0, &lResult);
    return (DWORD)lResult;
}

DWORD CRichEditUI::SetEventMask(DWORD dwEventMask)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask, &lResult);
    return (DWORD)lResult;
}

CDuiString CRichEditUI::GetTextRange(long nStartChar, long nEndChar) const
{
    CDuiString sText;
    TEXTRANGEW tr = { 0 };
    tr.chrg.cpMin = nStartChar;
    tr.chrg.cpMax = nEndChar;
    LPWSTR lpText = nullptr;
    long wCharSize = nEndChar - nStartChar + 1;
    if (wCharSize == 0 || wCharSize > BUFSIZ) {
        return sText;
    }
    lpText = new WCHAR[wCharSize];
    if (lpText == nullptr) {
        return sText;
    }
    ::ZeroMemory(lpText, wCharSize * sizeof(WCHAR));
    tr.lpstrText = lpText;
    TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr, 0);
    sText = (LPCWSTR)lpText;
    delete[] lpText;
    return sText;
}

void CRichEditUI::HideSelection(bool bHide, bool bChangeStyle)
{
    TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle, 0);
}

void CRichEditUI::ScrollCaret()
{
    TxSendMessage(EM_SCROLLCARET, 0, 0, 0);
}

int CRichEditUI::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

int CRichEditUI::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
    int nRet = SetSel(-1, -1);
    ReplaceSel(lpstrText, bCanUndo);
    return nRet;
}

DWORD CRichEditUI::GetDefaultCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetDefaultCharFormat(CHARFORMAT2 &cf)
{
    if (!m_pTwh) {
        return false;
    }
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf, &lResult);
    if ((BOOL)lResult == TRUE) {
        CHARFORMAT2W cfw;
        cfw.cbSize = sizeof(CHARFORMAT2W);
        TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cfw, 0);
        m_pTwh->SetCharFormat(cfw);
        return true;
    }
    return false;
}

DWORD CRichEditUI::GetSelectionCharFormat(CHARFORMAT2 &cf) const
{
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetSelectionCharFormat(CHARFORMAT2 &cf)
{
    if (m_pManager == nullptr) {
        return false;
    }
    if (m_pManager->IsLayered()) {
        CRenderEngine::CheckAlphaColor(cf.crTextColor);
        CRenderEngine::CheckAlphaColor(cf.crBackColor);
    }
    if (!m_pTwh) {
        return false;
    }
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::SetWordCharFormat(CHARFORMAT2 &cf)
{
    if (!m_pTwh) {
        return false;
    }
    cf.cbSize = sizeof(CHARFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf, &lResult);
    return (BOOL)lResult == TRUE;
}

DWORD CRichEditUI::GetParaFormat(PARAFORMAT2 &pf) const
{
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    return (DWORD)lResult;
}

bool CRichEditUI::SetParaFormat(PARAFORMAT2 &pf)
{
    if (!m_pTwh) {
        return false;
    }
    pf.cbSize = sizeof(PARAFORMAT2);
    LRESULT lResult;
    TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf, &lResult);
    if ((BOOL)lResult == TRUE) {
        m_pTwh->SetParaFormat(pf);
        return true;
    }
    return false;
}

bool CRichEditUI::Redo()
{
    if (!m_pTwh) {
        return false;
    }
    LRESULT lResult;
    TxSendMessage(EM_REDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

bool CRichEditUI::Undo()
{
    if (!m_pTwh) {
        return false;
    }
    LRESULT lResult;
    TxSendMessage(EM_UNDO, 0, 0, &lResult);
    return (BOOL)lResult == TRUE;
}

void CRichEditUI::Clear()
{
    SetSel(0, -1);
    TxSendMessage(WM_CLEAR, 0, 0, 0);
}

void CRichEditUI::Copy()
{
    TxSendMessage(WM_COPY, 0, 0, 0);
}

void CRichEditUI::Cut()
{
    TxSendMessage(WM_CUT, 0, 0, 0);
}

void CRichEditUI::Paste()
{
    TxSendMessage(WM_PASTE, 0, 0, 0);
}

int CRichEditUI::GetLineCount() const
{
    if (!m_pTwh) {
        return 0;
    }
    LRESULT lResult;
    TxSendMessage(EM_GETLINECOUNT, 0, 0, &lResult);
    return static_cast<int>(lResult);
}

CDuiString CRichEditUI::GetLine(int nIndex, int nMaxLength) const
{
    CDuiString sText;
    if (nMaxLength == 0 || nMaxLength > BUFSIZ) {
        return sText;
    }
    LPWSTR lpText = nullptr;
    lpText = new WCHAR[nMaxLength + 1];
    if (lpText == nullptr) {
        return sText;
    }
    ::ZeroMemory(lpText, (nMaxLength + 1) * sizeof(WCHAR));
    *(LPWORD)lpText = (WORD)nMaxLength;
    TxSendMessage(EM_GETLINE, nIndex, reinterpret_cast<LPARAM>(lpText), 0);

    sText = (LPCWSTR)lpText;
    delete[] lpText;
    lpText = nullptr;
    return sText;
}

int CRichEditUI::LineIndex(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINEINDEX, nLine, 0, &lResult);
    return static_cast<int>(lResult);
}

int CRichEditUI::LineLength(int nLine) const
{
    LRESULT lResult;
    TxSendMessage(EM_LINELENGTH, nLine, 0, &lResult);
    return static_cast<int>(lResult);
}

bool CRichEditUI::LineScroll(int nLines, int nChars)
{
    LRESULT lResult;
    TxSendMessage(EM_LINESCROLL, nChars, nLines, &lResult);
    return (BOOL)lResult == TRUE;
}

CDuiPoint CRichEditUI::GetCharPos(long lChar) const
{
    CDuiPoint pt;
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar, 0);
    return pt;
}

long CRichEditUI::LineFromChar(long nIndex) const
{
    if (!m_pTwh) {
        return 0L;
    }
    LRESULT lResult;
    TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex, &lResult);
    return (long)lResult;
}

CDuiPoint CRichEditUI::PosFromChar(UINT nChar) const
{
    POINTL pt;
    TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar, 0);
    return CDuiPoint(pt.x, pt.y);
}

int CRichEditUI::CharFromPos(CDuiPoint pt) const
{
    POINTL ptl = { pt.x, pt.y };
    if (!m_pTwh) {
        return 0;
    }
    LRESULT lResult;
    TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl, &lResult);
    return static_cast<int>(lResult);
}

void CRichEditUI::EmptyUndoBuffer()
{
    TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0, 0);
}

UINT CRichEditUI::SetUndoLimit(UINT nLimit)
{
    if (!m_pTwh) {
        return 0;
    }
    LRESULT lResult;
    TxSendMessage(EM_SETUNDOLIMIT, (WPARAM)nLimit, 0, &lResult);
    return (UINT)lResult;
}

long CRichEditUI::StreamIn(int nFormat, EDITSTREAM &es)
{
    if (!m_pTwh) {
        return 0L;
    }
    LRESULT lResult;
    TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

long CRichEditUI::StreamOut(int nFormat, EDITSTREAM &es)
{
    if (!m_pTwh) {
        return 0L;
    }
    LRESULT lResult;
    TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es, &lResult);
    return (long)lResult;
}

void CRichEditUI::DoInit()
{
    if (m_bInited) {
        return;
    }

    CREATESTRUCT cs;
    cs.style = m_lTwhStyle;
    cs.x = 0;
    cs.y = 0;
    cs.cy = 0;
    cs.cx = 0;
    cs.lpszName = L"";
    CreateHost(this, &cs, &m_pTwh);
    if (m_pManager == nullptr) {
        return;
    }
    if (m_pTwh) {
        m_pTwh->SetFont(GetManager()->GetFont(m_iFont));
        m_pTwh->SetTransparent(TRUE);
        LRESULT lResult;
        m_pTwh->GetTextServices()->TxSendMessage(EM_SETLANGOPTIONS, 0, 0, &lResult);
        m_pTwh->GetTextServices()->TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES | ENM_LINK | ENM_CHANGE, &lResult);
        m_pTwh->OnTxInPlaceActivate(nullptr);
        m_pManager->AddMessageFilter(this);
        if (!m_bEnabled) {
            m_pTwh->SetColor(m_pManager->GetDefaultDisabledColor());
        }

        SetText(m_sText);
    }

    m_bInited = true;
}

HRESULT CRichEditUI::TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const
{
    if (m_pTwh) {
        if (msg == WM_KEYDOWN && TCHAR(wparam) == VK_RETURN) {
            if (!m_bWantReturn || (::GetKeyState(VK_CONTROL) < 0 && !m_bWantCtrlReturn)) {
                if (m_pManager != nullptr)
                    m_pManager->SendNotify((CControlUI *)this, DUI_MSGTYPE_RETURN);
                return S_OK;
            }
        }
        return m_pTwh->GetTextServices()->TxSendMessage(msg, wparam, lparam, plresult);
    }
    return S_FALSE;
}

IDropTarget *CRichEditUI::GetTxDropTarget()
{
    IDropTarget *pdt = nullptr;
    if (m_pTwh->GetTextServices()->TxGetDropTarget(&pdt) == NOERROR) {
        return pdt;
    }
    return nullptr;
}

bool CRichEditUI::OnTxViewChanged(BOOL bUpdate)
{
    return true;
}

bool CRichEditUI::SetDropAcceptFile(bool bAccept)
{
    LRESULT lResult;
    TxSendMessage(EM_SETEVENTMASK, 0, ENM_DROPFILES | ENM_LINK, &lResult);
    return (BOOL)lResult == FALSE;
}

void CRichEditUI::OnTxNotify(DWORD iNotify, void *pv)
{
    switch (iNotify) {
        case EN_CHANGE: {
            GetManager()->SendNotify(this, DUI_MSGTYPE_TEXTCHANGED);
            break;
        }
        case EN_DROPFILES:
        case EN_MSGFILTER:
        case EN_OLEOPFAILED:
        case EN_PROTECTED:
        case EN_SAVECLIPBOARD:
        case EN_SELCHANGE:
        case EN_STOPNOUNDO:
        case EN_LINK:
        case EN_OBJECTPOSITIONS:
        case EN_DRAGDROPDONE: {
            if (pv) { // Fill out NMHDR portion of pv
                LONG nId = GetWindowLong(this->GetManager()->GetPaintWindow(), GWL_ID);
                NMHDR *phdr = reinterpret_cast<NMHDR *>(pv);
                phdr->hwndFrom = this->GetManager()->GetPaintWindow();
                phdr->idFrom = nId;
                phdr->code = iNotify;

                if (SendMessage(this->GetManager()->GetPaintWindow(), WM_NOTIFY, static_cast<WPARAM>(nId),
                    reinterpret_cast<LPARAM>(pv))) {
                }
            }
        } break;
    }
}

// 多行非rich格式的richedit有一个滚动条bug，在最后一行是空行时，LineDown和SetScrollPos无法滚动到最后
// 引入iPos就是为了修正这个bug
void CRichEditUI::SetScrollPos(SIZE szPos, bool bMsg)
{
    int cx = 0;
    int cy = 0;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }
    if (cy != 0) {
        int iPos = 0;
        if (m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
            iPos = m_pVerticalScrollBar->GetScrollPos();
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pVerticalScrollBar->GetScrollPos());
        TxSendMessage(WM_VSCROLL, wParam, 0L, 0);
        if (m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
            if (cy > 0 && m_pVerticalScrollBar->GetScrollPos() <= iPos)
                m_pVerticalScrollBar->SetScrollPos(iPos);
        }
    }
    if (cx != 0) {
        WPARAM wParam = MAKEWPARAM(SB_THUMBPOSITION, m_pHorizontalScrollBar->GetScrollPos());
        TxSendMessage(WM_HSCROLL, wParam, 0L, 0);
    }
}

void CRichEditUI::LineUp()
{
    TxSendMessage(WM_VSCROLL, SB_LINEUP, 0L, 0);
}

void CRichEditUI::LineDown()
{
    int iPos = 0;
    if (m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
        iPos = m_pVerticalScrollBar->GetScrollPos();
    }
    TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0L, 0);
    if (m_pTwh && !m_bRich && m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
        if (m_pVerticalScrollBar->GetScrollPos() <= iPos) {
            m_pVerticalScrollBar->SetScrollPos(m_pVerticalScrollBar->GetScrollRange());
        }
    }
}

void CRichEditUI::PageUp()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEUP, 0L, 0);
}

void CRichEditUI::PageDown()
{
    TxSendMessage(WM_VSCROLL, SB_PAGEDOWN, 0L, 0);
}

void CRichEditUI::HomeUp()
{
    TxSendMessage(WM_VSCROLL, SB_TOP, 0L, 0);
}

void CRichEditUI::EndDown()
{
    TxSendMessage(WM_VSCROLL, SB_BOTTOM, 0L, 0);
}

void CRichEditUI::LineLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LINELEFT, 0L, 0);
}

void CRichEditUI::LineRight()
{
    TxSendMessage(WM_HSCROLL, SB_LINERIGHT, 0L, 0);
}

void CRichEditUI::PageLeft()
{
    TxSendMessage(WM_HSCROLL, SB_PAGELEFT, 0L, 0);
}

void CRichEditUI::PageRight()
{
    TxSendMessage(WM_HSCROLL, SB_PAGERIGHT, 0L, 0);
}

void CRichEditUI::HomeLeft()
{
    TxSendMessage(WM_HSCROLL, SB_LEFT, 0L, 0);
}

void CRichEditUI::EndRight()
{
    TxSendMessage(WM_HSCROLL, SB_RIGHT, 0L, 0);
}

void CRichEditUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pParent != nullptr) {
            m_pParent->DoEvent(event);
        } else {
            CControlUI::DoEvent(event);
        }
        return;
    } else if (event.Type == UIEVENT_SETCURSOR && IsEnabled()) {
        if (m_pTwh && m_pTwh->DoSetCursor(nullptr, &event.ptMouse)) {
            return;
        }
    }
    if (event.Type == UIEVENT_SETFOCUS) {
        if (m_pTwh) {
            m_pTwh->OnTxInPlaceActivate(nullptr);
            m_pTwh->GetTextServices()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
        }
        m_bFocused = true;
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_KILLFOCUS) {
        if (m_pTwh) {
            m_pTwh->OnTxInPlaceActivate(nullptr);
            m_pTwh->GetTextServices()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
        }
        m_bFocused = false;
        Invalidate();
        return;
    } else if (event.Type == UIEVENT_TIMER) {
        if (m_pTwh) {
            m_pTwh->GetTextServices()->TxSendMessage(WM_TIMER, event.wParam, event.lParam, 0);
        }
        return;
    } else if (event.Type == UIEVENT_SCROLLWHEEL) {
        if ((event.wKeyState & MK_CONTROL) != 0) {
            return;
        }
    } else if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK) {
        return;
    } else if (event.Type == UIEVENT_MOUSEMOVE) {
        return;
    } else if (event.Type == UIEVENT_BUTTONUP) {
        return;
    } else if (event.Type == UIEVENT_MOUSEENTER) {
        if (IsEnabled()) {
            m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    } else if (event.Type == UIEVENT_MOUSELEAVE) {
        if (IsEnabled()) {
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if (event.Type > UIEVENT__KEYBEGIN && event.Type < UIEVENT__KEYEND) {
        return;
    }
    CContainerUI::DoEvent(event);
}


LPCTSTR CRichEditUI::GetNormalImage()
{
    return m_sNormalImage;
}

void CRichEditUI::SetNormalImage(LPCTSTR pStrImage)
{
    m_sNormalImage = pStrImage;
    Invalidate();
}

LPCTSTR CRichEditUI::GetHotImage()
{
    return m_sHotImage;
}

void CRichEditUI::SetHotImage(LPCTSTR pStrImage)
{
    m_sHotImage = pStrImage;
    Invalidate();
}

LPCTSTR CRichEditUI::GetFocusedImage()
{
    return m_sFocusedImage;
}

void CRichEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
    m_sFocusedImage = pStrImage;
    Invalidate();
}

LPCTSTR CRichEditUI::GetDisabledImage()
{
    return m_sDisabledImage;
}

void CRichEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
    m_sDisabledImage = pStrImage;
    Invalidate();
}

RECT CRichEditUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CRichEditUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

void CRichEditUI::SetTipValue(LPCTSTR pStrTipValue)
{
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString text(pStrTipValue);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    const int valueNum = 2;
    if (valueList.size() == valueNum) {
        m_sTipValue = valueList[0];
        m_sTipValueMap = valueList[1];
    } else {
#endif
        m_sTipValue = pStrTipValue;
#ifdef SWITCH_LANGUAGE_TEST
    }
#endif
}

LPCTSTR CRichEditUI::GetTipValue()
{
    return m_sTipValue.GetData();
}

void CRichEditUI::SetTipValueColor(LPCTSTR pStrColor)
{
    if (pStrColor == nullptr) {
        return;
    }
    if (*pStrColor == _T('#')) {
        pStrColor = ::CharNext(pStrColor);
    }
    LPTSTR pstr = nullptr;
    DWORD clrColor = _tcstoul(pStrColor, &pstr, NUM_BASE_COLOR);

    m_dwTipValueColor = clrColor;
}

DWORD CRichEditUI::GetTipValueColor()
{
    return m_dwTipValueColor;
}

void CRichEditUI::SetTipValueAlign(UINT uAlign)
{
    m_uTipValueAlign = uAlign;
    if (GetText().IsEmpty()) {
        Invalidate();
    }
}

UINT CRichEditUI::GetTipValueAlign()
{
    return m_uTipValueAlign;
}

void CRichEditUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) {
        m_uButtonState |= UISTATE_FOCUSED;
    } else {
        m_uButtonState &= ~UISTATE_FOCUSED;
    }
    if (!IsEnabled()) {
        m_uButtonState |= UISTATE_DISABLED;
    } else {
        m_uButtonState &= ~UISTATE_DISABLED;
    }

    if ((m_uButtonState & UISTATE_DISABLED) != 0) {
        if (!m_sDisabledImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sDisabledImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {
        if (!m_sFocusedImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sFocusedImage)) {
            } else {
                return;
            }
        }
    } else if ((m_uButtonState & UISTATE_HOT) != 0) {
        if (!m_sHotImage.IsEmpty()) {
            if (!DrawImage(hDC, (LPCTSTR)m_sHotImage)) {
            } else {
                return;
            }
        }
    }

    if (!m_sNormalImage.IsEmpty()) {
        if (!DrawImage(hDC, (LPCTSTR)m_sNormalImage)) {
        } else {
            return;
        }
    }
}

SIZE CRichEditUI::EstimateSize(SIZE szAvailable)
{
    return CContainerUI::EstimateSize(szAvailable);
}

void CRichEditUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    RECT m_rcInset = CRichEditUI::m_rcInset;
    CResourceManager::GetInstance()->Scale(&m_rcInset);

    RECT m_rcTextPadding = CRichEditUI::m_rcTextPadding;
    CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;
    bool bVScrollBarVisiable = false;
    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
        bVScrollBarVisiable = true;
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    }
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    if (m_pTwh) {
        RECT rcRich = rc;
        rcRich.left += m_rcTextPadding.left;
        rcRich.right -= m_rcTextPadding.right;
        rcRich.top += m_rcTextPadding.top;
        rcRich.bottom -= m_rcTextPadding.bottom;
        m_pTwh->SetClientRect(&rcRich);
        if (bVScrollBarVisiable && (!m_pVerticalScrollBar->IsVisible() || m_bVScrollBarFixing)) {
            LONG lWidth = rcRich.right - rcRich.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTwh->GetTextServices()->TxGetNaturalSize(DVASPECT_CONTENT, GetManager()->GetPaintDC(), nullptr, nullptr,
                TXTNS_FITTOCONTENT, &szExtent, &lWidth, &lHeight);
            if (lHeight > rcRich.bottom - rcRich.top) {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_bVScrollBarFixing = true;
            } else {
                if (m_bVScrollBarFixing) {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_bVScrollBarFixing = false;
                }
            }
        }
    }

    if (m_pVerticalScrollBar != nullptr && m_pVerticalScrollBar->IsVisible()) {
        RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
        m_pVerticalScrollBar->SetPos(rcScrollBarPos);
    }
    if (m_pHorizontalScrollBar != nullptr && m_pHorizontalScrollBar->IsVisible()) {
        RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight() };
        m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
    }

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
    }

    int nAdjustables = 0;
    int cxFixed = 0;
    int nEstimateNum = 0;
    for (int it1 = 0; it1 < m_items.GetSize(); it1++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it1));
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsFloat()) {
            continue;
        }
        SIZE sz = pControl->EstimateSize(szAvailable);
        if (sz.cx == 0) {
            nAdjustables++;
        } else {
            if (sz.cx < pControl->GetMinWidth()) {
                sz.cx = pControl->GetMinWidth();
            }
            if (sz.cx > pControl->GetMaxWidth()) {
                sz.cx = pControl->GetMaxWidth();
            }
        }
        cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;
        nEstimateNum++;
    }
    if ((nEstimateNum - 1) > 0 && m_iChildPadding > (INT_MAX - cxFixed) / (nEstimateNum - 1)) {
        return;
    }
    cxFixed += (nEstimateNum - 1) * m_iChildPadding;

    int cxExpand = 0;
    int cxNeeded = 0;
    if (nAdjustables > 0) {
        cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
    }
    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosX = rc.left;
    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    }
    int iAdjustable = 0;
    int cxFixedRemaining = cxFixed;
    for (int it2 = 0; it2 < m_items.GetSize(); it2++) {
        CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it2));
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsFloat()) {
            SetFloatPos(it2);
            continue;
        }
        RECT rcPadding = pControl->GetPadding();
        szRemaining.cx -= rcPadding.left;
        SIZE sz = pControl->EstimateSize(szRemaining);
        if (sz.cx == 0) {
            iAdjustable++;
            sz.cx = cxExpand;

            if (sz.cx < pControl->GetMinWidth()) {
                sz.cx = pControl->GetMinWidth();
            }
            if (sz.cx > pControl->GetMaxWidth()) {
                sz.cx = pControl->GetMaxWidth();
            }
        } else {
            if (sz.cx < pControl->GetMinWidth()) {
                sz.cx = pControl->GetMinWidth();
            }
            if (sz.cx > pControl->GetMaxWidth()) {
                sz.cx = pControl->GetMaxWidth();
            }
        }

        sz.cy = pControl->GetFixedHeight();
        if (sz.cy == 0) {
            sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
        }
        if (sz.cy < 0) {
            sz.cy = 0;
        }
        if (sz.cy < pControl->GetMinHeight()) {
            sz.cy = pControl->GetMinHeight();
        }
        if (sz.cy > pControl->GetMaxHeight()) {
            sz.cy = pControl->GetMaxHeight();
        }

        RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left,
                        rc.top + rcPadding.top + sz.cy };
        pControl->SetPos(rcCtrl, true);
        iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
        cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
        szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
    }
    cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
    if (m_pHorizontalScrollBar != nullptr) {
        if (cxNeeded > rc.right - rc.left) {
            if (m_pHorizontalScrollBar->IsVisible()) {
                m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
            } else {
                m_pHorizontalScrollBar->SetVisible(true);
                m_pHorizontalScrollBar->SetScrollRange(cxNeeded - (rc.right - rc.left));
                m_pHorizontalScrollBar->SetScrollPos(0);
                rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
            }
        }
    }
}

void CRichEditUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CContainerUI::Move(szOffset, bNeedInvalidate);
    if (m_pTwh != nullptr) {
        RECT rc = m_rcItem;

        RECT m_rcInset = CRichEditUI::m_rcInset;
        CResourceManager::GetInstance()->Scale(&m_rcInset);

        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
            rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        }
        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
            rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
        }
        m_pTwh->SetClientRect(&rc);
    }
}

void CRichEditUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    RECT rcTemp = { 0 };
    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) {
        return;
    }

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint);

    if (m_pTwh) {
        RECT rc;
        m_pTwh->GetControlRect(&rc);
        // Remember wparam is actually the hdc and lparam is the update
        // rect because this message has been preprocessed by the window.
        m_pTwh->GetTextServices()->TxDraw(DVASPECT_CONTENT, // Draw Aspect
            0, // Lindex
            nullptr, // Info for drawing optimazation
            nullptr, // target device information
            hDC, // Draw device HDC
            nullptr, // Target device HDC
            (RECTL *)&rc, // Bounding client rectangle
            nullptr, // Clipping rectangle for metafiles
            (RECT *)&rcPaint, // Update rectangle
            nullptr, // Call back function
            NULL, // Call back parameter
            0); // What view of the object

        if (m_bVScrollBarFixing && m_pVerticalScrollBar != nullptr) {
            LONG lWidth = rc.right - rc.left + m_pVerticalScrollBar->GetFixedWidth();
            LONG lHeight = 0;
            SIZEL szExtent = { -1, -1 };
            m_pTwh->GetTextServices()->TxGetNaturalSize(DVASPECT_CONTENT, GetManager()->GetPaintDC(), nullptr, nullptr,
                TXTNS_FITTOCONTENT, &szExtent, &lWidth, &lHeight);
            if (lHeight <= rc.bottom - rc.top) {
                NeedUpdate();
            }
        }
    }

    if (m_items.GetSize() > 0) {
        RECT rc = m_rcItem;
        RECT m_rcInset = CRichEditUI::m_rcInset;
        CResourceManager::GetInstance()->Scale(&m_rcInset);
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) {
            rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        }
        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) {
            rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
        }

        if (!::IntersectRect(&rcTemp, &rcPaint, &rc)) {
            for (int it = 0; it < m_items.GetSize(); it++) {
                CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it));
                if (pControl == nullptr) {
                    continue;
                }
                if (!pControl->IsVisible()) {
                    continue;
                }
                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) {
                    continue;
                }
                if (pControl->IsFloat()) {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) {
                        continue;
                    }
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        } else {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);
            for (int it = 0; it < m_items.GetSize(); it++) {
                CControlUI *pControl = static_cast<CControlUI *>(m_items.GetAt(it));
                if (pControl == nullptr) {
                    continue;
                }
                if (!pControl->IsVisible()) {
                    continue;
                }
                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) {
                    continue;
                }
                if (pControl->IsFloat()) {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) {
                        continue;
                    }
                    CRenderClip::UseOldClipBegin(hDC, childClip);
                    pControl->DoPaint(hDC, rcPaint);
                    CRenderClip::UseOldClipEnd(hDC, childClip);
                } else {
                    if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) {
                        continue;
                    }
                    pControl->DoPaint(hDC, rcPaint);
                }
            }
        }
    }

    if (m_pVerticalScrollBar != nullptr && m_pVerticalScrollBar->IsVisible()) {
        if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) {
            m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
        }
    }

    if (m_pHorizontalScrollBar != nullptr && m_pHorizontalScrollBar->IsVisible()) {
        if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) {
            m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
        }
    }
    // 绘制提示文字
    CDuiString sDrawText = GetText();
    if (sDrawText.IsEmpty()) {
        DWORD dwTextColor = GetTipValueColor();
        CDuiString sTipValue = GetTipValue();
        RECT rc = m_rcItem;

        RECT m_rcInset = CRichEditUI::m_rcInset;
        CResourceManager::GetInstance()->Scale(&m_rcInset);

        RECT m_rcTextPadding = CRichEditUI::m_rcTextPadding;
        CResourceManager::GetInstance()->Scale(&m_rcTextPadding);

        rc.left += (m_rcTextPadding.left + m_rcInset.left);
        rc.right -= (m_rcTextPadding.right + m_rcInset.right);
        rc.top += (m_rcTextPadding.top + m_rcInset.top);
        rc.bottom -= (m_rcTextPadding.bottom + m_rcInset.bottom);

        UINT uTextAlign = GetTipValueAlign();
        if (IsMultiLine()) {
            uTextAlign |= DT_TOP;
        } else {
            uTextAlign |= DT_VCENTER;
        }
        CRenderEngine::DrawText(hDC, m_pManager, rc, sTipValue, dwTextColor, m_iFont, uTextAlign);
    }
}

void CRichEditUI::JudgeTipDirection(LPCTSTR pstrValue)
{
    bool directRTL = false;
    switch (UICulture::GetInstance()->getDirection()) {
        case DIRECTION_RTL:
            directRTL = true;
        default:
            break;
    }
    if (_tcsstr(pstrValue, _T("left")) != nullptr) {
        m_uTipValueAlign = DT_SINGLELINE | DT_LEFT;
    }
    if (_tcsstr(pstrValue, _T("center")) != nullptr) {
        m_uTipValueAlign = DT_SINGLELINE | DT_CENTER;
    }
    if (_tcsstr(pstrValue, _T("right")) != nullptr) {
        m_uTipValueAlign = DT_SINGLELINE | DT_RIGHT;
    }

}
void CRichEditUI::JudgeDirection(LPCTSTR pstrValue)
{
    bool directRTL = false;
    switch (UICulture::GetInstance()->getDirection()) {
        case DIRECTION_RTL:
            directRTL = true;
        default:
            break;
    }

    if (_tcsstr(pstrValue, _T("left")) != nullptr) {
        m_lTwhStyle &= ~(ES_CENTER | ES_RIGHT);
        m_lTwhStyle |= ES_LEFT;
    }
    if (_tcsstr(pstrValue, _T("center")) != nullptr) {
        m_lTwhStyle &= ~(ES_LEFT | ES_RIGHT);
        m_lTwhStyle |= ES_CENTER;
    }
    if (_tcsstr(pstrValue, _T("right")) != nullptr) {
        m_lTwhStyle &= ~(ES_LEFT | ES_CENTER);
        m_lTwhStyle |= ES_RIGHT;
    }
}
void CRichEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("vscrollbar")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_DISABLENOSCROLL | WS_VSCROLL;
        }
    } else if (_tcsicmp(pstrName, _T("autovscroll")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_AUTOVSCROLL;
        }
    } else if (_tcsicmp(pstrName, _T("hscrollbar")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_DISABLENOSCROLL | WS_HSCROLL;
        }
    } else if (_tcsicmp(pstrName, _T("autohscroll")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_AUTOHSCROLL;
        }
    } else if (_tcsicmp(pstrName, _T("wanttab")) == 0) {
        SetWantTab(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("wantreturn")) == 0) {
        SetWantReturn(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("wantctrlreturn")) == 0) {
        SetWantCtrlReturn(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("rich")) == 0) {
        SetRich(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("multiline")) == 0) {
        if (_tcsicmp(pstrValue, _T("false")) == 0) {
            m_lTwhStyle &= ~ES_MULTILINE;
        }
    } else if (_tcsicmp(pstrName, _T("readonly")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_READONLY;
            m_bReadOnly = true;
        }
    } else if (_tcsicmp(pstrName, _T("password")) == 0) {
        if (_tcsicmp(pstrValue, _T("true")) == 0) {
            m_lTwhStyle |= ES_PASSWORD;
        }
    } else if (_tcsicmp(pstrName, _T("align")) == 0) {
        JudgeDirection(pstrValue);
    } else if (_tcsicmp(pstrName, _T("tipvaluealign")) == 0) {
        JudgeTipDirection(pstrValue);
    } else if (_tcsicmp(pstrName, _T("font")) == 0) {
        SetFont(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, NUM_BASE_COLOR);
            SetTextColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("maxchar")) == 0) {
        SetLimitText(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("normalimage")) == 0) {
        SetNormalImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("hotimage")) == 0) {
        SetHotImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("focusedimage")) == 0) {
        SetFocusedImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("disabledimage")) == 0) {
        SetDisabledImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("textpadding")) == 0) {
        RECT rcTextPadding = { 0 };
        LPTSTR pstr = nullptr;
        rcTextPadding.left = _tcstol(pstrValue, &pstr, NUM_BASE_PADDING);
        ASSERT(pstr);
        rcTextPadding.top = _tcstol(pstr + 1, &pstr, NUM_BASE_PADDING);
        ASSERT(pstr);
        rcTextPadding.right = _tcstol(pstr + 1, &pstr, NUM_BASE_PADDING);
        ASSERT(pstr);
        rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, NUM_BASE_PADDING);
        ASSERT(pstr);
        SetTextPadding(rcTextPadding);
    } else if (_tcsicmp(pstrName, _T("tipvalue")) == 0) {
        SetTipValue(pstrValue);
    } else if (_tcsicmp(pstrName, _T("tipvaluecolor")) == 0) {
        SetTipValueColor(pstrValue);
    } else {
        CContainerUI::SetAttribute(pstrName, pstrValue);
    }
}

LRESULT CRichEditUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
    if (!IsVisible() || !IsEnabled()) {
        return 0;
    }
    if (!IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
        return 0;
    }
    if (uMsg == WM_MOUSEWHEEL && (LOWORD(wParam) & MK_CONTROL) == 0) {
        return 0;
    }

    if (uMsg == WM_IME_COMPOSITION) {
        // 解决微软输入法位置异常的问题
        HIMC hIMC = ImmGetContext(GetManager()->GetPaintWindow());
        if (hIMC) {
            POINT point;
            SecureZeroMemory(&point, sizeof(point));
            GetCaretPos(&point);

            COMPOSITIONFORM Composition;
            SecureZeroMemory(&Composition, sizeof(Composition));
            Composition.dwStyle = CFS_POINT;
            Composition.ptCurrentPos.x = point.x;
            Composition.ptCurrentPos.y = point.y;
            ImmSetCompositionWindow(hIMC, &Composition);

            ImmReleaseContext(GetManager()->GetPaintWindow(), hIMC);
        }

        return 0;
    }

    bool bWasHandled = true;
    if (m_pTwh == nullptr) {
        return E_POINTER;
    }
    if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR) {
        if (!m_pTwh->IsCaptured()) {
            switch (uMsg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP: {
                    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    CControlUI *pHover = GetManager()->FindControl(pt);
                    if (pHover != this) {
                        bWasHandled = false;
                        return 0;
                    }
                } break;
            }
        }
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pTwh->IsCaptured() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if (dwHitResult == HITRESULT_OUTSIDE) {
            RECT rc;
            m_pTwh->GetControlRect(&rc);
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if (uMsg == WM_MOUSEWHEEL) {
                ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
            }
            if (::PtInRect(&rc, pt) && !GetManager()->IsCaptured()) {
                dwHitResult = HITRESULT_HIT;
            }
        }
        if (dwHitResult != HITRESULT_HIT) {
            return 0;
        }
        if (uMsg == WM_SETCURSOR) {
            bWasHandled = false;
        } else if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN) {
            if (GetManager() && (GetManager()->GetFocus() != this)) {
                HWND phwnd = GetManager()->GetPaintWindow(); // 主窗口
                HWND fhwnd = ::GetFocus();                   // 当前焦点所在的窗口
                if (fhwnd != phwnd) {
                    ::SetFocus(phwnd); // 把焦点移到主窗口上
                }
            }
            SetFocus();
        }
    }
#ifdef _UNICODE
    else if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) {
#else
    else if ((uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR) {
#endif
        if (!IsFocused()) {
            return 0;
        }
    } else if (uMsg == WM_CONTEXTMENU) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);
        CControlUI *pHover = GetManager()->FindControl(pt);
        if (pHover != this) {
            bWasHandled = false;
            return 0;
        }
    } else {
        switch (uMsg) {
            case WM_HELP:
                bWasHandled = false;
                break;
            default:
                return 0;
        }
    }

    if (uMsg == WM_CHAR) {
#ifndef _UNICODE
        // check if we are waiting for 2 consecutive WM_CHAR messages
        if (IsAccumulateDBCMode()) {
            if ((GetKeyState(VK_KANA) & 0x1)) {
                // turn off accumulate mode
                SetAccumulateDBCMode(false);
                m_chLeadByte = 0;
            } else {
                if (!m_chLeadByte) {
                    // This is the first WM_CHAR message,
                    // accumulate it if this is a LeadByte.  Otherwise, fall thru to
                    // regular WM_CHAR processing.
                    if (IsDBCSLeadByte((WORD)wParam)) {
                        // save the Lead Byte and don't process this message
                        const int byteMultiples = 8;
                        m_chLeadByte = (WORD)wParam << byteMultiples;
                        return 0;
                    }
                } else {
                    // This is the second WM_CHAR message,
                    // combine the current byte with previous byte.
                    // This DBC will be handled as WM_IME_CHAR.
                    wParam |= m_chLeadByte;
                    uMsg = WM_IME_CHAR;
                    // setup to accumulate more WM_CHAR
                    m_chLeadByte = 0;
                }
            }
        }
#endif
    }

    LRESULT lResult = 0;
    HRESULT Hr = TxSendMessage(uMsg, wParam, lParam, &lResult);

    if (Hr == S_OK) {
        bHandled = bWasHandled;
    } else if ((uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) || uMsg == WM_CHAR || uMsg == WM_IME_CHAR) {
        bHandled = bWasHandled;
    } else if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
        if (m_pTwh == nullptr) {
            return S_FALSE;
        }
        if (m_pTwh->IsCaptured()) {
            bHandled = bWasHandled;
        }
    }
    return lResult;
}

void CRichEditUI::SetAccumulateDBCMode(bool bDBCMode)
{
    m_fAccumulateDBC = bDBCMode;
}

bool CRichEditUI::IsAccumulateDBCMode()
{
    return m_fAccumulateDBC;
}
} // namespace DuiLib