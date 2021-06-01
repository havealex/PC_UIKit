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

#ifndef DUILIB_UIMAINQUEUE_H
#define DUILIB_UIMAINQUEUE_H
#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace DuiLib {
class UILIB_API UIMainQueue {
private:
    UIMainQueue();
    ~UIMainQueue();

public:
    void over();
    static UIMainQueue &getInstance();
    bool dispatch(const std::function<void()> &function);
    bool finish();
    // 修改UI消息循环线程ID
    void SetUIMainThread(DWORD threadId);

private:
    DWORD main_thread_id_ = 0;
    std::recursive_mutex mutex_;
    std::queue<std::function<void()> > queue_;
    bool isOver = false;
#ifdef DEBUG
    std::unordered_map<int32_t, int32_t> requestStatics_; // 用于统计工作线程对主动投递的请求数
#endif
};
} // namespace DuiLib

#endif // ! DUILIB_UIMAINQUEUE_H