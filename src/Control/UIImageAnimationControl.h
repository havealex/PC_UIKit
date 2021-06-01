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

#ifndef _IMAGEANIMATIONCONTROL_INCLUDE__
#define _IMAGEANIMATIONCONTROL_INCLUDE__

#include "UIlib.h"

NAMESPACE_DUILIB_BEGIN

class ImageAnimationPainter;
class ImageAnimationControlHelper;
class UILIB_API CImageAnimationControlUI : public DuiLib::CControlUI {
public:
    using BitmapList = std::vector<::HBITMAP>;
    using FileResList = std::vector<std::wstring>;

    CImageAnimationControlUI();

    // CImageAnimationControlUI will take ownership of aniPainter.
    explicit CImageAnimationControlUI(ImageAnimationPainter *aniPainter);
    ~CImageAnimationControlUI();

    void SetBitmaps(const BitmapList &bitmaps);

    void SetFileRes(const FileResList &fileRes, bool preLoad, bool backgroundLoading, bool isReverse = false);

    void SetPrecision(unsigned int precision);

    void SetDuring(unsigned int msec);

    void SetMaxRound(unsigned int maxRound = 1);

    void ShowLastIndex(bool isShow);

    bool IsAnimating();

    void SetAnimationBackgroundColor(DWORD color);

    void CloneStateFrom(CImageAnimationControlUI *ctrl);

    void Start();

    void Stop();

    void SetPadding(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom);

    void SetVisible(bool bVisible = true) override;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    ImageAnimationPainter *GetPainter() const;

private:
    virtual void DoPaint(HDC hDC, const RECT &rcPaint);

public:
    ImageAnimationControlHelper *helper;
};

NAMESPACE_DUILIB_END

#endif // _IMAGEANIMATIONCONTROL_INCLUDE__
