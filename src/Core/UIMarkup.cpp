/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "../Debug/LogOut.h"

#ifndef TRACE
#define TRACE
#endif


DECLARE_HANDLE(HZIP); // An HZIP identifies a zip file that has been opened
using ZRESULT = DWORD;
using ZIPENTRY = struct {
    int index;                    // index of this file within the zip
    char name[MAX_PATH];          // filename within the zip
    DWORD attr;                   // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size;               // sizes of item, compressed and uncompressed. These
    long unc_size;                // may be -1 if not yet known (e.g. being streamed in)
};
using ZIPENTRYW= struct {
    int index;                    // index of this file within the zip
    TCHAR name[MAX_PATH];         // filename within the zip
    DWORD attr;                   // attributes, as in GetFileAttributes.
    FILETIME atime, ctime, mtime; // access, create, modify filetimes
    long comp_size;               // sizes of item, compressed and uncompressed. These
    long unc_size;                // may be -1 if not yet known (e.g. being streamed in)
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
constexpr int LENGTH_VALUE_ONE = 1; // set length 1
constexpr int LENGTH_VALUE_TWO = 2; // set length 2
constexpr int LENGTH_VALUE_THREE = 3; // set length 3
constexpr int LENGTH_VALUE_FOUR = 4; // set length 4
#define CHECK_RESULT_WITH_LOG(expression) { if (expression) {LOGGER_ERROR("[DUILIB]: safe fun ret fail");}}

CMarkupNode::CMarkupNode() : m_pOwner(nullptr), m_iPos(0), m_nAttributes(0) {}

CMarkupNode::CMarkupNode(CMarkup *pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(0) {}

CMarkupNode CMarkupNode::GetSibling()
{
    if (m_pOwner == nullptr) {
        return CMarkupNode();
    }
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
    if (iPos == 0) {
        return CMarkupNode();
    }
    return CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::HasSiblings() const
{
    if (m_pOwner == nullptr) {
        return false;
    }
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iNext;
    return iPos > 0;
}

CMarkupNode CMarkupNode::GetChild()
{
    if (m_pOwner == nullptr) {
        return CMarkupNode();
    }
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
    if (iPos == 0) {
        return CMarkupNode();
    }
    return CMarkupNode(m_pOwner, iPos);
}

CMarkupNode CMarkupNode::GetChild(LPCTSTR pstrName)
{
    if (m_pOwner == nullptr) {
        return CMarkupNode();
    }
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iChild;
    while (iPos != 0) {
        if (_tcsicmp(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0) {
            return CMarkupNode(m_pOwner, iPos);
        }
        iPos = m_pOwner->m_pElements[iPos].iNext;
    }
    return CMarkupNode();
}

bool CMarkupNode::HasChildren() const
{
    if (m_pOwner == nullptr) {
        return false;
    }
    return m_pOwner->m_pElements[m_iPos].iChild != 0;
}

CMarkupNode CMarkupNode::GetParent()
{
    if (m_pOwner == nullptr) {
        return CMarkupNode();
    }
    ULONG iPos = m_pOwner->m_pElements[m_iPos].iParent;
    if (iPos == 0) {
        return CMarkupNode();
    }
    return CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::IsValid() const
{
    return m_pOwner != nullptr;
}

LPCTSTR CMarkupNode::GetName() const
{
    if (m_pOwner == nullptr) {
        return nullptr;
    }
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

LPCTSTR CMarkupNode::GetValue() const
{
    if (m_pOwner == nullptr) {
        return nullptr;
    }
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
}

LPCTSTR CMarkupNode::GetAttributeName(int iIndex)
{
    if (m_pOwner == nullptr) {
        return nullptr;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    if (iIndex < 0 || iIndex >= m_nAttributes) {
        return _T("");
    }
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

LPCTSTR CMarkupNode::GetAttributeValue(int iIndex)
{
    if (m_pOwner == nullptr) {
        return nullptr;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    if (iIndex < 0 || iIndex >= m_nAttributes) {
        return _T("");
    }
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

LPCTSTR CMarkupNode::GetAttributeValue(LPCTSTR pstrName)
{
    if (m_pOwner == nullptr) {
        return nullptr;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    for (int i = 0; i < m_nAttributes; i++) {
        if (_tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0) {
            return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
        }
    }
    return _T("");
}

bool CMarkupNode::GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax)
{
    if (m_pOwner == nullptr) {
        return false;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    if (iIndex < 0 || iIndex >= m_nAttributes) {
        return false;
    }
    if (pstrValue == nullptr) {
        return false;
    }
    _tcsncpy_s(pstrValue, cchMax, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
    return true;
}

bool CMarkupNode::GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax)
{
    if (m_pOwner == nullptr || pstrValue == nullptr) {
        return false;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    for (int i = 0; i < m_nAttributes; i++) {
        if (_tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0) {
            _tcsncpy_s(pstrValue, cchMax, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
            return true;
        }
    }
    return false;
}

int CMarkupNode::GetAttributeCount()
{
    if (m_pOwner == nullptr) {
        return 0;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    return m_nAttributes;
}

bool CMarkupNode::HasAttributes()
{
    if (m_pOwner == nullptr) {
        return false;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    return m_nAttributes > 0;
}

bool CMarkupNode::HasAttribute(LPCTSTR pstrName)
{
    if (m_pOwner == nullptr) {
        return false;
    }
    if (m_nAttributes == 0) {
        _MapAttributes();
    }
    for (int i = 0; i < m_nAttributes; i++) {
        if (_tcsicmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0) {
            return true;
        }
    }
    return false;
}

void CMarkupNode::_MapAttributes()
{
    m_nAttributes = 0;
    if (m_pOwner == nullptr) {
        return;
    }
    LPCTSTR pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
    LPCTSTR pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
    pstr += _tcslen(pstr) + 1;
    while (pstr < pstrEnd) {
        m_pOwner->_SkipWhitespace(pstr);
        if (m_nAttributes >= MAX_XML_ATTRIBUTES) {
            return;
        }
        m_aAttributes[m_nAttributes].iName = static_cast<ULONG>(pstr - m_pOwner->m_pstrXML);
        pstr += _tcslen(pstr) + 1;
        m_pOwner->_SkipWhitespace(pstr);
        if (*pstr++ != _T('\"')) {
            return; // if( *pstr != _T('\"') ) { pstr = ::CharNext(pstr); return; }
        }
        m_aAttributes[m_nAttributes].iValue = static_cast<ULONG>(pstr - m_pOwner->m_pstrXML);
        m_nAttributes++;
        pstr += _tcslen(pstr) + 1;
    }
}

CMarkup::CMarkup(LPCTSTR pstrXML)
{
    m_pstrXML = nullptr;
    m_pElements = nullptr;
    m_nElements = 0;
    m_nReservedElements = 0;
    m_bPreserveWhitespace = true;
    if (pstrXML != nullptr) {
        Load(pstrXML);
    }
}

CMarkup::~CMarkup()
{
    Release();
}

bool CMarkup::IsValid() const
{
    return m_pElements != nullptr;
}

void CMarkup::SetPreserveWhitespace(bool bPreserve)
{
    m_bPreserveWhitespace = bPreserve;
}

bool CMarkup::Load(LPCTSTR pstrXML)
{
    if (pstrXML == nullptr) {
        return false;
    }
    Release();
    SIZE_T lenth = (_tcslen(pstrXML) + 1) * sizeof(TCHAR);
    if (lenth > 0) {
        m_pstrXML = static_cast<LPTSTR>(malloc(lenth));
        if (m_pstrXML == nullptr) {
            return false;
        }
        memcpy_s(m_pstrXML, lenth, pstrXML, lenth);
    }
    bool bRes = _Parse();
    if (!bRes) {
        Release();
    }
    return bRes;
}

bool CMarkup::LoadFromMem(BYTE *pByte, DWORD dwSize, int encoding)
{
    if (pByte == nullptr) {
        return false;
    }
#ifdef _UNICODE
    const int indexLen = 2;
    const int stepValue = 3;
    if (encoding == XMLFILE_ENCODING_UTF8) {
        if (dwSize >= stepValue && pByte[0] == 0xEF && pByte[LENGTH_VALUE_ONE] == 0xBB &&
            pByte[LENGTH_VALUE_TWO] == 0xBF) {
            pByte += stepValue;
            dwSize -= stepValue;
        }
        DWORD wideLen = static_cast<DWORD>(::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pByte, dwSize, nullptr, 0));
        DWORD lenth = static_cast<DWORD>((wideLen + 1) * sizeof(TCHAR));
        if (lenth > 0) {
            m_pstrXML = static_cast<LPTSTR>(malloc(lenth));
            if (m_pstrXML == nullptr) {
                return false;
            }
            ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, wideLen);
            m_pstrXML[wideLen] = _T('\0');
        }
    } else if (encoding == XMLFILE_ENCODING_ASNI) {
        DWORD wideLen = static_cast<DWORD>(::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pByte, dwSize, nullptr, 0));
        DWORD lenth = static_cast<DWORD>((wideLen + 1) * sizeof(TCHAR));
        if (lenth > 0) {
            m_pstrXML = static_cast<LPTSTR>(malloc(lenth));
            if (m_pstrXML == nullptr) {
                return false;
            }
            ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, wideLen);
            m_pstrXML[wideLen] = _T('\0');
        }
    } else {
        if (dwSize >= indexLen && ((pByte[0] == 0xFE && pByte[LENGTH_VALUE_ONE] == 0xFF)
            || (pByte[0] == 0xFF && pByte[LENGTH_VALUE_ONE] == 0xFE))) {
            dwSize = dwSize / indexLen - 1;

            if (pByte[0] == 0xFE && pByte[LENGTH_VALUE_ONE] == 0xFF) {
                pByte += indexLen;

                for (DWORD nSwap = 0; nSwap < dwSize; nSwap++) {
                    register CHAR nTemp = pByte[(nSwap << 1) + 0];
                    pByte[(nSwap << 1) + 0] = pByte[(nSwap << 1) + 1];
                    pByte[(nSwap << 1) + 1] = nTemp;
                }
            } else {
                pByte += indexLen;
            }
            DWORD lenth = static_cast<DWORD>((dwSize + 1) * sizeof(TCHAR));
            if (lenth > 0) {
                m_pstrXML = static_cast<LPTSTR>(malloc(lenth));
                if (m_pstrXML == nullptr) {
                    return false;
                }
                DWORD copyLenth = static_cast<DWORD>(dwSize * sizeof(TCHAR));
                memcpy_s(m_pstrXML, lenth, pByte, copyLenth);
                m_pstrXML[dwSize] = _T('\0');
            }
            pByte -= indexLen;
        }
    }
#else // !_UNICODE
    if (encoding == XMLFILE_ENCODING_UTF8) {
        if (dwSize >= stepValue && pByte[0] == 0xEF && pByte[LENGTH_VALUE_ONE] == 0xBB &&
            pByte[LENGTH_VALUE_TWO] == 0xBF) {
            pByte += stepValue;
            dwSize -= stepValue;
        }
        DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pByte, dwSize, nullptr, 0);

        LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1) * sizeof(WCHAR)));
        if (w_str == nullptr) {
            return false;
        }
        ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide);
        w_str[nWide] = L'\0';

        DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, nullptr, 0, nullptr, nullptr);

        m_pstrXML = static_cast<LPTSTR>(malloc((wide + 1) * sizeof(TCHAR)));
        if (m_pstrXML == nullptr) {
            free(w_str);
            return false;
        }
        ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, m_pstrXML, wide, nullptr, nullptr);
        m_pstrXML[wide] = _T('\0');

        free(w_str);
    } else if (encoding == XMLFILE_ENCODING_UNICODE) {
        if (dwSize >= indexLen && ((pByte[0] == 0xFE && pByte[LENGTH_VALUE_ONE] == 0xFF) || (pByte[0] == 0xFF
            && pByte[LENGTH_VALUE_ONE] == 0xFE))) {
            dwSize = dwSize / indexLen - 1;

            if (pByte[0] == 0xFE && pByte[LENGTH_VALUE_ONE] == 0xFF) {
                pByte += indexLen;

                for (DWORD nSwap = 0; nSwap < dwSize; nSwap++) {
                    register CHAR nTemp = pByte[(nSwap << 1) + 0];
                    pByte[(nSwap << 1) + 0] = pByte[(nSwap << 1) + 1];
                    pByte[(nSwap << 1) + 1] = nTemp;
                }
            } else {
                pByte += indexLen;
            }

            DWORD nWide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, nullptr, 0, nullptr, nullptr);
            m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1) * sizeof(TCHAR)));
            if (m_pstrXML == nullptr) {
                return false;
            }
            ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, m_pstrXML, nWide, nullptr, nullptr);
            m_pstrXML[nWide] = _T('\0');

            pByte -= indexLen;
        }
    } else {
        int lenth = (dwSize + 1) * sizeof(TCHAR);
        if (lenth > 0) {
            m_pstrXML = static_cast<LPTSTR>(malloc(lenth));
            if (m_pstrXML == nullptr) {
                return false;
            }
            DWORD copyLenth = static_cast<DWORD>(dwSize * sizeof(TCHAR));
            memcpy_s(m_pstrXML, lenth, pByte, copyLenth);
            m_pstrXML[dwSize] = _T('\0');
        }
    }
#endif // _UNICODE

    bool bRes = _Parse();
    if (!bRes) {
        Release();
    }
    return bRes;
}

bool CMarkup::LoadFromFile(LPCTSTR pstrFilename, int encoding)
{
    Release();

    std::wstring Lang = UICulture::GetInstance()->GetLocaleLanguage();
    CDuiString sFile = CPaintManagerUI::GetResourcePath();
    sFile += pstrFilename;

    if (Lang != std::wstring(L"zh_cn")) {
        std::wstring wstrFile = sFile.GetData();
        size_t pos = wstrFile.rfind(L".xml");
        if (pos != std::wstring::npos) {
            std::wstring wstrFileTemp = wstrFile;
            std::wstring insertStr = std::wstring(L"@") + Lang;
            wstrFileTemp.insert(pos, insertStr);
            if (_taccess(wstrFileTemp.c_str(), 0) == 0) {
                sFile = wstrFileTemp.c_str();
            } else {
                wstrFileTemp = wstrFile;
                std::wstring insertStr2 = std::wstring(L"@en_us");
                wstrFileTemp.insert(pos, insertStr2);
                if (_taccess(wstrFileTemp.c_str(), 0) == 0) {
                    sFile = wstrFileTemp.c_str();
                }
            }
        } else {
            LOGGER_ERROR(L"[DUILIB]cannot find suffix of file.xml");
        }
    }

    HANDLE hFile = ::CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        LOGGER_ERROR(L"Error opening file " << sFile.GetData());
        return _Failed(_T("Error opening file"));
    }
    DWORD dwSize = ::GetFileSize(hFile, nullptr);
    if (dwSize == 0) {
        ::CloseHandle(hFile);
        LOGGER_ERROR(L"File is empty " << sFile.GetData());
        return _Failed(_T("File is empty"));
    }
    if (dwSize > 4096 * 1024) {
        ::CloseHandle(hFile);
        LOGGER_ERROR(L"File too large " << sFile.GetData());
        return _Failed(_T("File too large"));
    }

    DWORD dwRead = 0;
    BYTE *pByte = new BYTE[dwSize];
    ::ReadFile(hFile, pByte, dwSize, &dwRead, nullptr);
    ::CloseHandle(hFile);

    if (dwRead != dwSize) {
        delete[] pByte;
        Release();
        LOGGER_ERROR(L"Could not read file " << sFile.GetData());
        return _Failed(_T("Could not read file"));
    }
    bool ret = LoadFromMem(pByte, dwSize, encoding);
    delete[] pByte;

    return ret;
}

void CMarkup::Release()
{
    if (m_pstrXML != nullptr) {
        free(m_pstrXML);
    }
    if (m_pElements != nullptr) {
        free(m_pElements);
    }
    m_pstrXML = nullptr;
    m_pElements = nullptr;
    m_nElements;
}

void CMarkup::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    if (pstrMessage == nullptr) {
        return;
    }
    _tcsncpy_s(pstrMessage, cchMax, m_szErrorMsg, cchMax);
}

void CMarkup::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    if (pstrSource == nullptr) {
        return;
    }
    _tcsncpy_s(pstrSource, cchMax, m_szErrorXML, cchMax);
}

CMarkupNode CMarkup::GetRoot()
{
    if (m_nElements == 0) {
        return CMarkupNode();
    }
    return CMarkupNode(this, 1);
}

bool CMarkup::_Parse()
{
    _ReserveElement(); // Reserve index 0 for errors
    ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
    ::ZeroMemory(m_szErrorXML, sizeof(m_szErrorXML));
    LPTSTR pstrXML = m_pstrXML;
    return _Parse(pstrXML, 0);
}

bool CMarkup::_Parse(LPTSTR &pstrText, ULONG iParent)
{
    _SkipWhitespace(pstrText);
    ULONG iPrevious = 0;
    for (;;) {
        if (*pstrText == _T('\0') && iParent <= 1) {
            return true;
        }
        _SkipWhitespace(pstrText);
        if (*pstrText != _T('<')) {
            return _Failed(_T("Expected start tag"), pstrText);
        }
        if (pstrText[LENGTH_VALUE_ONE] == _T('/')) {
            return true;
        }
        *pstrText++ = _T('\0');
        _SkipWhitespace(pstrText);
        // Skip comment or processing directive
        if (*pstrText == _T('!') || *pstrText == _T('?')) {
            TCHAR ch = *pstrText;
            if (*pstrText == _T('!')) {
                ch = _T('-');
            }
            while (*pstrText != _T('\0') && !(*pstrText == ch && *(pstrText + 1) == _T('>'))) {
                pstrText = ::CharNext(pstrText);
            }
            if (*pstrText != _T('\0')) {
                pstrText += 2;
            }
            _SkipWhitespace(pstrText);
            continue;
        }
        _SkipWhitespace(pstrText);
        // Fill out element structure
        XMLELEMENT *pEl = _ReserveElement();

        // Null pointer dereferences, Dereference null return value
        if (pEl == nullptr || m_pElements == nullptr) {
            return false;
        }

        ULONG iPos = static_cast<ULONG>(pEl - m_pElements);
        pEl->iStart = static_cast<ULONG>(pstrText - m_pstrXML);
        pEl->iParent = iParent;
        pEl->iNext = pEl->iChild = 0;
        if (iPrevious != 0) {
            m_pElements[iPrevious].iNext = iPos;
        } else if (iParent > 0) {
            m_pElements[iParent].iChild = iPos;
        }
        iPrevious = iPos;
        // Parse name
        LPCTSTR pstrName = pstrText;
        _SkipIdentifier(pstrText);
        LPTSTR pstrNameEnd = pstrText;
        if (*pstrText == _T('\0')) {
            return _Failed(_T("Error parsing element name"), pstrText);
        }
        // Parse attributes
        if (!_ParseAttributes(pstrText)) {
            return false;
        }
        _SkipWhitespace(pstrText);
        if (pstrText[0] == _T('/') && pstrText[LENGTH_VALUE_ONE] == _T('>')) {
            pEl->iData = static_cast<ULONG>(pstrText - m_pstrXML);
            *pstrText = _T('\0');
            pstrText += 2;
        } else {
            if (*pstrText != _T('>')) {
                return _Failed(_T("Expected start-tag closing"), pstrText);
            }
            // Parse node data
            pEl->iData = static_cast<ULONG>(++pstrText - m_pstrXML);
            LPTSTR pstrDest = pstrText;
            if (!_ParseData(pstrText, pstrDest, _T('<'))) {
                return false;
            }
            // Determine type of next element
            if (*pstrText == _T('\0') && iParent <= 1) {
                return true;
            }
            if (*pstrText != _T('<')) {
                return _Failed(_T("Expected end-tag start"), pstrText);
            }
            if (pstrText[0] == _T('<') && pstrText[LENGTH_VALUE_ONE] != _T('/')) {
                if (!_Parse(pstrText, iPos)) {
                    return false;
                }
            }
            if (pstrText[0] == _T('<') && pstrText[LENGTH_VALUE_ONE] == _T('/')) {
                *pstrDest = _T('\0');
                *pstrText = _T('\0');
                pstrText += 2;
                _SkipWhitespace(pstrText);
                SIZE_T cchName = pstrNameEnd - pstrName;
                if (_tcsncmp(pstrText, pstrName, cchName) != 0) {
                    return _Failed(_T("Unmatched closing tag"), pstrText);
                }
                pstrText += cchName;
                _SkipWhitespace(pstrText);
                if (*pstrText++ != _T('>')) {
                    return _Failed(_T("Unmatched closing tag"), pstrText);
                }
            }
        }
        *pstrNameEnd = _T('\0');
        _SkipWhitespace(pstrText);
    }
}

// 功能: 返回最后的空闲元素，顺便增加内存大小, 每次增加500个以上
// 说明: m_nElements和m_nReservedElements都只在本函数中赋值（除了初始化为0外）
//      m_nElements 指最后一个未使用的元素
//      m_nReservedElements 已分配空间的元素总和
//  本次修改的目的在于去掉realloc这个安全编码专家认为不安全的函数
CMarkup::XMLELEMENT *CMarkup::_ReserveElement()
{
    if (m_nElements >= m_nReservedElements) { // 预分配内存满了
        int reserved_elements = m_nReservedElements + (m_nReservedElements / 2) + 500;

        int old_size = static_cast<int>(m_nReservedElements * sizeof(XMLELEMENT));
        int new_size = static_cast<int>(reserved_elements * sizeof(XMLELEMENT));

        if (new_size <= 0) {
            return nullptr;
        }
        void *p1 = malloc(new_size);

        if (p1 == nullptr) {
            return nullptr;
        }

        if (m_pElements != nullptr) {
            memcpy_s(p1, new_size, m_pElements, old_size);
            free(m_pElements);
            m_pElements = nullptr;
        }

        m_pElements = reinterpret_cast<XMLELEMENT *>(p1);
        m_nReservedElements = reserved_elements;
    }

    if (m_pElements == nullptr) {
        return nullptr;
    }
    XMLELEMENT *p2 = &(m_pElements[m_nElements]);
    m_nElements++;

    return p2;
}

void CMarkup::_SkipWhitespace(LPCTSTR &pstr) const
{
    while (*pstr > _T('\0') && *pstr <= _T(' ')) {
        pstr = ::CharNext(pstr);
    }
}

void CMarkup::_SkipWhitespace(LPTSTR &pstr) const
{
    while (*pstr > _T('\0') && *pstr <= _T(' ')) {
        pstr = ::CharNext(pstr);
    }
}

void CMarkup::_SkipIdentifier(LPCTSTR &pstr) const
{
    // 属性只能用英文，所以这样处理没有问题
    while (*pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr))) {
        pstr = ::CharNext(pstr);
    }
}

void CMarkup::_SkipIdentifier(LPTSTR &pstr) const
{
    // 属性只能用英文，所以这样处理没有问题
    while (*pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || _istalnum(*pstr))) {
        pstr = ::CharNext(pstr);
    }
}

bool CMarkup::_ParseAttributes(LPTSTR &pstrText)
{
    // 无属性
    LPTSTR pstrIdentifier = pstrText;
    if (*pstrIdentifier == _T('/') && *++pstrIdentifier == _T('>')) {
        return true;
    }
    if (*pstrText == _T('>')) {
        return true;
    }
    *pstrText++ = _T('\0');
    _SkipWhitespace(pstrText);
    while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('/')) {
        _SkipIdentifier(pstrText);
        LPTSTR pstrIdentifierEnd = pstrText;
        _SkipWhitespace(pstrText);
        if (*pstrText != _T('=')) {
            return _Failed(_T("Error while parsing attributes"), pstrText);
        }
        *pstrText++ = _T(' ');
        *pstrIdentifierEnd = _T('\0');
        _SkipWhitespace(pstrText);
        if (*pstrText++ != _T('\"')) {
            return _Failed(_T("Expected attribute value"), pstrText);
        }
        LPTSTR pstrDest = pstrText;
        if (!_ParseData(pstrText, pstrDest, _T('\"'))) {
            return false;
        }
        if (*pstrText == _T('\0')) {
            return _Failed(_T("Error while parsing attribute string"), pstrText);
        }
        *pstrDest = _T('\0');
        if (pstrText != pstrDest) {
            *pstrText = _T(' ');
        }
        pstrText++;
        _SkipWhitespace(pstrText);
    }
    return true;
}

bool CMarkup::_ParseData(LPTSTR &pstrText, LPTSTR &pstrDest, char cEnd)
{
    while (*pstrText != _T('\0') && *pstrText != cEnd) {
        if (*pstrText == _T('&')) {
            while (*pstrText == _T('&')) {
                _ParseMetaChar(++pstrText, pstrDest);
            }
            if (*pstrText == cEnd) {
                break;
            }
        }

        if (*pstrText == _T(' ')) {
            *pstrDest++ = *pstrText++;
            if (!m_bPreserveWhitespace) {
                _SkipWhitespace(pstrText);
            }
        } else {
            LPTSTR pstrTemp = ::CharNext(pstrText);
            while (pstrText < pstrTemp) {
                *pstrDest++ = *pstrText++;
            }
        }
    }
    // Make sure that MapAttributes() works correctly when it parses
    // over a value that has been transformed.
    LPTSTR pstrFill = pstrDest + 1;
    while (pstrFill < pstrText) {
        *pstrFill++ = _T(' ');
    }
    return true;
}

void CMarkup::_ParseMetaChar(LPTSTR &pstrText, LPTSTR &pstrDest)
{
    if (pstrText[0] == _T('a') && pstrText[LENGTH_VALUE_ONE] == _T('m') &&
        pstrText[LENGTH_VALUE_TWO] == _T('p') && pstrText[LENGTH_VALUE_THREE] == _T(';')) {
        *pstrDest++ = _T('&');
        pstrText += 4;
    } else if (pstrText[0] == _T('l') && pstrText[LENGTH_VALUE_ONE] == _T('t')
        && pstrText[LENGTH_VALUE_TWO] == _T(';')) {
        *pstrDest++ = _T('<');
        pstrText += 3;
    } else if (pstrText[0] == _T('g') && pstrText[LENGTH_VALUE_ONE] == _T('t')
        && pstrText[LENGTH_VALUE_TWO] == _T(';')) {
        *pstrDest++ = _T('>');
        pstrText += 3;
    } else if (pstrText[0] == _T('q') && pstrText[LENGTH_VALUE_ONE] == _T('u')
        && pstrText[LENGTH_VALUE_TWO] == _T('o') && pstrText[LENGTH_VALUE_THREE] == _T('t')
        && pstrText[LENGTH_VALUE_FOUR] == _T(';')) {
        *pstrDest++ = _T('\"');
        pstrText += 5;
    } else if (pstrText[0] == _T('a') && pstrText[LENGTH_VALUE_ONE] == _T('p')
        && pstrText[LENGTH_VALUE_TWO] == _T('o') && pstrText[LENGTH_VALUE_THREE] == _T('s')
        && pstrText[LENGTH_VALUE_FOUR] == _T(';')) {
        *pstrDest++ = _T('\'');
        pstrText += 5;
    } else {
        *pstrDest++ = _T('&');
    }
}

bool CMarkup::_Failed(LPCTSTR pstrError, LPCTSTR pstrLocation)
{
    if (pstrError == nullptr) {
        return false;
    }
    // Register last error
    TRACE(_T("XML Error: %s"), pstrError);
    if (pstrLocation != nullptr) {
        TRACE(pstrLocation);
    }
    auto ret = _tcsncpy_s(m_szErrorMsg, sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0]),
        pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
    CHECK_RESULT_WITH_LOG(ret != S_OK)
    ret = _tcsncpy_s(m_szErrorXML, lengthof(m_szErrorXML),
        pstrLocation != nullptr ? pstrLocation : _T(""), lengthof(m_szErrorXML) - 1);
    CHECK_RESULT_WITH_LOG(ret != S_OK)
    return false; // Always return 'false'
}
} // namespace DuiLib
