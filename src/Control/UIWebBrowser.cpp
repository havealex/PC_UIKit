/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIWebBrowser.h"
#include <atlconv.h>
#include <atlcomcli.h>
#include "../Utils/downloadmgr.h"
#include <mshtml.h>

namespace DuiLib {
IMPLEMENT_DUICONTROL(CWebBrowserUI)

CWebBrowserUI::CWebBrowserUI()
    : m_pWebBrowser2(nullptr),
      _pHtmlWnd2(nullptr),
      m_pWebBrowserEventHandler(nullptr),
      m_bAutoNavi(false),
      m_dwRef(0),
      m_dwCookie(0)
{
    m_clsid = CLSID_WebBrowser;
    m_sHomePage.Empty();
}

bool CWebBrowserUI::DoCreateControl()
{
    if (!CActiveXUI::DoCreateControl())
        return false;
    GetManager()->AddTranslateAccelerator(this);
    GetControl(IID_IWebBrowser2, reinterpret_cast<LPVOID *>(&m_pWebBrowser2));
    if (m_bAutoNavi && !m_sHomePage.IsEmpty()) {
        this->Navigate2(m_sHomePage);
    }
    RegisterEventHandler(TRUE);
    return true;
}

void CWebBrowserUI::ReleaseControl()
{
    m_bCreated = false;
    GetManager()->RemoveTranslateAccelerator(this);
    RegisterEventHandler(FALSE);
}

CWebBrowserUI::~CWebBrowserUI()
{
    ReleaseControl();
}

STDMETHODIMP CWebBrowserUI::GetTypeInfoCount(UINT *iTInfo)
{
    if (iTInfo == nullptr) {
        return E_NOINTERFACE;
    }
    *iTInfo = 0;
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
    VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    if ((riid != IID_NULL)) {
        return E_INVALIDARG;
    }
    if (pDispParams == nullptr) {
        return E_NOINTERFACE;
    }
    switch (dispIdMember) {
        case DISPID_BEFORENAVIGATE2:
            // 开始导航的参数
            if (pDispParams->rgvarg == nullptr) {
                return E_INVALIDARG;
            }
            BeforeNavigate2(pDispParams->rgvarg[6].pdispVal, pDispParams->rgvarg[5].pvarVal,
                pDispParams->rgvarg[4].pvarVal, pDispParams->rgvarg[3].pvarVal, pDispParams->rgvarg[2].pvarVal,
                pDispParams->rgvarg[1].pvarVal, pDispParams->rgvarg[0].pboolVal);
            break;
        case DISPID_COMMANDSTATECHANGE:
            if (pDispParams->rgvarg == nullptr) {
                return E_INVALIDARG;
            }
            CommandStateChange(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].boolVal); // 更改命令状态的参数
            break;
        case DISPID_NAVIGATECOMPLETE2:
            if (pDispParams->rgvarg == nullptr) {
                return E_INVALIDARG;
            }
            NavigateComplete2(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal); // 完成导航的参数
            break;
        case DISPID_NAVIGATEERROR:
            if (pDispParams->rgvarg == nullptr) {
                return E_INVALIDARG;
            }
            // 导航错误
            NavigateError(pDispParams->rgvarg[4].pdispVal, pDispParams->rgvarg[3].pvarVal,
                pDispParams->rgvarg[2].pvarVal, pDispParams->rgvarg[1].pvarVal, pDispParams->rgvarg[0].pboolVal);
            break;
        case DISPID_STATUSTEXTCHANGE:
            break;
        case DISPID_NEWWINDOW3:
            // 新窗口
            if (pDispParams->rgvarg != nullptr) {
                NewWindow3(pDispParams->rgvarg[4].ppdispVal, pDispParams->rgvarg[3].pboolVal,
                    pDispParams->rgvarg[2].uintVal, pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
            }
            break;
        case DISPID_TITLECHANGE: {
            if (pDispParams->rgvarg != nullptr) {
                TitleChange(pDispParams->rgvarg[0].bstrVal); // 更换标题
            }
            break;
        }
        case DISPID_DOCUMENTCOMPLETE: {
            if (pDispParams != nullptr && pDispParams->rgvarg != nullptr) {
                DocumentComplete(pDispParams->rgvarg[1].pdispVal, pDispParams->rgvarg[0].pvarVal); // 完成文档
            }

            break;
        }
        case DISPID_WINDOWCLOSING: {
            if (pDispParams->rgvarg == nullptr) {
                return E_INVALIDARG;
            }
            VARIANT_BOOL IsChildWindow = pDispParams->rgvarg[1].boolVal;
            VARIANT_BOOL *Cancel = pDispParams->rgvarg[0].pboolVal;
            WindowClosing(IsChildWindow, Cancel);
        } break;
        default:
            return DISP_E_MEMBERNOTFOUND;
    }
    return S_OK;
}

STDMETHODIMP CWebBrowserUI::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    if (ppvObject == nullptr) {
        return E_NOINTERFACE;
    }
    *ppvObject = nullptr;

    if (riid == IID_IDocHostUIHandler) {
        *ppvObject = static_cast<IDocHostUIHandler *>(this);
    } else if (riid == IID_IDispatch) {
        *ppvObject = static_cast<IDispatch *>(this);
    } else if (riid == IID_IServiceProvider) {
        *ppvObject = static_cast<IServiceProvider *>(this);
    } else if (riid == IID_IOleCommandTarget) {
        *ppvObject = static_cast<IOleCommandTarget *>(this);
    }

    if (*ppvObject != nullptr) {
        AddRef();
    }
    return *ppvObject == nullptr ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG)CWebBrowserUI::AddRef()
{
    InterlockedIncrement(&m_dwRef);
    return static_cast<ULONG>(m_dwRef);
}

STDMETHODIMP_(ULONG)CWebBrowserUI::Release()
{
    ULONG ulRefCount = static_cast<ULONG>(InterlockedDecrement(&m_dwRef));
    return ulRefCount;
}

void CWebBrowserUI::Navigate2(LPCTSTR lpszUrl)
{
    if (lpszUrl == nullptr) {
        return;
    }

    if (m_pWebBrowser2) {
        CDuiVariant url;
        url.vt = VT_BSTR;
        url.bstrVal = T2BSTR(lpszUrl);
        VARIANT flag;
        flag.vt = VT_I4;
        flag.iVal = navNoReadFromCache + navNoWriteToCache;
        (void)m_pWebBrowser2->Navigate2(&url, &flag, nullptr, nullptr, nullptr);
    }
}

void CWebBrowserUI::Refresh()
{
    if (m_pWebBrowser2) {
        m_pWebBrowser2->Refresh();
    }
}
void CWebBrowserUI::GoBack()
{
    if (m_pWebBrowser2) {
        m_pWebBrowser2->GoBack();
    }
}
void CWebBrowserUI::GoForward()
{
    if (m_pWebBrowser2) {
        m_pWebBrowser2->GoForward();
    }
}
// / DWebBrowserEvents2
void CWebBrowserUI::BeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, VARIANT *&TargetFrameName,
    VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->BeforeNavigate2(this, pDisp, url, Flags, TargetFrameName, PostData, Headers, Cancel);
    }
}

void CWebBrowserUI::NavigateError(IDispatch *pDisp, VARIANT *&url, VARIANT *&TargetFrameName, VARIANT *&StatusCode,
    VARIANT_BOOL *&Cancel)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->NavigateError(this, pDisp, url, TargetFrameName, StatusCode, Cancel);
    }
}

void CWebBrowserUI::NavigateComplete2(IDispatch *pDisp, VARIANT *&url)
{
    CComPtr<IDispatch> spDoc;
    if (m_pWebBrowser2 == nullptr) {
        return;
    }
    m_pWebBrowser2->get_Document(&spDoc);

    if (spDoc) {
        CComQIPtr<ICustomDoc, &IID_ICustomDoc> spCustomDoc(spDoc);
        if (spCustomDoc) {
            spCustomDoc->SetUIHandler(this);
        }
    }

    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->NavigateComplete2(this, pDisp, url);
    }
}

void CWebBrowserUI::ProgressChange(LONG nProgress, LONG nProgressMax)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->ProgressChange(this, nProgress, nProgressMax);
    }
}

void CWebBrowserUI::NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext,
    BSTR bstrUrl)
{
    if (m_pWebBrowserEventHandler) {
        (*Cancel) = TRUE;
        (void)ShellExecute(nullptr, _T("open"), bstrUrl, nullptr, nullptr, SW_SHOWNORMAL);
    }
}
void CWebBrowserUI::CommandStateChange(long Command, VARIANT_BOOL Enable)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->CommandStateChange(this, Command, Enable);
    }
}

void CWebBrowserUI::TitleChange(BSTR bstrTitle)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->TitleChange(this, bstrTitle);
    }
}

void CWebBrowserUI::DocumentComplete(IDispatch *pDisp, VARIANT *&url)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->DocumentComplete(this, pDisp, url);
    }
}

void CWebBrowserUI::WindowClosing(VARIANT_BOOL bValue, VARIANT_BOOL *&bCancel)
{
    if (m_pWebBrowserEventHandler) {
        m_pWebBrowserEventHandler->WindowClosing(this, bValue, bCancel);
    }
}

// IDownloadManager
STDMETHODIMP CWebBrowserUI::Download(IMoniker *pmk, IBindCtx *pbc, DWORD dwBindVerb,
    LONG grfBINDF, BINDINFO *pBindInfo, LPCOLESTR pszHeaders,
    LPCOLESTR pszRedir, UINT uiCP)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->Download(this, pmk, pbc, dwBindVerb, grfBINDF, pBindInfo, pszHeaders,
            pszRedir, uiCP);
    }
    return S_OK;
}

// IDocHostUIHandler
STDMETHODIMP CWebBrowserUI::ShowContextMenu(DWORD dwID, POINT *pptPosition, IUnknown *pCommandTarget,
    IDispatch *pDispatchObjectHit)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->ShowContextMenu(this, dwID, pptPosition, pCommandTarget, pDispatchObjectHit);
    }
    return S_FALSE;
}

STDMETHODIMP CWebBrowserUI::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
    if (pInfo != nullptr) {
        pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;
    }
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->GetHostInfo(this, pInfo);
    }
    return S_OK;
}

STDMETHODIMP CWebBrowserUI::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject,
    IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->ShowUI(this, dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
    }
    return S_OK;
}

STDMETHODIMP CWebBrowserUI::HideUI()
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->HideUI(this);
    }
    return S_OK;
}

STDMETHODIMP CWebBrowserUI::UpdateUI()
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->UpdateUI(this);
    }
    return S_OK;
}

STDMETHODIMP CWebBrowserUI::EnableModeless(BOOL fEnable)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->EnableModeless(this, fEnable);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::OnDocWindowActivate(BOOL fActivate)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->OnDocWindowActivate(this, fActivate);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::OnFrameWindowActivate(BOOL fActivate)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->OnFrameWindowActivate(this, fActivate);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fFrameWindow)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->ResizeBorder(this, prcBorder, pUIWindow, fFrameWindow);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->TranslateAccelerator(this, lpMsg, pguidCmdGroup, nCmdID);
    }
    return S_FALSE;
}

LRESULT CWebBrowserUI::TranslateAccelerator(MSG *pMsg)
{
    if (pMsg == nullptr) {
        return NULL;
    }
    if (pMsg->message == WM_TIMER) {
        if (m_pWebBrowser2 == nullptr || !isUserString(L"ogrizoom")) {
            return E_NOTIMPL;
        }
        VARIANT varZoom;
        m_pWebBrowser2->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, nullptr, &varZoom);
        if (V_VT(&varZoom) == VT_I4) {
            LONG newZoom = varZoom.lVal;
            static LONG normalZoom = varZoom.lVal;
            if (newZoom != normalZoom) {
                varZoom.vt = VT_I4;
                varZoom.lVal = normalZoom;
                m_pWebBrowser2->ExecWB(OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DODEFAULT, &varZoom, nullptr);
            }
        }
    }
    if (pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) {
        return S_FALSE;
    }

    if (m_pWebBrowser2 == nullptr) {
        return E_NOTIMPL;
    }

    // 当前Web窗口不是焦点,不处理加速键
    BOOL bIsChild = FALSE;
    HWND hTempWnd = nullptr;
    HWND hWndFocus = ::GetFocus();

    hTempWnd = hWndFocus;
    while (hTempWnd != nullptr) {
        if (hTempWnd == m_hwndHost) {
            bIsChild = TRUE;
            break;
        }
        hTempWnd = ::GetParent(hTempWnd);
    }
    if (!bIsChild) {
        return S_FALSE;
    }

    IOleInPlaceActiveObject *pObj = nullptr;
    if (FAILED(m_pWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, reinterpret_cast<LPVOID *>(&pObj)))) {
        return S_FALSE;
    }

    HRESULT hResult = pObj->TranslateAccelerator(pMsg);
    pObj->Release();
    return hResult;
}

STDMETHODIMP CWebBrowserUI::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dwReserved)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->GetOptionKeyPath(this, pchKey, dwReserved);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CWebBrowserUI::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
{
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->GetDropTarget(this, pDropTarget, ppDropTarget);
    }
    return S_FALSE; // 使用系统拖拽
}

STDMETHODIMP CWebBrowserUI::GetExternal(IDispatch **ppDispatch)
{
    if (ppDispatch == nullptr) {
        return E_NOTIMPL;
    }
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->GetExternal(this, ppDispatch);
    }
    return S_FALSE;
}

STDMETHODIMP CWebBrowserUI::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut)
{
    if (ppchURLOut == nullptr) {
        return E_INVALIDARG;
    }
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->TranslateUrl(this, dwTranslate, pchURLIn, ppchURLOut);
    } else {
        *ppchURLOut = nullptr;
        return E_NOTIMPL;
    }
}

STDMETHODIMP CWebBrowserUI::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
{
    if (pDO == nullptr || ppDORet == nullptr) {
        return E_INVALIDARG;
    }
    if (m_pWebBrowserEventHandler) {
        return m_pWebBrowserEventHandler->FilterDataObject(this, pDO, ppDORet);
    } else {
        *ppDORet = nullptr;
        return E_NOTIMPL;
    }
}

void CWebBrowserUI::SetWebBrowserEventHandler(CWebBrowserEventHandler *pEventHandler)
{
    if (pEventHandler != nullptr && m_pWebBrowserEventHandler != pEventHandler) {
        m_pWebBrowserEventHandler = pEventHandler;
    }
}

void CWebBrowserUI::Refresh2(int Level)
{
    CDuiVariant vLevel;
    vLevel.vt = VT_I4;
    vLevel.intVal = Level;
    if (m_pWebBrowser2 == nullptr) {
        return;
    }
    m_pWebBrowser2->Refresh2(&vLevel);
}

void CWebBrowserUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("homepage")) == 0) {
        m_sHomePage = pstrValue;
    } else if (_tcsicmp(pstrName, _T("autonavi")) == 0) {
        m_bAutoNavi = (_tcsicmp(pstrValue, _T("true")) == 0);
    } else {
        CActiveXUI::SetAttribute(pstrName, pstrValue);
    }
}

void CWebBrowserUI::NavigateHomePage()
{
    if (!m_sHomePage.IsEmpty()) {
        this->NavigateUrl(m_sHomePage);
    }
}

void CWebBrowserUI::NavigateUrl(LPCTSTR lpszUrl)
{
    if (m_pWebBrowser2 && lpszUrl) {
        m_pWebBrowser2->Navigate((BSTR)SysAllocString(T2BSTR(lpszUrl)), nullptr, nullptr, nullptr, nullptr);
    }
}

LPCTSTR CWebBrowserUI::GetClass() const
{
    return _T("WebBrowserUI");
}

LPVOID CWebBrowserUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_WEBBROWSER) == 0) {
        return static_cast<CWebBrowserUI *>(this);
    }
    return CActiveXUI::GetInterface(pstrName);
}

void CWebBrowserUI::SetHomePage(LPCTSTR lpszUrl)
{
    m_sHomePage.Format(_T("%s"), lpszUrl);
}

LPCTSTR CWebBrowserUI::GetHomePage()
{
    return m_sHomePage;
}

void CWebBrowserUI::SetAutoNavigation(bool bAuto)
{
    if (m_bAutoNavi == bAuto) {
        return;
    }

    m_bAutoNavi = bAuto;
}

bool CWebBrowserUI::IsAutoNavigation()
{
    return m_bAutoNavi;
}

STDMETHODIMP CWebBrowserUI::QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    if (ppvObject == nullptr) {
        return NULL;
    }
    *ppvObject = nullptr;

    if (guidService == SID_SDownloadManager && riid == IID_IDownloadManager) {
        *ppvObject = this;
        return S_OK;
    }

    return hr;
}

HRESULT CWebBrowserUI::RegisterEventHandler(BOOL inAdvise)
{
    CComPtr<IWebBrowser2> pWebBrowser;
    CComPtr<IConnectionPointContainer> pCPC;
    CComPtr<IConnectionPoint> pCP;
    HRESULT hr = GetControl(IID_IWebBrowser2, reinterpret_cast<void **>(&pWebBrowser));
    if (FAILED(hr)) {
        return hr;
    }
    hr = pWebBrowser->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void **>(&pCPC));
    if (FAILED(hr)) {
        return hr;
    }
    hr = pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2, &pCP);
    if (FAILED(hr)) {
        return hr;
    }

    if (inAdvise) {
        hr = pCP->Advise(static_cast<IDispatch *>(this), &m_dwCookie);
    } else {
        hr = pCP->Unadvise(m_dwCookie);
    }
    return hr;
}

DISPID CWebBrowserUI::FindId(IDispatch *pObj, LPOLESTR pName)
{
    DISPID id = 0;
    if (FAILED(pObj->GetIDsOfNames(IID_NULL, &pName, 1, LOCALE_SYSTEM_DEFAULT, &id))) {
        id = -1;
    }
    return id;
}

HRESULT CWebBrowserUI::InvokeMethod(IDispatch *pObj, LPOLESTR pMehtod, VARIANT *pVarResult, VARIANT *ps, int cArgs)
{
    DISPID dispid = FindId(pObj, pMehtod);
    if (dispid == -1) {
        return E_FAIL;
    }

    DISPPARAMS dispparams;
    dispparams.cArgs = cArgs;
    dispparams.rgvarg = ps;
    dispparams.cNamedArgs = 0;
    dispparams.rgdispidNamedArgs = nullptr;

    return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dispparams,
        pVarResult, nullptr, nullptr);
}

HRESULT CWebBrowserUI::GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
    DISPID dispid = FindId(pObj, pName);
    if (dispid == -1) {
        return E_FAIL;
    }

    DISPPARAMS ps;
    ps.cArgs = 0;
    ps.rgvarg = nullptr;
    ps.cNamedArgs = 0;
    ps.rgdispidNamedArgs = nullptr;
    return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, nullptr, nullptr);
}

HRESULT CWebBrowserUI::SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
    DISPID dispid = FindId(pObj, pName);
    if (dispid == -1) {
        return E_FAIL;
    }

    DISPPARAMS ps;
    ps.cArgs = 1;
    ps.rgvarg = pValue;
    ps.cNamedArgs = 0;
    ps.rgdispidNamedArgs = nullptr;

    return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, nullptr, nullptr, nullptr);
}

IDispatch *CWebBrowserUI::GetHtmlWindow()
{
    IDispatch *pDp = nullptr;
    HRESULT hr = 0;
    if (m_pWebBrowser2) {
        hr = m_pWebBrowser2->get_Document(&pDp);
    }

    if (FAILED(hr)) {
        return nullptr;
    }

    CComQIPtr<IHTMLDocument2> pHtmlDoc2 = pDp;

    if (pHtmlDoc2 == nullptr) {
        return nullptr;
    }

    hr = pHtmlDoc2->get_parentWindow(&_pHtmlWnd2);

    if (FAILED(hr)) {
        return nullptr;
    }

    IDispatch *pHtmlWindown = nullptr;
    if (_pHtmlWnd2 == nullptr) {
        return nullptr;
    }
    hr = _pHtmlWnd2->QueryInterface(IID_IDispatch, reinterpret_cast<void **>(&pHtmlWindown));
    if (FAILED(hr)) {
        return nullptr;
    }

    return pHtmlWindown;
}

IWebBrowser2 *CWebBrowserUI::GetWebBrowser2(void)
{
    return m_pWebBrowser2;
}

HRESULT STDMETHODCALLTYPE CWebBrowserUI::QueryStatus(__RPC__in_opt const GUID *pguidCmdGroup, ULONG cCmds,
    __RPC__inout_ecount_full(cCmds)OLECMD prgCmds[], __RPC__inout_opt OLECMDTEXT *pCmdText)
{
    HRESULT hr = OLECMDERR_E_NOTSUPPORTED;
    return hr;
}

HRESULT STDMETHODCALLTYPE CWebBrowserUI::Exec(__RPC__in_opt const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt,
    __RPC__in_opt VARIANT *pvaIn, __RPC__inout_opt VARIANT *pvaOut)
{
    HRESULT hr = S_OK;

    if (pguidCmdGroup && IsEqualGUID(*pguidCmdGroup, CGID_DocHostCommandHandler)) {
        switch (nCmdID) {
            case OLECMDID_SHOWSCRIPTERROR: {
                IHTMLDocument2 *pDoc = nullptr;
                IHTMLWindow2 *pWindow = nullptr;
                IHTMLEventObj *pEventObj = nullptr;
                BSTR rgwszNames[5] = {
                    SysAllocString(L"errorLine"),
                    SysAllocString(L"errorCharacter"),
                    SysAllocString(L"errorCode"),
                    SysAllocString(L"errorMessage"),
                    SysAllocString(L"errorUrl")
                };
                DISPID rgDispIDs[5];
                VARIANT rgvaEventInfo[5];
                DISPPARAMS params;
                BOOL fContinueRunningScripts = true;
                int i;

                params.cArgs = 0;
                params.cNamedArgs = 0;

                // Get the document that is currently being viewed.
                if (pvaIn == nullptr) {
                    return E_FAIL;
                }
                hr = pvaIn->punkVal->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void **>(&pDoc));
                // Get document.parentWindow.
                hr = pDoc->get_parentWindow(&pWindow);
                pDoc->Release();
                // Get the window.event object.
                hr = pWindow->get_event(&pEventObj);
                // Get the error info from the window.event object.
                for (i = 0; i < 5; i++) {
                    // Get the property's dispID.
                    hr = pEventObj->GetIDsOfNames(IID_NULL, &rgwszNames[i], 1, LOCALE_SYSTEM_DEFAULT, &rgDispIDs[i]);
                    // Get the value of the property.
                    hr = pEventObj->Invoke(rgDispIDs[i], IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &params,
                        &rgvaEventInfo[i], nullptr, nullptr);
                    SysFreeString(rgwszNames[i]);
                }

                // At this point, you would normally alert the user with
                // the information about the error, which is now contained
                // in rgvaEventInfo[]. Or, you could just exit silently.
                if (pvaOut == nullptr) {
                    return E_FAIL;
                }
                (*pvaOut).vt = VT_BOOL;
                if (fContinueRunningScripts) {
                    // Continue running scripts on the page.
                    (*pvaOut).boolVal = VARIANT_TRUE;
                } else {
                    // Stop running scripts on the page.
                    (*pvaOut).boolVal = VARIANT_FALSE;
                }
                break;
            }
            default:
                hr = OLECMDERR_E_NOTSUPPORTED;
                break;
        }
    } else {
        hr = OLECMDERR_E_UNKNOWNGROUP;
    }
    return (hr);
}
}