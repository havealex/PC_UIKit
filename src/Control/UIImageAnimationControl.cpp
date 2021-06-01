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
#include "UIImageAnimationControl.h"
#include "../Utils/ImageAnimationPainter.h"

NAMESPACE_DUILIB_BEGIN
#define WHETHER_RETURN_DEFUALT(expression) {if (expression) {return;}}

class ImageAnimationControlHelper {
public:
    ImageAnimationControlHelper() {}

    ~ImageAnimationControlHelper() {}

    ImageAnimationPainter *painter = nullptr;
    bool bindWithVisible = false;
};

CImageAnimationControlUI::CImageAnimationControlUI() : helper(new ImageAnimationControlHelper)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    helper->painter = new (std::nothrow) ImageAnimationPainter(this);
    if (helper->painter == nullptr) {
        return;
    }
}

CImageAnimationControlUI::CImageAnimationControlUI(ImageAnimationPainter *aniPainter)
    : helper(new ImageAnimationControlHelper)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    helper->painter = aniPainter;
}

CImageAnimationControlUI::~CImageAnimationControlUI()
{
    if (helper != nullptr) {
        if (helper->painter != nullptr) {
            delete helper->painter;
            helper->painter = nullptr;
        }
        delete helper;
        helper = nullptr;
    }
}

void CImageAnimationControlUI::SetBitmaps(const BitmapList &bitmaps)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetBitmaps(bitmaps);
}

void CImageAnimationControlUI::SetDuring(unsigned int msec)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetDuring(msec);
}

void CImageAnimationControlUI::SetMaxRound(unsigned int maxRound)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetMaxRound(maxRound);
}

void CImageAnimationControlUI::ShowLastIndex(bool isShow)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->ShowLastIndex(isShow);
}

bool CImageAnimationControlUI::IsAnimating()
{
    if (this->helper == nullptr || this->helper->painter == nullptr) {
        return false;
    }
    return helper->painter->IsAnimating();
}

void CImageAnimationControlUI::CloneStateFrom(CImageAnimationControlUI *ctrl)
{
    if (ctrl == nullptr || helper == nullptr || helper->painter == nullptr || ctrl->helper == nullptr ||
        ctrl->helper->painter == nullptr) {
        return;
    }
    helper->painter->CloneStateFrom(ctrl->helper->painter);
}

void CImageAnimationControlUI::Start()
{
    if (helper == nullptr || helper->painter == nullptr) {
        return;
    }
    helper->painter->Start();
}

void CImageAnimationControlUI::Stop()
{
    if (helper != nullptr && helper->painter != nullptr) {
        helper->painter->Stop();
    }
}

void CImageAnimationControlUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (helper == nullptr || helper->painter == nullptr) {
        return;
    }
    helper->painter->Paint(hDC);
}

void CImageAnimationControlUI::SetPadding(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetPadding(left, top, right, bottom);
}

void DuiLib::CImageAnimationControlUI::SetVisible(bool bVisible)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    if (helper->bindWithVisible) {
        if (bVisible) {
            Start();
        } else {
            Stop();
        }
    }
    __super::SetVisible(bVisible);
}

void CImageAnimationControlUI::SetFileRes(const FileResList &fileRes, bool preLoad, bool backgroundLoading,
    bool isReverse)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetFileRes(fileRes, preLoad, backgroundLoading, isReverse);
}

void CImageAnimationControlUI::SetAnimationBackgroundColor(DWORD color)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetAnimationBackgroundColor(color);
}

void CImageAnimationControlUI::SetPrecision(unsigned int precision)
{
    WHETHER_RETURN_DEFUALT(this->helper == nullptr);
    WHETHER_RETURN_DEFUALT(this->helper->painter == nullptr);
    helper->painter->SetPrecision(precision);
}

void DuiLib::CImageAnimationControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("bindwithvisible")) == 0) {
        CDuiString val = pstrValue;
        if (val == _T("true")) {
            WHETHER_RETURN_DEFUALT(this->helper == nullptr);
            helper->bindWithVisible = true;
        }
    }
    __super::SetAttribute(pstrName, pstrValue);
}

DuiLib::ImageAnimationPainter *CImageAnimationControlUI::GetPainter() const
{
    if (this->helper == nullptr) {
        return nullptr;
    }
    return helper->painter;
}

NAMESPACE_DUILIB_END
