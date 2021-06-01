/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "Utils.h"
#include "../Utils/VersionHelpers.h"
#include <string>
#include <codecvt>
#include <atlcomcli.h>
#include <shlwapi.h>
#include "../Core/win32blur.h"

#define UISTR_UI_DEFAULT_FONTNAME _T("微软雅黑")
#define UISTR_UI_VALUE_FONTNAME _T("Calibri")
#define WHETHER_RETURN_FALSE(expression) {if (expression) {return false;}}

namespace {
    const int ALLOC_SIZE = 2;
    const int MIN_ALLOC_SIZE = 11;
    const int END_SIZE = 1;
    const int COLOR_MULTIPLES = 24;
    const int BIT_COUNT = 32;
    const int ERROR_STATUS = -1;
}

namespace DuiLib {
CDuiPoint::CDuiPoint()
{
    x = y = 0;
}

CDuiPoint::CDuiPoint(const POINT &src)
{
    x = src.x;
    y = src.y;
}

CDuiPoint::CDuiPoint(int _x, int _y)
{
    x = _x;
    y = _y;
}

CDuiPoint::CDuiPoint(LPARAM lParam)
{
    x = GET_X_LPARAM(lParam);
    y = GET_Y_LPARAM(lParam);
}

CDuiPoint::~CDuiPoint() {}

CDuiSize::CDuiSize()
{
    cx = cy = 0;
}

CDuiSize::CDuiSize(const SIZE &src)
{
    cx = src.cx;
    cy = src.cy;
}

CDuiSize::CDuiSize(const RECT rc)
{
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;
}

CDuiSize::CDuiSize(int _cx, int _cy)
{
    cx = _cx;
    cy = _cy;
}

CDuiSize::~CDuiSize() {}

CDuiRect::CDuiRect()
{
    left = top = right = bottom = 0;
}

CDuiRect::CDuiRect(const RECT &src)
{
    left = src.left;
    top = src.top;
    right = src.right;
    bottom = src.bottom;
}

CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
{
    left = iLeft;
    top = iTop;
    right = iRight;
    bottom = iBottom;
}

CDuiRect::~CDuiRect() {}

int CDuiRect::GetWidth() const
{
    return right - left;
}

int CDuiRect::GetHeight() const
{
    return bottom - top;
}

void CDuiRect::Empty()
{
    left = top = right = bottom = 0;
}

bool CDuiRect::IsNull() const
{
    return (left == 0 && right == 0 && top == 0 && bottom == 0);
}

void CDuiRect::Join(const RECT &rc)
{
    if (rc.left < left) {
        left = rc.left;
    }
    if (rc.top < top) {
        top = rc.top;
    }
    if (rc.right > right) {
        right = rc.right;
    }
    if (rc.bottom > bottom) {
        bottom = rc.bottom;
    }
}

void CDuiRect::ResetOffset()
{
    ::OffsetRect(this, -left, -top);
}

void CDuiRect::Normalize()
{
    if (left > right) {
        int iTemp = left;
        left = right;
        right = iTemp;
    }
    if (top > bottom) {
        int iTemp = top;
        top = bottom;
        bottom = iTemp;
    }
}

void CDuiRect::Offset(int cx, int cy)
{
    ::OffsetRect(this, cx, cy);
}

void CDuiRect::Inflate(int cx, int cy)
{
    ::InflateRect(this, cx, cy);
}

void CDuiRect::Deflate(int cx, int cy)
{
    ::InflateRect(this, -cx, -cy);
}

void CDuiRect::Union(CDuiRect &rc)
{
    ::UnionRect(this, this, &rc);
}

void CStdPtrArray::Initialize()
{
    InitializeCriticalSection(&m_Mutex);
    m_ppVoid = nullptr;
    m_nCount = 0;
    m_nAllocated = 0;
    m_Exiting = false;
}

CStdPtrArray::CStdPtrArray()
{
    Initialize();
}

CStdPtrArray::CStdPtrArray(int iPreallocSize)
{
    Initialize();

    if (iPreallocSize > 0) {
        auto ppVoid = malloc(iPreallocSize * sizeof(LPVOID));
        if (ppVoid != nullptr) {
            m_ppVoid = static_cast<LPVOID *>(ppVoid);
        } else {
            m_ppVoid = nullptr;
        }
        m_nAllocated = iPreallocSize;
    }
}

CStdPtrArray::CStdPtrArray(CStdPtrArray &src)
{
    Initialize();

    for (int i = 0; i < src.GetSize(); i++) {
        Add(src.GetAt(i));
    }
}

CStdPtrArray::~CStdPtrArray()
{
    Empty();
    m_Exiting = true;
    // 删除后, 再执行EnterCriticalSection和InitializeCriticalSection会崩溃, 但执行LeaveCriticalSection不会出错
    DeleteCriticalSection(&m_Mutex);
    m_Exiting = true;
}

void CStdPtrArray::Empty()
{
    if (m_Exiting) {
        return;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return;
    }

    if (m_ppVoid != nullptr) {
        free(m_ppVoid);
    }
    m_ppVoid = nullptr;
    m_nCount = m_nAllocated = 0;
}

void CStdPtrArray::Resize(int iSize)
{
    if (m_Exiting) {
        return;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return;
    }
    Empty();
    if (iSize * sizeof(LPVOID) <= 0) {
        return;
    }
    m_ppVoid = static_cast<LPVOID *>(malloc(iSize * sizeof(LPVOID)));
    if (m_ppVoid == nullptr) {
        return;
    }

    ::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
    m_nAllocated = iSize;
    m_nCount = iSize;
}

bool CStdPtrArray::IsEmpty()
{
    if (m_Exiting) {
        return true;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return true;
    }

    if (m_ppVoid == nullptr) {
        Empty();
        return true;
    }
    return m_nCount == 0;
}

bool CStdPtrArray::Add(LPVOID pData)
{
    if (m_Exiting) {
        return false;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return false;
    }

    if (m_nCount >= m_nAllocated) { // 预分配内存是否已满
        int allocated = m_nAllocated * ALLOC_SIZE;

        if (allocated == 0) {
            allocated = MIN_ALLOC_SIZE;
        }

        int old_size = static_cast<int>(m_nAllocated * sizeof(void *));
        int new_size = static_cast<int>(allocated * sizeof(void *));
        if (new_size <= 0) {
            return false;
        }
        void *p1 = malloc(new_size);

        if (p1 == nullptr) {
            return false;
        }

        if (m_ppVoid != nullptr) {
            errno_t ret = memcpy_s(p1, new_size, m_ppVoid, old_size);
            if (ret != 0) {
            }
            free(m_ppVoid);
            m_ppVoid = nullptr;
        }

        m_ppVoid = reinterpret_cast<void **>(p1);
        m_nAllocated = allocated;
    }

    m_ppVoid[m_nCount] = pData;
    m_nCount++;
    return true;
}

bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
{
    if (m_Exiting) {
        return false;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return false;
    }

    if (iIndex == m_nCount) {
        return Add(pData);
    }
    if (iIndex < 0 || iIndex > m_nCount) {
        return false;
    }

    if (m_nCount >= m_nAllocated) { // 预分配内存是否已满
        int allocated = m_nAllocated * ALLOC_SIZE;

        if (allocated == 0) {
            allocated = MIN_ALLOC_SIZE;
        }

        int old_size = static_cast<int>(m_nAllocated * sizeof(void *));
        int new_size = static_cast<int>(allocated * sizeof(void *));
        if (new_size <= 0) {
            return false;
        }
        void *p1 = malloc(new_size);

        if (p1 == nullptr) {
            // 暂时不做任何处理, 只返回
            return false;
        }

        if (m_ppVoid != nullptr) {
            errno_t ret = memcpy_s(p1, new_size, m_ppVoid, old_size);
            if (ret != 0) {
            }
            free(m_ppVoid);
            m_ppVoid = nullptr;
        }

        m_ppVoid = reinterpret_cast<void **>(p1);
        m_nAllocated = allocated;
    }

    errno_t ppRet = memmove_s(&m_ppVoid[iIndex + 1], (m_nCount - iIndex) * sizeof(LPVOID),
        &m_ppVoid[iIndex], (m_nCount - iIndex) * sizeof(LPVOID));
    if (ppRet != 0) {
    }
    m_ppVoid[iIndex] = pData;
    m_nCount++;
    return true;
}

bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
{
    if (m_Exiting) {
        return false;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return false;
    }

    if (m_ppVoid == nullptr) {
        Empty();
        return false;
    }
    if (iIndex < 0 || iIndex >= m_nCount) {
        return false;
    }
    m_ppVoid[iIndex] = pData;
    return true;
}

bool CStdPtrArray::Remove(int iIndex)
{
    if (m_Exiting) {
        return false;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return false;
    }

    if (m_ppVoid == nullptr) {
        Empty();
        return false;
    }
    if (iIndex < 0 || iIndex >= m_nCount) {
        return false;
    }
    if (iIndex < --m_nCount) {
        errno_t ret = ::memcpy_s(m_ppVoid + iIndex, (m_nCount - iIndex) * sizeof(LPVOID),
            m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
        if (ret != 0) {
        }
    }
    return true;
}

int CStdPtrArray::Find(LPVOID pData)
{
    if (m_Exiting) {
        return ERROR_STATUS;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return ERROR_STATUS;
    }

    if (m_ppVoid == nullptr) {
        Empty();
        return ERROR_STATUS;
    }
    for (int i = 0; i < m_nCount; i++) {
        if (m_ppVoid[i] == pData) {
            return i;
        }
    }
    return ERROR_STATUS;
}

int CStdPtrArray::GetSize()
{
    if (m_Exiting) {
        return 0;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return 0;
    }

    if (m_ppVoid == nullptr) {
        Empty();
        return 0;
    }
    return m_nCount;
}

LPVOID *CStdPtrArray::GetData()
{
    if (m_Exiting) {
        return nullptr;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return nullptr;
    }

    return m_ppVoid;
}

LPVOID CStdPtrArray::GetAt(int iIndex)
{
    if (m_Exiting) {
        return nullptr;
    }
    CDuiLock lock(&m_Mutex);
    if (m_Exiting) {
        return nullptr;
    }

    if (iIndex < 0 || iIndex >= m_nCount) {
        return nullptr;
    }
    if (m_ppVoid == nullptr) {
        Empty();
        return nullptr;
    }
    return m_ppVoid[iIndex];
}

CStdValArray::CStdValArray(int iElementSize, int iPreallocSize)
    : m_pVoid(nullptr), m_nCount(0), m_iElementSize(iElementSize), m_nAllocated(iPreallocSize)
{
    ASSERT(iElementSize > 0);
    ASSERT(iPreallocSize >= 0);
    if (iPreallocSize * m_iElementSize > 0) {
        m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
    }
}

CStdValArray::~CStdValArray()
{
    if (m_pVoid != nullptr) {
        free(m_pVoid);
    }
    m_pVoid = nullptr;
}

void CStdValArray::Empty()
{
    m_nCount = 0; // keep the memory in place
}

bool CStdValArray::IsEmpty() const
{
    return m_nCount == 0;
}

bool CStdValArray::Add(LPCVOID pData)
{
    if (m_nCount >= m_nAllocated) { // 预分配内存是否已满
        int allocated = m_nAllocated * ALLOC_SIZE;

        if (allocated == 0) {
            allocated = MIN_ALLOC_SIZE;
        }
        int old_size = m_nAllocated * m_iElementSize;
        int new_size = allocated * m_iElementSize;
        if (new_size <= 0) {
            return false;
        }

        void *p1 = malloc(new_size);

        if (p1 == nullptr) {
            // 暂时不做任何处理, 只返回
            return false;
        }

        if (m_pVoid != nullptr) {
            errno_t retPtr = memcpy_s(p1, new_size, m_pVoid, old_size);
            if (retPtr != 0) {
            }
            free(m_pVoid);
            m_pVoid = nullptr;
        }

        m_pVoid = reinterpret_cast<BYTE *>(p1);
        m_nAllocated = allocated;
    }

    errno_t retPtrVoid = ::memcpy_s(m_pVoid + (m_nCount * m_iElementSize), m_iElementSize, pData, m_iElementSize);
    if (retPtrVoid != 0) {
    }
    m_nCount++;
    return true;
}

bool CStdValArray::Remove(int iIndex)
{
    if (iIndex < 0 || iIndex >= m_nCount || this->m_pVoid == nullptr) {
        return false;
    }
    if (iIndex < --m_nCount) {
        errno_t ret = ::memcpy_s(m_pVoid + (iIndex * m_iElementSize), (m_nCount - iIndex) * m_iElementSize,
            m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
        if (ret != 0) {
        }
    }
    return true;
}

int CStdValArray::GetSize() const
{
    return m_nCount;
}

LPVOID CStdValArray::GetData()
{
    return static_cast<LPVOID>(m_pVoid);
}

LPVOID CStdValArray::GetAt(int iIndex) const
{
    if (iIndex < 0 || iIndex >= m_nCount) {
        return nullptr;
    }
    return m_pVoid + (iIndex * m_iElementSize);
}

LPVOID CStdValArray::operator[](int iIndex) const
{
    ASSERT(iIndex >= 0 && iIndex < m_nCount);
    return m_pVoid + (iIndex * m_iElementSize);
}

CDuiString::CDuiString() : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = '\0';
}

CDuiString::CDuiString(const TCHAR ch) : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = ch;
    m_szBuffer[1] = '\0';
}

CDuiString::CDuiString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
{
    ASSERT(!::IsBadStringPtr(lpsz, -1) || lpsz == nullptr);
    m_szBuffer[0] = '\0';
    Assign(lpsz, nLen);
}

CDuiString::CDuiString(const CDuiString &src) : m_pstr(m_szBuffer)
{
    m_szBuffer[0] = '\0';
    Assign(src.m_pstr);
}

CDuiString::~CDuiString()
{
    if (m_pstr != m_szBuffer) {
        free(m_pstr);
    }
    m_pstr = nullptr;
}

int CDuiString::GetLength() const
{
    return static_cast<int>(_tcslen(m_pstr));
}

CDuiString::operator LPCTSTR() const
{
    return m_pstr;
}

void CDuiString::Append(LPCTSTR pstr)
{
    if (pstr == nullptr) {
        return;
    }
    int pstr_len = static_cast<int>(_tcslen(pstr));
    int nNewLength = GetLength() + pstr_len;
    if (nNewLength >= MAX_LOCAL_STRING_LEN) {
        int old_size = static_cast<int>(static_cast<int>(wcslen(m_pstr)) * sizeof(wchar_t));
        int add_size = static_cast<int>(static_cast<int>(wcslen(pstr)) * sizeof(wchar_t));
        int new_size = static_cast<int>(old_size + add_size + sizeof(wchar_t));

        if (new_size <= 0) {
            return;
        }

        BYTE *p1 = reinterpret_cast<BYTE *>(malloc(new_size));

        if (p1 == nullptr) {
            // 暂时不做任何处理, 只返回
            return;
        }

        // Miscellaneous, Other violation
        errno_t retPtr = memcpy_s(p1, old_size, m_pstr, old_size);
        errno_t retPtrOld = memcpy_s(p1 + old_size, add_size, pstr, add_size);
        if (retPtr != 0 || retPtrOld != 0) {
        }

        wchar_t *p2 = reinterpret_cast<wchar_t *>(p1);
        int endchar = static_cast<int>((old_size + add_size) / sizeof(wchar_t));
        p2[endchar] = L'\0';

        if (m_pstr != m_szBuffer) {
            free(m_pstr);
        }

        m_pstr = reinterpret_cast<wchar_t *>(p2);
    } else {
        if (m_pstr != m_szBuffer) {
            free(m_pstr);
            m_pstr = m_szBuffer;
        }
        _tcscat_s(m_szBuffer, pstr);
    }
}

void CDuiString::Assign(LPCTSTR pstr, int cchMax)
{
    if (pstr == nullptr) {
        pstr = _T("");
    }
    cchMax = (cchMax < 0 ? static_cast<int>(_tcslen(pstr)) : cchMax);
    if (cchMax < MAX_LOCAL_STRING_LEN) {
        if (m_pstr != m_szBuffer) {
            free(m_pstr);
            m_pstr = m_szBuffer;
        }
    } else if (cchMax > GetLength() || m_pstr == m_szBuffer) {
        if (cchMax + 1 > (MAXINT / sizeof(wchar_t))) {
            return;
        }
        int new_size = static_cast<int>((cchMax + 1) * sizeof(wchar_t));
        if (new_size <= 0) {
            return;
        }
        void *p1 = malloc(new_size);

        if (p1 == nullptr) {
            // 暂时不做任何处理, 只返回
            return;
        }

        m_pstr = reinterpret_cast<wchar_t *>(p1);
    }
    if (m_pstr == nullptr) {
        return;
    }
    errno_t ret = _tcsncpy_s(m_pstr, cchMax + 1, pstr, cchMax);
    if (ret != 0) {
    }
    m_pstr[cchMax] = '\0';
}

bool CDuiString::IsEmpty() const
{
    if (m_pstr == nullptr) {
        return false;
    }
    return m_pstr[0] == '\0';
}

void CDuiString::Empty()
{
    if (m_pstr != m_szBuffer) {
        free(m_pstr);
    }
    m_pstr = m_szBuffer;
    m_szBuffer[0] = '\0';
}

LPCTSTR CDuiString::GetData() const
{
    return m_pstr;
}

void CDuiString::Trim()
{
    std::wstring s = GetData();
    s.erase(0, s.find_first_not_of(_T(" \n\r\t")));
    s.erase(s.find_last_not_of(_T(" \n\r\t")) + 1);
    *this = s.c_str();
}

std::vector<DuiLib::CDuiString> CDuiString::Split(const DuiLib::CDuiString &str)
{
    std::vector<DuiLib::CDuiString> strList;
    int start = 0;
    int pos = Find(str.GetData(), start);
    while (pos != -1) {
        strList.push_back(Mid(start, pos - start));
        start = pos + str.GetLength();
        pos = Find(str.GetData(), start);
    }

    if (pos != start) {
        strList.push_back(Mid(start, pos - start));
    }

    return strList;
}

TCHAR CDuiString::GetAt(int nIndex) const
{
    if (m_pstr == nullptr) {
        return _T('\0');
    }
    return m_pstr[nIndex];
}

TCHAR CDuiString::operator[](int nIndex) const
{
    if (m_pstr == nullptr) {
        return _T('\0');
    }
    return m_pstr[nIndex];
}

const CDuiString &CDuiString::operator = (const CDuiString &src)
{
    Assign(src);
    return *this;
}

const CDuiString &CDuiString::operator = (LPCTSTR lpStr)
{
    if (lpStr) {
        ASSERT(!::IsBadStringPtr(lpStr, -1));
        Assign(lpStr);
    } else {
        Empty();
    }
    return *this;
}

#ifdef _UNICODE

const CDuiString &CDuiString::operator = (LPCSTR lpStr)
{
    if (lpStr) {
        ASSERT(!::IsBadStringPtrA(lpStr, -1));
        int cchStr = static_cast<int>(strlen(lpStr)) + 1;
        LPWSTR pwstr = (LPWSTR)malloc(cchStr);
        if (pwstr != nullptr) {
            ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
        }
        Assign(pwstr);
        if (pwstr != nullptr) {
            free(pwstr);
        }
    } else {
        Empty();
    }
    return *this;
}

const CDuiString &CDuiString::operator += (LPCSTR lpStr)
{
    if (lpStr) {
        ASSERT(!::IsBadStringPtrA(lpStr, -1));
        // 返回转换后unicode的长度,实现ANSI与Unicode之间的转换
        size_t unicodeLen = static_cast<size_t>(::MultiByteToWideChar(CP_ACP, 0, lpStr, -1, nullptr, 0));
        // 申请内存
        auto pwstr = new (std::nothrow) wchar_t[unicodeLen + END_SIZE];
        if (pwstr != nullptr) {
            ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, static_cast<int>(unicodeLen));
        }
        Append(pwstr);

        if (pwstr != nullptr) {
            delete[] pwstr;
            pwstr = nullptr;
        }
    }

    return *this;
}

#else

const CDuiString &CDuiString::operator = (LPCWSTR lpwStr)
{
    if (lpwStr) {
        ASSERT(!::IsBadStringPtrW(lpwStr, -1));
        int cchStr = (static_cast<int>(wcslen(lpwStr)) * ALLOC_SIZE) + END_SIZE;
        LPSTR pstr = (LPSTR)malloc(cchStr);
        if (pstr != nullptr) {
            ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, nullptr, nullptr);
        }
        Assign(pstr);
        if (pstr != nullptr) {
            free pstr;
            pstr = nullptr;
        }
    } else {
        Empty();
    }

    return *this;
}

const CDuiString &CDuiString::operator += (LPCWSTR lpwStr)
{
    if (lpwStr) {
        ASSERT(!::IsBadStringPtrW(lpwStr, -1));
        int cchStr = (static_cast<int>(wcslen(lpwStr)) * ALLOC_SIZE) + END_SIZE;
        LPSTR pstr = (LPSTR)malloc(cchStr);
        if (pstr != nullptr) {
            ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, nullptr, nullptr);
        }
        Append(pstr);
        if (pstr != nullptr) {
            free(pstr);
            pstr = nullptr;
        }
    }

    return *this;
}

#endif // _UNICODE

const CDuiString &CDuiString::operator = (const TCHAR ch)
{
    Empty();
    m_szBuffer[0] = ch;
    m_szBuffer[1] = '\0';
    return *this;
}

CDuiString CDuiString::operator + (const CDuiString &src) const
{
    CDuiString sTemp = *this;
    sTemp.Append(src);
    return sTemp;
}

CDuiString CDuiString::operator + (LPCTSTR lpStr) const
{
    if (lpStr) {
        ASSERT(!::IsBadStringPtr(lpStr, -1));
        CDuiString sTemp = *this;
        sTemp.Append(lpStr);
        return sTemp;
    }

    return *this;
}

const CDuiString &CDuiString::operator += (const CDuiString &src)
{
    Append(src);
    return *this;
}

const CDuiString &CDuiString::operator += (LPCTSTR lpStr)
{
    if (lpStr) {
        ASSERT(!::IsBadStringPtr(lpStr, -1));
        Append(lpStr);
    }

    return *this;
}

const CDuiString &CDuiString::operator += (const TCHAR ch)
{
    TCHAR str[] = { ch, '\0' };
    Append(str);
    return *this;
}

bool CDuiString::operator == (LPCTSTR str) const
{
    return (Compare(str) == 0);
};
bool CDuiString::operator != (LPCTSTR str) const
{
    return (Compare(str) != 0);
};
bool CDuiString::operator <= (LPCTSTR str) const
{
    return (Compare(str) <= 0);
};
bool CDuiString::operator < (LPCTSTR str) const
{
    return (Compare(str) < 0);
};
bool CDuiString::operator >= (LPCTSTR str) const
{
    return (Compare(str) >= 0);
};
bool CDuiString::operator > (LPCTSTR str) const
{
    return (Compare(str) > 0);
};

void CDuiString::SetAt(int nIndex, TCHAR ch)
{
    if (m_pstr == nullptr) {
        return;
    }
    ASSERT(nIndex >= 0 && nIndex < GetLength());
    m_pstr[nIndex] = ch;
}

int CDuiString::Compare(LPCTSTR lpsz) const
{
    if (m_pstr == nullptr && lpsz == nullptr) {
        return 0;
    } else if (m_pstr == nullptr) {
        return -1; // 若m_pstr为空指针，m_pstr长度小于lpsz
    } else if (lpsz == nullptr) {
        return 1; // 若lpsz为空指针，m_pstr长度大于lpsz
    } else {
        // _tcscmp函数是字符串长度比较函数，大于返回1，等于返回0，小于返回-1。
        return _tcscmp(m_pstr, lpsz);
    }
}

int CDuiString::CompareNoCase(LPCTSTR lpsz) const
{
    if (m_pstr == nullptr && lpsz == nullptr) {
        return 0;
    } else if (m_pstr == nullptr) {
        return -1; // 若m_pstr为空指针，m_pstr长度小于lpsz
    } else if (lpsz == nullptr) {
        return 1; // 若lpsz为空指针，m_pstr长度大于lpsz
    } else {
        // _tcsicmp函数是字符串长度比较函数，大于返回1，等于返回0，小于返回-1。
        return _tcsicmp(m_pstr, lpsz);
    }
}

void CDuiString::MakeUpper()
{
    if (m_pstr == nullptr) {
        return;
    }
    _tcsupr(m_pstr);
}

void CDuiString::MakeLower()
{
    if (m_pstr == nullptr) {
        return;
    }
    _tcslwr(m_pstr);
}

CDuiString CDuiString::Left(int iLength) const
{
    if (m_pstr == nullptr) {
        return CDuiString();
    }
    if (iLength < 0) {
        iLength = 0;
    }
    if (iLength > GetLength()) {
        iLength = GetLength();
    }
    return CDuiString(m_pstr, iLength);
}

CDuiString CDuiString::Mid(int iPos, int iLength) const
{
    if (iLength < 0) {
        iLength = GetLength() - iPos;
    }
    if (iPos + iLength > GetLength()) {
        iLength = GetLength() - iPos;
    }
    if (iLength <= 0) {
        return CDuiString();
    }
    return CDuiString(m_pstr + iPos, iLength);
}

CDuiString CDuiString::Right(int iLength) const
{
    int iPos = GetLength() - iLength;
    if (iPos < 0) {
        iPos = 0;
        iLength = GetLength();
    }
    return CDuiString(m_pstr + iPos, iLength);
}

int CDuiString::Find(TCHAR ch, int iPos) const
{
    ASSERT(iPos >= 0 && iPos <= GetLength());
    if (iPos != 0 && (iPos < 0 || iPos >= GetLength())) {
        return ERROR_STATUS;
    }
    LPCTSTR p = _tcschr(m_pstr + iPos, ch);
    if (p == nullptr) {
        return ERROR_STATUS;
    }
    return static_cast<int>(p - m_pstr);
}

int CDuiString::Find(LPCTSTR pstrSub, int iPos) const
{
    ASSERT(!::IsBadStringPtr(pstrSub, -1));
    ASSERT(iPos >= 0 && iPos <= GetLength());
    if (iPos != 0 && (iPos < 0 || iPos > GetLength())) {
        return ERROR_STATUS;
    }
    LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
    if (p == nullptr) {
        return ERROR_STATUS;
    }
    return static_cast<int>(p - m_pstr);
}

int CDuiString::ReverseFind(TCHAR ch) const
{
    LPCTSTR p = _tcsrchr(m_pstr, ch);
    if (p == nullptr) {
        return ERROR_STATUS;
    }
    return static_cast<int>(p - m_pstr);
}

int CDuiString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
{
    CDuiString sTemp;
    int nCount = 0;
    int iPos = Find(pstrFrom);
    if (iPos < 0) {
        return 0;
    }
    int cchFrom = static_cast<int>(_tcslen(pstrFrom));
    int cchTo = static_cast<int>(_tcslen(pstrTo));
    while (iPos >= 0) {
        sTemp = Left(iPos);
        sTemp += pstrTo;
        sTemp += Mid(iPos + cchFrom);
        Assign(sTemp);
        iPos = Find(pstrFrom, iPos + cchTo);
        nCount++;
    }
    return nCount;
}

int CDuiString::Format(LPCTSTR pstrFormat, ...)
{
    int nRet;
    va_list Args;

    // Code Quality, Poor Style:Redundant Initialization
    va_start(Args, pstrFormat);
    nRet = InnerFormat(pstrFormat, Args);
    va_end(Args);

    return nRet;
}

int CDuiString::SmallFormat(LPCTSTR pstrFormat, ...)
{
    // Code Quality, Poor Style:Redundant Initialization
    const int bufferSize = 64;
    wchar_t szBuffer[bufferSize] = { 0 };

    va_list argList;
    va_start(argList, pstrFormat);
    int iRet = _vsnwprintf_s(szBuffer, bufferSize, _countof(szBuffer) - 1, pstrFormat, argList);
    va_end(argList);

    if (iRet < 0) {
        return ERROR_STATUS;
    }

    Assign(szBuffer);
    return iRet;
}

int CDuiString::ToInt(bool *ok) const
{
    std::wstring val = GetData();
    int r = 0;
    try {
        r = std::stoi(val);
        if (ok) {
            *ok = true;
        }
    } catch (...) {
        if (ok) {
            *ok = false;
        }
    }

    return r;
}

int CDuiString::UnicodeFormat(LPCTSTR pstrFormat, ...)
{
    wstring szBuffer = L"";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    va_list argList;
    va_start(argList, pstrFormat);
    int nRet = UICulture::GetInstance()->UnicodeFormat((converter.to_bytes(pstrFormat).c_str()),
        szBuffer, argList);
    va_end(argList);
    if (nRet < 0) {
        return ERROR_STATUS;
    }
    Assign(szBuffer.c_str());
    return nRet;
}

int CDuiString::InnerFormat(LPCTSTR pstrFormat, va_list Args)
{
    // 注意, 该用法在MSDN上没有解释
    int nLen = _vsnwprintf(nullptr, 0, pstrFormat, Args);

    if (nLen < 0) {
        return ERROR_STATUS;
    }
    int totalLen = static_cast<int>((nLen + 1) * sizeof(wchar_t));
    if (totalLen <= 0) {
        return ERROR_STATUS;
    }
    auto szBuffer = reinterpret_cast<wchar_t *>(malloc(totalLen));

    if (szBuffer == nullptr) {
        return ERROR_STATUS;
    }

    ZeroMemory(szBuffer, totalLen);

    nLen = _vsnwprintf_s(szBuffer, nLen + 1, nLen, pstrFormat, Args);

    if (nLen < 0) {
        free(szBuffer);
        return ERROR_STATUS;
    }

    Assign(szBuffer);
    free(szBuffer);
    szBuffer = nullptr;

    // Code Quality, Poor Style:Redundant Initialization
    return nLen;
}

static UINT HashKey(LPCTSTR Key)
{
    if (Key == nullptr) {
        return 0;
    }

    UINT i = 0;
    SIZE_T len = _tcslen(Key);

    // Code Quality, Poor Style:Redundant Initialization
    const int iMultiples = 5;
    while (len > 0) {
        len--;
        i = (i << iMultiples) + i + Key[len];
    }
    return i;
}

static UINT HashKey(const CDuiString &Key)
{
    return HashKey((LPCTSTR)Key);
};

CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
{
    // Code Quality, Poor Style:Redundant Initialization
    const int minSize = 16;
    if (nSize < minSize) {
        nSize = minSize;
    }
    m_nBuckets = nSize;
    m_aT = new (std::nothrow) TITEM *[nSize];
    if (m_aT != nullptr) {
        SecureZeroMemory(m_aT, nSize * sizeof(TITEM *));
    }
}

CStdStringPtrMap::~CStdStringPtrMap()
{
    if (m_aT) {
        int len = m_nBuckets;
        while (len--) {
            TITEM *pItem = m_aT[len];
            while (pItem) {
                TITEM *pKill = pItem;
                pItem = pItem->pNext;
                delete pKill;
            }
        }
        delete[] m_aT;
        m_aT = nullptr;
    }
}

void CStdStringPtrMap::RemoveAll()
{
    this->Resize(m_nBuckets);
}

void CStdStringPtrMap::Resize(int nSize)
{
    if (m_aT) {
        int len = m_nBuckets;
        while (len--) {
            TITEM *pItem = m_aT[len];
            while (pItem) {
                TITEM *pKill = pItem;
                pItem = pItem->pNext;
                delete pKill;
            }
        }
        delete[] m_aT;
        m_aT = nullptr;
    }

    if (nSize < 0) {
        nSize = 0;
    }
    if (nSize > 0) {
        m_aT = new TITEM *[nSize];
        SecureZeroMemory(m_aT, nSize * sizeof(TITEM *));
    }
    m_nBuckets = nSize;
    m_nCount = 0;
}

LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
{
    if (m_nBuckets == 0 || GetSize() == 0) {
        return nullptr;
    }

    UINT slot = HashKey(key) % m_nBuckets;
    for (TITEM *pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
        if (pItem->Key == key) {
            if (optimize && pItem != m_aT[slot]) {
                if (pItem->pNext) {
                    pItem->pNext->pPrev = pItem->pPrev;
                }
                if (pItem->pPrev != nullptr) {
                    pItem->pPrev->pNext = pItem->pNext;
                    pItem->pPrev = nullptr;
                }
                pItem->pNext = m_aT[slot];
                pItem->pNext->pPrev = pItem;
                // 将item移动至链条头部
                m_aT[slot] = pItem;
            }
            return pItem->Data;
        }
    }

    return nullptr;
}

bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
{
    if (m_nBuckets == 0) {
        return false;
    }
    if (Find(key)) {
        return false;
    }

    // Add first in bucket
    UINT slot = HashKey(key) % m_nBuckets;
    auto pItem = new TITEM;
    pItem->Key = key;
    pItem->Data = pData;
    pItem->pPrev = nullptr;
    pItem->pNext = m_aT[slot];
    if (pItem->pNext) {
        pItem->pNext->pPrev = pItem;
    }
    m_aT[slot] = pItem;
    m_nCount++;
    return true;
}

LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
{
    if (m_nBuckets == 0) {
        return pData;
    }

    if (GetSize() > 0) {
        UINT slot = HashKey(key) % m_nBuckets;
        // Modify existing item
        for (TITEM *pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
            if (pItem->Key == key) {
                LPVOID pOldData = pItem->Data;
                pItem->Data = pData;
                return pOldData;
            }
        }
    }

    Insert(key, pData);
    return nullptr;
}

bool CStdStringPtrMap::Remove(LPCTSTR key)
{
    if (m_nBuckets == 0 || GetSize() == 0) {
        return false;
    }

    UINT slot = HashKey(key) % m_nBuckets;
    TITEM **ppItem = &m_aT[slot];
    while (*ppItem) {
        if ((*ppItem)->Key == key) {
            TITEM *pKill = *ppItem;
            *ppItem = (*ppItem)->pNext;
            if (*ppItem) {
                (*ppItem)->pPrev = pKill->pPrev;
            }
            delete pKill;
            m_nCount--;
            return true;
        }
        ppItem = &((*ppItem)->pNext);
    }

    return false;
}

int CStdStringPtrMap::GetSize() const
{
#if 0 // def _DEBUG
    int nCount = 0;
    int len = m_nBuckets;
    while (len--) {
        for (const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
            nCount++;
        }
    }
    ASSERT(m_nCount == nCount);
#endif
    return m_nCount;
}

LPCTSTR CStdStringPtrMap::GetAt(int iIndex) const
{
    if (m_nBuckets == 0 || GetSize() == 0) {
        return false;
    }

    int pos = 0;
    int len = m_nBuckets;
    while (len--) {
        for (TITEM *pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
            if (pos++ == iIndex) {
                return pItem->Key.GetData();
            }
        }
    }

    return nullptr;
}

LPCTSTR CStdStringPtrMap::operator[](int nIndex) const
{
    return GetAt(nIndex);
}

CWaitCursor::CWaitCursor()
{
    m_hOrigCursor = ::SetCursor(::LoadCursor(nullptr, IDC_WAIT));
}

CWaitCursor::~CWaitCursor()
{
    ::SetCursor(m_hOrigCursor);
}

namespace Utils {
    const int DEFAULT_DPI = 96;

Gdiplus::Color GetTransitionColor(const Gdiplus::Color &colorFrom, const Gdiplus::Color &colorTo, float f)
{
    Gdiplus::Color colorResult = colorFrom;
    BYTE r = colorFrom.GetR() + (BYTE)((colorTo.GetR() - colorFrom.GetR()) * f);
    BYTE g = colorFrom.GetG() + (BYTE)((colorTo.GetG() - colorFrom.GetG()) * f);
    BYTE b = colorFrom.GetB() + (BYTE)((colorTo.GetB() - colorFrom.GetB()) * f);
    BYTE a = colorTo.GetA();
    colorResult.SetValue(Gdiplus::Color::MakeARGB(a, r, g, b));
    return colorResult;
}

Gdiplus::Rect FromWin32RECT(const RECT &rc)
{
    Gdiplus::Rect grc(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    return grc;
}

using LPGetDpiForMonitor = HRESULT(WINAPI *)(_In_ HMONITOR moniter, _In_ MONITOR_DPI_TYPE value,
    _Out_ UINT *dpix, _Out_ UINT *dpiy);

int GetDPI(bool &result, DWORD &err)
{
    HMONITOR hMonitor;
    POINT pt;
    UINT dpix = DEFAULT_DPI;
    UINT dpiy = DEFAULT_DPI;
    pt.x = 1;
    pt.y = 1;
    result = true;
    err = 0;
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (IsWindows7OrGreater()) {
        CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
        HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
        if (hModule != nullptr) {
            LPGetDpiForMonitor getDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
            if (getDpiForMonitor != nullptr) {
                if (FAILED(getDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy))) {
                    result = false;
                    err = GetLastError();
                }
            }

            FreeLibrary(hModule);
        }

        return dpix;
    }

    return DEFAULT_DPI;
}

double GetDPIFactor(bool &result, DWORD &err)
{
    if (GetDPI(result, err) == DEFAULT_DPI) {
        return 1;
    }

    double f = GetDPI(result, err) / static_cast<double>(DEFAULT_DPI);
    return f;
}

void InitFontIDs(DuiLib::CPaintManagerUI *pm)
{
    if (pm == nullptr) {
        return;
    }
    const int maxNum = 64;
    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_TEXT_SIZE_ + i, UISTR_UI_DEFAULT_FONTNAME, i, false, false, false, false);
    }

    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_TEXT_BOLD_SIZE_ + i, UISTR_UI_DEFAULT_FONTNAME, i, true, false, false, false);
    }

    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_TEXT_UNDERLINE_SIZE_ + i, UISTR_UI_DEFAULT_FONTNAME, i, false, true, false, false);
    }

    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_VALUE_SIZE_ + i, UISTR_UI_VALUE_FONTNAME, i, false, false, false, false, true);
    }

    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_VALUE_BOLD_SIZE_ + i, UISTR_UI_VALUE_FONTNAME, i, true, false, false, false, true);
    }

    for (int i = 1; i < maxNum; ++i) {
        pm->AddFont(FONT_ID_VALUE_UNDERLINE_SIZE_ + i, UISTR_UI_VALUE_FONTNAME, i, false, true, false, false, true);
    }
}


HBITMAP LoadBitmapFromDuiRes(LPCTSTR filename)
{
    std::unique_ptr<DuiLib::TImageInfo> data(DuiLib::CRenderEngine::LoadImage((DuiLib::STRINGorID)filename, 0, 0, 0));
    if (data) {
        return data->hBitmap;
    }

    return nullptr;
}

DWORD GetOpacityColor(DWORD color, DWORD opacity, bool combine)
{
    if (combine) {
        const double baseRatio = 255.0;
        double opacityRatio = opacity / baseRatio;
        DWORD orgOpacity = (color & 0xff000000) >> COLOR_MULTIPLES;
        opacity = orgOpacity * opacityRatio;
    }

    DWORD pureColor = color & 0x00ffffff;
    DWORD resultColor = pureColor | (opacity << COLOR_MULTIPLES);
    return resultColor;
}

CDuiString GetFileNameFromPath(const CDuiString &filePath)
{
    CDuiString fp = filePath;
    fp.Replace(_T("\\"), _T("/"));
    int pos = fp.ReverseFind(_T('/'));
    if (pos == -1) {
        return fp;
    }

    int len = fp.GetLength() - pos - 1;
    if (len > 0 && len <= fp.GetLength()) {
        return fp.Right(len);
    } else {
        return fp;
    }
}

RECT GetTransitionRect(const RECT &rcSrc, const RECT &rcDest, float f, bool offset)
{
    RECT rcResult = { 0, 0, 0, 0 };

    rcResult.left = rcSrc.left + (rcDest.left - rcSrc.left) * f;
    rcResult.top = rcSrc.top + (rcDest.top - rcSrc.top) * f;
    if (!offset) {
        rcResult.right = rcSrc.right + (rcDest.right - rcSrc.right) * f;
        rcResult.bottom = rcSrc.bottom + (rcDest.bottom - rcSrc.bottom) * f;
    } else {
        rcResult.right = rcResult.left + (rcSrc.right - rcSrc.left);
        rcResult.bottom = rcResult.top + (rcSrc.bottom - rcSrc.top);
    }
    return rcResult;
}

POINT GetTransitionPoint(const POINT &ptSrc, const POINT &ptDest, float f)
{
    POINT ptResult = { 0, 0 };
    ptResult.x = ptSrc.x + (ptDest.x - ptSrc.x) * f;
    ptResult.y = ptSrc.y + (ptDest.y - ptSrc.y) * f;
    return ptResult;
}

DWORD GetColorFromString(const DuiLib::CDuiString &str)
{
    LPCTSTR pstrValue = str.GetData();
    while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
        pstrValue = ::CharNext(pstrValue);
    }
    if (*pstrValue == _T('#')) {
        pstrValue = ::CharNext(pstrValue);
    }
    LPTSTR pstr = nullptr;
    const int numBase = 16;
    DWORD clrColor = _tcstoul(pstrValue, &pstr, numBase);
    return clrColor;
}

using WideStringConverter =  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>;

std::wstring ConvertFromBytes(const std::string &str)
{
    WideStringConverter converter;
    std::wstring ret;
    try {
        ret = converter.from_bytes(str);
    } catch (const std::exception &) {
    }

    return ret;
}

void EnableBlurWindow(HWND hwnd)
{
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        pfnSetWindowCompositionAttribute setWindowCompositionAttribute =
            (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute) {
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.dwAttrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(hwnd, &data);
        }
    }
}

UILIB_API Gdiplus::Bitmap *CreateBitmapFromHBITMAP(HBITMAP hBitmap)
{
    BITMAP bmp = { 0 };
    if (GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bmp) == 0) {
        return nullptr;
    }

    // Although we can get bitmap data address by bmp.bmBits member of BITMAP
    // which is got by GetObject function sometime,
    // we can determine the bitmap data in the HBITMAP is arranged bottom-up
    // or top-down, so we should always use GetDIBits to get bitmap data.
    BYTE *piexlsSrc = nullptr;
    LONG cbSize = bmp.bmWidthBytes * bmp.bmHeight;
    piexlsSrc = new BYTE[cbSize];

    BITMAPINFO bmpInfo = { 0 };
    // We should initialize the first six members of BITMAPINFOHEADER structure.
    // A bottom-up DIB is specified by setting the height to a positive number,
    // while a top-down DIB is specified by setting the height to a negative number.
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
    bmpInfo.bmiHeader.biHeight = bmp.bmHeight; // 正数，说明数据从下到上，如未负数，则从上到下
    bmpInfo.bmiHeader.biPlanes = bmp.bmPlanes;
    bmpInfo.bmiHeader.biBitCount = bmp.bmBitsPixel;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = BIT_COUNT;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;

    HDC hdcScreen = CreateDC(L"DISPLAY", nullptr, nullptr, nullptr);
    LONG cbCopied = GetDIBits(hdcScreen, hBitmap, 0, bmp.bmHeight, piexlsSrc, &bmpInfo, DIB_RGB_COLORS);
    DeleteDC(hdcScreen);
    if (cbCopied == 0) {
        delete[] piexlsSrc;
        return nullptr;
    }

    // Create an GDI+ Bitmap has the same dimensions with hbitmap
    Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat32bppPARGB);

    // Access to the Gdiplus::Bitmap's pixel data
    Gdiplus::BitmapData bitmapData = {0};
    Gdiplus::Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
    if (Gdiplus::Ok != pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &bitmapData)) {
        delete[] piexlsSrc;
        delete pBitmap;
        return nullptr;
    }

    BYTE *pixelsDest = reinterpret_cast<BYTE *>(bitmapData.Scan0);
    int nLinesize = static_cast<int>(bmp.bmWidth * sizeof(UINT));
    int nHeight = bmp.bmHeight;

    // Copy pixel data from HBITMAP by bottom-up.
    for (int y = 0; y < nHeight; y++) {
        // 从下到上复制数据，因为前面设置高度时是正数。
        errno_t ret = memcpy_s((pixelsDest + y * nLinesize), nLinesize,
            (piexlsSrc + (nHeight - y - 1) * nLinesize), nLinesize);
        if (ret != 0) {
        }
    }

    // Copy the data in temporary buffer to pBitmap
    if (Gdiplus::Ok != pBitmap->UnlockBits(&bitmapData)) {
        delete[] piexlsSrc;
        delete pBitmap;
        return nullptr;
    }

    delete[] piexlsSrc;
    return pBitmap;
}

::SIZE FitToSize(const ::SIZE &sizeSrc, const ::SIZE &sizeDest)
{
    ::SIZE sizeResult = { 0, 0 };
    if (sizeSrc.cx == 0 || sizeSrc.cy == 0 || sizeDest.cx == 0 || sizeDest.cy == 0) {
        return sizeResult;
    }

    if (static_cast<double>(sizeSrc.cx) / sizeSrc.cy > static_cast<double>(sizeDest.cx) / sizeDest.cy) {
        double scale = static_cast<double>(sizeDest.cx) / sizeSrc.cx;
        sizeResult.cx = sizeDest.cx;
        sizeResult.cy = static_cast<int>(sizeSrc.cy * scale);
    } else {
        double scale = static_cast<double>(sizeDest.cy) / sizeSrc.cy;
        sizeResult.cy = sizeDest.cy;
        sizeResult.cx = static_cast<int>(sizeSrc.cx * scale);
    }

    return sizeResult;
}

::SIZE FillSize(const ::SIZE &sizeSrc, const ::SIZE &sizeDest)
{
    ::SIZE sizeResult = { 0, 0 };
    if (sizeSrc.cx == 0 || sizeSrc.cy == 0 || sizeDest.cx == 0 || sizeDest.cy == 0) {
        return sizeResult;
    }

    if (static_cast<double>(sizeSrc.cx) / sizeSrc.cy > static_cast<double>(sizeDest.cx) / sizeDest.cy) {
        double scale = static_cast<double>(sizeDest.cy) / sizeSrc.cy;
        sizeResult.cy = sizeDest.cy;
        sizeResult.cx = static_cast<int>(sizeSrc.cx * scale);
    } else {
        double scale = static_cast<double>(sizeDest.cx) / sizeSrc.cx;
        sizeResult.cx = sizeDest.cx;
        sizeResult.cy = static_cast<int>(sizeSrc.cy * scale);
    }

    return sizeResult;
}

Gdiplus::Bitmap *BlurBitmap(Gdiplus::Bitmap *srcBitmap, int blurValue, bool expandEdge)
{
    if (!srcBitmap || srcBitmap->GetWidth() == 0 || srcBitmap->GetHeight() == 0) {
        return nullptr;
    }

    Gdiplus::Blur blur;
    Gdiplus::BlurParams blurParams;
    const float baseRadius = 100.0f;
    const int baseValue = 255;
    blurParams.radius = baseValue * blurValue / baseRadius;
    blurParams.expandEdge = TRUE;
    blur.SetParameters(&blurParams);
    srcBitmap->ApplyEffect(&blur, nullptr);

    return srcBitmap;
}

Gdiplus::Bitmap *BrightnessBitmap(Gdiplus::Bitmap *srcBitmap, int brightness)
{
    if (!srcBitmap || srcBitmap->GetWidth() == 0 || srcBitmap->GetHeight() == 0) {
        return nullptr;
    }

    Gdiplus::BrightnessContrastParams briConParams;
    briConParams.brightnessLevel = brightness;
    briConParams.contrastLevel = 0;
    Gdiplus::BrightnessContrast briCon;
    briCon.SetParameters(&briConParams);
    srcBitmap->ApplyEffect(&briCon, nullptr);

    return srcBitmap;
}

Gdiplus::Bitmap *CopyBitmap(Gdiplus::Bitmap *oldBitmap)
{
    if (!oldBitmap) {
        return nullptr;
    }

    UINT width = oldBitmap->GetWidth();
    UINT height = oldBitmap->GetHeight();

    Gdiplus::Bitmap *newBitmap = new Gdiplus::Bitmap(width, height, oldBitmap->GetPixelFormat());

    if (newBitmap->GetLastStatus() != Gdiplus::Ok) {
        delete newBitmap;
        return nullptr;
    }

    Gdiplus::Graphics g(newBitmap);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
    g.DrawImage(oldBitmap, Gdiplus::RectF(0, 0, (Gdiplus::REAL)width, (Gdiplus::REAL)height), 0, 0,
        (Gdiplus::REAL)width, (Gdiplus::REAL)height, Gdiplus::UnitPixel);
    return newBitmap;
}

bool GetExifShootTime(const std::wstring &filePath, std::wstring &timeStr)
{
    std::unique_ptr<Gdiplus::Bitmap> bmp(new Gdiplus::Bitmap(filePath.c_str()));
    if (!bmp) {
        return false;
    }

    if (bmp->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    const int maxSize = 256;
    const int mixSize = 0;
    UINT size = bmp->GetPropertyItemSize(PropertyTagExifDTOrig);
    if (size == mixSize || size > maxSize) {
        return false;
    }

    std::unique_ptr<char[]> buffer(new char[size]);
    SecureZeroMemory(buffer.get(), size);
    Gdiplus::PropertyItem *item = (Gdiplus::PropertyItem *)buffer.get();

    Gdiplus::Status st = bmp->GetPropertyItem(PropertyTagExifDTOrig, size, item);
    if (st != Gdiplus::Ok) {
        return false;
    }

    if (item->type != PropertyTagTypeASCII) {
        return false;
    }

    std::string str(reinterpret_cast<char *>(item->value), item->length);
    timeStr = ConvertFromBytes(str);

    return true;
}

static bool GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
    UINT num = 0;
    UINT size = 0;
    Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return false;
    }

    pImageCodecInfo = new Gdiplus::ImageCodecInfo[size];
    if (pImageCodecInfo == nullptr) {
        return false;
    }

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            delete[] pImageCodecInfo;
            return true;
        }
    }

    delete[] pImageCodecInfo;
    pImageCodecInfo = nullptr;
    return false;
}

bool SaveBitmap(Gdiplus::Bitmap *bitmap, const std::wstring &filePath)
{
    CLSID cls;
    if (!bitmap || !GetEncoderClsid(_T("image/png"), &cls)) {
        return false;
    }

    return bitmap->Save(filePath.c_str(), &cls) == Gdiplus::Ok;
}
char *GetBitmapDataFromFile(const std::wstring &filePath, unsigned int &len)
{
    len = 0;
    std::unique_ptr<Gdiplus::Bitmap> bmp;
    bmp.reset(Gdiplus::Bitmap::FromFile(filePath.c_str()));
    if (!bmp) {
        return nullptr;
    }

    if (bmp->GetWidth() == 0 || bmp->GetHeight() == 0) {
        return nullptr;
    }

    return GetDataFromBitmap(bmp.get(), len);
}

void SetImageAttributeOpacity(Gdiplus::ImageAttributes &imageAttr, int opacity)
{
    const int maxOpacity = 100; // maximium opacity.
    const float getPercentage = 100.0f; // to get percentage.
    if (opacity < maxOpacity) {
        float alpha = opacity / getPercentage;
        Gdiplus::ColorMatrix cm = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, alpha, 0, 0, 0, 0, 0, 1 };
        imageAttr.SetColorMatrix(&cm);
    }
}

class GlobalAllocFree {
public:
    explicit GlobalAllocFree(HGLOBAL hMemBmp)
    {
        this->hMemBmp = hMemBmp;
    }

    ~GlobalAllocFree()
    {
        if (hMemBmp) {
            GlobalFree(hMemBmp);
        }
    }

    HGLOBAL hMemBmp = nullptr;
};

char *GetDataFromBitmap(Gdiplus::Bitmap *bitmap, unsigned int &len)
{
    len = 0;
    CLSID cls;
    if (!bitmap || !GetEncoderClsid(_T("image/png"), &cls)) {
        return nullptr;
    }
    const int getBmpSize = 4;
    int bmpSize = bitmap->GetWidth() * bitmap->GetHeight() * getBmpSize;

    HGLOBAL hMemBmp = GlobalAlloc(GMEM_FIXED, bmpSize);

    if (hMemBmp == nullptr) {
        return nullptr;
    }

    GlobalAllocFree scopeFree(hMemBmp);

    IStream *stmBmp = nullptr;
    CreateStreamOnHGlobal(hMemBmp, FALSE, &stmBmp);
    if (!stmBmp) {
        return nullptr;
    }

    CComPtr<IStream> pstmBmp(stmBmp);

    if (bitmap->Save(stmBmp, &cls) != Gdiplus::Ok) {
        return nullptr;
    }
    if (GlobalLock(hMemBmp) == nullptr) {
        return nullptr;
    }
    BYTE *pMem = reinterpret_cast<BYTE *>(GlobalLock(hMemBmp));
    char *buffer = new char[bmpSize];
    SecureZeroMemory(buffer, bmpSize);
    errno_t ret = memcpy_s(buffer, bmpSize, pMem, bmpSize);
    if (ret != 0) {
    }
    GlobalUnlock(hMemBmp);
    len = bmpSize;
    return buffer;
}

SIZE GetScreenSize()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    return SIZE{screenWidth, screenHeight};
}

Gdiplus::Bitmap *ScaleBitmap(Gdiplus::Bitmap *bitmap, bool fill, int &width, int &height,
    bool fast, bool cutRedundant)
{
    if (width == 0 || height == 0 || bitmap == nullptr) {
        return nullptr;
    }

    if (fill) {
        int destWidth = width;
        int destHeight = height;
        SIZE szBitmap{(LONG)bitmap->GetWidth(), (LONG)bitmap->GetHeight()};
        SIZE sz = FillSize(szBitmap, SIZE{width, height});
        Gdiplus::Bitmap *newBitmap = nullptr;
        if (!cutRedundant) {
            newBitmap = new Gdiplus::Bitmap(sz.cx, sz.cy, PixelFormat32bppARGB);
        } else {
            newBitmap = new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
        }

        Gdiplus::Graphics graphic(newBitmap);
        graphic.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
        graphic.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        graphic.ScaleTransform((sz.cx) / (Gdiplus::REAL)szBitmap.cx, (sz.cy) / (Gdiplus::REAL)szBitmap.cy);
        Gdiplus::TextureBrush brush(bitmap);
        if (!cutRedundant) {
            if (fast) {
                graphic.FillRectangle(&brush, Gdiplus::Rect(0, 0, szBitmap.cx, szBitmap.cy));
            } else {
                graphic.DrawImage(bitmap, 0, 0, sz.cx, sz.cy);
            }
        } else {
            if (fast) {
                // 笔刷填充矩形
                graphic.FillRectangle(&brush,
                    Gdiplus::Rect((sz.cx - destWidth) / 2, (sz.cy - destHeight) / 2, szBitmap.cx, szBitmap.cy));
            } else {
                // 位图填充矩形
                graphic.DrawImage(bitmap,
                    Gdiplus::Rect((sz.cx - destWidth) / 2, (sz.cy - destHeight) / 2, szBitmap.cx, szBitmap.cy));
            }
        }

        width = sz.cx;
        height = sz.cy;
        return newBitmap;
    } else {
        Gdiplus::Bitmap *newBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
        Gdiplus::Graphics graphic(newBitmap);
        graphic.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
        graphic.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        SIZE sz = FitToSize(SIZE{(LONG)bitmap->GetWidth(), (LONG)bitmap->GetHeight()}, SIZE{width, height});
        graphic.ScaleTransform(sz.cx / (Gdiplus::REAL)width, sz.cy / (Gdiplus::REAL)height);
        Gdiplus::TextureBrush brush(bitmap);

        if (fast) {
            graphic.FillRectangle(&brush, Gdiplus::Rect(0, 0, newBitmap->GetWidth(), newBitmap->GetHeight()));
        } else {
            graphic.DrawImage(bitmap, Gdiplus::Rect(0, 0, newBitmap->GetWidth(), newBitmap->GetHeight()));
        }

        width = sz.cx;
        height = sz.cy;
        return newBitmap;
    }
}

Gdiplus::Bitmap *ScaleBitmap(HBITMAP hbitmap, bool fill, int &width, int &height)
{
    Gdiplus::Bitmap *orgBitmap = Gdiplus::Bitmap::FromHBITMAP(hbitmap, nullptr);

    if (!orgBitmap || width == 0 || height == 0) {
        if (orgBitmap) {
            delete orgBitmap;
            orgBitmap = nullptr;
        }

        return nullptr;
    }

    Gdiplus::BitmapData bitmapData = {0};
    Gdiplus::Rect rcImage(0, 0, orgBitmap->GetWidth(), orgBitmap->GetHeight());
    if (orgBitmap->LockBits(&rcImage, Gdiplus::ImageLockModeRead, orgBitmap->GetPixelFormat(),
        &bitmapData) != Gdiplus::Ok) {
        delete orgBitmap;
        orgBitmap = nullptr;
        return nullptr;
    }

    Gdiplus::Bitmap *alphaBitmap = new Gdiplus::Bitmap(bitmapData.Width, bitmapData.Height,
        bitmapData.Stride, PixelFormat32bppARGB, reinterpret_cast<BYTE *>(bitmapData.Scan0));

    if (orgBitmap->UnlockBits(&bitmapData) != Gdiplus::Ok) {
        delete orgBitmap;
        delete alphaBitmap;
        orgBitmap = nullptr;
        alphaBitmap = nullptr;
        return nullptr;
    }

    Gdiplus::Bitmap *newBitmap = ScaleBitmap(alphaBitmap, fill, width, width);

    delete orgBitmap;
    delete alphaBitmap;
    return newBitmap;
}
}

CDuiString GetSystemDllPath(WCHAR *dllName)
{
    CDuiString fullPath;
    if (dllName == nullptr) {
        return fullPath;
    }

    WCHAR sysPath[MAX_PATH] = { 0 };
    UINT result = GetSystemDirectory(sysPath, MAX_PATH);
    if (result == 0) {
        return fullPath;
    }

    if (!PathAppend(sysPath, dllName)) {
        return fullPath;
    }
    fullPath.Append(sysPath);
    return fullPath;
}
} // namespace DuiLib
