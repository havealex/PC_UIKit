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

#pragma once
#ifndef UIIMAGE_INCLUDE
#define UIIMAGE_INCLUDE

#include "UIlib.h"

NAMESPACE_DUILIB_BEGIN

class UIImageHelper;
class UILIB_API UIImage final {
public:
    UIImage();
    // UIIMage will take ownership of resource, excpet SetAutoDestroyResource(false)
    explicit UIImage(HBITMAP hbitmap);
    explicit UIImage(Gdiplus::Bitmap *bitmap);
    UIImage(int width, int height);
    ~UIImage();

    UIImage *Clone() const;

    Gdiplus::Bitmap *GetBitmap() const;

    int GetWidth() const;

    int GetHeight() const;

    HBITMAP GetHBITMAP() const;

    void SetOpacity(int opacity);

    void SetBlur(int blur, bool expandEdge = false);

    void SetBrightness(int brightness);

    int GetBrightness() const;

    int GetOpacity() const;

    int GetBlur() const;

    void SetAutoDestroyResource(bool b);

    void Save(const std::wstring &filePath);

public:
    UIImageHelper *helper = nullptr;
};

NAMESPACE_DUILIB_END

#endif // UIIMAGE_INCLUDE
