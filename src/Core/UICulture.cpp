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
#include "UICulture.h"

#include <codecvt>
#include <locale>

#include "unicode/locid.h"
#include "unicode/resbund.h"
#include "unicode/ubidi.h"
#include "unicode/plurfmt.h"
#include "unicode/msgfmt.h"
#include "unicode/ustring.h"
#include "unicode/ustdio.h"
using namespace std;
using namespace icu;

#define WHETHER_TO_CONTINUE(variable) if (variable) {continue;}

#ifdef SWITCH_LANGUAGE_TEST
enum {
    U_ZERO = 0,
    U_ONE,
    U_TWO,
    U_THREE,
    U_FOUR,
    U_FIVE,
    U_SIX,
    U_SEVEN,
    U_EIGHT,
    U_NINE,
    U_TEN,
    U_ELEVEN,
    U_TWELEVE,
    U_THIREET,
    U_FOURTEEN,
    U_FIFTEEN,
    U_SIXTEEN,
    U_SEVENTEEN,
    U_EIGHTEEN,
    U_NINETEEN,
    U_TWENTY,
    U_TWENTYONE,
    U_TWENTYTWO,
    U_TWENTYTHREE,
    U_TWENTYFOUR
};

std::vector<LPCTSTR> g_localeVec = {
    L"ZH-CN", L"EN-US", L"EN-ES", L"RU-RU", L"JA-JP", L"EN-GB", L"DE-DE", L"ES-MX",
    L"FR-CA", L"FR-FR", L"IT-IT", L"KO-KR", L"CS-CZ", L"DA-DK", L"EL-GR", L"FI-FI",
    L"NB-NO", L"NL-NL", L"PL-PL", L"PT-PT", L"SV-SE", L"TH-TH", L"TR-TR", L"ZZ-ZX"
};

using ResourceBundleMap = std::map<LPCTSTR, icu::ResourceBundle *>;
ResourceBundleMap g_resourceBundleMap;

#define RESOURCEBUNDLE(name, locale)                                                                  \
    {                                                                                                 \
        static ResourceBundle s_country_res##name(core_->res_path_, locale, errcode1);                \
        if (errcode1 == U_ZERO_ERROR) {                                                               \
            static ResourceBundle s_country_rb##name = (s_country_res##name).get("string", errcode1); \
            if (errcode1 == U_ZERO_ERROR) {                                                           \
                ResourceBundle *s_country_strings##name = &(s_country_rb##name);                      \
                g_resourceBundleMap.insert(make_pair(g_localeVec.at(i), s_country_strings##name));        \
            }                                                                                         \
        }                                                                                             \
    }

#define INIT_ALL_RESOURCEBUNDLE(count, locale)  \
    switch (count) {                            \
        case U_ZERO:                            \
            RESOURCEBUNDLE(one, locale)         \
            break;                              \
        case U_ONE:                             \
            RESOURCEBUNDLE(two, locale)         \
            break;                              \
        case U_TWO:                             \
            RESOURCEBUNDLE(three, locale)       \
            break;                              \
        case U_THREE:                           \
            RESOURCEBUNDLE(four, locale)        \
            break;                              \
        case U_FOUR:                            \
            RESOURCEBUNDLE(five, locale)        \
            break;                              \
        case U_FIVE:                            \
            RESOURCEBUNDLE(six, locale)         \
            break;                              \
        case U_SIX:                             \
            RESOURCEBUNDLE(seven, locale)       \
            break;                              \
        case U_SEVEN:                           \
            RESOURCEBUNDLE(eight, locale)       \
            break;                              \
        case U_EIGHT:                           \
            RESOURCEBUNDLE(nine, locale)        \
            break;                              \
        case U_NINE:                            \
            RESOURCEBUNDLE(ten, locale)         \
            break;                              \
        case U_TEN:                             \
            RESOURCEBUNDLE(eleven, locale)      \
            break;                              \
        case U_ELEVEN:                          \
            RESOURCEBUNDLE(twelve, locale)      \
            break;                              \
        case U_TWELEVE:                         \
            RESOURCEBUNDLE(thirteen, locale)    \
            break;                              \
        case U_THIREET:                         \
            RESOURCEBUNDLE(fourteen, locale)    \
            break;                              \
        case U_FOURTEEN:                        \
            RESOURCEBUNDLE(fifteen, locale)     \
            break;                              \
        case U_FIFTEEN:                         \
            RESOURCEBUNDLE(sixteen, locale)     \
            break;                              \
        case U_SIXTEEN:                         \
            RESOURCEBUNDLE(seventeen, locale)   \
            break;                              \
        case U_SEVENTEEN:                       \
            RESOURCEBUNDLE(eighteen, locale)    \
            break;                              \
        case U_EIGHTEEN:                        \
            RESOURCEBUNDLE(nineteen, locale)    \
            break;                              \
        case U_NINETEEN:                        \
            RESOURCEBUNDLE(twenty, locale)      \
            break;                              \
        case U_TWENTY:                          \
            RESOURCEBUNDLE(twentyOne, locale)   \
            break;                              \
        case U_TWENTYONE:                       \
            RESOURCEBUNDLE(twentyTwo, locale)   \
            break;                              \
        case U_TWENTYTWO:                       \
            RESOURCEBUNDLE(twentyThree, locale) \
            break;                              \
        case U_TWENTYTHREE:                     \
            RESOURCEBUNDLE(twentyFour, locale)  \
            break;                              \
        case U_TWENTYFOUR:                      \
            RESOURCEBUNDLE(twentyFive, locale)  \
            break;                              \
        default:                                \
            break;                              \
    }

#endif

namespace DuiLib {
const LONG LONG_MUL_MAX = static_cast<LONG>(sqrt(LONG_MAX));
constexpr size_t MAX_VEC_NUM = 100000;

struct UICulture::Core {
    UnicodeString res_path_;
    Locale locale_;

    Core() {}
};

UICulture::UICulture() : core_(new Core) {}


UICulture::~UICulture()
{
#ifdef SWITCH_LANGUAGE_TEST
    for (ResourceBundleMap::iterator it = g_resourceBundleMap.begin(); it != g_resourceBundleMap.end(); ++it) {
        if (it->second != nullptr) {
            delete it->second;
            it->second = nullptr;
        }
    }
    g_resourceBundleMap.clear();
#endif
}

UICulture *UICulture::GetInstance()
{
    static UICulture instance;
    return &instance;
}

void UICulture::setResPath(LPCTSTR path)
{
    core_->res_path_ = path;
}

void UICulture::setLocale(LPCTSTR locale, bool loadAll)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    core_->locale_ = Locale(converter.to_bytes(locale).c_str());
    UErrorCode errcode = U_ZERO_ERROR;
    uloc_setDefault(converter.to_bytes(locale).c_str(), &errcode);
    if (loadAll) {
        UploadAllResourceBundle();
    }

    UErrorCode supportedLangErrorCode = U_ZERO_ERROR;
    ResourceBundle languageRes(core_->res_path_, core_->locale_, supportedLangErrorCode);

    if (supportedLangErrorCode != U_ZERO_ERROR) {
        isSupportCurLang = false;
    }
}

// get text direction by icu
LayoutDirection UICulture::getDirection()
{
    switch (core_->locale_.isRightToLeft()) {
        case FALSE:
            return DIRECTION_LTR;
        default:
            return (isSupportCurLang ? DIRECTION_RTL : DIRECTION_LTR);
    }
}


void UICulture::UploadAllResourceBundle()
{
#ifdef SWITCH_LANGUAGE_TEST
    for (size_t i = 0; i < g_localeVec.size(); i++) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        Locale locale = Locale(converter.to_bytes(g_localeVec.at(i)).c_str());
        ResourceBundle *s_country_strings = nullptr;
        UErrorCode errcode1 = U_ZERO_ERROR;
        INIT_ALL_RESOURCEBUNDLE(i, locale);
    }
#endif
}


CDuiString UICulture::GetAllString(LPCTSTR key)
{
    CDuiString value = L"";
    CDuiString localValue = L"";
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString enValue = L"";
    wstring language = GetLocaleLanguage();
    std::transform(language.begin(), language.end(), language.begin(), ::toupper);
    wstring oldStr = L"_";
    size_t pos = language.find(oldStr);
    if (pos != std::wstring::npos) {
        language = language.replace(pos, oldStr.size(), L"-");
    }
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    string keyStr = converter.to_bytes(key);
    const char *keyChar = keyStr.c_str();
    UnicodeString valueStr;

    const char16_t *valueChart = nullptr;
    for (ResourceBundleMap::iterator it = g_resourceBundleMap.begin(); it != g_resourceBundleMap.end(); ++it) {
        WHETHER_TO_CONTINUE(it->second == nullptr);
        UErrorCode errcode1 = U_ZERO_ERROR;
        valueStr = it->second->getStringEx(keyChar, errcode1);
        WHETHER_TO_CONTINUE(errcode1 != U_ZERO_ERROR);
        valueChart = valueStr.getBuffer();
        value += it->first;
        value += L"&UI&";
        if (valueChart == nullptr) {
            value += L"";
        } else {
            value += reinterpret_cast<wchar_t *>(valueChart);
        }
        if (language == it->first) {
            localValue = reinterpret_cast<wchar_t *>(valueChart);
            localValue += L"@UI@";
        }
        if (it->first == L"EN-US") {
            enValue = reinterpret_cast<wchar_t *>(valueChart);
            enValue += L"@UI@";
        }
        value += L"$UI$";
    }
    if (localValue.IsEmpty()) {
        localValue = enValue;
    }
#endif
    return localValue + value;
}

CDuiString UICulture::AddAllText(LPCTSTR key, CDuiString text)
{
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString textValue = GetAllString(key);
    if (textValue.IsEmpty()) {
        return text;
    } else {
        return textValue;
    }
#endif
    return text;
}

CDuiString UICulture::GetLocalString(LPCTSTR key)
{
    static bool s_not_initialize = true;
    static ResourceBundle *s_root_strings = nullptr;
    static ResourceBundle *s_country_strings = nullptr;
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> s_converter;

    if (s_not_initialize) {
        // 获取某个国家的字符串资源
        UErrorCode errcode1 = U_ZERO_ERROR;
        static ResourceBundle s_country_res(core_->res_path_, core_->locale_, errcode1);

        if (errcode1 == U_ZERO_ERROR) {
            // 如果目标语言中找不到, 则在默认语言中找, 但是默认语言文件有时候并不是root.txt，而是zh_CN.txt
            // U_USING_DEFAULT_WARNING = -127 A resource bundle lookup returned a result from the root locale (not an
            // error)
            //
            static ResourceBundle s_country_rb = s_country_res.get("string", errcode1);
            if (errcode1 == U_ZERO_ERROR) {
                s_country_strings = &s_country_rb;
            }
        }

        // 获取root.txt的字符串资源
        Locale root_loc = Locale::getRoot();
        UErrorCode errcode2 = U_ZERO_ERROR;
        static ResourceBundle s_root_res(core_->res_path_, root_loc, errcode2);

        if (errcode2 == U_ZERO_ERROR) {
            static ResourceBundle s_root_rb = s_root_res.get("string", errcode2);
            if (errcode2 == U_ZERO_ERROR) {
                s_root_strings = &s_root_rb;
            }
        }

        s_not_initialize = false;
    }

    string key_str1 = s_converter.to_bytes(key);
    const char *key_str2 = key_str1.c_str();

    UnicodeString value_str1;
    const char16_t *value_str2 = nullptr;

    if (s_country_strings != nullptr) {
        UErrorCode errcode1 = U_ZERO_ERROR;
        value_str1 = s_country_strings->getStringEx(key_str2, errcode1);
        if (errcode1 == U_ZERO_ERROR) {
            value_str2 = value_str1.getBuffer();
        }
    }

    if (value_str2 == nullptr && s_root_strings != nullptr) {
        UErrorCode errcode2 = U_ZERO_ERROR;
        value_str1 = s_root_strings->getStringEx(key_str2, errcode2);
        if (errcode2 == U_ZERO_ERROR) {
            value_str2 = value_str1.getBuffer();
        }
    }

    if (value_str2 == nullptr) {
        return L"";
    } else {
        return (wchar_t *)(value_str2); // ICU内部全部是unicode所以这里强转到wchar_t,在多字符集下可能会编译出错
    }
}

CDuiString UICulture::GetString(LPCTSTR key)
{
    CDuiString localStr = GetLocalString(key);
#ifdef SWITCH_LANGUAGE_TEST
    if (localStr != L"") {
        localStr = AddAllText(key, localStr);
    }
#endif
    return localStr;
}

// -------------------------------------------------------------------------------------------------
// replace by dynamic parameter
// example:
//  "bind='IDS_XXXX_NAME'"
CDuiString UICulture::ParseBind(LPCTSTR pstrValue)
{
    const auto bindstr = _T("bind");
    const auto len = _tcslen(bindstr);
    auto pcur = pstrValue;
    SkipWhitespace(pcur);
    if (_tcsncicmp(pcur, bindstr, len) == 0) {
        pcur += len;
        SkipWhitespace(pcur);
        if (*pcur++ != _T('=')) {
            return pstrValue;
        }
        SkipWhitespace(pcur);
        if (*pcur++ != _T('\'')) {
            return pstrValue;
        }

        auto count = 0;
        auto pstrTemp = pcur;
        while (*pstrTemp != _T('\0') && *pstrTemp != _T('\'')) {
            pstrTemp = ::CharNext(pstrTemp);
            ++count;
        }

        // get locale name by icu4c
        *const_cast<LPTSTR>(pstrTemp) = '\0';
        return GetString(pcur);
    }

    return CDuiString();
}


std::wstring UICulture::GetLocaleLanguage()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    string strT = core_->locale_.getName();
    std::wstring wideStr = conv.from_bytes(strT);
    std::transform(wideStr.begin(), wideStr.end(), wideStr.begin(), ::tolower);
    return wideStr;
}

std::wstring UICulture::GetPluralStr(const wstring &language, const wstring &value, const vector<int> &vecNum,
    int count)
{
    UnicodeString msgSl;
    FieldPosition fPos = 0;
    UnicodeString result;
    UErrorCode status = U_ZERO_ERROR;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    Locale Lanlocale = Locale(converter.to_bytes(language).c_str());

    wstring wstrText = value;
    wstring wstrRet = wstrText;
    if (vecNum.size() == 0 || vecNum.size() > MAX_VEC_NUM) {
        return wstrText;
    }
    wstring wstrPre;
    wstring wstrMid;
    wstring wstrEnd;
    if (count > 0) {
        wstring subwstr = L"{0,plural";
        size_t indexStart = wstrText.find(subwstr.c_str());
        if (indexStart == std::wstring::npos) {
            return wstrText;
        }
        int NumOfBracks = 0;
        size_t indexEnd = indexStart;
        for (size_t i = indexStart + 1; i < wstrText.length(); ++i) {
            if (wstrText.at(i) == L'{') {
                NumOfBracks++;
            }
        }
        for (size_t i = indexStart + 1; NumOfBracks > 0 && i < wstrText.length(); ++i) {
            if (wstrText.at(i) == L'}') {
                NumOfBracks--;
            }
            indexEnd++;
        }
        wstrPre = wstrText.substr(0, indexStart);
        wstrMid = wstrText.substr(indexStart, indexEnd - indexStart + 2);
        if (indexEnd + 2 < wstrText.length()) {
            wstrEnd = wstrText.substr(indexEnd + 2);
        }
        wstrText = wstrMid;
    }
    UnicodeString msgPatSl = wstrText.c_str();
    MessageFormat *altMsgfmtSl = new MessageFormat(msgPatSl, Lanlocale, status);
    if (altMsgfmtSl == nullptr) {
        return wstrRet;
    }
    Formattable *arg = new Formattable[sizeof(Formattable) * vecNum.size()];
    if (arg == nullptr) {
        delete altMsgfmtSl;
        return wstrRet;
    }
    std::copy(vecNum.begin(), vecNum.end(), arg);
    altMsgfmtSl->format(arg, (int32_t)vecNum.size(), msgSl, fPos, status);
    delete altMsgfmtSl;
    delete[] arg;
    // ICU内部全部是unicode所以这里强转到wchar_t,在多字符集下可能会编译出错
    wstring wstrMsgSlBuf;
    if (msgSl.getTerminatedBuffer() != nullptr) {
        wstrMsgSlBuf = (wchar_t *)(msgSl.getTerminatedBuffer());
    }
    wstring wstrResult = wstrPre + wstrMsgSlBuf + wstrEnd;
    return wstrResult;
}

std::wstring UICulture::GetMultiVarStringForDotnet(const wstring& localString,
    const vector<int>& nums, const string& language)
{
    UnicodeString msgSl;
    FieldPosition fPos = 0;

    UErrorCode status = U_ZERO_ERROR;
    Locale locale = language.c_str();

    wstring wstrRet = localString;
    if (nums.size() == 0 || nums.size() > MAX_VEC_NUM) {
        return wstrRet;
    }

    UnicodeString msgPatSl = wstrRet.c_str();
    MessageFormat* altMsgfmtSl = new MessageFormat(msgPatSl, locale, status);
    if (altMsgfmtSl == nullptr) {
        return wstrRet;
    }
    Formattable* arg = new Formattable[sizeof(Formattable) * nums.size()];
    if (arg == nullptr) {
        delete altMsgfmtSl;
        return wstrRet;
    }
    std::copy(nums.begin(), nums.end(), arg);
    altMsgfmtSl->format(arg, (int32_t)nums.size(), msgSl, fPos, status);

    delete altMsgfmtSl;
    altMsgfmtSl = nullptr;

    delete[] arg;
    arg = nullptr;

    // ICU内部全部是unicode所以这里强转到wchar_t,在多字符集下可能会编译出错
    wstring wstrMsgSlBuf;
    if (msgSl.getTerminatedBuffer() != nullptr) {
        wstrMsgSlBuf = (wchar_t*)msgSl.getTerminatedBuffer();
    }
    return wstrMsgSlBuf;
}

std::wstring UICulture::GetLocalMultiVarStr(const wstring &strIDS, const vector<int> &vecNum, int count)
{
    UnicodeString msgSl;
    FieldPosition fPos = 0;
    UnicodeString result;
    UErrorCode status = U_ZERO_ERROR;
    Locale Lanlocale = core_->locale_.getName();
    if (getDirection() == DIRECTION_RTL) {
        if (Lanlocale != "ar_SA") { // 对于波斯语等管家不支持但有本地化计数方式，全部强制使用阿拉伯数字
            Lanlocale = "en_US";
        }
    }

    static bool s_checkLanguage = false;
    static bool s_useEnUs = false;
    if (!s_checkLanguage) {
        // 获取某个国家的字符串资源
        UErrorCode errcode1 = U_ZERO_ERROR;
        ResourceBundle s_country_res(core_->res_path_, core_->locale_, errcode1);

        if (errcode1 != U_ZERO_ERROR) {
            s_useEnUs = true; // 目标语言中找不到
        } else {
            s_useEnUs = false; // 目标语言能找不到
        }
        s_checkLanguage = true;
    }
    if (s_useEnUs == true) {
        Lanlocale = "en_US";
    }

    wstring wstrText = GetLocalString(strIDS.c_str());
    wstring wstrRet = wstrText;
    if (vecNum.size() == 0 || vecNum.size() > MAX_VEC_NUM) {
        return wstrText;
    }
    wstring wstrPre;
    wstring wstrMid;
    wstring wstrEnd;
    if (count > 0) {
        wstring subwstr = L"{0,plural";
        size_t indexStart = wstrText.find(subwstr.c_str());
        if (indexStart == std::wstring::npos) {
            return wstrText;
        }
        int NumOfBracks = 0;
        size_t indexEnd = indexStart;
        for (size_t i = indexStart + 1; i < wstrText.length(); ++i) {
            if (wstrText.at(i) == L'{') {
                NumOfBracks++;
            }
        }
        for (size_t i = indexStart + 1; NumOfBracks > 0 && i < wstrText.length(); ++i) {
            if (wstrText.at(i) == L'}') {
                NumOfBracks--;
            }
            indexEnd++;
        }
        wstrPre = wstrText.substr(0, indexStart);
        wstrMid = wstrText.substr(indexStart, indexEnd - indexStart + 2);
        if (indexEnd + 2 < wstrText.length()) {
            wstrEnd = wstrText.substr(indexEnd + 2);
        }
        wstrText = wstrMid;
    }
    UnicodeString msgPatSl = wstrText.c_str();
    MessageFormat *altMsgfmtSl = new MessageFormat(msgPatSl, Lanlocale, status);
    if (altMsgfmtSl == nullptr) {
        return wstrRet;
    }
    Formattable *arg = new Formattable[sizeof(Formattable) * vecNum.size()];
    if (arg == nullptr) {
        delete altMsgfmtSl;
        return wstrRet;
    }
    std::copy(vecNum.begin(), vecNum.end(), arg);
    altMsgfmtSl->format(arg, (int32_t)vecNum.size(), msgSl, fPos, status);
    delete altMsgfmtSl;
    delete[] arg;
    // ICU内部全部是unicode所以这里强转到wchar_t,在多字符集下可能会编译出错
    wstring wstrMsgSlBuf;
    if (msgSl.getTerminatedBuffer() != nullptr) {
        wstrMsgSlBuf = (wchar_t *)(msgSl.getTerminatedBuffer());
    }
    wstring wstrResult = wstrPre + wstrMsgSlBuf + wstrEnd;
    return wstrResult;
}

std::wstring UICulture::GetMultiVarStr(wstring strIDS, vector<int> vecNum, int count)
{
    wstring multiVarStr = GetLocalMultiVarStr(strIDS, vecNum, count);
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString strAll = GetString(strIDS.c_str());
    vector<DuiLib::CDuiString> valueList = strAll.Split(L"@UI@");
    if (valueList.size() != 2) { // 确认容器的大小
        return multiVarStr;
    }
    multiVarStr += L"@UI@";
    strAll = valueList.at(1); // 获取容器的第二个元素
    vector<DuiLib::CDuiString> valueTemp = strAll.Split(L"$UI$");
    for (size_t i = 0; i < valueTemp.size(); ++i) {
        DuiLib::CDuiString strVal = valueTemp[i];
        vector<DuiLib::CDuiString> endlist = strVal.Split(L"&UI&");
        if (endlist.size() == 2) { // 确认容器的大小
            multiVarStr += endlist[0].GetData();
            multiVarStr += L"&UI&";
            multiVarStr += GetPluralStr(endlist[0].GetData(), endlist[1].GetData(), vecNum, count);
            multiVarStr += L"$UI$";
        }
    }
#endif // SWITCH_LANGUAGE_TEST
    return multiVarStr;
}

wstring UICulture::GetTextFromHtmlText(CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont)
{
    wstring wstrRet;
    if (pstrText == nullptr || pManager == nullptr) {
        return wstrRet;
    }
    TEXTMETRIC *pTm = &(pManager->GetFontInfo(iFont)->tm);
    while (pstrText != nullptr && *pstrText != _T('\0')) {
        if (*pstrText == _T('\n')) {
            pstrText++;
        } else if ((*pstrText == _T('<') || *pstrText == _T('{')) &&
            (pstrText[1] >= _T('a') && pstrText[1] <= _T('z')) &&
            (pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}'))) {
            pstrText++;
            LPCTSTR pstrNextStart = nullptr;
            switch (*pstrText) {
                case _T('a'): { // Link
                    pstrText++;
                    break;
                }
                case _T('b'): {  // Bold
                    pstrText++;
                    break;
                }
                case _T('c'): {  // Color
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    if (pstrText != nullptr && *pstrText == _T('#')) {
                        pstrText++;
                    }
                    break;
                }
                case _T('f'): {  // Font
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    break;
                }
                case _T('i'): {  // Italic or Image
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    break;
                }
                case _T('n'): {  // Newline
                    pstrText++;
                    break;
                }
                case _T('p'): { // Paragraph
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    break;
                }
                case _T('r'): {  // Raw Text
                    pstrText++;
                    break;
                }
                case _T('s'): { // Selected text background color
                    pstrText++;
                    break;
                }
                case _T('u'): { // Underline text
                    pstrText++;
                    break;
                }
                case _T('x'): {  // X Indent
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    break;
                }
                case _T('y'): {  // Y Indent
                    pstrText++;
                    while (pstrText != nullptr && *pstrText > _T('\0') && *pstrText <= _T(' ')) {
                        pstrText = ::CharNext(pstrText);
                    }
                    break;
                }
            }
            if (pstrNextStart != nullptr) {
                pstrText = pstrNextStart;
            } else {
                while (pstrText != nullptr && *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}')) {
                    pstrText = ::CharNext(pstrText);
                }
                pstrText = ::CharNext(pstrText);
            }
        } else if ((*pstrText == _T('<') || *pstrText == _T('{')) && pstrText[1] == _T('/')) {
            pstrText++;
            pstrText++;
            switch (*pstrText) {
                case _T('c'): {
                    pstrText++;
                    break;
                }
                case _T('p'): {
                    pstrText++;
                    break;
                }
                case _T('s'): {
                    pstrText++;
                    break;
                }
                case _T('a'):
                case _T('b'):
                case _T('f'):
                case _T('i'):
                case _T('u'): {
                    pstrText++;
                    break;
                }
            }
            while (pstrText != nullptr && *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}')) {
                pstrText = ::CharNext(pstrText);
            }
            pstrText = ::CharNext(pstrText);
        } else if (*pstrText == _T('<') && pstrText[2] == _T('>') &&
            (pstrText[1] == _T('{') || pstrText[1] == _T('}'))) {
            pstrText++;
            pstrText++;
            pstrText++;
        } else if (*pstrText == _T('{') && pstrText[2] == _T('}') &&
            (pstrText[1] == _T('<') || pstrText[1] == _T('>'))) {
            pstrText++;
            pstrText++;
            pstrText++;
        } else if (*pstrText == _T(' ')) {
            pstrText++;
        } else {
            int cchChars = 0;
            int cchSize = 0;
            int cchLastGoodWord = 0;
            int cchLastGoodSize = 0;
            LPCTSTR p = pstrText;
            LPCTSTR pstrNext = nullptr;
            SIZE szText = { 0 };
            if (p != nullptr && (*p == _T('<') || *p == _T('{'))) {
                p++, cchSize++;
            }
            while (p != nullptr && *p != _T('\0') && *p != _T('\n')) {
                // This part makes sure that we're word-wrapping if needed or providing support
                // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
                // slow when repeated so often.
                // Rewrite and use GetTextExtentExPoint() instead!
                if ((*p == _T('<') || *p == _T('{')) && p[1] == _T('/') && p[2] == _T('r') &&
                    (p[3] == _T('>') || p[3] == _T('}'))) {
                    p += 4;
                    break;
                }
                if (*p == _T('<') || *p == _T('{')) {
                    break;
                }

                pstrNext = ::CharNext(p);
                cchSize += static_cast<int>(pstrNext - p);
                if (pTm != nullptr && abs(cchChars) < LONG_MUL_MAX && abs(pTm->tmMaxCharWidth) < LONG_MUL_MAX) {
                    szText.cx = cchChars * pTm->tmMaxCharWidth;
                }
                p = ::CharNext(p);
            }

            wstring layeredWstr(pstrText, cchSize);
            wstrRet += layeredWstr;
            pstrText += cchSize;
        }
    }
    return wstrRet;
}

int32_t UICulture::UnicodeFormat(const string &pstrFormat, wstring &unicodeChar, va_list vaList)
{
    UChar uStringBuf[MAX_TOUCH_COUNT];
    std::fill(uStringBuf, uStringBuf + sizeof(uStringBuf) / sizeof(uStringBuf[0]), 0x0a);
    int32_t nRet = u_vsprintf(uStringBuf, pstrFormat.c_str(), vaList);
    UnicodeString result = UnicodeString(uStringBuf);
    if (result.getTerminatedBuffer() != nullptr) {
        unicodeChar = reinterpret_cast<const wchar_t *>(result.getTerminatedBuffer());
    }
    return nRet;
}

std::wstring UICulture::ToUpper(const wstring &src)
{
    UnicodeString result(src.c_str());
    result.toUpper(core_->locale_);
    if (result.getTerminatedBuffer() != nullptr) {
        return reinterpret_cast<const wchar_t*>(result.getTerminatedBuffer());
    }
    return L"";
}

CDuiString UICulture::GetMultiVarString(LPCTSTR originalString, ...)
{
    DuiLib::CDuiString formattedString = DuiLib::UICulture::GetInstance()->GetString(originalString);
    int32_t index = 0;
    int32_t varIndex = 0;
    while (index != -1) {
        std::wstring varStr = _T("{") + std::to_wstring(varIndex) + _T("}");
        index = formattedString.Find(varStr.c_str(), 0);
        if (index != -1) {
            varIndex++;
        }
    }

    if (varIndex > 0) {
        UnicodeString result;
        Formattable *arguments = new Formattable[varIndex];
        if (arguments == nullptr) {
            return formattedString;
        }
        va_list list;
        va_start(list, originalString);
        int32_t count = va_arg(list, int32_t);
        varIndex = varIndex > count ? count : varIndex;

        for (int32_t i = 0; i < varIndex; ++i) {
            LPCTSTR vastr = va_arg(list, LPCTSTR);
            try {
                // 如挂在这里，请检查调用GetLocaleString时是否指定了参数的个数。
                arguments[i] = Formattable(vastr);
            } catch (...) {
                OutputDebugString(L"GetMultiVarStr format string with multiple strings error");
            }
        }
        va_end(list);

        UErrorCode err = U_ZERO_ERROR;
        MessageFormat::format(formattedString.GetData(), arguments, varIndex, result, err);

        delete[] arguments;
        arguments = nullptr;
        return (const wchar_t *)result.getTerminatedBuffer();
    } else {
        return formattedString;
    }
}

std::wstring UICulture::GetPercentageStr(const int percentage)
{
    UnicodeString msgSl;
    FieldPosition fPos = 0;
    UErrorCode status = U_ZERO_ERROR;
    Locale Lanlocale = core_->locale_.getName();
    if (getDirection() == DIRECTION_RTL) {
        if (Lanlocale != "ar_SA") {
            Lanlocale = "en_US";
        }
    }

    wstring wstrText = L"{0}%";
    if (Lanlocale == "tr_TR") {
        wstrText = L"%{0}";
    } else if (Lanlocale == "sv_SE" || Lanlocale == "es_MX" || Lanlocale == "fr_FR") {
        // LT测试部分语言要求%号与数字之间有空格
        wstrText = L"{0} %";
    }
    wstring wstrRet = wstrText;
    UnicodeString msgPatSl = wstrText.c_str();
    auto altMsgfmtSl = std::make_unique<MessageFormat>(msgPatSl, Lanlocale, status);
    if (altMsgfmtSl == nullptr) {
        return wstrRet;
    }
    auto arg = std::make_unique<Formattable>();
    if (arg == nullptr) {
        return wstrRet;
    }
    arg->setLong(percentage);
    constexpr int32_t argNum = 1;
    altMsgfmtSl->format(arg.get(), argNum, msgSl, fPos, status);

    wstring wstrMsgSlBuf;
    if (msgSl.getTerminatedBuffer() != nullptr) {
        wstrMsgSlBuf = (wchar_t*)msgSl.getTerminatedBuffer();
    }
    return wstrMsgSlBuf;
}
} // namespace DuiLib