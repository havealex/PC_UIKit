#ifndef GifAnimUIEX_h__
#define GifAnimUIEX_h__
#pragma once
/*
 * ������gif�ؼ���gdi+������ռ��CPU���ߵ����⣬�������ximage���
 * ע�⣺ʹ�õ�ʱ����Ԥ����ͷ�ļ��а���UIlib.hǰ�ȶ����USE_XIMAGE_EFFECT
 * #define USE_XIMAGE_EFFECT
 * #include "UIlib.h"
 */

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

#ifdef USE_XIMAGE_EFFECT
namespace DuiLib {
class CLabelUI;

class UILIB_API CGifAnimExUI : public CLabelUI {
    DECLARE_DUICONTROL(CGifAnimExUI)
public:
    CGifAnimExUI(void);
    ~CGifAnimExUI(void);

public:
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual void Init();
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true);
    virtual void DoPaint(HDC hDC, const RECT &rcPaint);
    virtual void DoEvent(TEventUI &event);

public:
    void StartAnim();
    void StopAnim();

protected:
    struct Imp;
    Imp *m_pImp;
};
}
#endif // USE_XIMAGE_EFFECT
#endif // GifAnimUIEx_h__
