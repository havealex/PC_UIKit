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

#ifndef _IMAGEANIMATIONPAINTER_INCLUDE__
#define _IMAGEANIMATIONPAINTER_INCLUDE__

#include "../UIlib.h"

NAMESPACE_DUILIB_BEGIN

class ImageAnimationPainterHelper;
class UILIB_API ImageAnimationPainter {
public:
    using BitmapList = std::vector<::HBITMAP>;
    using FileResList = std::vector<std::wstring>;

    explicit ImageAnimationPainter(DuiLib::CControlUI *ctrl);
    virtual ~ImageAnimationPainter();

    void SetBitmaps(const BitmapList &bitmaps);

    /* !
     * @brief 使用duilib文件资源。
     *
     * @detail
     * @param	fileRes				文件资源路径。
     * @param	preLoad				是否预加载。
     * @param	backgroundLoading	不预加载图片时（preLoad为false时），是否在后台加载图片。
     * @return
     */
    void SetFileRes(const FileResList &fileRes, bool preLoad = false, bool backgroundLoading = false,
        bool isReverse = false);

    void SetPrecision(int precision);

    void SetDuring(int msec);
    // 设置循环播放轮数
    void SetMaxRound(int count = 1);
    // 是否播放中
    bool IsAnimating();
    // 结束时是否显示最后一帧
    void ShowLastIndex(bool isShow);
    void SetAnimationBackgroundColor(DWORD color);

    void Paint(HDC hdc);

    void Start();

    void Stop();

    void SetPadding(int left, int top, int right, int bottom);

    void CloneStateFrom(ImageAnimationPainter *painter);

private:
    ImageAnimationPainterHelper *helper;
};

NAMESPACE_DUILIB_END

#endif // _IMAGEANIMATIONPAINTER_INCLUDE__
