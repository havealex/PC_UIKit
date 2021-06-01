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

#ifndef DUILIB_UICulture_H_
#define DUILIB_UICulture_H_
#pragma once

#include "UILib.h"

#include <memory>

namespace DuiLib {
enum LayoutDirection {
    DIRECTION_LTR,   // Left - to - right, default 0 value
    DIRECTION_RTL,   // Right - to - left text
    DIRECTION_MIXED, // both
};
using LayoutDirection = LayoutDirection;

class UILIB_API UICulture {
public:
    UICulture();
    ~UICulture();

    static UICulture *GetInstance();

    void setResPath(LPCTSTR path);
    void setLocale(LPCTSTR locale, bool loadAll = true);

    // get text direction by icu
    LayoutDirection getDirection();

    CDuiString ParseBind(LPCTSTR pstrValue);
    CDuiString GetString(LPCTSTR key);
    CDuiString GetLocalString(LPCTSTR key);
    std::wstring GetLocaleLanguage();
    void SetCustomFont(wstring _fontstr)
    {
        m_customFont = _fontstr;
    }
    wstring GetCustomFont()
    {
        return m_customFont;
    }
    std::wstring GetMultiVarStringForDotnet(const wstring& localString,
        const vector<int>& nums, const string& language);
    std::wstring GetLocalMultiVarStr(const wstring &strIDS, const vector<int> &vecNum, int count = 0);
    std::wstring GetMultiVarStr(wstring strIDS, vector<int> vecNum, int count = 0);
    wstring GetTextFromHtmlText(CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont);
    CDuiString GetMultiVarString(LPCTSTR originalString, ...);
    int32_t UnicodeFormat(const string &pstrFormat, wstring &unicodeChar, va_list vaList);
    void UploadAllResourceBundle();
    CDuiString GetAllString(LPCTSTR key);
    CDuiString AddAllText(LPCTSTR key, CDuiString text);
    std::wstring GetPercentageStr(const int percentage);
    std::wstring ToUpper(const wstring &src);
private:
    std::wstring GetPluralStr(const wstring &language, const wstring &value, const vector<int> &vecNum, int count = 0);
    struct Core;
    std::unique_ptr<Core> core_;
    wstring m_customFont;
    bool isSupportCurLang = true;
};
} // namespace DuiLib

#endif // !DUILIB_UICulture_H_
