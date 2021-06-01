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
#include "UIMainQueue.h"
#include "../Debug/LogOut.h"

using namespace std;


#include <tlhelp32.h>
DWORD GetMainThreadId(DWORD processId = 0)
{
    if (processId == 0)
        processId = GetCurrentProcessId();

    DWORD threadId = 0;
    THREADENTRY32 te32 = { sizeof(te32) };
    HANDLE threadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (Thread32First(threadSnap, &te32)) {
        do {
            if (processId == te32.th32OwnerProcessID) {
                threadId = te32.th32ThreadID;
                break;
            }
        } while (Thread32Next(threadSnap, &te32));
    }
    return threadId;
}

namespace DuiLib {
UIMainQueue::UIMainQueue()
{
    main_thread_id_ = GetMainThreadId();
}

UIMainQueue::~UIMainQueue()
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    queue_.swap(decltype(queue_)()); // 清空queue
}

UIMainQueue &UIMainQueue::getInstance()
{
    static UIMainQueue main;
    return main;
}

bool UIMainQueue::dispatch(const std::function<void()> &function)
{
    mutex_.lock();
    if (!isOver) {
        queue_.emplace(function);
    }

#ifdef DEBUG
    auto threadid = (int32_t)GetCurrentThreadId();
    auto count = ++requestStatics_[GetCurrentThreadId()];
    LOGGER_DEBUG("Workthread: " << threadid << ", request count=" << count);
#endif
    mutex_.unlock();
    ::PostThreadMessage(main_thread_id_, UIMSG_UPDATE_ON_MAIN, 0, 0);

    return true;
}

bool UIMainQueue::finish()
{
    // 使用临时列表取出队列中的function，再执行，在执行过程中dispatch进来的function留到下一次执行
    // 以保证界面循环执行的比较均匀
    std::vector<std::function<void()>> funList;
    mutex_.lock();
    while (!queue_.empty()) {
        funList.push_back(std::move(queue_.front()));
        queue_.pop();
    }
    mutex_.unlock();
    // 执行取出的function
    for (auto &func : funList) {
        if (func) {
            func();
            this_thread::yield();
        }
    }
    return true;
}

void UIMainQueue::SetUIMainThread(DWORD threadId)
{
    main_thread_id_ = threadId;
}

void UIMainQueue::over()
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    queue_.swap(decltype(queue_)()); // 清空queue
    isOver = true;
}
} // namespace DuiLib
