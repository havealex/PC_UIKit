/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"
#include <vector>

#ifdef DPI_SCALE
#undef DPI_SCALE
#endif
#define DPI_SCALE(x) DuiLib::CResourceManager::GetInstance()->Scale(x)

#ifdef DPI_UNSCALE
#undef DPI_UNSCALE
#endif
#define DPI_UNSCALE(x) DuiLib::CResourceManager::GetInstance()->RestoreScale(x)

#ifdef DISPATCH_UI
#undef DISPATCH_UI
#endif
#define DISPATCH_UI(x) DuiLib::UIMainQueue::getInstance().dispatch([=]() { x })

namespace DuiLib {
class CPaintManagerUI;
class UILIB_API STRINGorID {
public:
    STRINGorID(LPCTSTR lpString) : m_lpstr(lpString) {}
    STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID)) {}
    ~STRINGorID() {}
    LPCTSTR m_lpstr;
};


class UILIB_API CDuiPoint : public tagPOINT {
public:
    CDuiPoint();
    ~CDuiPoint();
    CDuiPoint(const POINT &src);
    CDuiPoint(int x, int y);
    CDuiPoint(LPARAM lParam);
};


class UILIB_API CDuiSize : public tagSIZE {
public:
    CDuiSize();
    CDuiSize(const SIZE &src);
    CDuiSize(const RECT rc);
    CDuiSize(int cx, int cy);
    ~CDuiSize();
};



class UILIB_API CDuiRect : public tagRECT {
public:
    CDuiRect();
    CDuiRect(const RECT &src);
    CDuiRect(int iLeft, int iTop, int iRight, int iBottom);
    ~CDuiRect();

    int GetWidth() const;
    int GetHeight() const;
    void Empty();
    bool IsNull() const;
    void Join(const RECT &rc);
    void ResetOffset();
    void Normalize();
    void Offset(int cx, int cy);
    void Inflate(int cx, int cy);
    void Deflate(int cx, int cy);
    void Union(CDuiRect &rc);
};


class CDuiLock {
public:
    explicit CDuiLock(CRITICAL_SECTION *cs)
    {
        EnterCriticalSection(cs);
        m_cs = cs;
    }
    ~CDuiLock()
    {
        LeaveCriticalSection(m_cs);
    }

private:
    CRITICAL_SECTION *m_cs;
};

enum class FontType {
    Text,
    Value
};

class UILIB_API CStdPtrArray {
public:
    void Initialize();
    CStdPtrArray();
    CStdPtrArray(int iPreallocSize);
    CStdPtrArray(CStdPtrArray &src);
    ~CStdPtrArray();

    void Empty();
    void Resize(int iSize);
    bool IsEmpty();
    int Find(LPVOID iIndex);
    bool Add(LPVOID pData);
    bool SetAt(int iIndex, LPVOID pData);
    bool InsertAt(int iIndex, LPVOID pData);
    bool Remove(int iIndex);
    int GetSize();
    LPVOID *GetData();

    LPVOID GetAt(int iIndex);

protected:
    int m_nCount;
    int m_nAllocated;

    // 防止多线程修改
    CRITICAL_SECTION m_Mutex;
    bool m_Exiting;

public:
    LPVOID *m_ppVoid;
};


class UILIB_API CStdValArray {
public:
    CStdValArray(int iElementSize, int iPreallocSize = 0);
    ~CStdValArray();

    void Empty();
    bool IsEmpty() const;
    bool Add(LPCVOID pData);
    bool Remove(int iIndex);
    int GetSize() const;
    LPVOID GetData();

    LPVOID GetAt(int iIndex) const;
    LPVOID operator[](int nIndex) const;

protected:
    int m_iElementSize;
    int m_nCount;
    int m_nAllocated;

public:
    LPBYTE m_pVoid;
};


class UILIB_API CDuiString {
public:
    enum { MAX_LOCAL_STRING_LEN = 63 };

    CDuiString();
    CDuiString(const TCHAR ch);
    CDuiString(const CDuiString &src);
    CDuiString(LPCTSTR lpsz, int nLen = -1);
    ~CDuiString();

    void Empty();
    int GetLength() const;
    bool IsEmpty() const;
    TCHAR GetAt(int nIndex) const;
    void Append(LPCTSTR pstr);
    void Assign(LPCTSTR pstr, int nLength = -1);
    LPCTSTR GetData() const;
    void Trim();
    std::vector<CDuiString> Split(const DuiLib::CDuiString &str);

    void SetAt(int nIndex, TCHAR ch);
    operator LPCTSTR() const;

    TCHAR operator[](int nIndex) const;
    const CDuiString &operator = (const CDuiString &src);
    const CDuiString &operator = (const TCHAR ch);
    const CDuiString &operator = (LPCTSTR pstr);
#ifdef _UNICODE
    const CDuiString &CDuiString::operator = (LPCSTR lpStr);
    const CDuiString &CDuiString::operator += (LPCSTR lpStr);
#else
    const CDuiString &CDuiString::operator = (LPCWSTR lpwStr);
    const CDuiString &CDuiString::operator += (LPCWSTR lpwStr);
#endif
    CDuiString operator + (const CDuiString &src) const;
    CDuiString operator + (LPCTSTR pstr) const;
    const CDuiString &operator += (const CDuiString &src);
    const CDuiString &operator += (LPCTSTR pstr);
    const CDuiString &operator += (const TCHAR ch);

    bool operator == (LPCTSTR str) const;
    bool operator != (LPCTSTR str) const;
    bool operator <= (LPCTSTR str) const;
    bool operator < (LPCTSTR str) const;
    bool operator >= (LPCTSTR str) const;
    bool operator > (LPCTSTR str) const;

    int Compare(LPCTSTR pstr) const;
    int CompareNoCase(LPCTSTR pstr) const;

    void MakeUpper();
    void MakeLower();

    CDuiString Left(int nLength) const;
    CDuiString Mid(int iPos, int nLength = -1) const;
    CDuiString Right(int nLength) const;

    int Find(TCHAR ch, int iPos = 0) const;
    int Find(LPCTSTR pstr, int iPos = 0) const;
    int ReverseFind(TCHAR ch) const;
    int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

    int __cdecl Format(LPCTSTR pstrFormat, ...);
    int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);
    int __cdecl UnicodeFormat(LPCTSTR pstrFormat, ...);

    int ToInt(bool *ok = nullptr) const;

protected:
    int __cdecl InnerFormat(LPCTSTR pstrFormat, va_list Args);

protected:
    LPTSTR m_pstr;
    TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};

static std::vector<CDuiString> StrSplit(CDuiString text, CDuiString sp)
{
    std::vector<CDuiString> vResults;
    int pos = text.Find(sp, 0);
    while (pos >= 0) {
        CDuiString t = text.Left(pos);
        vResults.push_back(t);
        text = text.Right(text.GetLength() - pos - sp.GetLength());
        pos = text.Find(sp);
    }
    vResults.push_back(text);
    return vResults;
}

struct TITEM {
    CDuiString Key;
    LPVOID Data;
    struct TITEM *pPrev;
    struct TITEM *pNext;
};

class UILIB_API CStdStringPtrMap {
public:
    explicit CStdStringPtrMap(int nSize = 83);
    ~CStdStringPtrMap();

    void Resize(int nSize = 83);
    LPVOID Find(LPCTSTR key, bool optimize = true) const;
    bool Insert(LPCTSTR key, LPVOID pData);
    LPVOID Set(LPCTSTR key, LPVOID pData);
    bool Remove(LPCTSTR key);
    void RemoveAll();
    int GetSize() const;
    LPCTSTR GetAt(int iIndex) const;
    LPCTSTR operator[](int nIndex) const;

protected:
    TITEM **m_aT;
    int m_nBuckets;
    int m_nCount;
};


class UILIB_API CWaitCursor {
public:
    CWaitCursor();
    ~CWaitCursor();

protected:
    HCURSOR m_hOrigCursor;
};


class CDuiVariant : public VARIANT {
public:
    CDuiVariant()
    {
        VariantInit(this);
    }
    explicit CDuiVariant(int i)
    {
        VariantInit(this);
        this->vt = VT_I4;
        this->intVal = i;
    }
    explicit CDuiVariant(float f)
    {
        VariantInit(this);
        this->vt = VT_R4;
        this->fltVal = f;
    }
    explicit CDuiVariant(LPOLESTR s)
    {
        VariantInit(this);
        this->vt = VT_BSTR;
        this->bstrVal = s;
    }
    explicit CDuiVariant(IDispatch *disp)
    {
        VariantInit(this);
        this->vt = VT_DISPATCH;
        this->pdispVal = disp;
    }

    ~CDuiVariant()
    {
        VariantClear(this);
    }
};

template <typename T> void SkipWhitespace(T &pstr)
{
    while (*pstr > _T('\0') && *pstr <= _T(' ')) {
        pstr = ::CharNext(pstr);
    }
}

template <typename T> void SkipIdentifier(T &pstr)
{
    // 属性只能用英文，所以这样处理没有问题
    while (*pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr))) {
        pstr = ::CharNext(pstr);
    }
}

namespace Utils {
UILIB_API Gdiplus::Color GetTransitionColor(const Gdiplus::Color &colorFrom, const Gdiplus::Color &colorTo, float f);
UILIB_API Gdiplus::Rect FromWin32RECT(const RECT &rc);
UILIB_API int GetDPI(bool &result, DWORD &err);
UILIB_API double GetDPIFactor(bool &result, DWORD &err);
UILIB_API HBITMAP LoadBitmapFromDuiRes(LPCTSTR filename);
UILIB_API DWORD GetOpacityColor(DWORD color, DWORD opacity, bool combine = false);
UILIB_API CDuiString GetFileNameFromPath(const CDuiString &filePath);
UILIB_API RECT GetTransitionRect(const RECT &rcSrc, const RECT &rcDest, float f, bool offset = false);
UILIB_API POINT GetTransitionPoint(const POINT &ptSrc, const POINT &ptDest, float f);
UILIB_API DWORD GetColorFromString(const DuiLib::CDuiString &str);
UILIB_API std::wstring ConvertFromBytes(const std::string &str);
UILIB_API void EnableBlurWindow(HWND hwnd);
UILIB_API Gdiplus::Bitmap *CreateBitmapFromHBITMAP(HBITMAP hBitmap);
UILIB_API ::SIZE FitToSize(const ::SIZE &sizeSrc, const ::SIZE &sizeDest);
UILIB_API ::SIZE FillSize(const ::SIZE &sizeSrc, const ::SIZE &sizeDest);
UILIB_API Gdiplus::Bitmap *BlurBitmap(Gdiplus::Bitmap *srcBitmap, int blurValue = 10, bool expandEdge = false);
UILIB_API Gdiplus::Bitmap *BrightnessBitmap(Gdiplus::Bitmap *srcBitmap, int brightness);
UILIB_API Gdiplus::Bitmap *ScaleBitmap(Gdiplus::Bitmap *bitmap, bool fill, int &width, int &height, bool fast = true,
    bool cutRedundant = false);
UILIB_API Gdiplus::Bitmap *ScaleBitmap(HBITMAP hbitmap, bool fill, int &width, int &height);
UILIB_API bool SaveBitmap(Gdiplus::Bitmap *oldBitmap, const std::wstring &filePath);
UILIB_API SIZE GetScreenSize();
UILIB_API Gdiplus::Bitmap *CopyBitmap(Gdiplus::Bitmap *oldBitmap);
UILIB_API bool GetExifShootTime(const std::wstring &filePath, std::wstring &timeStr);
UILIB_API char *GetDataFromBitmap(Gdiplus::Bitmap *bitmap, unsigned int &len);
UILIB_API char *GetBitmapDataFromFile(const std::wstring &filePath, unsigned int &len);
UILIB_API void SetImageAttributeOpacity(Gdiplus::ImageAttributes &attr, int opacity);
}

class UILIB_API UniqueHDC {
public:
    explicit UniqueHDC(HDC hdc)
    {
        this->hdc = hdc;
    }

    ~UniqueHDC()
    {
        if (hdc != nullptr) {
            DeleteDC(hdc);
        }
    }

    operator HDC() const
    {
        return hdc;
    }

    HDC hdc = nullptr;
};

class UILIB_API UniqueHBitmap {
public:
    UniqueHBitmap(HBITMAP hbitmap)
    {
        this->hbitmap = hbitmap;
    }

    ~UniqueHBitmap()
    {
        if (hbitmap != nullptr) {
            DeleteObject(hbitmap);
        }
    }

    operator HBITMAP() const
    {
        return hbitmap;
    }

    HBITMAP hbitmap = nullptr;
};

class UniqueBoolean {
public:
    UniqueBoolean(bool &b, bool endVal) : val(b)
    {
        this->endVal = endVal;
    }

    ~UniqueBoolean()
    {
        val = endVal;
    }

    bool &val;
    bool endVal = false;
};

CDuiString GetSystemDllPath(WCHAR *dllName);
} // namespace DuiLib

#endif // __UTILS_H__