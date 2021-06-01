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

#include "FirstApp.h"
#include "FirstWnd.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + L"res");

    FirstWnd* firstWnd = new (std::nothrow) FirstWnd();
    firstWnd->Create(nullptr, L"FirstWnd", UI_WNDSTYLE_FRAME, 0L);
    firstWnd->CenterWindow();
    firstWnd->ShowWindow();

    CPaintManagerUI::MessageLoop();

    return 0;
}