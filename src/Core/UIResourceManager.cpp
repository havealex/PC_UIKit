/*
 * Copyright (C) 2021. Huawei Device Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 */

#include "../StdAfx.h"
#include "UIResourceManager.h"
#include "../Utils/VersionHelpers.h"

namespace DuiLib {
    // DPI相关操作
    const int DPI_ONE = 96; // 96 DPI = 100% scaling
    const int SCALE_ONE = 100;

    using LPSetProcessDpiAwareness = HRESULT(WINAPI *)(_In_ PROCESS_DPI_AWARENESS value);
    using LPGetProcessDpiAwareness = HRESULT(WINAPI *)(_In_ HANDLE hprocess,
        _Out_ PROCESS_DPI_AWARENESS *value);
    using LPGetDpiForMonitor = HRESULT(WINAPI *)(_In_ HMONITOR moniter,
        _In_ MONITOR_DPI_TYPE value, _Out_ UINT *dpix, _Out_ UINT *dpiy);

CResourceManager::CResourceManager(void)
{
    m_pQuerypInterface = nullptr;
    m_nScaleFactor = 0;
    m_nScaleFactorSDA = 0;
    m_Awareness = PROCESS_PER_MONITOR_DPI_AWARE;
    SetScale(DPI_ONE); // 96 DPI = 100% scaling
}

CResourceManager::~CResourceManager(void)
{
    // 重置语言文本缓冲map
    ResetTextMap();

    // 重置图片资源map
    ResetResourceMap();
}

BOOL CResourceManager::LoadResource(STRINGorID xml, LPCTSTR type)
{
    if (HIWORD(xml.m_lpstr) != NULL) {
        if (*(xml.m_lpstr) == _T('<')) {
            if (!m_xml.Load(xml.m_lpstr)) {
                return NULL;
            }
        } else {
            if (!m_xml.LoadFromFile(xml.m_lpstr)) {
                return NULL;
            }
        }
    } else {
        HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
        if (hResource == nullptr) {
            return NULL;
        }
        HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
        if (hGlobal == nullptr) {
            FreeResource(hResource);
            return NULL;
        }

        auto resource = reinterpret_cast<BYTE *>(::LockResource(hGlobal));
        if (resource == nullptr || !m_xml.LoadFromMem(reinterpret_cast<BYTE *>(::LockResource(hGlobal)),
            ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource))) {
            return NULL;
        }

        ::FreeResource(hResource);
    }

    return LoadResource(m_xml.GetRoot());
}

BOOL CResourceManager::LoadResource(CMarkupNode Root)
{
    if (!Root.IsValid()) {
        return FALSE;
    }

    LPCTSTR pstrClass = nullptr;
    int nAttributes = 0;
    LPCTSTR pstrName = nullptr;
    LPCTSTR pstrValue = nullptr;
    LPTSTR pstr = nullptr;

    // 加载图片资源
    LPCTSTR pstrId = nullptr;
    LPCTSTR pstrPath = nullptr;
    for (CMarkupNode node = Root.GetChild(); node.IsValid(); node = node.GetSibling()) {
        pstrClass = node.GetName();
        CMarkupNode ChildNode = node.GetChild();
        if (ChildNode.IsValid()) {
            LoadResource(node);
        } else if ((_tcsicmp(pstrClass, _T("Image")) == 0) && node.HasAttributes()) {
            // 加载图片资源
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++) {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);
                auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;

                if (_tcsicmp(pstrName, _T("id")) == 0) {
                    pstrId = pstrValue;
                } else if (_tcsicmp(pstrName, _T("path")) == 0) {
                    pstrPath = pstrValue;
                }
            }
            if (pstrId == nullptr || pstrPath == nullptr) {
                continue;
            }
            CDuiString *pstrFind = static_cast<CDuiString *>(m_mImageHashMap.Find(pstrId));
            if (pstrFind != nullptr) {
                continue;
            }
            m_mImageHashMap.Insert(pstrId, (LPVOID) new CDuiString(pstrPath));
        } else if (_tcsicmp(pstrClass, _T("Xml")) == 0 && node.HasAttributes()) {
            // 加载XML配置文件
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++) {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);
                auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;

                if (_tcsicmp(pstrName, _T("id")) == 0) {
                    pstrId = pstrValue;
                } else if (_tcsicmp(pstrName, _T("path")) == 0) {
                    pstrPath = pstrValue;
                }
            }
            if (pstrId == nullptr || pstrPath == nullptr) {
                continue;
            }
            CDuiString *pstrFind = static_cast<CDuiString *>(m_mXmlHashMap.Find(pstrId));
            if (pstrFind != nullptr) {
                continue;
            }
            m_mXmlHashMap.Insert(pstrId, (LPVOID) new CDuiString(pstrPath));
        } else {
            continue;
        }
    }
    return TRUE;
}

LPCTSTR CResourceManager::GetImagePath(LPCTSTR lpstrId)
{
    CDuiString *lpStr = static_cast<CDuiString *>(m_mImageHashMap.Find(lpstrId));
    return lpStr == nullptr ? nullptr : lpStr->GetData();
}

LPCTSTR CResourceManager::GetXmlPath(LPCTSTR lpstrId)
{
    CDuiString *lpStr = static_cast<CDuiString *>(m_mXmlHashMap.Find(lpstrId));
    return lpStr == nullptr ? nullptr : lpStr->GetData();
}

void CResourceManager::ResetResourceMap()
{
    CDuiString *lpStr = {};
    for (int i = 0; i < m_mImageHashMap.GetSize(); i++) {
        if (LPCTSTR key = m_mImageHashMap.GetAt(i)) {
            lpStr = static_cast<CDuiString *>(m_mImageHashMap.Find(key));
            delete lpStr;
        }
    }
    for (int i = 0; i < m_mXmlHashMap.GetSize(); i++) {
        if (LPCTSTR key = m_mXmlHashMap.GetAt(i)) {
            lpStr = static_cast<CDuiString *>(m_mXmlHashMap.Find(key));
            delete lpStr;
        }
    }
}

BOOL CResourceManager::LoadLanguage(LPCTSTR pstrXml)
{
    if (pstrXml == nullptr) {
        return FALSE;
    }
    CMarkup xml;
    if (*(pstrXml) == _T('<')) {
        if (!xml.Load(pstrXml)) {
            return FALSE;
        }
    } else {
        if (!xml.LoadFromFile(pstrXml)) {
            return FALSE;
        }
    }
    CMarkupNode Root = xml.GetRoot();
    if (!Root.IsValid()) {
        return FALSE;
    }

    LPCTSTR pstrClass = nullptr;
    int nAttributes = 0;
    LPCTSTR pstrName = nullptr;
    LPCTSTR pstrValue = nullptr;
    LPTSTR pstr = nullptr;

    // 加载图片资源
    LPCTSTR pstrId = nullptr;
    LPCTSTR pstrText = nullptr;
    for (CMarkupNode node = Root.GetChild(); node.IsValid(); node = node.GetSibling()) {
        pstrClass = node.GetName();
        if ((_tcsicmp(pstrClass, _T("Text")) == 0) && node.HasAttributes()) {
            // 加载图片资源
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++) {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);

                if (_tcsicmp(pstrName, _T("id")) == 0) {
                    pstrId = pstrValue;
                } else if (_tcsicmp(pstrName, _T("value")) == 0) {
                    pstrText = pstrValue;
                }
            }
            if (pstrId == nullptr || pstrText == nullptr) {
                continue;
            }

            CDuiString *lpstrFind = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(pstrId));
            if (lpstrFind != nullptr) {
                lpstrFind->Assign(pstrText);
            } else {
                m_mTextResourceHashMap.Insert(pstrId, (LPVOID) new CDuiString(pstrText));
            }
        } else {
            continue;
        }
    }

    return TRUE;
}

CDuiString CResourceManager::GetText(LPCTSTR lpstrId, LPCTSTR lpstrType)
{
    if (lpstrId == nullptr) {
        return _T("");
    }

    CDuiString *lpstrFind = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(lpstrId));
    if (lpstrFind == nullptr && m_pQuerypInterface) {
        lpstrFind = new CDuiString(m_pQuerypInterface->QueryControlText(lpstrId, lpstrType));
        m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
    }
    return lpstrFind == nullptr ? lpstrId : *lpstrFind;
}

void CResourceManager::ReloadText()
{
    if (m_pQuerypInterface == nullptr) {
        return;
    }
    // 重载文字描述
    LPCTSTR lpstrId = nullptr;
    LPCTSTR lpstrText = nullptr;
    for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
        lpstrId = m_mTextResourceHashMap.GetAt(i);
        if (lpstrId == nullptr) {
            continue;
        }
        lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, nullptr);
        if (lpstrText != nullptr) {
            CDuiString *lpStr = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(lpstrId));
            if (lpStr != nullptr) {
                lpStr->Assign(lpstrText);
            }
        }
    }
}
void CResourceManager::ResetTextMap()
{
    CDuiString *lpStr = {};
    for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
        if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i)) {
            lpStr = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(key));
            delete lpStr;
        }
    }
}

PROCESS_DPI_AWARENESS CResourceManager::GetDPIAwareness()
{
    if (IsWindows8Point1OrGreater()) {
        CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
        HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
        if (hModule != nullptr) {
            LPGetProcessDpiAwareness GetProcessDpiAwareness =
                (LPGetProcessDpiAwareness)GetProcAddress(hModule, "GetProcessDpiAwareness");
            if (GetProcessDpiAwareness != nullptr) {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
                if (hProcess != nullptr) {
                    (void)GetProcessDpiAwareness(hProcess, &m_Awareness);
                    CloseHandle(hProcess);
                }
            }
            ::FreeLibrary(hModule);
        }
    }

    return m_Awareness;
}

BOOL CResourceManager::SetDPIAwareness(PROCESS_DPI_AWARENESS Awareness)
{
    BOOL bRet = FALSE;
    if (IsWindows8Point1OrGreater()) {
        CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
        HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
        if (hModule == nullptr) {
            return FALSE;
        } else {
            LPSetProcessDpiAwareness SetProcessDpiAwareness =
                (LPSetProcessDpiAwareness)GetProcAddress(hModule, "SetProcessDpiAwareness");
            if (SetProcessDpiAwareness != nullptr && SetProcessDpiAwareness(Awareness) == S_OK) {
                m_Awareness = Awareness;
                bRet = TRUE;
            }
        }
        ::FreeLibrary(hModule);
    } else {
        m_Awareness = Awareness;
    }
    return bRet;
}

UINT DuiLib::CResourceManager::GetDPI()
{
    if (m_Awareness == PROCESS_DPI_UNAWARE) {
        return DPI_ONE; // 96 DPI = 100% scaling
    }
    if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
        // 96 DPI = 100% scaling
        return static_cast<UINT>(MulDiv(m_nScaleFactorSDA, DPI_ONE, SCALE_ONE));
    }

    return static_cast<UINT>(MulDiv(m_nScaleFactor, DPI_ONE, SCALE_ONE)); // 96 DPI = 100% scaling
}

UINT DuiLib::CResourceManager::GetTrueDPI()
{
    HMONITOR hMonitor;
    POINT pt;
    UINT dpix = DPI_ONE; // set DPI x
    UINT dpiy = DPI_ONE; // set DPI y
    pt.x = 1;
    pt.y = 1;
    hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (IsWindows7OrGreater()) {
        CDuiString dllFullPath = GetSystemDllPath(_T("Shcore.dll"));
        HMODULE hModule = ::LoadLibrary(dllFullPath.GetData());
        if (hModule != nullptr) {
            LPGetDpiForMonitor getDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
            if (getDpiForMonitor != nullptr) {
                getDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy);
            }
            FreeLibrary(hModule);
        }

        return dpix;
    }

    return DPI_ONE; // 96 DPI = 100% scaling
}

UINT CResourceManager::GetScale()
{
    if (m_Awareness == PROCESS_DPI_UNAWARE) {
        return SCALE_ONE; // 缩放比为100%
    }
    if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
        return static_cast<UINT>(m_nScaleFactorSDA);
    }
    return static_cast<UINT>(m_nScaleFactor);
}


void CResourceManager::SetScale(UINT uDPI)
{
    m_nScaleFactor = MulDiv(uDPI, SCALE_ONE, DPI_ONE); // 96 DPI = 100% scaling
    if (m_nScaleFactorSDA == 0) {
        m_nScaleFactorSDA = m_nScaleFactor;
    }
}

int CResourceManager::Scale(int iValue)
{
    if (iValue == 1) {
        return iValue;
    }
    if (m_Awareness == PROCESS_DPI_UNAWARE) {
        return iValue;
    }
    if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
        return MulDiv(iValue, m_nScaleFactorSDA, SCALE_ONE); // 缩放比为100%
    }
    return MulDiv(iValue, m_nScaleFactor, SCALE_ONE); // 缩放比为100%
}

RECT CResourceManager::Scale(RECT rcRect)
{
    RECT rcScale = rcRect;
    int sw = Scale(rcRect.right - rcRect.left);
    int sh = Scale(rcRect.bottom - rcRect.top);
    rcScale.left = Scale(rcRect.left);
    rcScale.top = Scale(rcRect.top);
    rcScale.right = rcScale.left + sw;
    rcScale.bottom = rcScale.top + sh;
    return rcScale;
}

void CResourceManager::Scale(RECT *pRect)
{
    if (pRect == nullptr) {
        return;
    }
    int sw = Scale(pRect->right - pRect->left);
    int sh = Scale(pRect->bottom - pRect->top);
    pRect->left = Scale(pRect->left);
    pRect->top = Scale(pRect->top);
    pRect->right = pRect->left + sw;
    pRect->bottom = pRect->top + sh;
}

void CResourceManager::Scale(POINT *pPoint)
{
    if (pPoint == nullptr) {
        return;
    }
    pPoint->x = Scale(pPoint->x);
    pPoint->y = Scale(pPoint->y);
}

POINT CResourceManager::Scale(POINT ptPoint)
{
    POINT ptScale = ptPoint;
    ptScale.x = Scale(ptPoint.x);
    ptScale.y = Scale(ptPoint.y);
    return ptScale;
}

void CResourceManager::Scale(SIZE *pSize)
{
    if (pSize == nullptr) {
        return;
    }
    if (pSize->cx > 1 || pSize->cy > 1) {
        pSize->cx = Scale(pSize->cx);
        pSize->cy = Scale(pSize->cy);
    }
}

SIZE CResourceManager::Scale(SIZE szSize)
{
    SIZE szScale = szSize;
    szScale.cx = Scale(szSize.cx);
    szScale.cy = Scale(szSize.cy);
    return szScale;
}

int CResourceManager::RestoreScale(int iValue)
{
    if (iValue == 1) {
        return iValue;
    }
    UINT scale = GetScale();
    scale = (scale == 0) ? SCALE_ONE : scale; // scale调整为100%或保持不变
    float percent = scale / 100.f;

    int ret = static_cast<int>(ceil(iValue / percent));
    return ret;
}
} // namespace DuiLib