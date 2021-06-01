/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib {
class UILIB_API CTextUI : public CLabelUI {
    DECLARE_DUICONTROL(CTextUI)
public:
    CTextUI();
    ~CTextUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CDuiString *GetLinkContent(int iIndex);

    void DoEvent(TEventUI &event) override;
    SIZE EstimateSize(SIZE szAvailable);

    void PaintText(HDC hDC);
    int GetLinkContentIndex(const POINT &ptMouse);

    void Shark(int duration = 2000);

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    int m_nHoverLink;
    int m_iLinkIndex[MAX_LINK];

    UINT64 m_sharkStartTick = 0;
    bool m_bShark = false;
    int m_durationTime = 2000;

    UINT32 m_sharkpos = 0;

public:
    CDuiString m_sLinks[MAX_LINK];
};
} // namespace DuiLib

#endif // __UITEXT_H__