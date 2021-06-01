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

#include "../stdafx.h"
#include "../Utils/ImageAnimationPainter.h"
#include "../Control/UILoadingControl.h"
#include <string>

NAMESPACE_DUILIB_BEGIN

IMPLEMENT_DUICONTROL(CLoadingControlUI)

class LoadingAnimationPainter : public DuiLib::ImageAnimationPainter {
public:
    LoadingAnimationPainter(CLoadingControlUI::LoadingAnimationImageSize size, DuiLib::CControlUI *ctrl)
        : ImageAnimationPainter(ctrl)
    {
        SetSize(size);
    }

    ~LoadingAnimationPainter() {}

    void SetSize(CLoadingControlUI::LoadingAnimationImageSize size)
    {
        FileResList resList;
        std::wstring sizeStr;
        switch (size) {
            case DuiLib::CLoadingControlUI::LoadingAnimationSize26:
                sizeStr = _T("26");
                break;
            case DuiLib::CLoadingControlUI::LoadingAnimationSize47:
                sizeStr = _T("47");
                break;
            case DuiLib::CLoadingControlUI::LoadingAnimationSize70:
                sizeStr = _T("70");
                break;
            case DuiLib::CLoadingControlUI::LoadingAnimationSizeWhite70:
                sizeStr = _T("white_70");
                break;
            default:
                break;
        }
        std::wstring basePath = _T("drawable/synergy_common/loading_") + sizeStr + _T("/");
        const int iLen = 31;
        for (unsigned int i = 1; i < iLen; ++i) {
            std::wstring fn = _T("loading_") + std::to_wstring(i) + _T(".png");
            resList.push_back(basePath + fn);
        }

        SetFileRes(resList, true);
    }
};

CLoadingControlUI::CLoadingControlUI(CLoadingControlUI::LoadingAnimationImageSize size)
    : CImageAnimationControlUI(new LoadingAnimationPainter(size, this))
{}

CLoadingControlUI::CLoadingControlUI()
    : CImageAnimationControlUI(
    new LoadingAnimationPainter(CLoadingControlUI::LoadingAnimationImageSize::LoadingAnimationSize26, this))
{}

CLoadingControlUI::~CLoadingControlUI() {}

void CLoadingControlUI::SetImageSize(CLoadingControlUI::LoadingAnimationImageSize size)
{
    if (LoadingAnimationPainter *painter = dynamic_cast<LoadingAnimationPainter *>(GetPainter())) {
        painter->SetSize(size);
    }
}

LPCTSTR CLoadingControlUI::GetClass() const
{
    return _T("LoadingControlUI");
}

LPVOID CLoadingControlUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_LOADINGCONTROL) == 0)
        return static_cast<CLoadingControlUI *>(this);
    return CControlUI::GetInterface(pstrName);
}

void CLoadingControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("image_size")) == 0) {
        CDuiString val = pstrValue;
        if (val == _T("big")) {
            SetImageSize(CLoadingControlUI::LoadingAnimationImageSize::LoadingAnimationSize47);
        } else if (val == _T("small")) {
            SetImageSize(CLoadingControlUI::LoadingAnimationImageSize::LoadingAnimationSize26);
        } else if (val == _T("large")) {
            SetImageSize(CLoadingControlUI::LoadingAnimationImageSize::LoadingAnimationSize70);
        } else if (val == _T("largewhite")) {
            SetImageSize(CLoadingControlUI::LoadingAnimationImageSize::LoadingAnimationSizeWhite70);
        }
    } else if (_tcsicmp(pstrName, _T("state")) == 0) {
        CDuiString val = pstrValue;
        if (val == _T("start")) {
            Start();
            startState = true;
        }
    }
    __super::SetAttribute(pstrName, pstrValue);
}

void CLoadingControlUI::DoInit()
{
    if (startState) {
        Start();
    }
}

NAMESPACE_DUILIB_END
