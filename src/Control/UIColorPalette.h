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

#ifndef UI_PALLET_H
#define UI_PALLET_H
#pragma once

namespace DuiLib {
class UILIB_API CColorPaletteUI : public CControlUI {
    DECLARE_DUICONTROL(CColorPaletteUI)
public:
    CColorPaletteUI();
    virtual ~CColorPaletteUI();

    // ��ȡ���ձ�ѡ�����ɫ������ֱ����������duilib����ɫ
    DWORD GetSelectColor();
    void SetSelectColor(DWORD dwColor);

    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    // ����/��ȡ Pallet����ɫ�������棩�ĸ߶�
    void SetPalletHeight(int nHeight);
    int GetPalletHeight() const;

    // ����/��ȡ �·�Bar������ѡ�������ĸ߶�
    void SetBarHeight(int nHeight);
    int GetBarHeight() const;
    // ����/��ȡ ѡ��ͼ���·��
    void SetThumbImage(LPCTSTR pszImage);
    LPCTSTR GetThumbImage() const;

    virtual void SetPos(RECT rc);
    void DoInit() override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintPallet(HDC hDC);

protected:
    // ��������
    void UpdatePalletData();
    void UpdateBarData();

private:
    HDC m_MemDc;
    HBITMAP m_hMemBitmap;
    BITMAP m_bmInfo;
    BYTE *m_pBits;
    UINT m_uButtonState;
    bool m_bIsInBar;
    bool m_bIsInPallet;
    int m_nCurH;
    int m_nCurS;
    int m_nCurB;

    int m_nPalletHeight;
    int m_nBarHeight;
    CDuiPoint m_ptLastPalletMouse;
    CDuiPoint m_ptLastBarMouse;
    CDuiString m_strThumbImage;
};
}

#endif // UI_PALLET_H