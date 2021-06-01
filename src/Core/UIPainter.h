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
#ifndef UIPAINTER_INCLUDE
#define UIPAINTER_INCLUDE

#include "../UIlib.h"

NAMESPACE_DUILIB_BEGIN

class UIPenHelper;
class UILIB_API UIPen {
public:
    UIPen();
    UIPen(const UIPen &other);
    explicit UIPen(DWORD color);
    ~UIPen();

    DWORD GetColor() const;

    UIPen &operator = (const UIPen &other);

private:
    UIPenHelper *helper = nullptr;
};

class UIBrushHelper;
class UILIB_API UIBrush {
public:
    UIBrush();
    UIBrush(const UIBrush &other);
    UIBrush(DWORD color);
    ~UIBrush();

    DWORD GetColor() const;

    UIBrush &operator = (const UIBrush &other);

private:
    UIBrushHelper *helper = nullptr;
};

class UIImage;
class UIPainterHelper;
class UILIB_API UIPainter {
public:
    explicit UIPainter(DuiLib::CControlUI *control, HDC hdc = nullptr);
    explicit UIPainter(DuiLib::UIImage *image);
    virtual ~UIPainter();

    enum RenderHint { Default, AntiAlias };

    void SetRenderHint(RenderHint rh);

    void SetPen(const UIPen &pen);

    void SetBrush(const UIBrush &brush);

    void DrawImage(int x, int y, int width, int height, UIImage *image);

    void DrawImage(int x, int y, UIImage *image);

    void DrawImage(const RECT &rc, UIImage *image);

    void DrawImage(int x, int y, int width, int height, UIImage *image, int srcX, int srcY, int srcWidth,
        int srcHeight);

    void DrawLine(int x1, int y1, int x2, int y2);

    void DrawLine(const POINT &p1, const POINT &p2);

    void FillRectange(const RECT &rc);

    void DrawRectange(const RECT &rc);

    void Translate(int x, int y);

    void ResetTransform();

    void Save();

    void Restore();

private:
    UIPainterHelper *helper = nullptr;
};

NAMESPACE_DUILIB_END

#endif // UIPAINTER_INCLUDE
