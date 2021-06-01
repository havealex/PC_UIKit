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

#ifndef __UITEXTSCROLLH__
#define __UITEXTSCROLLH__

#pragma once

namespace DuiLib {
#define ROLLTEXT_TIMERID_SPAN 50U
#define ROLLTEXT_ROLL_END_SPAN 1000 * 6U

constexpr int ROLLTEXT_LEFT = 0;
constexpr int ROLLTEXT_RIGHT = 1;
constexpr int ROLLTEXT_UP = 2;
constexpr int ROLLTEXT_DOWN = 3;
constexpr int ROLLTEXT_TIMERID = 20;
constexpr int ROLLTEXT_ROLL_END = 21;

class UILIB_API CRollTextUI : public CLabelUI {
    DECLARE_DUICONTROL(CRollTextUI)
public:
    CRollTextUI(void);
    ~CRollTextUI(void);

public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

public:
    virtual void PaintText(HDC hDC);
    void DoEvent(TEventUI &event) override;
    virtual void SetPos(RECT rc);
    void SetText(LPCTSTR pstrText) override;

public:
    void BeginRoll(int nDirect = ROLLTEXT_RIGHT, LONG lTimeSpan = ROLLTEXT_TIMERID_SPAN, LONG lMaxTimeLimited = 60);
    void EndRoll();

private:
    int m_nStep;
    int m_nScrollPos;
    BOOL m_bUseRoll;
    int m_nRollDirection;
    int m_nText_W_H;
};
} // namespace DuiLib

#endif // __UITEXTSCROLLH__