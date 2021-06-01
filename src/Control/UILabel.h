/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib {
class UILIB_API CLabelUI : public CAnimPanelUI {
    DECLARE_DUICONTROL(CLabelUI)
public:
    CLabelUI();
    ~CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetTextStyle(UINT uStyle);
    UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI &event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void JudgeDirection(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintText(HDC hDC);

    virtual bool GetAutoCalcWidth() const;
    virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
    virtual void SetText(LPCTSTR pstrText);
    CDuiRect GetTextRect(SIZE szAvailable);

    bool GetAutoCalcHeight() const;
    void SetAutoCalcHeight(bool bAutoCalcHeight);

    bool GetAutoHeightByTextSingle() const;
    void SetAutoHeightByTextSingle(bool bAutoHeightByTextSingle);

    bool GetAutoHeightByTextMulti() const;
    void SetAutoHeightByTextMulti(bool bAutoHeightByTextMulti);

    bool EstimateSizeYX(SIZE &_size);

    virtual void DoPaint(HDC hDC, const RECT &rcPaint);

    void SetEllipseMask(bool b);

    void SetFixedHeight(int cy) override;
    void SetFixedHeightEx(int cy) override;
    void SetFixedWidth(int cx) override;
    void SetFixedWidthEx(int cx) override;
    void SetFixedXY(SIZE szXY);
    void SetMinWidth(int cx) override;
    void SetMaxWidth(int cx) override;
    void SetMinHeight(int cy) override;
    void SetMaxHeight(int cy) override;
    void SetVisible(bool bVisible = true) override;
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    RECT SetDirectionPos(RECT rc);
    void SetPadding(RECT rcPadding);
    void SetFloat(bool bFloat = true) override;
    CDuiString GetToolTip() const override;

protected:
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    bool m_bShowHtml;
    bool m_bAutoCalcWidth;
    bool m_vcenterForce; // added by fanzhenyin to support vcenter when using wordbreak
    bool m_wordbreak;
    bool m_bAutoCalcHeight;
    bool m_bChangeParentHeight;
    bool m_bEllipseMask = false;

    bool m_bAutoHeightByTextSingle;
    bool m_bAutoHeightByTextMulti;
    bool m_bUpdateSize = true;
    UINT m_uLastScale = 0;
    CDuiSize m_lastSize;
#ifdef TEST_TIPS_BUILD
    TOOLINFO m_toolTips;
    void ShowToolTips();
    void SetInternVisible(bool bVisible = true) override;
    HWND m_hwndTooltip;
#endif
};
}

#endif // __UILABEL_H__