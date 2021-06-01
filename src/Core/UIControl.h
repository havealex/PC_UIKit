/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#pragma once
#ifndef __UICONTROL_H__
#define __UICONTROL_H__

namespace DuiLib {
class UIImage;
using FINDCONTROLPROC = CControlUI *(CALLBACK *)(CControlUI *, LPVOID);

class CControlUIHelper;
class UILIB_API CControlUI {
    DECLARE_DUICONTROL(CControlUI)
public:
    CControlUI();
    virtual ~CControlUI();

public:
    UIImage *GrabImage(const SIZE &sz, const POINT &pt = POINT { 0, 0 });
    HBITMAP GrabHBITMAP(const SIZE &sz, const POINT &pt = POINT { 0, 0 });
    bool IsGrabbing() const;
    bool GetAutoChangeParentHeight() const;
    void SetAutoChangeParentHeight(bool bChangeParentHeight);
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    void SetControlFlags(UINT nCtrlFlags);
    virtual UINT GetControlFlags() const;

    virtual bool Activate();
    virtual CPaintManagerUI *GetManager() const;
    virtual void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true);
    virtual CControlUI *GetParent() const;
    void setInstance(HINSTANCE instance = nullptr)
    {
        m_instance = instance;
    };

    // 定时器
    bool SetTimer(UINT nTimerID, UINT nElapse);
    void KillTimer(UINT nTimerID);

    // 文本相关
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);
    virtual void SetDisableText(LPCTSTR pstrText);
    virtual CDuiString GetDisableText() const;
    virtual void SetLanguageText(LPCTSTR language);
    virtual bool IsResourceText() const;
    virtual void SetResourceText(bool bResource);


    bool m_bDragEnabled = false;
    bool m_bDropEnabled = false;

    virtual bool IsDragEnabled() const;
    virtual void SetDragEnable(bool bDrag);

    virtual bool IsDropEnabled() const;
    virtual void SetDropEnable(bool bDrop);

    virtual void SetTipValue(LPCTSTR pStrTipValue);

    virtual void OnDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINT pt, DWORD *pdwEffect);
    virtual void OnDragOver(DWORD grfKeyState, POINT pt, DWORD *pdwEffect);
    virtual void OnDragLeave();
    virtual void OnDrop(IDataObject *pDataObj, DWORD grfKeyState, POINT pt, DWORD *pdwEffect);

    void SetEnsureTextVisible(bool ensureTextVisible);
    bool IsEnsureTextVisible() const;

    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    DWORD GetBkColor4() const;
    void SetBkColor4(DWORD dwBackColor);
    DWORD GetForeColor() const;
    void SetForeColor(DWORD dwForeColor);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);

    DWORD GetFocusBorderColor() const;
    void SetFocusBorderColor(DWORD dwBorderColor);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = nullptr);

    virtual void SetBlur(int n);

    virtual void SetOpacity(int n);

    int GetOpacity() const;

    int GetBlur() const;

    // 边框相关
    int GetBorderSize() const;
    void SetBorderSize(int nSize);
    DWORD GetBorderColor() const;
    void SetBorderColor(DWORD dwBorderColor);
    void SetBorderSize(RECT rc);
    int GetLeftBorderSize() const;
    void SetLeftBorderSize(int nSize);
    int GetTopBorderSize() const;
    void SetTopBorderSize(int nSize);
    int GetRightBorderSize() const;
    void SetRightBorderSize(int nSize);
    int GetBottomBorderSize() const;
    void SetBottomBorderSize(int nSize);
    int GetBorderStyle() const;
    void SetBorderStyle(int nStyle);
    void SetBorderFilledColor(DWORD borderFilledColor);
    void SetCanRefreshSelf(bool bCanRefreshSelf);
    bool GetCanRefreshSelf();
    void SetFont(int index);
    int GetFont() const;
    void SetPtFont(int index, bool bold = false, bool underline = false, FontType ft = FontType::Text);
    void CachePos(const RECT &rc);

    // 位置相关
    virtual RECT GetRelativePos() const; // 相对(父控件)位置
    virtual RECT GetClientPos() const;   // 客户区域（除去scrollbar和inset）
    virtual const RECT &GetPos() const;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
    virtual RECT SetDirectionPos(RECT rc);
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual void SetFixedWidthEx(int cx);
    virtual void SetFixedHeightEx(int cy);
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
    virtual TPercentInfo GetFloatPercent() const;
    virtual void SetFloatPercent(TPercentInfo piFloatPercent);

    virtual void SetWave(bool _val);
    // 鼠标提示
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
    virtual void SetToolTipWidth(int nWidth);
    virtual int GetToolTipWidth(void); // 多行ToolTip单行最长宽度

    // 光标
    virtual WORD GetCursor();
    virtual void SetCursor(WORD wCursor);

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const CDuiString &GetUserData();    // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const;            // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag);         // 辅助函数，供用户使用

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

    virtual CControlUI *FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    void SetBkRound(int round);
    void SetToUpper(bool enToUpper);
    CDuiString SetTextToUpper(const CDuiString &pstrText) const;

    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI &event);
    virtual void DoEvent(TEventUI &event);
    virtual void SetDirectionPadding(LPCTSTR pstrValue);
    virtual void SetCurAttr(LPCTSTR pstrValue);
    virtual bool GetNcHit();
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetAttribute2(LPCTSTR pstrName, LPCTSTR pstrValue);
    CControlUI *ApplyAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(HDC hDC, const RECT &rcPaint);

    void PaintBlurControl(HDC hDC, RECT &rcItem);

    void PaintBlurWallpaper(HDC hDC, RECT &rcItem);

    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintForeColor(HDC hDC);
    virtual void PaintForeImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT &rcPaint);

    // 虚拟窗口参数
    void SetVirtualWnd(LPCTSTR pstrValue);
    CDuiString GetVirtualWnd() const;

    void setUserString(const CDuiString &_key, const CDuiString &_value);
    CDuiString getUserString(const CDuiString &_key) const;
    const std::map<CDuiString, CDuiString> &getUserStrings() const;
    bool clearUserString(const CDuiString &_key);
    bool isUserString(const CDuiString &_key) const;
    void clearUserStrings();

    RECT GetFloatRelativePos();

    void AddBlurControl(const CDuiString &controlName);

    void RemoveBlurControl(const CDuiString &controlName);
    void SetAttributeForMap(std::vector<CDuiString> vecDuiStr, LPCTSTR language, LPCTSTR attribute);

    enum class BackgroundEffect { None, BlurControl, BlurWallpaper };

    void SetBackgroundEffect(BackgroundEffect be);

public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;
    CPaintManagerUI *m_pManager;
    CControlUI *m_pParent;

protected:
    CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bGrabbing = false;
    bool m_bMenuUsed;
    int m_iFont = -1;
    UINT m_nControlFlags = 0;
    RECT m_rcItem;
    bool m_bPosCached = false;
    RECT m_rcCachedPos = RECT { 0 };
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
    bool m_bKeyboardEnabled;
    bool m_bFocused;
    bool m_bFloat;
    bool m_ensureTextVisible = false;
    TPercentInfo m_piFloatPercent;
    bool m_bSetPos; // 防止SetPos循环调用
    bool m_IsNcHit = false;

    bool m_bCanRefreshSelf;
    bool m_bSafeDrawBkImage = false;

    bool m_bResourceText;
    CDuiString m_sText;
    CDuiString m_sDisableText;
    CDuiString m_sDisableMap;
    CDuiString m_sToolTip;
    CDuiString m_sTextAll;
    CDuiString m_sToolTipAll;
    CDuiString m_sTipValue;
    CDuiString m_sTipValueMap;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;
    bool usePtFont = false;
    int fontSize = 10;
    bool fontBold = false;
    bool fontUnderline = false;
    FontType fontType = FontType::Text;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    DWORD m_dwBackColor4;
    DWORD m_dwForeColor;
    CDuiString m_sBkImage;
    CDuiString m_sForeImage;
    DWORD m_dwBorderColor;
    DWORD m_dwFocusBorderColor;
    bool m_bColorHSL;
    int m_nBorderSize;
    int m_nBorderStyle;
    int m_nTooltipWidth;
    WORD m_wCursor;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
    RECT m_rcBorderSize;
    HINSTANCE m_instance;
    int m_nOpacity = 100;
    int m_nBlur = 0;
    int m_nBackgroundRound = 0;

    std::map<CDuiString, CDuiString> m_mapUserString;
    bool m_bChangeParentHeight;
    bool m_wave = false;
    int m_waveR = 10;
    CControlUIHelper *m_helper = nullptr;
    bool mouseMsg = false;
    int m_bkRound = 0;
    bool m_toUpper = false;
};
} // namespace DuiLib

#endif // __UICONTROL_H__
