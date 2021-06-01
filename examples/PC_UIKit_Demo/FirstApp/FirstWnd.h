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

#ifndef PC_UIKIT_DEMO_FIRSTWND_H
#define PC_UIKIT_DEMO_FIRSTWND_H

#include "framework.h"

class FirstWnd : public WindowImplBase {
    DUI_DECLARE_MESSAGE_MAP()

public:
    CDuiString GetSkinFile() override;
    LPCTSTR GetWindowClassName(void) const override;
    void OnClick(TNotifyUI &msg);
    void OnSelectMenuChanged(TNotifyUI &msg);
};
#endif
