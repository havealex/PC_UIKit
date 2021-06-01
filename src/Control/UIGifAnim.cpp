/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UIGifAnim.h"

DECLARE_HANDLE(HZIP); // An HZIP identifies a zip file that has been opened
using ZRESULT = DWORD;
using ZIPENTRY = struct {
    int index; // index of this file within the zip
    char name[MAX_PATH]; // filename within the zip
    DWORD attr; // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size; // sizes of item, compressed and uncompressed. These
    long unc_size; // may be -1 if not yet known (e.g. being streamed in)
};
using ZIPENTRYW = struct {
    int index; // index of this file within the zip
    TCHAR name[MAX_PATH]; // filename within the zip
    DWORD attr; // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size; // sizes of item, compressed and uncompressed. These
    long unc_size; // may be -1 if not yet known (e.g. being streamed in)
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
IMPLEMENT_DUICONTROL(CGifAnimUI)

CGifAnimUI::CGifAnimUI(void)
{
    m_pGifImage = nullptr;
    m_pPropertyItem = nullptr;
    m_nFrameCount = 0;
    m_nFramePosition = 0;
    m_bIsAutoPlay = true;
    m_bIsAutoSize = false;
    m_bIsPlaying = false;
}


CGifAnimUI::~CGifAnimUI(void)
{
    DeleteGif();
    if (m_pManager == nullptr) {
        return;
    }
    m_pManager->KillTimer(this, EVENT_TIEM_ID);
}

LPCTSTR CGifAnimUI::GetClass() const
{
    return _T("GifAnimUI");
}

LPVOID CGifAnimUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_GIFANIM) == 0) {
        return static_cast<CGifAnimUI *>(this);
    }
    return CControlUI::GetInterface(pstrName);
}

void CGifAnimUI::DoInit()
{
    InitGifImage();
}

void CGifAnimUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) {
        return;
    }
    if (m_pGifImage == nullptr) {
        InitGifImage();
    }
    DrawFrame(hDC);
}

void CGifAnimUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER) {
        OnTimer((UINT_PTR)event.wParam);
    }
}

void CGifAnimUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
    if (bVisible) {
        PlayGif();
    } else {
        StopGif();
    }
}

void CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("bkimage")) == 0) {
        SetBkImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("autoplay")) == 0) {
        SetAutoPlay(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("autosize")) == 0) {
        SetAutoSize(_tcsicmp(pstrValue, _T("true")) == 0);
    } else {
        CControlUI::SetAttribute(pstrName, pstrValue);
    }
}

void CGifAnimUI::SetBkImage(LPCTSTR pStrImage)
{
    if (pStrImage == nullptr || m_sBkImage == pStrImage) {
        return;
    }

    m_sBkImage = pStrImage;

    StopGif();
    DeleteGif();

    Invalidate();
}

LPCTSTR CGifAnimUI::GetBkImage()
{
    return m_sBkImage.GetData();
}

void CGifAnimUI::SetAutoPlay(bool bIsAuto)
{
    m_bIsAutoPlay = bIsAuto;
}

bool CGifAnimUI::IsAutoPlay() const
{
    return m_bIsAutoPlay;
}

void CGifAnimUI::SetAutoSize(bool bIsAuto)
{
    m_bIsAutoSize = bIsAuto;
}

bool CGifAnimUI::IsAutoSize() const
{
    return m_bIsAutoSize;
}

void CGifAnimUI::PlayGif()
{
    if (m_bIsPlaying || m_pPropertyItem == nullptr || m_pManager == nullptr) {
        return;
    }
    const int multiples = 10;
    const int lpValue = 100;
    long lPause = (static_cast<long *>(m_pPropertyItem->value))[m_nFramePosition] * multiples;
    if (lPause == 0) {
        lPause = lpValue;
    }
    m_pManager->SetTimer(this, EVENT_TIEM_ID, lPause);

    m_bIsPlaying = true;
}

void CGifAnimUI::PauseGif()
{
    if (!m_bIsPlaying || m_pManager == nullptr) {
        return;
    }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    this->Invalidate();
    m_bIsPlaying = false;
}

void CGifAnimUI::StopGif()
{
    if (!m_bIsPlaying || m_pManager == nullptr) {
        return;
    }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    m_nFramePosition = 0;
    this->Invalidate();
    m_bIsPlaying = false;
}

void CGifAnimUI::InitGifImage()
{
    m_pGifImage = LoadGifFromFile(GetBkImage());
    if (m_pGifImage == nullptr) {
        return;
    }
    UINT nCount = 0;
    nCount = m_pGifImage->GetFrameDimensionsCount();
    GUID *pDimensionIDs = new GUID[nCount];
    m_pGifImage->GetFrameDimensionsList(pDimensionIDs, nCount);
    m_nFrameCount = m_pGifImage->GetFrameCount(&pDimensionIDs[0]);
    int size = static_cast<int>(m_pGifImage->GetPropertyItemSize(PropertyTagFrameDelay));
    if (size > 0) {
        m_pPropertyItem = (Gdiplus::PropertyItem *)malloc(size);
        if (m_pPropertyItem == nullptr) {
            delete pDimensionIDs;
            pDimensionIDs = nullptr;
            return;
        }
        m_pGifImage->GetPropertyItem(PropertyTagFrameDelay, size, m_pPropertyItem);
        if (m_bIsAutoSize) {
            SetFixedWidth(m_pGifImage->GetWidth());
            SetFixedHeight(m_pGifImage->GetHeight());
        }
        if (m_bIsAutoPlay) {
            PlayGif();
        }
    }
    delete pDimensionIDs;
    pDimensionIDs = nullptr;
}

void CGifAnimUI::DeleteGif()
{
    if (m_pGifImage != nullptr) {
        delete m_pGifImage;
        m_pGifImage = nullptr;
    }

    if (m_pPropertyItem != nullptr) {
        free(m_pPropertyItem);
        m_pPropertyItem = nullptr;
    }
    m_nFrameCount = 0;
    m_nFramePosition = 0;
}

void CGifAnimUI::OnTimer(UINT_PTR idEvent)
{
    if (idEvent != EVENT_TIEM_ID) {
        return;
    }
    if (m_pManager == nullptr || m_pPropertyItem == nullptr) {
        return;
    }
    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    this->Invalidate();
    UINT curPos = ++m_nFramePosition;
    m_nFramePosition = (curPos) % m_nFrameCount;
    const int multiples = 10;
    const int lpValue = 100;
    long lPause = (static_cast<long *>(m_pPropertyItem->value))[m_nFramePosition] * multiples;
    if (lPause == 0) {
        lPause = lpValue;
    }
    m_pManager->SetTimer(this, EVENT_TIEM_ID, lPause);
}

void CGifAnimUI::DrawFrame(HDC hDC)
{
    if (hDC == nullptr || m_pGifImage == nullptr) {
        return;
    }
    GUID pageGuid = Gdiplus::FrameDimensionTime;
    Gdiplus::Graphics graphics(hDC);
    graphics.DrawImage(m_pGifImage, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
        m_rcItem.bottom - m_rcItem.top);
    m_pGifImage->SelectActiveFrame(&pageGuid, m_nFramePosition);
}

Gdiplus::Image *CGifAnimUI::LoadGifFromFile(LPCTSTR pstrGifPath)
{
    LPBYTE pData = nullptr;
    DWORD dwSize = 0;

    do {
        CDuiString sFile = CPaintManagerUI::GetResourcePath();

        sFile += pstrGifPath;
        HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, nullptr);
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
            break;
        }
    } while (0);

    while (!pData) {
        // 读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile =
            ::CreateFile(pstrGifPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, nullptr);
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
        if (pData == nullptr) {
            ::CloseHandle(hFile);
            break;
        }
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

    Gdiplus::Image *pImage = LoadGifFromMemory(pData, dwSize);
    delete[] pData;
    pData = nullptr;
    return pImage;
}

Gdiplus::Image *CGifAnimUI::LoadGifFromMemory(LPVOID pBuf, size_t dwSize)
{
    HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
    if (hMem == nullptr) {
        return nullptr;
    }
    BYTE *pMem = reinterpret_cast<BYTE *>(::GlobalLock(hMem));
    if (pMem == nullptr) {
        ::GlobalFree(hMem);
        return nullptr;
    }
    if (!memcpy_s(pMem, dwSize, pBuf, dwSize)) {
        ::GlobalFree(hMem);
        return nullptr;
    }

    IStream *pStm = nullptr;
    ::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
    Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);
    if (!pImg || pImg->GetLastStatus() != Gdiplus::Ok) {
        pStm->Release();
        ::GlobalUnlock(hMem);
        ::GlobalFree(hMem);
        return nullptr;
    }
    ::GlobalUnlock(hMem);
    ::GlobalFree(hMem);
    return pImg;
}
}