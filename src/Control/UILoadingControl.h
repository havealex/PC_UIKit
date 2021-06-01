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

#ifndef _LOADINGCONTROL_INCLUDE__
#define _LOADINGCONTROL_INCLUDE__

#include "UIlib.h"
#include "Control/UIImageAnimationControl.h"

NAMESPACE_DUILIB_BEGIN

class LoadingControlHelper;
class UILIB_API CLoadingControlUI : public DuiLib::CImageAnimationControlUI {
    DECLARE_DUICONTROL(CLoadingControlUI)
public:
    enum LoadingAnimationImageSize {
        LoadingAnimationSize26,
        LoadingAnimationSize47,
        LoadingAnimationSize70,
        LoadingAnimationSizeWhite70,
    };

    CLoadingControlUI();
    explicit CLoadingControlUI(CLoadingControlUI::LoadingAnimationImageSize size);
    ~CLoadingControlUI();

    void SetImageSize(CLoadingControlUI::LoadingAnimationImageSize size);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

protected:
    void DoInit() override;
    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    bool startState = false;
};

NAMESPACE_DUILIB_END

#endif // _LOADINGCONTROL_INCLUDE__
