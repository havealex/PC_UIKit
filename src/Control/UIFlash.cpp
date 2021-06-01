/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIFlash.h"
#include <atlcomcli.h>

constexpr int DISPID_FLASHEVENT_FLASHCALL = 0x00C5;
constexpr int DISPID_FLASHEVENT_FSCOMMAND = 0x0096;
constexpr int DISPID_FLASHEVENT_ONPROGRESS = 0x07A6;

namespace DuiLib {
IMPLEMENT_DUICONTROL(CFlashUI)

CFlashUI::CFlashUI(void) : m_dwRef(0), m_dwCookie(0), m_pFlash(nullptr), m_pFlashEventHandler(nullptr)
{
    CDuiString strFlashCLSID = _T("{D27CDB6E-AE6D-11CF-96B8-444553540000}");
    const int flashIDSpace = 100;
    OLECHAR szCLSID[flashIDSpace] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, strFlashCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
    _tcsncpy_s(szCLSID, lengthof(szCLSID), strFlashCLSID, lengthof(szCLSID) - 1);
#endif
    ::CLSIDFromString(szCLSID, &m_clsid);
}

CFlashUI::~CFlashUI(void)
{
    if (m_pFlashEventHandler) {
        m_pFlashEventHandler->Release();
        m_pFlashEventHandler = nullptr;
    }
    ReleaseControl();
}

LPCTSTR CFlashUI::GetClass() const
{
    return DUI_CTR_FLASH;
}

LPVOID CFlashUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_FLASH) == 0) {
        return static_cast<CFlashUI *>(this);
    }
    return CActiveXUI::GetInterface(pstrName);
}

HRESULT STDMETHODCALLTYPE CFlashUI::GetTypeInfoCount(__RPC__out UINT *pctinfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashUI::GetTypeInfo(UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashUI::GetIDsOfNames(__RPC__in REFIID riid,
    __RPC__in_ecount_full(cNames)LPOLESTR *rgszNames, UINT cNames, LCID lcid,
    __RPC__out_ecount_full(cNames)DISPID *rgDispId)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashUI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    return S_OK;
    switch (dispIdMember) {
        case DISPID_FLASHEVENT_FLASHCALL: {
            if (pDispParams->cArgs != 1 || pDispParams->rgvarg[0].vt != VT_BSTR) {
                return E_INVALIDARG;
            }
            return this->FlashCall(pDispParams->rgvarg[0].bstrVal);
        }
        case DISPID_FLASHEVENT_FSCOMMAND: {
            const int argsValue = 2;
            if (pDispParams && pDispParams->cArgs == argsValue) {
                if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR) {
                    return FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
                } else {
                    return DISP_E_TYPEMISMATCH;
                }
            } else {
                return DISP_E_BADPARAMCOUNT;
            }
        }
        case DISPID_FLASHEVENT_ONPROGRESS: {
            return OnProgress(*pDispParams->rgvarg[0].plVal);
        }
        case DISPID_READYSTATECHANGE: {
            return this->OnReadyStateChange(pDispParams->rgvarg[0].lVal);
        }
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CFlashUI::QueryInterface(REFIID riid, void **ppvObject)
{
    if (ppvObject == nullptr) {
        return E_NOINTERFACE;
    }
    *ppvObject = nullptr;
    if (riid == IID_IUnknown) {
        *ppvObject = static_cast<LPUNKNOWN>(this);
    } else if (riid == IID_IDispatch) {
        *ppvObject = static_cast<IDispatch *>(this);
    } else if (riid == __uuidof(_IShockwaveFlashEvents)) {
        *ppvObject = static_cast<_IShockwaveFlashEvents *>(this);
    }
    if (*ppvObject != nullptr) {
        AddRef();
    }
    return *ppvObject == nullptr ? E_NOINTERFACE : S_OK;
}

ULONG STDMETHODCALLTYPE CFlashUI::AddRef(void)
{
    ::InterlockedIncrement(&m_dwRef);
    return static_cast<ULONG>(m_dwRef);
}

ULONG STDMETHODCALLTYPE CFlashUI::Release(void)
{
    ::InterlockedDecrement(&m_dwRef);
    return static_cast<ULONG>(m_dwRef);
}

HRESULT CFlashUI::OnReadyStateChange(long newState)
{
    if (m_pFlashEventHandler) {
        return m_pFlashEventHandler->OnReadyStateChange(newState);
    }
    return S_OK;
}

HRESULT CFlashUI::OnProgress(long percentDone)
{
    if (m_pFlashEventHandler) {
        return m_pFlashEventHandler->OnProgress(percentDone);
    }
    return S_OK;
}

HRESULT CFlashUI::FSCommand(_bstr_t command, _bstr_t args)
{
    if (m_pFlashEventHandler) {
        return m_pFlashEventHandler->FSCommand(command, args);
    }
    return S_OK;
}

HRESULT CFlashUI::FlashCall(_bstr_t request)
{
    if (m_pFlashEventHandler) {
        return m_pFlashEventHandler->FlashCall(request);
    }
    return S_OK;
}

void CFlashUI::ReleaseControl()
{
    RegisterEventHandler(FALSE);
    if (m_pFlash) {
        m_pFlash->Release();
        m_pFlash = nullptr;
    }
}

bool CFlashUI::DoCreateControl()
{
    if (!CActiveXUI::DoCreateControl()) {
        return false;
    }
    GetControl(__uuidof(IShockwaveFlash), reinterpret_cast<LPVOID *>(&m_pFlash));
    RegisterEventHandler(TRUE);
    return true;
}

void CFlashUI::SetFlashEventHandler(CFlashEventHandler *pHandler)
{
    if (m_pFlashEventHandler != nullptr) {
        m_pFlashEventHandler->Release();
    }
    if (pHandler == nullptr) {
        m_pFlashEventHandler = pHandler;
        return;
    }
    m_pFlashEventHandler = pHandler;
    if (m_pFlashEventHandler != nullptr) {
        m_pFlashEventHandler->AddRef();
    }
}

LRESULT CFlashUI::TranslateAccelerator(MSG *pMsg)
{
    if (pMsg == nullptr) {
        return S_FALSE;
    }
    if (pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) {
        return S_FALSE;
    }
    if (m_pFlash == nullptr) {
        return E_NOTIMPL;
    }

    // ��ǰWeb���ڲ��ǽ���,��������ټ�
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
    CComPtr<IOleInPlaceActiveObject> pObj;
    if (FAILED(m_pFlash->QueryInterface(IID_IOleInPlaceActiveObject, reinterpret_cast<LPVOID *>(&pObj)))) {
        return S_FALSE;
    }
    HRESULT hResult = pObj->TranslateAccelerator(pMsg);
    return hResult;
}

HRESULT CFlashUI::RegisterEventHandler(BOOL inAdvise)
{
    if (m_pFlash == nullptr) {
        return S_FALSE;
    }
    HRESULT hr = S_FALSE;
    CComPtr<IConnectionPointContainer> pCPC;
    CComPtr<IConnectionPoint> pCP;
    hr = m_pFlash->QueryInterface(IID_IConnectionPointContainer, reinterpret_cast<void **>(&pCPC));
    if (FAILED(hr)) {
        return hr;
    }
    hr = pCPC->FindConnectionPoint(__uuidof(_IShockwaveFlashEvents), &pCP);
    if (FAILED(hr)) {
        return hr;
    }
    if (inAdvise) {
        hr = pCP->Advise(reinterpret_cast<IDispatch *>(this), &m_dwCookie);
    } else {
        hr = pCP->Unadvise(m_dwCookie);
    }
    return hr;
}
};