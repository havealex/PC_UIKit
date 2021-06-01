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

#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

#include <ShellScalingApi.h>

namespace DuiLib {
// 控件文字查询接口
class UILIB_API IQueryControlText {
public:
    virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
};

class UILIB_API CResourceManager {
private:
    CResourceManager(void);
    ~CResourceManager(void);

public:
    static CResourceManager *GetInstance()
    {
        static auto p = new CResourceManager;
        return p;
    };
    void Release(void)
    {
        delete this;
    }

public:
    BOOL LoadResource(STRINGorID xml, LPCTSTR type = nullptr);
    BOOL LoadResource(CMarkupNode Root);
    void ResetResourceMap();
    LPCTSTR GetImagePath(LPCTSTR lpstrId);
    LPCTSTR GetXmlPath(LPCTSTR lpstrId);

public:
    void SetLanguage(LPCTSTR pstrLanguage)
    {
        m_sLauguage = pstrLanguage;
    }
    LPCTSTR GetLanguage()
    {
        return m_sLauguage;
    }
    BOOL LoadLanguage(LPCTSTR pstrXml);

public:
    void SetTextQueryInterface(IQueryControlText *pInterface)
    {
        m_pQuerypInterface = pInterface;
    }
    CDuiString GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = nullptr);
    void ReloadText();
    void ResetTextMap();

public:
    PROCESS_DPI_AWARENESS GetDPIAwareness();
    BOOL SetDPIAwareness(PROCESS_DPI_AWARENESS Awareness);
    UINT GetDPI();
    UINT GetTrueDPI();
    UINT GetScale();
    void SetScale(UINT uDPI);
    RECT Scale(RECT rcRect);
    void Scale(RECT *pRect);
    POINT Scale(POINT ptPoint);
    void Scale(POINT *pPoint);
    SIZE Scale(SIZE szSize);
    void Scale(SIZE *pSize);
    int Scale(int iValue);

    int RestoreScale(int iValue);

private:
    CStdStringPtrMap m_mTextResourceHashMap;
    IQueryControlText *m_pQuerypInterface;
    CStdStringPtrMap m_mImageHashMap;
    CStdStringPtrMap m_mXmlHashMap;
    CMarkup m_xml;
    CDuiString m_sLauguage;
    CStdStringPtrMap m_mTextHashMap;

    int m_nScaleFactor;
    int m_nScaleFactorSDA;
    PROCESS_DPI_AWARENESS m_Awareness;
};
} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__