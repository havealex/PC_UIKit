// IDataObjectImpl.h: interface for the CIDataObjectImpl class.
/* *************************************************************************
THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Author: Leon Finker  1/2001
************************************************************************* */
#ifndef DRAGDROPIMPL_H
#define DRAGDROPIMPL_H
#include <shlobj.h>
#include <vector>

namespace DuiLib {
using FormatEtcArray = std::vector<FORMATETC> ;
using PFormatEtcArray = std::vector<FORMATETC *>;
using PStgMediumArray = std::vector<STGMEDIUM *> ;

class UILIB_API CEnumFormatEtc : public IEnumFORMATETC {
private:
    ULONG m_cRefCount;
    FormatEtcArray m_pFmtEtc;
    int m_iCur;

public:
    explicit CEnumFormatEtc(const FormatEtcArray &ArrFE);
    explicit CEnumFormatEtc(const PFormatEtcArray &ArrFE);
    ~CEnumFormatEtc() = default;
    // IUnknown members
    STDMETHOD(QueryInterface)(REFIID, void FAR *FAR *);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    // IEnumFORMATETC members
    STDMETHOD(Next)(ULONG, LPFORMATETC, ULONG FAR *);
    STDMETHOD(Skip)(ULONG);
    STDMETHOD(Reset)(void);
    STDMETHOD(Clone)(IEnumFORMATETC FAR *FAR *);
};

class UILIB_API CIDropSource : public IDropSource {
    long m_cRefCount;

public:
    bool m_bDropped;

    CIDropSource() : m_cRefCount(0), m_bDropped(false) {}
    ~CIDropSource() = default;
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);
    // IDropSource
    virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);

    virtual HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);
};

class UILIB_API CIDataObject : public IDataObject
{
    CIDropSource *m_pDropSource;
    long m_cRefCount;
    PFormatEtcArray m_ArrFormatEtc;
    PStgMediumArray m_StgMedium;

public:
    explicit CIDataObject(CIDropSource *pDropSource);
    ~CIDataObject();
    void CopyMedium(STGMEDIUM *pMedDest, STGMEDIUM *pMedSrc, FORMATETC *pFmtSrc);
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // IDataObject
    virtual HRESULT STDMETHODCALLTYPE GetData(FORMATETC __RPC_FAR *pformatetcIn,
        STGMEDIUM __RPC_FAR *pmedium);

    virtual HRESULT STDMETHODCALLTYPE GetDataHere(FORMATETC __RPC_FAR *pformatetc,
        STGMEDIUM __RPC_FAR *pmedium);

    virtual HRESULT STDMETHODCALLTYPE QueryGetData(FORMATETC __RPC_FAR *pformatetc);

    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(FORMATETC __RPC_FAR *pformatectIn,
        FORMATETC __RPC_FAR *pformatetcOut);

    virtual HRESULT STDMETHODCALLTYPE SetData(FORMATETC __RPC_FAR *pformatetc,
        STGMEDIUM __RPC_FAR *pmedium, BOOL fRelease);

    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(DWORD dwDirection,
        IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc);

    virtual HRESULT STDMETHODCALLTYPE DAdvise(FORMATETC __RPC_FAR *pformatetc, DWORD advf,
        IAdviseSink __RPC_FAR *pAdvSink, DWORD __RPC_FAR *pdwConnection);

    virtual HRESULT STDMETHODCALLTYPE DUnadvise(DWORD dwConnection);

    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise);

};

class HicastDragCallback {
public:
    virtual DWORD OnDragEnter(IDataObject __RPC_FAR *pDataObj, DWORD grfKeyState,
        POINTL pt, DWORD __RPC_FAR *pdwEffect) = 0;
    virtual DWORD OnDragOver(POINTL pt, BOOL needLocalHelper) = 0;
    virtual DWORD OnDrop(IDataObject __RPC_FAR *pDataObj, DWORD grfKeyState,
        POINTL pt, DWORD __RPC_FAR *pdwEffect) = 0;
    virtual void OnDragLeave() = 0;
};

class UILIB_API DuiDropTarget {
public:
    virtual HRESULT ShareDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect) = 0;
    virtual HRESULT ShareDragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) = 0;
    virtual HRESULT ShareDragLeave() = 0;
    virtual HRESULT ShareDrop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) = 0;
};

class UILIB_API CIDropTarget : public IDropTarget {
    DWORD m_cRefCount;
    bool m_bAllowDrop;
    struct IDropTargetHelper *m_pDropTargetHelper;
    FormatEtcArray m_formatetc;
    FORMATETC *m_pSupportedFrmt;

protected:
    HicastDragCallback *castCallback;
    HWND m_hTargetWnd;

public:
    explicit CIDropTarget(HWND m_hTargetWnd = nullptr);
    virtual ~CIDropTarget();
    void AddSuportedFormat(FORMATETC &ftetc)
    {
        m_formatetc.push_back(ftetc);
    }
    void SetTargetWnd(HWND hWnd)
    {
        m_hTargetWnd = hWnd;
    }

    void RegisterHicastCallback(HicastDragCallback *callback);
    // return values: true - release the medium. false - don't release the medium
    virtual bool OnDrop(FORMATETC *pFmtEtc, STGMEDIUM &medium, DWORD *pdwEffect) = 0;

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return ++m_cRefCount;
    }
    virtual ULONG STDMETHODCALLTYPE Release(void);

    bool QueryDrop(DWORD grfKeyState, LPDWORD pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragEnter(IDataObject __RPC_FAR *pDataObj,
        DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt,
        DWORD __RPC_FAR *pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE DragLeave(void);
    virtual HRESULT STDMETHODCALLTYPE Drop(IDataObject __RPC_FAR *pDataObj,
        DWORD grfKeyState, POINTL pt, DWORD __RPC_FAR *pdwEffect);

    bool DragDropRegister(DuiDropTarget *pDuiDropTarget, HWND hWnd, DWORD AcceptKeyState = MK_LBUTTON);
    DuiDropTarget *m_pDuiDropTarget = nullptr;
    void DropTargetInit();
};

class UILIB_API CDragSourceHelper {
    IDragSourceHelper *pDragSourceHelper = nullptr;

public:
    CDragSourceHelper()
    {
        if (FAILED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER, IID_IDragSourceHelper,
            reinterpret_cast<void **>(&pDragSourceHelper)))) {
            pDragSourceHelper = nullptr;
        }
    }
    virtual ~CDragSourceHelper()
    {
        if (pDragSourceHelper != nullptr) {
            pDragSourceHelper->Release();
            pDragSourceHelper = nullptr;
        }
    }

    // IDragSourceHelper
    HRESULT InitializeFromBitmap(HBITMAP hBitmap,
        POINT &pt, // cursor position in client coords of the window
        RECT &rc,  // selected item's bounding rect
        IDataObject *pDataObject,
        COLORREF crColorKey = GetSysColor(COLOR_WINDOW) // color of the window used for transparent effect.
    )
    {
        if (pDragSourceHelper == nullptr) {
            return E_FAIL;
        }

        SHDRAGIMAGE di;
        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), &bm);
        di.sizeDragImage.cx = bm.bmWidth;
        di.sizeDragImage.cy = bm.bmHeight;
        di.hbmpDragImage = hBitmap;
        di.crColorKey = crColorKey;
        di.ptOffset.x = pt.x - rc.left;
        di.ptOffset.y = pt.y - rc.top;
        return pDragSourceHelper->InitializeFromBitmap(&di, pDataObject);
    }
    HRESULT InitializeFromWindow(HWND hwnd, POINT &pt, IDataObject *pDataObject)
    {
        if (pDragSourceHelper == nullptr) {
            return E_FAIL;
        }
        return pDragSourceHelper->InitializeFromWindow(hwnd, &pt, pDataObject);
    }
};
}
#endif // DRAGDROPIMPL_H