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
#include "imageanimationpainter.h"

NAMESPACE_DUILIB_BEGIN

const UINT IMAGE_ANI_TIMER_ID = 0x4397;
const int TIMER_PRECISION = 10;

class ImageAnimationPainterHelper : public std::enable_shared_from_this<ImageAnimationPainterHelper> {
public:
    enum DrawResType {
        UseBitmap,
        UseRes
    };

    bool OnDoNofify(void *param)
    {
        DuiLib::TNotifyUI *noti = (DuiLib::TNotifyUI *)(param);
        if (noti == nullptr) {
            return false;
        }
        if (noti->pSender == ctrl && noti->sType == _T("timer")) {
            elapsedMsecs += precision;
            if (elapsedMsecs > duringMsecs + precision) {
                elapsedMsecs = 0;
            }

            UpdateBitmapIndex();
        }

        return true;
    }

    void UpdateBitmapIndex()
    {
        if (this->ctrl == nullptr) {
            return;
        }
        int count = (resType == UseBitmap ? static_cast<int>(bitmapList.size())
            : static_cast<int>(fileResList.size()));

        int tempIndex = 0;
        if (duringMsecs == 0) {
            tempIndex = bitmapIndex + 1;
            if (tempIndex >= count) {
                tempIndex = 0;
            }
        } else {
            tempIndex = static_cast<int>((static_cast<double>(elapsedMsecs) / duringMsecs) * count);
        }
        if (roundCount && (loadingRound >= loadingMaxRound || tempIndex >= count)) {
            loadingRound = 0;
            tempIndex = 0;
            lastIndexPath = fileResList[fileResList.size() - 1];
            ctrl->GetManager()->KillTimer(ctrl, IMAGE_ANI_TIMER_ID);
            QuitLoadingThread();
            start = false;
            ctrl->Invalidate();
        }
        if (tempIndex != bitmapIndex) {
            bitmapIndex = tempIndex;

            if (bitmapIndex >= count) {
                bitmapIndex = count - 1;
                loadingRound++;
            }

            if (resType == UseRes && backgroundLoading) {
                std::unique_lock<std::mutex> autoLock(mutexUpdateFileRes);
                loadingFileRes = fileResList[bitmapIndex];
                conditionUpdateFileRes.notify_all();
            }

            ctrl->Invalidate();
        }
    }

    void DrawBitmap(HDC hdc, ::HBITMAP hbitmap)
    {
        if (this->ctrl == nullptr) {
            return;
        }
        BITMAP bm;
        GetObject(hbitmap, sizeof(BITMAP), &bm);
        int imageWidth = bm.bmWidth;
        int imageHeight = bm.bmHeight;
        RECT rcDraw = ctrl->GetPos();
        rcDraw.left += paddingLeft;
        rcDraw.top += paddingTop;
        rcDraw.right -= paddingRight;
        rcDraw.bottom -= paddingBottom;

        DuiLib::CRenderEngine::DrawImage(hdc, hbitmap, rcDraw, rcDraw, RECT { 0, 0, imageWidth, imageHeight },
            RECT { 0, 0, 0, 0 }, true);
    }

    void DrawImage(HDC hdc)
    {
        if (!ctrl) {
            return;
        }

        RECT rcItem;
        rcItem = ctrl->GetPos();
        if (!start && showLastIndex) {
            DuiLib::CRenderEngine::DrawImageString(hdc, ctrl->GetManager(), rcItem, rcItem, lastIndexPath.c_str());
            ctrl->GetManager()->RemoveImage(lastIndexPath.c_str());
        }
        if (!start) {
            return;
        }
        if (bkColor) {
            DuiLib::CRenderEngine::DrawColor(hdc, rcItem, bkColor);
        }

        if (resType == UseBitmap && bitmapIndex < bitmapList.size()) {
            ::HBITMAP hbitmap = bitmapList[bitmapIndex];
            DrawBitmap(hdc, hbitmap);
        } else if (resType == UseRes && bitmapIndex < fileResList.size() && ctrl && ctrl->GetManager()) {
            if (!isReverse) {
                if (backgroundLoading) {
                    if (loadingBitmap) {
                        DrawBitmap(hdc, loadingBitmap);
                    }
                } else {
                    std::wstring fileRes = fileResList[bitmapIndex];
                    DuiLib::CRenderEngine::DrawImageString(hdc, ctrl->GetManager(), rcItem, rcItem, fileRes.c_str());

                    // duilib会将使用过的image缓存起来，如果这里不RemoveImage，会导致大量内存占用；
                    // 但是加上这句，因为每次绘制都要重新加载图片，会导致大量CPU占用。
                    ctrl->GetManager()->RemoveImage(fileRes.c_str());
                }
            } else {
                std::wstring fileRes = fileResList[bitmapIndex];
                TDrawInfo imageInfo;
                imageInfo.sDrawString = fileRes.c_str();
                imageInfo.Parse(imageInfo.sDrawString, imageInfo.sDrawModify);
                DuiLib::CRenderEngine::DrawImage(hdc, ctrl->GetManager(), rcItem, rcItem, imageInfo);
                ctrl->GetManager()->RemoveImage(fileRes.c_str());
            }
        }
    }

    void ResetBitmaps(const ImageAnimationPainter::BitmapList &bitmaps)
    {
        for (int i = 0; i < bitmapList.size(); ++i) {
            ::HBITMAP hb = bitmapList[i];
            ::DeleteObject(hb);
        }

        bitmapList = bitmaps;
    }

    static void LoadResThread(void *param)
    {
        auto helper = reinterpret_cast<ImageAnimationPainterHelper *>(param);
        helper->backgroundThreadStarted = true;
        helper->conditionBackgroundThread.notify_all();

        while (!helper->quitLoadingThread) {
            std::wstring loadingFileRes;
            {
                std::unique_lock<std::mutex> autoLock(helper->mutexUpdateFileRes);
                while (helper->loadingFileRes.empty() && !helper->quitLoadingThread) {
                    helper->conditionUpdateFileRes.wait(autoLock);
                }

                loadingFileRes = helper->loadingFileRes;
                helper->loadingFileRes.clear();
            }

            if (helper->quitLoadingThread) {
                break;
            }

            DWORD timeStart = GetTickCount();

            if (::HBITMAP hbitmap = Utils::LoadBitmapFromDuiRes(loadingFileRes.c_str())) {
                ::HBITMAP tempBitmap = helper->loadingBitmap;
                helper->loadingBitmap = hbitmap;
                if (tempBitmap) {
                    ::DeleteObject(tempBitmap);
                }
            }

            DWORD timeUsed = GetTickCount() - timeStart;
            if (timeUsed < (DWORD)helper->precision) {
                Sleep(helper->precision - timeUsed);
            }
        }

        helper->backgroundThreadStarted = false;
    }

    void QuitLoadingThread()
    {
        {
            std::unique_lock<std::mutex> autoLock2(mutexUpdateFileRes);
            quitLoadingThread = true;
            loadingFileRes.clear();
            conditionUpdateFileRes.notify_all();
        }

        if (threadLoadBitmap.joinable()) {
            threadLoadBitmap.join();
        }
    }

    ImageAnimationPainter::BitmapList bitmapList = {};
    ImageAnimationPainter::FileResList fileResList = {};
    int bitmapIndex = 0;
    int elapsedMsecs = 0;
    int duringMsecs = 1000;
    int paddingLeft = 0;
    int paddingTop = 0;
    int paddingRight = 0;
    int paddingBottom = 0;
    int precision = TIMER_PRECISION;
    bool boundControl = false;
    bool backgroundLoading = false;
    // 是否反转：默认不反转，正常加载图片；否则阿语下反转，解决8位图显示失真问题
    bool isReverse = false;
    bool start = false;
    bool showLastIndex = false;
    wstring lastIndexPath = L"";
    DWORD bkColor = 0;
    DuiLib::CControlUI *ctrl = 0;

    bool backgroundThreadStarted = false;
    bool quitLoadingThread = false;
    std::mutex mutexBackgroundThread = {};
    std::condition_variable conditionBackgroundThread = {};
    std::mutex mutexUpdateFileRes = {};
    std::condition_variable conditionUpdateFileRes = {};
    std::wstring loadingFileRes = L"";
    ::HBITMAP loadingBitmap = 0;

    DrawResType resType = UseBitmap;

    std::thread threadLoadBitmap = {};
    int loadingRound = 0;
    int loadingMaxRound = 1;
    bool roundCount = false;
};

ImageAnimationPainter::ImageAnimationPainter(DuiLib::CControlUI *ctrl) : helper(new ImageAnimationPainterHelper)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->ctrl = ctrl;
}

ImageAnimationPainter::~ImageAnimationPainter()
{
    if (this->helper == nullptr) {
        return;
    }
    helper->QuitLoadingThread();
    for (int i = 0; i < helper->bitmapList.size(); ++i) {
        ::HBITMAP hb = helper->bitmapList[i];
        ::DeleteObject(hb);
    }

    delete helper;
    helper = nullptr;
}

void ImageAnimationPainter::SetBitmaps(const BitmapList &bitmaps)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->ResetBitmaps(bitmaps);
    helper->resType = ImageAnimationPainterHelper::UseBitmap;
}

void ImageAnimationPainter::SetFileRes(const FileResList &fileRes, bool preLoad, bool backgroundLoading, bool isReverse)
{
    if (helper == nullptr) {
        return;
    }
    helper->isReverse = isReverse;
    if (!preLoad) {
        helper->ResetBitmaps(BitmapList());
        helper->fileResList = fileRes;
        helper->resType = ImageAnimationPainterHelper::UseRes;
        helper->backgroundLoading = backgroundLoading;
    } else {
        BitmapList bitmaps;
        for (int i = 0; i < fileRes.size(); ++i) {
            std::wstring resStr = fileRes[i];
            ::HBITMAP hb = Utils::LoadBitmapFromDuiRes(resStr.c_str());
            if (hb) {
                bitmaps.push_back(hb);
            }
        }

        if (bitmaps.size() > 0) {
            helper->ResetBitmaps(bitmaps);
        }

        helper->resType = ImageAnimationPainterHelper::UseBitmap;
    }
}

void ImageAnimationPainter::SetPrecision(int precision)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->precision = precision;
}

void ImageAnimationPainter::Paint(HDC hdc)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->DrawImage(hdc);
}

void ImageAnimationPainter::SetDuring(int msec)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->duringMsecs = msec;
}

void ImageAnimationPainter::SetMaxRound(int count)
{
    if (this->helper == nullptr) {
        return;
    }
    if (count > 0) {
        helper->loadingMaxRound = count;
        helper->roundCount = true;
    } else {
        helper->loadingMaxRound = count;
        helper->roundCount = false;
    }
}

void ImageAnimationPainter::ShowLastIndex(bool isShow)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->showLastIndex = isShow;
}

void ImageAnimationPainter::SetAnimationBackgroundColor(DWORD color)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->bkColor = color;
}

bool ImageAnimationPainter::IsAnimating()
{
    if (this->helper == nullptr) {
        return false;
    }
    return helper->start;
}

void ImageAnimationPainter::Start()
{
    if (this->helper == nullptr || helper->ctrl == nullptr) {
        return;
    }
    if ((helper->resType == ImageAnimationPainterHelper::UseBitmap && helper->bitmapList.size() == 0) &&
        (helper->resType == ImageAnimationPainterHelper::UseRes && helper->fileResList.size() == 0)) {
        return;
    }

    helper->start = true;

    if (!helper->boundControl && helper->ctrl) {
        helper->ctrl->OnNotify += DuiLib::MakeDelegate(helper, &ImageAnimationPainterHelper::OnDoNofify);
        helper->boundControl = true;
    }

    helper->bitmapIndex = -1;
    helper->elapsedMsecs = 0;
    if (helper->ctrl->GetManager()) {
        helper->ctrl->GetManager()->SetTimer(helper->ctrl, IMAGE_ANI_TIMER_ID, helper->precision);
    }

    if (helper->resType == ImageAnimationPainterHelper::UseRes && helper->backgroundLoading &&
        !helper->backgroundThreadStarted) {
        helper->quitLoadingThread = false;
        helper->threadLoadBitmap = std::thread(ImageAnimationPainterHelper::LoadResThread, helper);

        std::unique_lock<std::mutex> autoLock(helper->mutexBackgroundThread);
        while (!helper->backgroundThreadStarted) {
            helper->conditionBackgroundThread.wait(autoLock);
        }
    }
}

void ImageAnimationPainter::Stop()
{
    if (this->helper == nullptr || helper->ctrl == nullptr) {
        return;
    }
    helper->ctrl->GetManager()->KillTimer(helper->ctrl, IMAGE_ANI_TIMER_ID);
    helper->QuitLoadingThread();
    helper->start = false;
    helper->ctrl->Invalidate();
}

void ImageAnimationPainter::SetPadding(int left, int top, int right, int bottom)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->paddingLeft = left;
    helper->paddingTop = top;
    helper->paddingRight = right;
    helper->paddingBottom = bottom;
}

void ImageAnimationPainter::CloneStateFrom(ImageAnimationPainter *painter)
{
    if (painter == nullptr) {
        return;
    }

    if (painter->helper == nullptr || this->helper == nullptr) {
        return;
    }
    helper->bitmapIndex = painter->helper->bitmapIndex;
    helper->elapsedMsecs = painter->helper->elapsedMsecs;
}

NAMESPACE_DUILIB_END
