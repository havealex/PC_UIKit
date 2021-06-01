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
#include "UIImage.h"

NAMESPACE_DUILIB_BEGIN

class UIImageHelper {
public:
    Gdiplus::Bitmap *bitmap = nullptr;
    HBITMAP hbitmap = nullptr;
    int opacity = 100;
    int blur = 0;
    int brightness = 0;
    bool autoDestroyRes = true;
};

UIImage::UIImage() : helper(new UIImageHelper) {}

UIImage::UIImage(Gdiplus::Bitmap *bitmap) : helper(new UIImageHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->bitmap = bitmap;
}

UIImage::UIImage(HBITMAP hbitmap) : helper(new UIImageHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->hbitmap = hbitmap;
}

UIImage::UIImage(int width, int height) : helper(new UIImageHelper)
{
    if (helper == nullptr) {
        return;
    }
    helper->bitmap = new Gdiplus::Bitmap(width, height);
}

UIImage::~UIImage()
{
    if (helper) {
        if (helper->bitmap && helper->autoDestroyRes) {
            delete helper->bitmap;
            helper->bitmap = nullptr;
        }

        if (helper->hbitmap && helper->autoDestroyRes) {
            DeleteObject(helper->hbitmap);
        }

        delete helper;
        helper = nullptr;
    }
}

Gdiplus::Bitmap *UIImage::GetBitmap() const
{
    if (helper == nullptr) {
        return nullptr;
    }
    return helper->bitmap;
}

DuiLib::UIImage *UIImage::Clone() const
{
    if (helper == nullptr) {
        return new UIImage;
    }
    if (helper->bitmap) {
        Gdiplus::Bitmap *newBitmap = helper->bitmap->Clone(0, 0, helper->bitmap->GetWidth(),
            helper->bitmap->GetHeight(), helper->bitmap->GetPixelFormat());
        return new UIImage(newBitmap);
    }

    if (helper->hbitmap) {
        BITMAP bmp = { 0 };
        if (GetObject(helper->hbitmap, sizeof(BITMAP), (LPVOID)&bmp)) {
            return new UIImage((HBITMAP)CopyImage(helper->hbitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION));
        }
    }

    return new UIImage;
}

HBITMAP UIImage::GetHBITMAP() const
{
    if (helper == nullptr) {
        return nullptr;
    }
    return helper->hbitmap;
}

int UIImage::GetWidth() const
{
    if (helper == nullptr) {
        return 0;
    }
    if (helper->bitmap) {
        return helper->bitmap->GetWidth();
    }

    return 0;
}

int UIImage::GetHeight() const
{
    if (helper == nullptr) {
        return 0;
    }
    if (helper->bitmap) {
        return helper->bitmap->GetHeight();
    }

    return 0;
}

void UIImage::SetOpacity(int opacity)
{
    if (helper == nullptr) {
        return;
    }
    helper->opacity = opacity;
}

int UIImage::GetOpacity() const
{
    if (helper == nullptr) {
        return 0;
    }
    return helper->opacity;
}

void UIImage::SetAutoDestroyResource(bool b)
{
    if (helper == nullptr) {
        return;
    }
    helper->autoDestroyRes = b;
}

void UIImage::SetBlur(int blurVal, bool expandEdge)
{
    if (helper == nullptr) {
        return;
    }
    helper->blur = blurVal;
    const int maxRGB = 255;
    const float getPercentage = 100.0f;
    if (blurVal > 0 && helper->bitmap) {
        Gdiplus::Blur blur;
        Gdiplus::BlurParams blurParams;
        blurParams.radius = maxRGB * blurVal / getPercentage;
        blurParams.expandEdge = expandEdge ? TRUE : FALSE;
        blur.SetParameters(&blurParams);
        helper->bitmap->ApplyEffect(&blur, nullptr);
    }
}

int UIImage::GetBlur() const
{
    if (helper == nullptr) {
        return 0;
    }
    return helper->blur;
}

void UIImage::Save(const std::wstring &filePath)
{
    if (helper == nullptr) {
        return;
    }
    if (helper->bitmap) {
        Utils::SaveBitmap(helper->bitmap, filePath);
    }
}

void UIImage::SetBrightness(int brightness)
{
    if (helper == nullptr) {
        return;
    }
    if (brightness > 0 && helper->bitmap) {
        Gdiplus::BrightnessContrastParams briConParams;
        briConParams.brightnessLevel = brightness;
        briConParams.contrastLevel = 0;
        Gdiplus::BrightnessContrast briCon;
        briCon.SetParameters(&briConParams);

        helper->bitmap->ApplyEffect(&briCon, nullptr);
    }
}

int UIImage::GetBrightness() const
{
    if (helper == nullptr) {
        return 0;
    }
    return helper->brightness;
}

NAMESPACE_DUILIB_END
