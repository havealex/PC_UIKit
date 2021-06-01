﻿#include "../StdAfx.h"
/* *************************************************************************
THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Author: Leon Finker  1/2001
************************************************************************* */
// IDataObjectImpl.cpp: implementation of the CIDataObjectImpl class.
// ////////////////////////////////////////////////////////////////////
#include <atlbase.h>
#include "DragDropImpl.h"

namespace DuiLib {
// ////////////////////////////////////////////////////////////////////
// CIDataObject Class
// ////////////////////////////////////////////////////////////////////
CIDataObject::CIDataObject(CIDropSource *pDropSource) : m_cRefCount(0), m_pDropSource(pDropSource) {}

CIDataObject::~CIDataObject()
{
    for (size_t i = 0; i < m_StgMedium.size(); ++i) {
        ReleaseStgMedium(m_StgMedium[i]);
        delete m_StgMedium[i];
        m_StgMedium[i] = nullptr;
    }
    for (size_t j = 0; j < m_ArrFormatEtc.size(); ++j) {
        delete m_ArrFormatEtc[j];
        m_ArrFormatEtc[j] = nullptr;
    }
}

STDMETHODIMP CIDataObject::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
    if (ppvObject == nullptr) {
        return S_FALSE;
    }
    *ppvObject = nullptr;
    if (IID_IUnknown == riid || IID_IDataObject == riid) {
        *ppvObject = this;
    }
    if (*ppvObject != nullptr) {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)CIDataObject::AddRef(void)
{
    ATLTRACE("CIDataObject::AddRef\n");
    return static_cast<ULONG>(++m_cRefCount);
}

STDMETHODIMP_(ULONG)CIDataObject::Release(void)
{
    ATLTRACE("CIDataObject::Release\n");
    long nTemp;
    nTemp = --m_cRefCount;
    if (nTemp == 0) {
        delete this;
    }
    return static_cast<ULONG>(nTemp);
}

STDMETHODIMP CIDataObject::GetData(FORMATETC __RPC_FAR *pformatetcIn,
    STGMEDIUM __RPC_FAR *pmedium)
{
    ATLTRACE("CIDataObject::GetData\n");
    if (pformatetcIn == nullptr || pmedium == nullptr) {
        return E_INVALIDARG;
    }
    pmedium->hGlobal = nullptr;

    ATLASSERT(m_StgMedium.size() == m_ArrFormatEtc.size());
    for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i) {
        if (pformatetcIn->tymed & m_ArrFormatEtc[i]->tymed && pformatetcIn->dwAspect == m_ArrFormatEtc[i]->dwAspect &&
            pformatetcIn->cfFormat == m_ArrFormatEtc[i]->cfFormat) {
            CopyMedium(pmedium, m_StgMedium[i], m_ArrFormatEtc[i]);
            return S_OK;
        }
    }
    return DV_E_FORMATETC;
}

STDMETHODIMP CIDataObject::GetDataHere(FORMATETC __RPC_FAR *pformatetc, STGMEDIUM __RPC_FAR *pmedium)
{
    ATLTRACE("CIDataObject::GetDataHere\n");

    return E_NOTIMPL;
}

STDMETHODIMP CIDataObject::QueryGetData(FORMATETC __RPC_FAR *pformatetc)
{
    ATLTRACE("CIDataObject::QueryGetData\n");
    if (pformatetc == nullptr) {
        return E_INVALIDARG;
    }

    // support others if needed DVASPECT_THUMBNAIL  //DVASPECT_ICON   //DVASPECT_DOCPRINT
    if (!(DVASPECT_CONTENT & pformatetc->dwAspect)) {
        return (DV_E_DVASPECT);
    }
    HRESULT hr = DV_E_TYMED;
    for (size_t i = 0; i < m_ArrFormatEtc.size(); ++i) {
        if (pformatetc->tymed & m_ArrFormatEtc[i]->tymed) {
            if (pformatetc->cfFormat == m_ArrFormatEtc[i]->cfFormat) {
                return S_OK;
            } else {
                hr = DV_E_CLIPFORMAT;
            }
        } else {
            hr = DV_E_TYMED;
        }
    }
    return hr;
}

STDMETHODIMP CIDataObject::GetCanonicalFormatEtc(FORMATETC __RPC_FAR *pformatectIn,
    FORMATETC __RPC_FAR *pformatetcOut)
{
    ATLTRACE("CIDataObject::GetCanonicalFormatEtc\n");
    if (pformatetcOut == nullptr) {
        return E_INVALIDARG;
    }
    return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP CIDataObject::SetData(FORMATETC __RPC_FAR *pformatetc, STGMEDIUM __RPC_FAR *pmedium,
    BOOL fRelease)
{
    ATLTRACE("CIDataObject::SetData\n");
    if (pformatetc == nullptr || pmedium == nullptr) {
        return E_INVALIDARG;
    }

    ATLASSERT(pformatetc->tymed == pmedium->tymed);
    FORMATETC *fetc = new FORMATETC;
    STGMEDIUM *pStgMed = new STGMEDIUM;

    if (fetc == nullptr || pStgMed == nullptr) {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(fetc, sizeof(FORMATETC));
    ZeroMemory(pStgMed, sizeof(STGMEDIUM));

    *fetc = *pformatetc;
    m_ArrFormatEtc.push_back(fetc);

    if (fRelease) {
        *pStgMed = *pmedium;
    } else {
        CopyMedium(pStgMed, pmedium, pformatetc);
    }
    m_StgMedium.push_back(pStgMed);

    return S_OK;
}
void CIDataObject::CopyMedium(STGMEDIUM *pMedDest, STGMEDIUM *pMedSrc, FORMATETC *pFmtSrc)
{
    if (pMedDest == nullptr || pMedSrc == nullptr || pFmtSrc == nullptr) {
        return;
    }
    switch (pMedSrc->tymed) {
        case TYMED_HGLOBAL: {
            if (pMedSrc->hGlobal != nullptr) {
                pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
            }
            break;
        }
        case TYMED_GDI: {
            if (pMedSrc->hBitmap != nullptr) {
                pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);
            }
            break;
        }
        case TYMED_MFPICT: {
            if (pMedSrc->hMetaFilePict != nullptr) {
                pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict,
                    pFmtSrc->cfFormat, NULL);
            }
            break;
        }
        case TYMED_ENHMF: {
            if (pMedSrc->hEnhMetaFile == nullptr) {
                return;
            }
            pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);
            break;
        }
        case TYMED_FILE: {
            pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);
            break;
        }
        case TYMED_ISTREAM: {
            pMedDest->pstm = pMedSrc->pstm;
            pMedSrc->pstm->AddRef();
            break;
        }
        case TYMED_ISTORAGE: {
            pMedDest->pstg = pMedSrc->pstg;
            pMedSrc->pstg->AddRef();
            break;
        }
        case TYMED_NULL: {
        }
        default:
            break;
    }
    pMedDest->tymed = pMedSrc->tymed;
    pMedDest->pUnkForRelease = nullptr;
    if (pMedSrc->pUnkForRelease != nullptr) {
        pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
        pMedSrc->pUnkForRelease->AddRef();
    }
}
STDMETHODIMP CIDataObject::EnumFormatEtc(DWORD dwDirection,
    IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc)
{
    ATLTRACE("CIDataObject::EnumFormatEtc\n");
    if (ppenumFormatEtc == nullptr) {
        return E_POINTER;
    }

    *ppenumFormatEtc = nullptr;
    switch (dwDirection) {
        case DATADIR_GET:
            *ppenumFormatEtc = new CEnumFormatEtc(m_ArrFormatEtc);
            if (*ppenumFormatEtc == nullptr) {
                return E_OUTOFMEMORY;
            }
            (*ppenumFormatEtc)->AddRef();
            break;

        case DATADIR_SET:
        default:
            return E_NOTIMPL;
            break;
    }

    return S_OK;
}

STDMETHODIMP CIDataObject::DAdvise(FORMATETC __RPC_FAR *pformatetc, DWORD advf,
    IAdviseSink __RPC_FAR *pAdvSink, DWORD __RPC_FAR *pdwConnection)
{
    ATLTRACE("CIDataObject::DAdvise\n");
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP CIDataObject::DUnadvise(DWORD dwConnection)
{
    ATLTRACE("CIDataObject::DUnadvise\n");
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CIDataObject::EnumDAdvise(IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise)
{
    ATLTRACE("CIDataObject::EnumDAdvise\n");
    return OLE_E_ADVISENOTSUPPORTED;
}

// ////////////////////////////////////////////////////////////////////
// CIDropSource Class
// ////////////////////////////////////////////////////////////////////
STDMETHODIMP CIDropSource::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
    if (ppvObject == nullptr) {
        return S_FALSE;
    }
    *ppvObject = nullptr;
    if (IID_IUnknown == riid || IID_IDropSource == riid) {
        *ppvObject = this;
    }

    if (*ppvObject != nullptr) {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)CIDropSource::AddRef(void)
{
    ATLTRACE("CIDropSource::AddRef\n");
    return static_cast<ULONG>(++m_cRefCount);
}

STDMETHODIMP_(ULONG)CIDropSource::Release(void)
{
    ATLTRACE("CIDropSource::Release\n");
    long nTemp;
    nTemp = --m_cRefCount;
    ATLASSERT(nTemp >= 0);
    if (nTemp == 0) {
        delete this;
    }
    return static_cast<ULONG>(nTemp);
}

STDMETHODIMP CIDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    if (fEscapePressed) {
        return DRAGDROP_S_CANCEL;
    }
    if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON))) {
        m_bDropped = true;
        return DRAGDROP_S_DROP;
    }

    return S_OK;
}

STDMETHODIMP CIDropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}

// ////////////////////////////////////////////////////////////////////
// CEnumFormatEtc Class
// ////////////////////////////////////////////////////////////////////
CEnumFormatEtc::CEnumFormatEtc(const FormatEtcArray &ArrFE) : m_cRefCount(0), m_iCur(0)
{
    ATLTRACE("CEnumFormatEtc::CEnumFormatEtc()\n");
    for (size_t i = 0; i < ArrFE.size(); ++i) {
        m_pFmtEtc.push_back(ArrFE[i]);
    }
}

CEnumFormatEtc::CEnumFormatEtc(const PFormatEtcArray &ArrFE) : m_cRefCount(0), m_iCur(0)
{
    for (size_t i = 0; i < ArrFE.size(); ++i) {
        m_pFmtEtc.push_back(*ArrFE[i]);
    }
}

STDMETHODIMP CEnumFormatEtc::QueryInterface(REFIID refiid, void FAR *FAR *ppv)
{
    if (ppv == nullptr) {
        return S_FALSE;
    }
    ATLTRACE("CEnumFormatEtc::QueryInterface()\n");
    *ppv = nullptr;
    if (IID_IUnknown == refiid || IID_IEnumFORMATETC == refiid) {
        *ppv = this;
    }

    if (*ppv != nullptr) {
        ((LPUNKNOWN)* ppv)->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)CEnumFormatEtc::AddRef(void)
{
    ATLTRACE("CEnumFormatEtc::AddRef()\n");
    return ++m_cRefCount;
}

STDMETHODIMP_(ULONG)CEnumFormatEtc::Release(void)
{
    ATLTRACE("CEnumFormatEtc::Release()\n");
    long nTemp = --m_cRefCount;
    ATLASSERT(nTemp >= 0);
    if (nTemp == 0) {
        delete this;
    }
    return static_cast<ULONG>(nTemp);
}

STDMETHODIMP CEnumFormatEtc::Next(ULONG celt, LPFORMATETC lpFormatEtc, ULONG FAR *pceltFetched)
{
    ATLTRACE("CEnumFormatEtc::Next()\n");
    if (pceltFetched != nullptr) {
        *pceltFetched = 0;
    }

    ULONG cReturn = celt;

    if (celt <= 0 || lpFormatEtc == nullptr || m_iCur >= decltype(m_iCur)(m_pFmtEtc.size())) {
        return S_FALSE;
    }

    if (pceltFetched == nullptr && celt != 1) {
        // pceltFetched can be nullptr only for 1 item request
        return S_FALSE;
    }

    while (m_iCur < decltype(m_iCur)(m_pFmtEtc.size()) && cReturn > 0) {
        *lpFormatEtc++ = m_pFmtEtc[m_iCur++];
        --cReturn;
    }
    if (pceltFetched != nullptr) {
        *pceltFetched = celt - cReturn;
    }

    return (cReturn == 0) ? S_OK : S_FALSE;
}

STDMETHODIMP CEnumFormatEtc::Skip(ULONG celt)
{
    ATLTRACE("CEnumFormatEtc::Skip()\n");
    if ((m_iCur + static_cast<int>(celt)) >= decltype(m_iCur)(m_pFmtEtc.size())) {
        return S_FALSE;
    }
    m_iCur += celt;
    return S_OK;
}

STDMETHODIMP CEnumFormatEtc::Reset(void)
{
    ATLTRACE("CEnumFormatEtc::Reset()\n");
    m_iCur = 0;
    return S_OK;
}

STDMETHODIMP CEnumFormatEtc::Clone(IEnumFORMATETC FAR *FAR *ppCloneEnumFormatEtc)
{
    ATLTRACE("CEnumFormatEtc::Clone()\n");
    if (ppCloneEnumFormatEtc == nullptr) {
        return E_POINTER;
    }

    CEnumFormatEtc *newEnum = new CEnumFormatEtc(m_pFmtEtc);
    if (newEnum == nullptr) {
        return E_OUTOFMEMORY;
    }
    newEnum->AddRef();
    newEnum->m_iCur = m_iCur;
    *ppCloneEnumFormatEtc = newEnum;
    return S_OK;
}

// ////////////////////////////////////////////////////////////////////
// CIDropTarget Class
// ////////////////////////////////////////////////////////////////////
CIDropTarget::CIDropTarget(HWND hTargetWnd)
    : m_hTargetWnd(hTargetWnd),
      castCallback(nullptr),
      m_cRefCount(0),
      m_bAllowDrop(false),
      m_pDropTargetHelper(nullptr),
      m_pSupportedFrmt(nullptr)
{
    DropTargetInit();
}

CIDropTarget::~CIDropTarget()
{
    if (m_pDropTargetHelper != nullptr) {
        m_pDropTargetHelper->Release();
        m_pDropTargetHelper = nullptr;
    }
}

void CIDropTarget::RegisterHicastCallback(HicastDragCallback *callback)
{
    castCallback = callback;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::QueryInterface(REFIID riid,
    void __RPC_FAR *__RPC_FAR *ppvObject)
{
    if (ppvObject == nullptr) {
        return S_FALSE;
    }
    *ppvObject = nullptr;
    if (IID_IUnknown == riid || IID_IDropTarget == riid) {
        *ppvObject = this;
    }

    if (*ppvObject != nullptr) {
        (reinterpret_cast<LPUNKNOWN>(*ppvObject))->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CIDropTarget::Release(void)
{
    ATLTRACE("CIDropTarget::Release\n");
    long nTemp;
    nTemp = --m_cRefCount;
    ATLASSERT(nTemp >= 0);
    if (nTemp == 0) {
        delete this;
    }
    return static_cast<ULONG>(nTemp);
}

bool CIDropTarget::QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect)
{
    if (pdwEffect == nullptr) {
        return false;
    }
    ATLTRACE("CIDropTarget::QueryDrop\n");
    DWORD dwOKEffects = *pdwEffect;

    if (!m_bAllowDrop) {
        *pdwEffect = DROPEFFECT_NONE;
        return false;
    }
    // CTRL+SHIFT  -- DROPEFFECT_LINK
    // CTRL        -- DROPEFFECT_COPY
    // SHIFT       -- DROPEFFECT_MOVE
    // no modifier -- DROPEFFECT_MOVE or whatever is allowed by src
    *pdwEffect = (grfKeyState & MK_CONTROL) ? ((grfKeyState & MK_SHIFT) ? DROPEFFECT_LINK : DROPEFFECT_COPY) :
        ((grfKeyState & MK_SHIFT) ? DROPEFFECT_MOVE : 0);
    if (*pdwEffect == 0) {
        // No modifier keys used by user while dragging.
        if (DROPEFFECT_COPY & dwOKEffects) {
            *pdwEffect = DROPEFFECT_COPY;
        } else if (DROPEFFECT_MOVE & dwOKEffects) {
            *pdwEffect = DROPEFFECT_MOVE;
        } else if (DROPEFFECT_LINK & dwOKEffects) {
            *pdwEffect = DROPEFFECT_LINK;
        } else {
            *pdwEffect = DROPEFFECT_NONE;
        }
    } else {
        // Check if the drag source application allows the drop effect desired by user.
        // The drag source specifies this in DoDragDrop
        if (!(*pdwEffect & dwOKEffects)) {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }

    return (*pdwEffect == DROPEFFECT_NONE) ? false : true;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::DragEnter(IDataObject __RPC_FAR *pDataObj,
    DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
    if (pdwEffect == nullptr) {
        return S_FALSE;
    }
    ATLTRACE("CIDropTarget::DragEnter\n");
    if (pDataObj == nullptr) {
        return E_INVALIDARG;
    }

    if (m_pDuiDropTarget != nullptr) {
        m_pDuiDropTarget->ShareDragEnter(pDataObj, grfKeyState, pt, pdwEffect);
    }
    if (castCallback != nullptr) {
        *pdwEffect = castCallback->OnDragEnter(pDataObj, grfKeyState, pt, pdwEffect);
    }
    if (m_pDropTargetHelper) {
        m_pDropTargetHelper->DragEnter(m_hTargetWnd, pDataObj, (LPPOINT)&pt, *pdwEffect);
    }
    m_pSupportedFrmt = nullptr;
    for (size_t i = 0; i < m_formatetc.size(); ++i) {
        m_bAllowDrop = (pDataObj->QueryGetData(&m_formatetc[i]) == S_OK) ? true : false;
        if (m_bAllowDrop) {
            m_pSupportedFrmt = &m_formatetc[i];
            break;
        }
    }

    QueryDrop(grfKeyState, pdwEffect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::DragOver(DWORD grfKeyState, POINTL pt,
    DWORD __RPC_FAR *pdwEffect)
{
    if (pdwEffect == nullptr || m_pDropTargetHelper == nullptr) {
        return S_FALSE;
    }
    if (m_pDuiDropTarget != nullptr) {
        m_pDuiDropTarget->ShareDragOver(grfKeyState, pt, pdwEffect);
    }
    if (castCallback) {
        bool needLocalHelper = !m_pDropTargetHelper;
        *pdwEffect = castCallback->OnDragOver(pt, needLocalHelper);
    }
    ATLTRACE("CIDropTarget::DragOver\n");
    if (m_pDropTargetHelper) {
        m_pDropTargetHelper->DragOver((LPPOINT)&pt, *pdwEffect);
    }
    QueryDrop(grfKeyState, pdwEffect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::DragLeave(void)
{
    ATLTRACE("CIDropTarget::DragLeave\n");
    if (m_pDuiDropTarget != nullptr) {
        m_pDuiDropTarget->ShareDragLeave();
    }
    if (castCallback) {
        castCallback->OnDragLeave();
    }

    if (m_pDropTargetHelper) {
        m_pDropTargetHelper->DragLeave();
    }

    m_bAllowDrop = false;
    m_pSupportedFrmt = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CIDropTarget::Drop(IDataObject __RPC_FAR *pDataObj,
    DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect)
{
    ATLTRACE("CIDropTarget::Drop\n");
    if (pDataObj == nullptr || pdwEffect == nullptr) {
        return E_INVALIDARG;
    }

    if (m_pDuiDropTarget != nullptr) {
        m_pDuiDropTarget->ShareDrop(pDataObj, grfKeyState, pt, pdwEffect);
    }
    if (castCallback) {
        DWORD hicastDW = castCallback->OnDrop(pDataObj, grfKeyState, pt, pdwEffect);
        if (hicastDW) {
            m_bAllowDrop = false;
            *pdwEffect = DROPEFFECT_NONE;
            m_pSupportedFrmt = nullptr;
            return S_OK;
        }
    }
    if (m_pDropTargetHelper) {
        m_pDropTargetHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
    }

    if (QueryDrop(grfKeyState, pdwEffect)) {
        if (m_bAllowDrop && m_pSupportedFrmt != nullptr) {
            STGMEDIUM medium;
            if (pDataObj->GetData(m_pSupportedFrmt, &medium) == S_OK) {
                if (OnDrop(m_pSupportedFrmt, medium, pdwEffect)) {
                    // does derive class wants us to free medium?
                    ReleaseStgMedium(&medium);
                }
            }
        }
    }
    m_bAllowDrop = false;
    *pdwEffect = DROPEFFECT_NONE;
    m_pSupportedFrmt = nullptr;
    return S_OK;
}

bool CIDropTarget::DragDropRegister(DuiDropTarget *pDuiDropTarget, HWND hWnd, DWORD AcceptKeyState)
{
    if (!IsWindow(hWnd) || pDuiDropTarget == nullptr) {
        return false;
    }
    m_pDuiDropTarget = pDuiDropTarget;
    return true;
}

void CIDropTarget::DropTargetInit()
{
    if (m_pDropTargetHelper == nullptr) {
        if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
            IID_IDropTargetHelper, reinterpret_cast<LPVOID *>(&m_pDropTargetHelper)))) {
            m_pDropTargetHelper = nullptr;
        }
    }
}
}