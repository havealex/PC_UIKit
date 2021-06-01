/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "../Debug/LogOut.h"
#include "UIPainter.h"
#include "UIManager.h"
#include <ppltasks.h>

#define TIMERID_GENERATE_WALLPAPER 0x14396
#define TIMERID_UPDATE_WALLPAPER 0x14397
#define TIMERID_UPDATE_WAVE 0x14398
namespace DuiLib {
constexpr int RADIX_TYPE_ZERO_VALUE = 8; // set radix type one value 8
constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10
constexpr int RADIX_TYPE_TWO_VALUE = 16; // set radix type two value 16
constexpr int TOOL_TIP_WIDTH_VALUE = 160; // set m_nTooltipWidth value 160

IMPLEMENT_DUICONTROL(CControlUI)

#define UI_VERIFY_PTR(ptr) \
    if (ptr == nullptr) { \
        return; \
    }
#define WHETHER_RETURN(expression) {if (expression) {return;}}

class CControlUIHelper : public IMessageFilterUI {
public:
    using BlurWallpaperTask = Concurrency::task<Gdiplus::Bitmap *>;

    CControlUI::BackgroundEffect backgroundEffect = CControlUI::BackgroundEffect::None;
    std::vector<CDuiString> blurControlList = {};
    Gdiplus::Bitmap *bitmapWallpaper = nullptr;
    BlurWallpaperTask taskBlurWallpaper = BlurWallpaperTask([]() { return (Gdiplus::Bitmap *)nullptr; });
    CControlUI *self = nullptr;
    bool hasNewBlurWallpaper = false;
    CDuiString m_sDisabledImage = {};
    DWORD disableBkColor = 0;

    DWORD disableBorderColor = 0;
    DWORD borderFillColor = 0;
    DWORD disableBorderFillColor = 0;
    SIZE borderRoundSize = {};

    void GenerateWallpaperBlurDelay()
    {
        const int nElapse = 500;
        if (!self) {
            return;
        }

        self->SetTimer(TIMERID_GENERATE_WALLPAPER, nElapse);
    }

    void SaveBlurWallpaper()
    {
        if (!hasNewBlurWallpaper) {
            return;
        }

        Gdiplus::Bitmap *bitmapWallpaperTemp = taskBlurWallpaper.get();
        if (bitmapWallpaperTemp) {
            if (bitmapWallpaper) {
                delete bitmapWallpaper;
            }

            bitmapWallpaper = bitmapWallpaperTemp;
        }

        hasNewBlurWallpaper = false;
    }

    void GenerateWallpaperBlur()
    {
        SaveBlurWallpaper();

        taskBlurWallpaper.wait();

        taskBlurWallpaper = BlurWallpaperTask([=]() {
            TCHAR filePath[MAX_PATH];
            SecureZeroMemory(filePath, MAX_PATH);
            BOOL ret = SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, filePath, NULL);
            if (ret == FALSE) {
                return (Gdiplus::Bitmap *)nullptr;
            }

            std::wstring wfilePath = filePath;

            if (!std::experimental::filesystem::exists(wfilePath)) {
                return (Gdiplus::Bitmap *)nullptr;
            }

            Gdiplus::Bitmap *orgBitmap = Gdiplus::Bitmap::FromFile(wfilePath.c_str());
            if (!orgBitmap) {
                return (Gdiplus::Bitmap *)nullptr;
            }

            if (orgBitmap->GetLastStatus() != Gdiplus::Ok) {
                delete orgBitmap;
                return (Gdiplus::Bitmap *)nullptr;
            }

            SIZE sz = Utils::GetScreenSize();
            int w = sz.cx;
            int h = sz.cy;

            Gdiplus::Bitmap *scaledBitmap = Utils::ScaleBitmap(orgBitmap, true, w, h);
            delete orgBitmap;
            orgBitmap = nullptr;

            if (!scaledBitmap) {
                return (Gdiplus::Bitmap *)nullptr;
            }

            if (scaledBitmap->GetLastStatus() != Gdiplus::Ok) {
                delete scaledBitmap;
                return (Gdiplus::Bitmap *)nullptr;
            }

            const int blurValue = 9;
            const int brightness = 22;
            Utils::BlurBitmap(scaledBitmap, blurValue);
            Utils::BrightnessBitmap(scaledBitmap, brightness);

            Gdiplus::Bitmap *resultBitmap = Utils::CopyBitmap(scaledBitmap);
            delete scaledBitmap;
            scaledBitmap = nullptr;

            if (!resultBitmap) {
                return (Gdiplus::Bitmap *)nullptr;
            }

            if (resultBitmap->GetLastStatus() != Gdiplus::Ok) {
                delete resultBitmap;
                return (Gdiplus::Bitmap *)nullptr;
            }
            if (this != nullptr) {
                hasNewBlurWallpaper = true;
            }
            return resultBitmap;
        });

        const int nElapse = 2000;
        if (self) {
            self->SetTimer(TIMERID_UPDATE_WALLPAPER, nElapse);
        }
    }

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override
    {
        switch (uMsg) {
            case WM_SETTINGCHANGE:
                if (wParam == SPI_SETDESKWALLPAPER) {
                    GenerateWallpaperBlurDelay();
                }
                break;
            case WM_EXITSIZEMOVE:
                if (self) {
                    self->Invalidate();
                }
                break;
            default:
                break;
        }

        return 0;
    }
};

CControlUI::CControlUI()
    : m_pManager(nullptr),
      m_pParent(nullptr),
      m_bUpdateNeeded(true),
      m_bMenuUsed(false),
      m_bVisible(true),
      m_bInternVisible(true),
      m_bFocused(false),
      m_bEnabled(true),
      m_bMouseEnabled(true),
      m_bKeyboardEnabled(true),
      m_bFloat(false),
      m_bSetPos(false),
      m_bDragEnabled(false),
      m_bDropEnabled(false),
      m_bResourceText(false),
      m_chShortcut('\0'),
      m_pTag(NULL),
      m_dwBackColor(0),
      m_dwBackColor2(0),
      m_dwBackColor3(0),
      m_dwBackColor4(0),
      m_dwForeColor(0),
      m_dwBorderColor(0),
      m_dwFocusBorderColor(0),
      m_bColorHSL(false),
      m_nBorderSize(0),
      m_nBorderStyle(PS_SOLID),
      m_nTooltipWidth(TOOL_TIP_WIDTH_VALUE),
      m_wCursor(0),
      m_instance(nullptr),
      m_bCanRefreshSelf(false),
      m_bChangeParentHeight(false),
      m_helper(new CControlUIHelper)
{
    const int xyMaxValue = 999999;
    if (m_helper != nullptr) {
        m_helper->self = this;
    }
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = xyMaxValue;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
    ::ZeroMemory(&m_rcBorderSize, sizeof(RECT));
    m_piFloatPercent.left = m_piFloatPercent.top = m_piFloatPercent.right = m_piFloatPercent.bottom = 0.0f;
}

CControlUI::~CControlUI()
{
    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetName().GetData()) {
            temp = GetName().GetData();
        }
        LOGGER_DEBUG(L"[DUILIB]: Cannot uninit control outside ui thread." << temp);
    }

    if (m_pManager) {
        m_pManager->CaptureMouse(this, false);
    }
    if (OnDestroy) {
        OnDestroy(this);
    }
    if (m_pManager != nullptr) {
        m_pManager->ReapObjects(this);
    }

    if (m_helper) {
        if (auto pm = GetManager()) {
            pm->RemoveMessageFilter(m_helper);
        }

        m_helper->SaveBlurWallpaper();

        if (m_helper->bitmapWallpaper) {
            delete m_helper->bitmapWallpaper;
            m_helper->bitmapWallpaper = nullptr;
        }

        delete m_helper;
        m_helper = nullptr;
    }
}

UIImage *CControlUI::GrabImage(const SIZE &sz, const POINT &pt)
{
    UniqueHBitmap grabBitmap = GrabHBITMAP(sz, pt);
    std::unique_ptr<UIImage> pGrabImage = std::make_unique<UIImage>(Utils::CreateBitmapFromHBITMAP(grabBitmap));
    return pGrabImage.release();
}

HBITMAP CControlUI::GrabHBITMAP(const SIZE &sz, const POINT &pt)
{
    SIZE grabSize = sz;

    if (grabSize.cx == 0) {
        grabSize.cx = m_rcItem.right - m_rcItem.left;
    }

    if (grabSize.cy == 0) {
        grabSize.cy = m_rcItem.bottom - m_rcItem.top;
    }

    if ((grabSize.cx == 0) || (grabSize.cy == 0)) {
        return nullptr;
    }

    CPaintManagerUI *pm = GetManager();
    UniqueHDC grabDC(pm ? CreateCompatibleDC(pm->GetPaintDC()) : CreateDC(_T("DISPLAY"), nullptr, nullptr, nullptr));
    if (grabDC == nullptr) {
        return nullptr;
    }

    BYTE *pbytes = nullptr;
    HBITMAP grabHBitmap(CRenderEngine::CreateARGB32Bitmap(grabDC, grabSize.cx, grabSize.cy, &pbytes));
    if (grabHBitmap == nullptr) {
        return nullptr;
    }

    if (!pbytes) {
        return nullptr;
    }

    HBITMAP oldBitmap = (HBITMAP)SelectObject(grabDC, grabHBitmap);

    RECT rcItem = m_rcItem;
    RECT rcPos = GetPos();
    bool visible = m_bVisible;

    if (!visible) {
        SetVisible(true);
    }

    m_rcItem = RECT { -pt.x, -pt.y, pt.x + grabSize.cx, pt.y + grabSize.cy };
    SetPos(m_rcItem, false);

    m_bGrabbing = true;
    UniqueBoolean grabValGuard(m_bGrabbing, false);
    int savedRenderHint = Gdiplus::TextRenderingHintSystemDefault;
    if (pm) {
        savedRenderHint = pm->GetGdiplusTextRenderingHint();
        pm->SetGdiplusTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    }
    DoPaint(grabDC, m_rcItem);
    m_rcItem = rcItem;
    SetPos(rcPos, false);
    if (visible != m_bVisible) {
        SetVisible(visible);
    }
    if (pm) {
        pm->SetGdiplusTextRenderingHint(savedRenderHint);
    }

    if (oldBitmap != nullptr) {
        SelectObject(grabDC, oldBitmap);
    }

    return grabHBitmap;
}


bool CControlUI::IsGrabbing() const
{
    return m_bGrabbing;
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
    m_sName = pstrName;
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_CONTROL) == 0) {
        return this;
    }
    return nullptr;
}

void CControlUI::SetControlFlags(UINT nCtrlFlags)
{
    m_nControlFlags = nCtrlFlags;
}

LPCTSTR CControlUI::GetClass() const
{
    return _T("ControlUI");
}

UINT CControlUI::GetControlFlags() const
{
    return m_nControlFlags;
}

bool CControlUI::Activate()
{
    if (!IsVisible()) {
        return false;
    }
    if (!IsEnabled()) {
        return false;
    }
    return true;
}

CPaintManagerUI *CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = pParent;
    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetName().GetData()) {
            temp = GetName().GetData();
        }
        LOGGER_DEBUG(L"[DUILIB]: Cannot init control outside ui thread. " < temp);
    }
    if (bInit && m_pParent) {
        Init();
    }

    if (pManager != nullptr && m_helper != nullptr) {
        if (m_helper->backgroundEffect == BackgroundEffect::BlurWallpaper) {
            pManager->AddMessageFilter(m_helper);
        }
    }
}

CControlUI *CControlUI::GetParent() const
{
    return m_pParent;
}

bool CControlUI::SetTimer(UINT nTimerID, UINT nElapse)
{
    if (m_pManager == nullptr) {
        return false;
    }

    return m_pManager->SetTimer(this, nTimerID, nElapse);
}

void CControlUI::KillTimer(UINT nTimerID)
{
    if (m_pManager == nullptr) {
        return;
    }

    m_pManager->KillTimer(this, nTimerID);
}

CDuiString CControlUI::GetText() const
{
    if (!IsResourceText()) {
        return SetTextToUpper(m_sText);
    }

    return SetTextToUpper(CResourceManager::GetInstance()->GetText(m_sText));
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if (!CPaintManagerUI::IsMainThread()) {
        LOGGER_DEBUG("[DUILIB]Cannot invoke CControlUI::SetText() outside ui thread");
    }
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString text(pstrText);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    if (valueList.size() == 2) { // 确认容器的大小
        if (m_sText == valueList[0]) {
            return;                // 获取容器的第一个元素
        }
        m_sText = valueList[0];    // 获取容器的第一个元素
        m_sTextAll = valueList[1]; // 获取容器的第二个元素
    } else {
#endif
        if (m_sText == pstrText) {
            return;
        }
        m_sText = pstrText;
#ifdef SWITCH_LANGUAGE_TEST
    }
#endif
    Invalidate();
}

void CControlUI::SetDisableText(LPCTSTR pstrText)
{
    if (!CPaintManagerUI::IsMainThread()) {
        LOGGER_DEBUG("[DUILIB]Cannot invoke CControlUI::SetText() outside ui thread");
    }

#ifdef SWITCH_LANGUAGE_TEST
    CDuiString text(pstrText);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    const int containValue = 2;
    if (valueList.size() == containValue) { // 确认容器的大小
        if (m_sDisableText == valueList[0]) {
            return;                    // 获取容器的第一个元素
        }
        m_sDisableText = valueList[0]; // 获取容器的第一个元素
        m_sDisableMap = valueList[1];  // 获取容器的第二个元素
    } else {
#endif
        if (m_sDisableText == pstrText) {
            return;
        }
        m_sDisableText = pstrText;
#ifdef SWITCH_LANGUAGE_TEST
    }
#endif
    Invalidate();
}


DuiLib::CDuiString CControlUI::GetDisableText() const
{
    return m_sDisableText;
}

bool CControlUI::IsResourceText() const
{
    return m_bResourceText;
}

void CControlUI::SetResourceText(bool bResource)
{
    if (m_bResourceText == bResource) {
        return;
    }
    m_bResourceText = bResource;
    Invalidate();
}

bool CControlUI::IsDragEnabled() const
{
    return m_bDragEnabled;
}

void CControlUI::SetDragEnable(bool bDrag)
{
    m_bDragEnabled = bDrag;
}

bool CControlUI::IsDropEnabled() const
{
    return m_bDropEnabled;
}

void CControlUI::SetDropEnable(bool bDrop)
{
    m_bDropEnabled = bDrop;
}

void CControlUI::OnDragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINT pt, DWORD *pdwEffect)
{
    if (pdwEffect == nullptr) {
        return;
    }
    if (IsDropEnabled()) {
        *pdwEffect = DROPEFFECT_COPY;
    } else {
        *pdwEffect = DROPEFFECT_NONE;
        if (m_pParent != nullptr) {
            m_pParent->OnDragEnter(pDataObj, grfKeyState, pt, pdwEffect);
        }
    }
}

void CControlUI::OnDragOver(DWORD grfKeyState, POINT pt, DWORD *pdwEffect)
{
    if (pdwEffect == nullptr) {
        return;
    }
    if (IsDropEnabled()) {
        *pdwEffect = DROPEFFECT_COPY;
    } else {
        *pdwEffect = DROPEFFECT_NONE;
        if (m_pParent != nullptr) {
            m_pParent->OnDragOver(grfKeyState, pt, pdwEffect);
        }
    }
}

void CControlUI::OnDragLeave()
{
    if (IsDropEnabled()) {
    } else {
        if (m_pParent != nullptr) {
            m_pParent->OnDragLeave();
        }
    }
}

void CControlUI::OnDrop(IDataObject *pDataObj, DWORD grfKeyState, POINT pt, DWORD *pdwEffect)
{
    if (pdwEffect == nullptr) {
        return;
    }
    if (IsDropEnabled()) {
        *pdwEffect = DROPEFFECT_COPY;
        // 这块数据pDataObj界面要处理
        TEventUI event;
        event.wParam = (WPARAM)(pDataObj);
        event.pSender = this;
        if (m_pManager != nullptr) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_DRAGDROP, event.wParam);
        }
    } else {
        *pdwEffect = DROPEFFECT_NONE;
        if (m_pParent != nullptr) {
            m_pParent->OnDrop(pDataObj, grfKeyState, pt, pdwEffect);
        }
    }
}

void CControlUI::SetEnsureTextVisible(bool ensureTextVisible)
{
    m_ensureTextVisible = ensureTextVisible;
}

bool CControlUI::IsEnsureTextVisible() const
{
    return m_ensureTextVisible;
}

DWORD CControlUI::GetBkColor() const
{
    return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
    if (m_dwBackColor == dwBackColor) {
        return;
    }

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if (m_dwBackColor2 == dwBackColor) {
        return;
    }

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if (m_dwBackColor3 == dwBackColor) {
        return;
    }

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}
DWORD CControlUI::GetBkColor4() const
{
    return m_dwBackColor4;
}
void CControlUI::SetBkColor4(DWORD dwBackColor)
{
    if (m_dwBackColor4 = dwBackColor) {
        return;
    }
    m_dwBackColor4 = dwBackColor;
    Invalidate();
}
DWORD CControlUI::GetForeColor() const
{
    return m_dwForeColor;
}

void CControlUI::SetForeColor(DWORD dwForeColor)
{
    if (m_dwForeColor == dwForeColor) {
        return;
    }

    m_dwForeColor = dwForeColor;
    Invalidate();
}

LPCTSTR CControlUI::GetBkImage()
{
    return m_sBkImage;
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
    if (!pStrImage) {
        return;
    }

    if (!CPaintManagerUI::IsMainThread()) {
        LOGGER_DEBUG("[DUILIB]Cannot invoke CControlUI::SetBkImage() outside ui thread");
    }

    if (!m_bCanRefreshSelf && m_sBkImage == pStrImage) {
        return;
    }
    if (m_pManager) {
        m_pManager->RemoveImage(pStrImage); // Need release old image
    }

    m_sBkImage = pStrImage;
    Invalidate();
}

LPCTSTR CControlUI::GetForeImage() const
{
    return m_sForeImage;
}

void CControlUI::SetForeImage(LPCTSTR pStrImage)
{
    if (!CPaintManagerUI::IsMainThread()) {
        LOGGER_DEBUG("[DUILIB]Cannot invoke CControlUI::SetForeImage() outside ui thread");
    }

    if (!m_bCanRefreshSelf && m_sForeImage == pStrImage) {
        return;
    }

    m_sForeImage = pStrImage;
    Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if (m_dwBorderColor == dwBorderColor) {
        return;
    }

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
    if (m_dwFocusBorderColor == dwBorderColor) {
        return;
    }

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if (m_bColorHSL == bColorHSL) {
        return;
    }

    m_bColorHSL = bColorHSL;
    Invalidate();
}

int CControlUI::GetBorderSize() const
{
    return CResourceManager::GetInstance()->Scale(m_nBorderSize);
}

void CControlUI::SetBorderSize(int nSize)
{
    if (m_nBorderSize == nSize) {
        return;
    }

    m_nBorderSize = nSize;
    Invalidate();
}

void CControlUI::SetBorderSize(RECT rc)
{
    m_rcBorderSize = rc;
    Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyBorderRound);
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    return CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify, m_instance,
        !m_bSafeDrawBkImage);
}

const RECT &CControlUI::GetPos() const
{
    return m_bPosCached ? m_rcCachedPos : m_rcItem;
}

RECT CControlUI::GetRelativePos() const
{
    CControlUI *pParent = GetParent();
    if (pParent != nullptr) {
        RECT rcParentPos = pParent->GetPos();
        CDuiRect rcRelativePos(m_rcItem);
        rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
        return rcRelativePos;
    } else {
        return CDuiRect(0, 0, 0, 0);
    }
}

RECT CControlUI::GetClientPos() const
{
    return m_rcItem;
}

RECT CControlUI::SetDirectionPos(RECT rc)
{
    bool directionRTL = false;
    switch (UICulture::GetInstance()->getDirection()) {
        case DIRECTION_RTL:
            directionRTL = true;
        default:
            break;
    }
    CControlUI *pParent = GetParent();
    if (pParent == nullptr) {
        return RECT { 0 };
    }
    RECT rcParentPos = pParent->GetPos();
    RECT rcCtrl;
    CDuiString tmp = pParent->GetName();
    if (directionRTL) {
        rcCtrl = { rcParentPos.right - rc.right, rcParentPos.top + rc.top, rcParentPos.right - rc.left,
            rcParentPos.top + rc.bottom };
    } else {
        rcCtrl = { rcParentPos.left + rc.left, rcParentPos.top + rc.top, rcParentPos.left + rc.right,
            rcParentPos.top + rc.bottom };
    }
    return rcCtrl;
}
void CControlUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetName().GetData()) {
            temp = GetName().GetData();
        }
        LOGGER_DEBUG(L"[DUILIB]Cannot invoke CControlUI::SetPos() outside ui thread " << temp);
    }

    m_bPosCached = false;

    if (rc.right < rc.left) {
        rc.right = rc.left;
    }
    if (rc.bottom < rc.top) {
        rc.bottom = rc.top;
    }

    CDuiRect invalidateRc = m_rcItem;
    if (::IsRectEmpty(&invalidateRc)) {
        invalidateRc = rc;
    }

    if (m_bFloat) {
        CControlUI *pParent = GetParent();
        if (pParent != nullptr) {
            RECT rcCtrl = SetDirectionPos(rc);
            m_rcItem = rcCtrl;
        }
    } else {
        m_rcItem = rc;
    }
    if (m_pManager == nullptr) {
        return;
    }

    if (!m_bSetPos) {
        m_bSetPos = true;
        if (OnSize) {
            OnSize(this);
        }
        m_bSetPos = false;
    }

    m_bUpdateNeeded = false;

    if (bNeedInvalidate && IsVisible()) {
        invalidateRc.Join(m_rcItem);
        CControlUI *pParent = this;
        RECT rcTemp;
        RECT rcParent;
        while (pParent = pParent->GetParent()) {
            if (!pParent->IsVisible()) {
                return;
            }
            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();
            if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) {
                return;
            }
        }
        m_pManager->Invalidate(invalidateRc);
    }
}

void CControlUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    m_cXY.cx += szOffset.cx;
    m_cXY.cy += szOffset.cy;
    SetPos(m_rcItem, bNeedInvalidate);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
    return CResourceManager::GetInstance()->Scale(m_rcPadding);
}

void CControlUI::SetPadding(RECT rcPadding)
{
    if (UICulture::GetInstance()->getDirection() == DIRECTION_RTL) {
        int tmp = rcPadding.left;
        rcPadding.left = rcPadding.right;
        rcPadding.right = tmp;
    }
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
    return CResourceManager::GetInstance()->Scale(m_cXY);
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

int CControlUI::GetFixedWidth() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyFixed.cx);
}


void CControlUI::SetFixedWidthEx(int cx)
{
    if (cx < 0) {
        return;
    }

    m_cxyFixed.cx = CResourceManager::GetInstance()->RestoreScale(cx);
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

void CControlUI::SetFixedWidth(int cx)
{
    if (cx < 0) {
        return;
    }
    m_cxyFixed.cx = cx;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

int CControlUI::GetFixedHeight() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyFixed.cy);
}

void CControlUI::SetFixedHeightEx(int cy)
{
    if (cy < 0) {
        return;
    }

    m_cxyFixed.cy = CResourceManager::GetInstance()->RestoreScale(cy);
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

void CControlUI::SetFixedHeight(int cy)
{
    if (cy < 0) {
        return;
    }
    m_cxyFixed.cy = cy;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

int CControlUI::GetMinWidth() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyMin.cx);
}

void CControlUI::SetMinWidth(int cx)
{
    if (m_cxyMin.cx == cx) {
        return;
    }

    if (cx < 0) {
        return;
    }
    m_cxyMin.cx = cx;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

int CControlUI::GetMaxWidth() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyMax.cx);
}

void CControlUI::SetMaxWidth(int cx)
{
    if (m_cxyMax.cx == cx) {
        return;
    }

    if (cx < 0) {
        return;
    }
    m_cxyMax.cx = cx;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

int CControlUI::GetMinHeight() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyMin.cy);
}

void CControlUI::SetMinHeight(int cy)
{
    if (m_cxyMin.cy == cy) {
        return;
    }

    if (cy < 0) {
        return;
    }
    m_cxyMin.cy = cy;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

void CControlUI::SetBkRound(int round)
{
    if (m_bkRound == round) {
        return;
    } else {
        m_bkRound = round;
    }
}

void CControlUI::SetToUpper(bool enToUpper)
{
    if (m_toUpper == enToUpper) {
        return;
    } else {
        m_toUpper = enToUpper;
    }
}

CDuiString CControlUI::SetTextToUpper(const CDuiString &pstrText) const
{
    if (!m_toUpper) {
        return pstrText;
    }
    if (pstrText.IsEmpty()) {
        return pstrText;
    }
    std::wstring str = pstrText;
    std::wstring upperStr = UICulture::GetInstance()->ToUpper(str);
    return upperStr.c_str();
}

int CControlUI::GetMaxHeight() const
{
    return CResourceManager::GetInstance()->Scale(m_cxyMax.cy);
}

void CControlUI::SetMaxHeight(int cy)
{
    if (m_cxyMax.cy == cy) {
        return;
    }

    if (cy < 0) {
        return;
    }
    m_cxyMax.cy = cy;
    if (!m_bFloat) {
        NeedParentUpdate();
    } else {
        NeedUpdate();
    }
}

TPercentInfo CControlUI::GetFloatPercent() const
{
    return m_piFloatPercent;
}

void CControlUI::SetFloatPercent(TPercentInfo piFloatPercent)
{
    m_piFloatPercent = piFloatPercent;
    NeedParentUpdate();
}


CDuiString CControlUI::GetToolTip() const
{
    if (!IsResourceText()) {
        return m_sToolTip;
    }
    return CResourceManager::GetInstance()->GetText(m_sToolTip);
}

void CControlUI::SetTipValue(LPCTSTR pStrTipValue)
{
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString text(pStrTipValue);
    vector<DuiLib::CDuiString> valueList = text.Split(L"@UI@");
    if (valueList.size() == 2) {
        m_sTipValue = valueList[0].GetData();
        m_sTipValueMap = valueList[1].GetData();
    } else {
#endif
        m_sTipValue = pStrTipValue;
#ifdef SWITCH_LANGUAGE_TEST
    }
#endif
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
    CDuiString strTemp(pstrText);
#ifdef SWITCH_LANGUAGE_TEST
    vector<DuiLib::CDuiString> valueList = strTemp.Split(L"@UI@");
    if (valueList.size() == 2) { // 确认容器的大小
        strTemp = valueList[0]; // 获取容器的第一个元素
        m_sToolTipAll = valueList[1]; // 获取容器的第二个元素
    }
#endif
    strTemp.Replace(_T("<n>"), _T("\r\n"));
    m_sToolTip = strTemp;
}

void CControlUI::SetToolTipWidth(int nWidth)
{
    m_nTooltipWidth = nWidth;
}

int CControlUI::GetToolTipWidth(void)
{
    return CResourceManager::GetInstance()->Scale(m_nTooltipWidth);
}

WORD CControlUI::GetCursor()
{
    return m_wCursor;
}

void CControlUI::SetCursor(WORD wCursor)
{
    m_wCursor = wCursor;
    SetControlFlags(GetControlFlags() | UIFLAG_SETCURSOR);
    Invalidate();
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString &CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool CControlUI::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if (m_bVisible == bVisible) {
        return;
    }

    bool v = IsVisible();
    m_bVisible = bVisible;
    if (m_bFocused) {
        m_bFocused = false;
    }
    if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
        m_pManager->SetFocus(nullptr);
    }
    if (IsVisible() != v) {
        NeedParentUpdate();
    }
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
    if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
        m_pManager->SetFocus(nullptr);
    }
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if (m_bEnabled == bEnabled) {
        return;
    }

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
    return m_bKeyboardEnabled;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
    m_bKeyboardEnabled = bEnabled;
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if (m_pManager != nullptr) {
        m_pManager->SetFocus(this);
    }
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if (m_bFloat == bFloat) {
        return;
    }

    m_bFloat = bFloat;
    NeedParentUpdate();
}

CControlUI *CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) {
        return nullptr;
    }
    if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) {
        return nullptr;
    }
    if ((uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, *static_cast<LPPOINT>(pData)))) {
        return nullptr;
    }
    return Proc(this, pData);
}

void CControlUI::Invalidate()
{
    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetName().GetData()) {
            temp = GetName().GetData();
        }
        LOGGER_ERROR(L"[DUILIB]Cannot invoke CControlUI::Invalidate() outside ui thread " << temp);
    }

    if (!IsVisible()) {
        return;
    }

    RECT invalidateRc = m_rcItem;

    CControlUI *pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while (pParent = pParent->GetParent()) {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) {
            return;
        }
    }

    if (m_pManager != nullptr) {
        m_pManager->Invalidate(invalidateRc);
    }
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if (!CPaintManagerUI::IsMainThread()) {
        wstring temp;
        if (GetName().GetData()) {
            temp = GetName().GetData();
        }
    }

    if (!IsVisible()) {
        return;
    }
    m_bUpdateNeeded = true;
    Invalidate();

    if (m_pManager != nullptr) {
        m_pManager->NeedUpdate();
    }
}

void CControlUI::NeedParentUpdate()
{
    if (GetParent()) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    } else {
        NeedUpdate();
    }

    if (m_pManager != nullptr) {
        m_pManager->NeedUpdate();
    }
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    if (!m_bColorHSL) {
        return dwColor;
    }
    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    CPaintManagerUI *pm = GetManager();
    if (usePtFont && pm) {
        m_iFont = pm->AddPtFont(fontType, fontSize, fontBold, fontUnderline);
    }

    DoInit();
    if (OnInit) {
        OnInit(this);
    }
}

void CControlUI::DoInit() {}

void CControlUI::SetFont(int index)
{
    usePtFont = false;
    m_iFont = index;
    Invalidate();
}

int CControlUI::GetFont() const
{
    return m_iFont;
}

void CControlUI::SetPtFont(int index, bool bold, bool underline, FontType ft)
{
    usePtFont = true;
    fontBold = bold;
    fontUnderline = underline;
    fontType = ft;
    fontSize = index;

    CPaintManagerUI *pm = GetManager();
    if (pm) {
        m_iFont = pm->AddPtFont(fontType, fontSize, fontBold, underline);
    }
}

void CControlUI::CachePos(const RECT &rc)
{
    m_bPosCached = true;
    m_rcCachedPos = rc;
}

void CControlUI::Event(TEventUI &event)
{
    // 对所有的控件事件分流
    auto upchain = this;
    while (upchain != nullptr) {
        event.pSender = upchain;
        if (upchain->OnEvent(&event)) { // 被捕获后就不向上传递
            break;
        }
        upchain = upchain->m_pParent; // 向父节点循环遍历
    }
    event.pSender = this;
    if (event.Type == UIEVENT_TIMER) {
        if (event.wParam == TIMERID_GENERATE_WALLPAPER) {
            KillTimer(TIMERID_GENERATE_WALLPAPER);
            if (m_helper != nullptr) {
                m_helper->GenerateWallpaperBlur();
            }
        } else if (event.wParam == TIMERID_UPDATE_WALLPAPER) {
            KillTimer(TIMERID_UPDATE_WALLPAPER);
            if (m_helper != nullptr) {
                m_helper->SaveBlurWallpaper();
            }
            Invalidate();
        } else if (event.wParam == TIMERID_UPDATE_WAVE) {
            if (IsVisible()) {
                Invalidate();
            }
        }
    }

    DoEvent(event);
}

void CControlUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_SETCURSOR) {
        if (GetCursor()) {
            ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(GetCursor())));
        } else {
            ::SetCursor(::LoadCursor(nullptr, MAKEINTRESOURCE(IDC_ARROW)));
        }
        return;
    }

    if (event.Type == UIEVENT_SETFOCUS) {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_KILLFOCUS) {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if (event.Type == UIEVENT_TIMER) {
        if (m_pManager) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        }
        return;
    }
    if (event.Type == UIEVENT_CONTEXTMENU) {
        if (IsContextMenuUsed() && m_pManager) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if (event.Type == UIEVENT_MOUSEENTER) {
        if (m_pManager != nullptr && mouseMsg) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSEENTER, event.wParam, event.lParam);
        }
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        if (m_pManager != nullptr && mouseMsg) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MOUSELEAVE, event.wParam, event.lParam);
        }
    }
    if (event.Type == UIEVENT_BUTTONUP) {
        if (m_pManager != nullptr) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_LBTN_UP, event.wParam, event.lParam);
        }
    }
    if (m_pParent != nullptr) {
        m_pParent->DoEvent(event);
    }
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
    m_sVirtualWnd = pstrValue;
    if (m_pManager) {
        m_pManager->UsedVirtualWnd(true);
    }
}

CDuiString CControlUI::GetVirtualWnd() const
{
    CDuiString str;
    if (!m_sVirtualWnd.IsEmpty()) {
        str = m_sVirtualWnd;
    } else {
        CControlUI *pParent = GetParent();
        if (pParent != nullptr) {
            str = pParent->GetVirtualWnd();
        } else {
            str = _T("");
        }
    }
    return str;
}
void CControlUI::SetDirectionPadding(LPCTSTR pstrValue)
{
    RECT rcPadding = { 0 };
    LPTSTR pstr = nullptr;
    rcPadding.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
    UI_VERIFY_PTR(pstr);
    rcPadding.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
    UI_VERIFY_PTR(pstr);
    rcPadding.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
    UI_VERIFY_PTR(pstr);
    rcPadding.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
    UI_VERIFY_PTR(pstr);
    SetPadding(rcPadding);
}

void CControlUI::SetCurAttr(LPCTSTR pstrValue)
{
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrValue, _T("arrow")) == 0) {
        SetCursor(DUI_ARROW);
    } else if (_tcsicmp(pstrValue, _T("ibeam")) == 0) {
        SetCursor(DUI_IBEAM);
    } else if (_tcsicmp(pstrValue, _T("wait")) == 0) {
        SetCursor(DUI_WAIT);
    } else if (_tcsicmp(pstrValue, _T("cross")) == 0) {
        SetCursor(DUI_CROSS);
    } else if (_tcsicmp(pstrValue, _T("uparrow")) == 0) {
        SetCursor(DUI_UPARROW);
    } else if (_tcsicmp(pstrValue, _T("size")) == 0) {
        SetCursor(DUI_SIZE);
    } else if (_tcsicmp(pstrValue, _T("icon")) == 0) {
        SetCursor(DUI_ICON);
    } else if (_tcsicmp(pstrValue, _T("sizenwse")) == 0) {
        SetCursor(DUI_SIZENWSE);
    } else if (_tcsicmp(pstrValue, _T("sizenesw")) == 0) {
        SetCursor(DUI_SIZENESW);
    } else if (_tcsicmp(pstrValue, _T("sizewe")) == 0) {
        SetCursor(DUI_SIZEWE);
    } else if (_tcsicmp(pstrValue, _T("sizens")) == 0) {
        SetCursor(DUI_SIZENS);
    } else if (_tcsicmp(pstrValue, _T("sizeall")) == 0) {
        SetCursor(DUI_SIZEALL);
    } else if (_tcsicmp(pstrValue, _T("no")) == 0) {
        SetCursor(DUI_NO);
    } else if (_tcsicmp(pstrValue, _T("hand")) == 0) {
        SetCursor(DUI_HAND);
    }
}
bool CControlUI::GetNcHit()
{
    return m_IsNcHit;
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }

    // 是否样式表
    if (m_pManager != nullptr) {
        LPCTSTR pStyle = m_pManager->GetStyle(pstrValue);
        if (pStyle != nullptr) {
            ApplyAttributeList(pStyle);
            return;
        }
    }
    if (_tcsicmp(pstrName, _T("pos")) == 0) {
        RECT rcPos = { 0 };
        LPTSTR pstr = nullptr;
        rcPos.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        rcPos.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        LONG cvt = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        rcPos.right = (cvt == 0 ? (rcPos.left + m_cxyFixed.cx) : cvt);
        UI_VERIFY_PTR(pstr);
        cvt = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        rcPos.bottom = (cvt == 0 ? (rcPos.top + m_cxyFixed.cy) : cvt);
        UI_VERIFY_PTR(pstr);
        SIZE szXY = { rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom };
        SetFixedXY(szXY);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    } else if (_tcsicmp(pstrName, _T("float")) == 0) {
        CDuiString nValue = pstrValue;
        // 动态计算相对比例
        if (nValue.Find(',') < 0) {
            SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
        } else {
            TPercentInfo piFloatPercent = { 0 };
            LPTSTR pstr = nullptr;
            piFloatPercent.left = _tcstod(pstrValue, &pstr);
            UI_VERIFY_PTR(pstr);
            piFloatPercent.top = _tcstod(pstr + 1, &pstr);
            UI_VERIFY_PTR(pstr);
            piFloatPercent.right = _tcstod(pstr + 1, &pstr);
            UI_VERIFY_PTR(pstr);
            piFloatPercent.bottom = _tcstod(pstr + 1, &pstr);
            UI_VERIFY_PTR(pstr);
            SetFloatPercent(piFloatPercent);
            SetFloat(true);
        }
    } else if (_tcsicmp(pstrName, _T("padding")) == 0) {
        SetDirectionPadding(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bkcolor")) == 0 || _tcsicmp(pstrName, _T("bkcolor1")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetBkColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("disablebkcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            UI_VERIFY_PTR(m_helper);
            m_helper->disableBkColor = clrColor;
        }
    } else if (_tcsicmp(pstrName, _T("borderfillcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            UI_VERIFY_PTR(m_helper);
            m_helper->borderFillColor = clrColor;
        }
    } else if (_tcsicmp(pstrName, _T("disableborderfillcolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            UI_VERIFY_PTR(m_helper);
            m_helper->disableBorderFillColor = clrColor;
        }
    } else if (_tcsicmp(pstrName, _T("disablebordercolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            UI_VERIFY_PTR(m_helper);
            m_helper->disableBorderColor = clrColor;
        }
    } else if (_tcsicmp(pstrName, _T("bkcolor2")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetBkColor2(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("bkcolor3")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetBkColor3(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("bkcolor4")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetBkColor4(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("forecolor")) == 0) {
        while (pstrValue != nullptr && *pstrValue > _T('\0') && *pstrValue <= _T(' ')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetForeColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("bordercolor")) == 0) {
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetBorderColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("focusbordercolor")) == 0) {
        if (pstrValue != nullptr && *pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        if (pstrValue != nullptr) {
            LPTSTR pstr = nullptr;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
            SetFocusBorderColor(clrColor);
        }

    } else if (_tcsicmp(pstrName, _T("colorhsl")) == 0) {
        SetColorHSL(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("bordersize")) == 0) {
        CDuiString nValue = pstrValue;
        if (nValue.Find(',') < 0) {
            SetBorderSize(_ttoi(pstrValue));
            RECT rcPadding = { 0 };
            SetBorderSize(rcPadding);
        } else {
            RECT rcPadding = { 0 };
            LPTSTR pstr = nullptr;
            rcPadding.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
            UI_VERIFY_PTR(pstr);
            rcPadding.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
            UI_VERIFY_PTR(pstr);
            rcPadding.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
            UI_VERIFY_PTR(pstr);
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
            UI_VERIFY_PTR(pstr);
            SetBorderSize(rcPadding);
        }
    } else if (_tcsicmp(pstrName, _T("leftbordersize")) == 0) {
        SetLeftBorderSize(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("topbordersize")) == 0) {
        SetTopBorderSize(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("rightbordersize")) == 0) {
        SetRightBorderSize(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("bottombordersize")) == 0) {
        SetBottomBorderSize(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("borderstyle")) == 0) {
        SetBorderStyle(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("disabledimage")) == 0) {
        UI_VERIFY_PTR(m_helper);
        m_helper->m_sDisabledImage = pstrValue;
    } else if (_tcsicmp(pstrName, _T("borderround")) == 0) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = nullptr;
        cxyRound.cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        cxyRound.cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        SetBorderRound(cxyRound);
    } else if (_tcsicmp(pstrName, _T("borderrounded")) == 0) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = nullptr;
        cxyRound.cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        cxyRound.cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
        UI_VERIFY_PTR(pstr);
        UI_VERIFY_PTR(m_helper);
        m_helper->borderRoundSize = cxyRound;
    } else if (_tcsicmp(pstrName, _T("mousemsg")) == 0) {
        mouseMsg = (_tcsicmp(pstrValue, _T("true")) == 0);
    } else {
        SetAttribute2(pstrName, pstrValue);
    }
}

void CControlUI::SetAttribute2(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("bkimage")) == 0) {
        SetBkImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("backgroundeffect")) == 0) {
        std::wstring val = pstrValue;
        if (val == _T("blurcontrol")) {
            SetBackgroundEffect(BackgroundEffect::BlurControl);
        } else if (val == _T("blurwallpaper")) {
            SetBackgroundEffect(BackgroundEffect::BlurWallpaper);
        }
    } else if (_tcsicmp(pstrName, _T("wave")) == 0) {
        SetWave(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("blurcontrol")) == 0) {
        AddBlurControl(pstrValue);
    } else if (_tcsicmp(pstrName, _T("foreimage")) == 0) {
        SetForeImage(pstrValue);
    } else if (_tcsicmp(pstrName, _T("width")) == 0) {
        SetFixedWidth(_ttoi(pstrValue));
        SetMinWidth(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("height")) == 0) {
        SetFixedHeight(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("opacity")) == 0) {
        SetOpacity(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("blur")) == 0) {
        SetBlur(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("safebkimage")) == 0) {
        std::wstring s = pstrValue;
        m_bSafeDrawBkImage = (s == _T("true"));
    } else if (_tcsicmp(pstrName, _T("minwidth")) == 0) {
        SetMinWidth(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("backgroundrounded")) == 0) {
        m_nBackgroundRound = _ttoi(pstrValue);
    } else if (_tcsicmp(pstrName, _T("minheight")) == 0) {
        SetMinHeight(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("maxwidth")) == 0) {
        SetMaxWidth(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("maxheight")) == 0) {
        SetMaxHeight(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("name")) == 0) {
        SetName(pstrValue);
    } else if (_tcsicmp(pstrName, _T("drag")) == 0) {
        SetDragEnable(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("drop")) == 0) {
        SetDropEnable(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("resourcetext")) == 0) {
        SetResourceText(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("text")) == 0) {
        SetText(pstrValue);
    } else if (_tcsicmp(pstrName, _T("disabletext")) == 0) {
        SetDisableText(pstrValue);
    } else if (_tcsicmp(pstrName, _T("tooltip")) == 0) {
        SetToolTip(pstrValue);
    } else if (_tcsicmp(pstrName, _T("tipvalue")) == 0) {
        SetTipValue(pstrValue);
    } else if (_tcsicmp(pstrName, _T("tooltipwidth")) == 0) {
        SetToolTipWidth(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("userdata")) == 0) {
        SetUserData(pstrValue);
    } else if (_tcsicmp(pstrName, _T("enabled")) == 0) {
        SetEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("ensuretextvisible")) == 0) {
        SetEnsureTextVisible(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("mouse")) == 0) {
        SetMouseEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("keyboard")) == 0) {
        SetKeyboardEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("visible")) == 0) {
        SetVisible(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("float")) == 0) {
        SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("shortcut")) == 0) {
        SetShortcut(pstrValue[0]);
    } else if (_tcsicmp(pstrName, _T("menu")) == 0) {
        SetContextMenuUsed(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("cursor")) == 0 && pstrValue) {
        SetCurAttr(pstrValue);
    } else if (_tcsicmp(pstrName, _T("IsNcHit")) == 0) {
        m_IsNcHit = _tcsicmp(pstrValue, _T("true")) == 0;
    } else if (_tcsicmp(pstrName, _T("virtualwnd")) == 0) {
        SetVirtualWnd(pstrValue);
    } else if (_tcsicmp(pstrName, _T("innerstyle")) == 0) {
        CDuiString sXmlData = pstrValue;
        sXmlData.Replace(_T("&quot;"), _T("\""));
        LPCTSTR pstrList = sXmlData.GetData();
        CDuiString sItem;
        CDuiString sValue;
        while (*pstrList != _T('\0')) {
            sItem.Empty();
            sValue.Empty();
            while (*pstrList != _T('\0') && *pstrList != _T('=')) {
                LPTSTR pstrTemp = ::CharNext(pstrList);
                while (pstrList < pstrTemp) {
                    sItem += *pstrList++;
                }
            }
            WHETHER_RETURN(*pstrList != _T('='));
            if (*pstrList++ != _T('=')) {
                return;
            }
            WHETHER_RETURN(*pstrList != _T('\"'));
            if (*pstrList++ != _T('\"')) {
                return;
            }
            while (*pstrList != _T('\0') && *pstrList != _T('\"')) {
                LPTSTR pstrTemp = ::CharNext(pstrList);
                while (pstrList < pstrTemp) {
                    sValue += *pstrList++;
                }
            }
            WHETHER_RETURN(*pstrList != _T('\"'));
            if (*pstrList++ != _T('\"')) {
                return;
            }
            SetAttribute(sItem, sValue);
            if (*pstrList++ != _T(' ') && *pstrList++ != _T(',')) {
                return;
            }
        }
    } else if (_tcsicmp(pstrName, _T("autochangeparentheight")) == 0) {
        SetAutoChangeParentHeight(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("font_family")) == 0) {
        do {
            if (!pstrValue) {
                break;
            }

            DuiLib::CDuiString strVals = pstrValue;
            std::vector<CDuiString> strValList = strVals.Split(_T(","));
            if (strValList.size() == 0) {
                break;
            }

            int fontPt = -1;
            bool bold = false;
            bool underline = false;
            FontType ft = FontType::Text;

            for (int i = 0; i < strValList.size(); ++i) {
                DuiLib::CDuiString strVal = strValList[i];
                strVal.MakeLower();

                int _fontPt = _ttoi(strVal.GetData());
                if (_fontPt != 0) {
                    fontPt = _fontPt;
                }

                if (strVal == _T("bold")) {
                    bold = true;
                }

                if (strVal == _T("underline")) {
                    underline = true;
                }

                if (strVal == _T("text")) {
                    ft = FontType::Text;
                }

                if (strVal == _T("value")) {
                    ft = FontType::Value;
                }
            }

            if (fontPt != -1) {
                SetPtFont(fontPt, bold, underline, ft);
            }
        } while (0);
    } else if (_tcsicmp(pstrName, _T("virtualvnd")) == 0) {
        SetVirtualWnd(pstrValue);
    } else if (_tcsicmp(pstrName, _T("bkround")) == 0) {
        SetBkRound(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("toupper")) == 0) {
        SetToUpper(_tcsicmp(pstrValue, _T("true")) == 0);
    }
}

CControlUI *CControlUI::ApplyAttributeList(LPCTSTR pstrValue)
{
    // 解析样式表
    if (m_pManager != nullptr) {
        LPCTSTR pStyle = m_pManager->GetStyle(pstrValue);
        if (pStyle != nullptr) {
            return ApplyAttributeList(pStyle);
        }
    }
    CDuiString sXmlData = pstrValue;
    sXmlData.Replace(_T("&quot;"), _T("\""));
    LPCTSTR pstrList = sXmlData.GetData();
    // 解析样式属性
    CDuiString sItem;
    CDuiString sValue;
    while (*pstrList != _T('\0')) {
        sItem.Empty();
        sValue.Empty();
        while (*pstrList != _T('\0') && *pstrList != _T('=')) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while (pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }
        if (*pstrList != _T('=')) {
            return nullptr;
        }

        if (*pstrList++ != _T('=')) {
            return this;
        }
        if (*pstrList != _T('\"')) {
            return nullptr;
        }
        if (*pstrList++ != _T('\"')) {
            return this;
        }
        while (*pstrList != _T('\0') && *pstrList != _T('\"')) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while (pstrList < pstrTemp) {
                sValue += *pstrList++;
            }
        }
        if (*pstrList != _T('\"')) {
            return nullptr;
        }
        if (*pstrList++ != _T('\"')) {
            return this;
        }
        SetAttribute(sItem, sValue);
        if (*pstrList++ != _T(' ') && *pstrList++ != _T(',')) {
            return this;
        }
    }
    return this;
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return CResourceManager::GetInstance()->Scale(m_cxyFixed);
}

void CControlUI::DoPaint(HDC hDC, const RECT &rcPaint)
{
    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) {
        return;
    }

    if (m_nOpacity == 0) {
        return;
    }

    // 绘制循序：背景颜色->背景图->状态图->文本->边框
    SIZE cxyBorderRound = CResourceManager::GetInstance()->Scale(m_cxyBorderRound);
    RECT rcBorderSize = CResourceManager::GetInstance()->Scale(m_rcBorderSize);

    HDC proxyDC = hDC;

    HBITMAP grabHBitmap = nullptr;
    HBITMAP oldBitmap = nullptr;
    RECT rcItem = m_rcItem;
    RECT rcPos = GetPos();
    RECT savedRcPaint = rcPaint;
    bool visible = m_bVisible;
    bool bfloat = m_bFloat;

    int savedRenderHint = Gdiplus::TextRenderingHintSystemDefault;
    if (GetManager()) {
        savedRenderHint = GetManager()->GetGdiplusTextRenderingHint();
    }

    const int rcMul = 2;
    const int minMul = 3;
    const int iWaveValue = 80;
    const int iAddoffValue = 6;
    const int colorOffset = 24;
    const int maxOpacitValue = 100;
    if (m_wave) {
        if (IsVisible() == false) {
            return;
        }
        PaintBkColor(hDC);
        Gdiplus::Graphics graphics(hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Rect rc = DuiLib::Utils::FromWin32RECT(GetPos());
        int rMin = min(rc.Height, rc.Width) / rcMul;
        int rMax = max(rc.Height, rc.Width) / rcMul; // 最大的半径
        int span = rMin / minMul;


        int waveSpace = CResourceManager::GetInstance()->Scale(iWaveValue);
        int addoffSpace = CResourceManager::GetInstance()->Scale(iAddoffValue);

        if (m_waveR > rMax) {
            m_waveR = m_waveR - waveSpace;
        }

        DWORD color = 0xffc3e1ff;
        int currentRadius = m_waveR;
        while (currentRadius > 0) {
            double rate = currentRadius / (static_cast<double>(rMin) + span);
            double colorRate =
                currentRadius > rMin * rcMul ? 0 : currentRadius / (static_cast<double>(rMin * rcMul));

            int colorAlpha = ((color & 0xff000000) >> colorOffset);
            int colorOpacity = static_cast<int>(colorAlpha * colorRate);
            unsigned int actualColorAlpha = colorAlpha - colorOpacity;
            double d = colorAlpha > 0 ? actualColorAlpha / static_cast<double>(colorAlpha) : 0;
            actualColorAlpha = static_cast<unsigned int>(d * d * colorAlpha);
            DWORD gdiColor = (actualColorAlpha << colorOffset) | (color & 0x00ffffff);

            Gdiplus::SolidBrush sb(gdiColor);
            Gdiplus::Pen pen(0xffc3e1ff);
            Gdiplus::Rect rcEllipse(rc.GetLeft() + rc.Width / rcMul - currentRadius,
                rc.GetTop() + rc.Height / rcMul - currentRadius, currentRadius * rcMul, currentRadius * rcMul);
            graphics.FillEllipse(&sb, rcEllipse);
            graphics.DrawEllipse(&pen, rcEllipse);

            currentRadius = currentRadius - waveSpace;
        }

        m_waveR += addoffSpace;
        return;
    }
    if (m_nOpacity < maxOpacitValue || m_nBlur > 0) {
        do {
            proxyDC = CreateCompatibleDC(hDC);
            if (proxyDC == nullptr) {
                proxyDC = hDC;
                break;
            }

            int width = m_rcItem.right - m_rcItem.left;
            int height = m_rcItem.bottom - m_rcItem.top;
            BYTE *pbytes = nullptr;
            grabHBitmap = CRenderEngine::CreateARGB32Bitmap(proxyDC, width, height, &pbytes);
            if (grabHBitmap == nullptr) {
                DeleteDC(proxyDC);
                proxyDC = hDC;
                break;
            }

            if (GetManager()) {
                GetManager()->SetGdiplusTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
            }

            oldBitmap = (HBITMAP)SelectObject(proxyDC, grabHBitmap);
            m_rcItem = RECT { 0, 0, width, height };
            bfloat = m_bFloat;
            m_bFloat = false;
            SetPos(m_rcItem, false);
            m_rcPaint = m_rcItem;
        } while (0);
    }

    if (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0) {
        CRenderClip roundClip;
        CRenderClip::GenerateRoundClip(proxyDC, m_rcPaint, m_rcItem, cxyBorderRound.cx, cxyBorderRound.cy, roundClip);
        UI_VERIFY_PTR(m_helper);
        if (m_helper->backgroundEffect == BackgroundEffect::BlurControl) {
            PaintBlurControl(hDC, rcItem);
        } else if (m_helper->backgroundEffect == BackgroundEffect::BlurWallpaper) {
            PaintBlurWallpaper(hDC, rcItem);
        } else {
            PaintBkColor(proxyDC);
        }

        PaintBkImage(proxyDC);
        PaintStatusImage(proxyDC);
        PaintForeColor(proxyDC);
        PaintForeImage(proxyDC);
        PaintBorder(proxyDC);
        PaintText(proxyDC);
    } else {
        UI_VERIFY_PTR(m_helper);
        if (m_helper->backgroundEffect == BackgroundEffect::BlurControl) {
            PaintBlurControl(hDC, rcItem);
        } else if (m_helper->backgroundEffect == BackgroundEffect::BlurWallpaper) {
            PaintBlurWallpaper(hDC, rcItem);
        } else {
            PaintBkColor(proxyDC);
        }

        PaintBkImage(proxyDC);
        PaintStatusImage(proxyDC);
        PaintForeColor(proxyDC);
        PaintForeImage(proxyDC);
        PaintBorder(proxyDC);
        PaintText(proxyDC);
    }

    if (m_nOpacity < maxOpacitValue || m_nBlur > 0) {
        if (grabHBitmap != nullptr) {
            UIImage image(Utils::CreateBitmapFromHBITMAP(grabHBitmap));
            image.SetOpacity(m_nOpacity);
            image.SetBlur(m_nBlur);
            UIPainter painter(this, hDC);

            painter.DrawImage(rcItem.left, rcItem.top, &image);
        }

        if (GetManager()) {
            GetManager()->SetGdiplusTextRenderingHint(savedRenderHint);
        }

        m_bFloat = bfloat;
        SetPos(rcPos, false);
        m_rcItem = rcItem;
        m_rcPaint = savedRcPaint;

        if (proxyDC != nullptr && oldBitmap != nullptr) {
            SelectObject(proxyDC, oldBitmap);
        }

        if (grabHBitmap != nullptr) {
            DeleteObject(grabHBitmap);
        }

        if (proxyDC != nullptr) {
            DeleteDC(proxyDC);
        }
    }
}


void CControlUI::PaintBlurControl(HDC hDC, RECT &rcItem)
{
    UI_VERIFY_PTR(m_helper);
    if (m_helper->blurControlList.empty()) {
        return;
    }

    UIPainter painter(this, hDC);
    RECT rcBlurBk = rcItem;
    if (m_bFloat) {
        OffsetRect(&rcBlurBk, -rcItem.left, -rcItem.top);
    }

    painter.SetBrush(m_dwBackColor);
    painter.FillRectange(rcBlurBk);

        for each(auto &ctrlName in m_helper->blurControlList) {
                CControlUI *ctrl = GetManager()->FindControl(ctrlName);
                if (!ctrl) {
                    continue;
                }

                RECT rcCtrl = ctrl->GetPos();
                RECT rcIntersectItem = rcItem;
                if (m_bFloat) {
                }

                RECT rcIntersects = { 0 };
                if (IntersectRect(&rcIntersects, &rcCtrl, &rcIntersectItem) == FALSE) {
                    continue;
                }

                int blurValue = 10;

                UIImage *ctrlImage = ctrl->GrabImage(
                    SIZE { rcIntersects.right - rcIntersects.left, rcIntersects.bottom - rcIntersects.top },
                    POINT { rcIntersects.left - rcCtrl.left, rcIntersects.top - rcCtrl.top });

                if (!ctrlImage) {
                    continue;
                }

                ctrlImage->SetOpacity(14);
                ctrlImage->SetBlur(9, true);
                ctrlImage->SetBrightness(60);
                painter.DrawImage(0, 0, ctrlImage);
                delete ctrlImage;
            }
}

void CControlUI::PaintBlurWallpaper(HDC hDC, RECT &rcItem)
{
    UI_VERIFY_PTR(m_helper);
    if (!m_helper->bitmapWallpaper) {
        PaintBkColor(hDC);
        return;
    }

    RECT rcWindow = { 0 };
    if (GetWindowRect(GetManager()->GetPaintWindow(), &rcWindow) == FALSE) {
        return;
    }

    RECT rcDrawWallpapaer { rcWindow.left + rcItem.left, rcWindow.top + rcItem.top,
        rcWindow.left + rcItem.left + rcItem.right - rcItem.left,
        rcWindow.top + rcItem.top + rcItem.bottom - rcItem.top };

    UIPainter painter(this, hDC);
    UIImage image(m_helper->bitmapWallpaper);
    image.SetAutoDestroyResource(false);

    RECT rcBlurBk = rcItem;
    OffsetRect(&rcBlurBk, -rcItem.left - 1, -rcItem.top - 1);
    rcBlurBk.right++;
    rcBlurBk.bottom++;

    painter.SetBrush(m_dwBackColor);
    painter.FillRectange(rcBlurBk);
    const int opatitValue = 18;
    image.SetOpacity(opatitValue);
    painter.DrawImage(0, 0, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top, &image, rcDrawWallpapaer.left,
        rcDrawWallpapaer.top, rcDrawWallpapaer.right - rcDrawWallpapaer.left,
        rcDrawWallpapaer.bottom - rcDrawWallpapaer.top);
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if (m_helper == nullptr) {
        return;
    }
    int round = CResourceManager::GetInstance()->Scale(m_bkRound);
    if (!IsEnabled() && m_helper->disableBkColor) {
        CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_helper->disableBkColor), round);
        return;
    }
    const int rcMulValue = 2;
    if (m_dwBackColor != 0) {
        if (m_dwBackColor2 != 0) {
            if (m_dwBackColor3 != 0) {
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / rcMulValue;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),
                    true, RADIX_TYPE_ZERO_VALUE);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3),
                    true, RADIX_TYPE_ZERO_VALUE);
            } else {
                CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor),
                    GetAdjustColor(m_dwBackColor2), true, RADIX_TYPE_TWO_VALUE);
            }
        } else if (m_dwBackColor4 != 0) {
            CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor4),
                false, RADIX_TYPE_TWO_VALUE);
        } else if (m_dwBackColor >= 0xFF000000) {
            CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), round);
        } else {
            CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor), round);
        }
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
    UI_VERIFY_PTR(m_helper);
    if (!IsEnabled() && !m_helper->m_sDisabledImage.IsEmpty()) {
        DrawImage(hDC, (LPCTSTR)m_helper->m_sDisabledImage);
        return;
    }

    if (!m_sBkImage.IsEmpty()) {
        DrawImage(hDC, (LPCTSTR)m_sBkImage);
    }
}

void CControlUI::PaintStatusImage(HDC hDC) {}

void CControlUI::PaintForeColor(HDC hDC)
{
    CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwForeColor));
}

void CControlUI::PaintForeImage(HDC hDC)
{
    if (m_sForeImage.IsEmpty()) {
        return;
    }
    DrawImage(hDC, (LPCTSTR)m_sForeImage);
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
    UI_VERIFY_PTR(m_helper);
    int nBorderSize = CResourceManager::GetInstance()->Scale(m_nBorderSize);
    SIZE cxyBorderRound = CResourceManager::GetInstance()->Scale(m_cxyBorderRound);
    RECT rcBorderSize = CResourceManager::GetInstance()->Scale(m_rcBorderSize);
    if (m_dwBorderColor != 0 || m_dwFocusBorderColor != 0) {
        DWORD borderColor2Draw = m_helper->disableBorderColor ?
            (IsEnabled() ? m_dwBorderColor : m_helper->disableBorderColor) :
            m_dwBorderColor;
        DWORD borderFillColor2Draw = m_helper->disableBorderFillColor ?
            (IsEnabled() ? m_helper->borderFillColor : m_helper->disableBorderFillColor) :
            m_helper->borderFillColor;
        // 画圆角边框
        // 原来长宽多给一个像素，导致绘制时背景被切掉一个像素。
        RECT rc2Draw = m_rcItem;
        if (rc2Draw.right > rc2Draw.left) {
            rc2Draw.right--;
        }
        if (rc2Draw.bottom > rc2Draw.top) {
            rc2Draw.bottom--;
        }

        if (cxyBorderRound.cx == 0 && cxyBorderRound.cy == 0) {
            cxyBorderRound = DPI_SCALE(m_helper->borderRoundSize);
        }
        if (nBorderSize > 0 && (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0)) {
            if (IsFocused() && m_dwFocusBorderColor != 0) {
                CRenderEngine::DrawRoundRect(hDC, rc2Draw, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy,
                    GetAdjustColor(m_dwFocusBorderColor), GetAdjustColor(borderFillColor2Draw), m_nBorderStyle);
            } else {
                CRenderEngine::DrawRoundRect(hDC, rc2Draw, nBorderSize, cxyBorderRound.cx, cxyBorderRound.cy,
                    GetAdjustColor(borderColor2Draw), GetAdjustColor(borderFillColor2Draw), m_nBorderStyle);
            }
        } else {
            if (IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize > 0) {
                CRenderEngine::DrawRect(hDC, rc2Draw, nBorderSize, GetAdjustColor(m_dwFocusBorderColor),
                    m_nBorderStyle);
            } else if (rcBorderSize.left > 0 || rcBorderSize.top > 0 || rcBorderSize.right > 0 ||
                rcBorderSize.bottom > 0) {
                RECT rcBorder;

                if (rcBorderSize.left > 0) {
                    rcBorder = rc2Draw;
                    rcBorder.right = rcBorder.left;
                    CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.left, GetAdjustColor(m_dwBorderColor),
                        m_nBorderStyle);
                }
                if (rcBorderSize.top > 0) {
                    rcBorder = rc2Draw;
                    rcBorder.bottom = rcBorder.top;
                    CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.top, GetAdjustColor(m_dwBorderColor),
                        m_nBorderStyle);
                }
                if (rcBorderSize.right > 0) {
                    rcBorder = rc2Draw;
                    rcBorder.right -= 1;
                    rcBorder.left = rcBorder.right;
                    CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.right, GetAdjustColor(m_dwBorderColor),
                        m_nBorderStyle);
                }
                if (rcBorderSize.bottom > 0) {
                    rcBorder = rc2Draw;
                    rcBorder.bottom -= 1;
                    rcBorder.top = rcBorder.bottom;
                    CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.bottom, GetAdjustColor(m_dwBorderColor),
                        m_nBorderStyle);
                }
            } else if (nBorderSize > 0) {
                CRenderEngine::DrawRect(hDC, rc2Draw, nBorderSize, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
            }
        }
    }
}

void CControlUI::DoPostPaint(HDC hDC, const RECT &rcPaint)
{
    return;
}

void CControlUI::SetOpacity(int n)
{
    m_nOpacity = n;
    Invalidate();
}

void CControlUI::SetBlur(int n)
{
    m_nBlur = n;
    Invalidate();
}

int CControlUI::GetOpacity() const
{
    return m_nOpacity;
}

int CControlUI::GetBlur() const
{
    return m_nBlur;
}

int CControlUI::GetLeftBorderSize() const
{
    return CResourceManager::GetInstance()->Scale(m_rcBorderSize.left);
}

void CControlUI::SetLeftBorderSize(int nSize)
{
    m_rcBorderSize.left = nSize;
    Invalidate();
}

int CControlUI::GetTopBorderSize() const
{
    return CResourceManager::GetInstance()->Scale(m_rcBorderSize.top);
}

void CControlUI::SetTopBorderSize(int nSize)
{
    m_rcBorderSize.top = nSize;
    Invalidate();
}

int CControlUI::GetRightBorderSize() const
{
    return CResourceManager::GetInstance()->Scale(m_rcBorderSize.right);
}

void CControlUI::SetRightBorderSize(int nSize)
{
    m_rcBorderSize.right = nSize;
    Invalidate();
}

int CControlUI::GetBottomBorderSize() const
{
    return CResourceManager::GetInstance()->Scale(m_rcBorderSize.bottom);
}

void CControlUI::SetBottomBorderSize(int nSize)
{
    m_rcBorderSize.bottom = nSize;
    Invalidate();
}

int CControlUI::GetBorderStyle() const
{
    return m_nBorderStyle;
}

void CControlUI::SetBorderStyle(int nStyle)
{
    m_nBorderStyle = nStyle;
    Invalidate();
}

void CControlUI::SetBorderFilledColor(DWORD borderFilledColor)
{
    if (m_helper != nullptr) {
        m_helper->borderFillColor = borderFilledColor;
    }
}

void CControlUI::SetCanRefreshSelf(bool bCanRefreshSelf)
{
    m_bCanRefreshSelf = bCanRefreshSelf;
}

bool CControlUI::GetCanRefreshSelf()
{
    return m_bCanRefreshSelf;
}

void CControlUI::setUserString(const CDuiString &_key, const CDuiString &_value)
{
    m_mapUserString[_key] = _value;
}

CDuiString CControlUI::getUserString(const CDuiString &_key) const
{
    std::map<CDuiString, CDuiString>::const_iterator iter = m_mapUserString.find(_key);
    if (iter != m_mapUserString.end()) {
        return iter->second;
    }
    return CDuiString();
}

const std::map<CDuiString, CDuiString> &CControlUI::getUserStrings() const
{
    return m_mapUserString;
}

bool CControlUI::clearUserString(const CDuiString &_key)
{
    std::map<CDuiString, CDuiString>::iterator iter = m_mapUserString.find(_key);
    if (iter != m_mapUserString.end()) {
        m_mapUserString.erase(iter);
        return true;
    }
    return false;
}

bool CControlUI::isUserString(const CDuiString &_key) const
{
    return m_mapUserString.find(_key) != m_mapUserString.end();
}

void CControlUI::clearUserStrings()
{
    m_mapUserString.clear();
}
bool CControlUI::GetAutoChangeParentHeight() const
{
    return m_bChangeParentHeight;
}

void CControlUI::SetAutoChangeParentHeight(bool bChangeParentHeight)
{
    m_bChangeParentHeight = bChangeParentHeight;
}


RECT CControlUI::GetFloatRelativePos()
{
    SIZE szXY = GetFixedXY();
    SIZE sz = { GetFixedWidth(), GetFixedHeight() };
    TPercentInfo rcPercent = GetFloatPercent();
    LONG width = m_rcItem.right - m_rcItem.left;
    LONG height = m_rcItem.bottom - m_rcItem.top;
    RECT rcCtrl = { 0 };
    rcCtrl.left = (LONG)(width * rcPercent.left) + szXY.cx;
    rcCtrl.top = (LONG)(height * rcPercent.top) + szXY.cy;
    rcCtrl.right = (LONG)(width * rcPercent.right) + szXY.cx + sz.cx;
    rcCtrl.bottom = (LONG)(height * rcPercent.bottom) + szXY.cy + sz.cy;

    if (GetClass() == _T("ButtonUI") || GetClass() == _T("LabelUI")) {
        SIZE szAvailable = { 0 };
        SIZE szEst = EstimateSize(szAvailable);

        RECT rcCtrlEst = { 0 };
        rcCtrlEst.left = szXY.cx;
        rcCtrlEst.top = szXY.cy;
        rcCtrlEst.right = szXY.cx + szEst.cx;
        rcCtrlEst.bottom = szXY.cy + szEst.cy;

        return rcCtrlEst;
    }
    return rcCtrl;
}

void CControlUI::AddBlurControl(const CDuiString &controlName)
{
    UI_VERIFY_PTR(m_helper);
    m_helper->blurControlList.push_back(controlName);
    m_helper->backgroundEffect = BackgroundEffect::BlurControl;
}

void CControlUI::RemoveBlurControl(const CDuiString &controlName)
{
    UI_VERIFY_PTR(m_helper);
    auto it = std::find(m_helper->blurControlList.begin(), m_helper->blurControlList.end(), controlName);
    if (it != m_helper->blurControlList.end()) {
        m_helper->blurControlList.erase(it);
    }

    if (m_helper->blurControlList.empty() && m_helper->backgroundEffect == BackgroundEffect::BlurWallpaper) {
        m_helper->backgroundEffect = BackgroundEffect::None;
    }
}

void CControlUI::SetBackgroundEffect(BackgroundEffect be)
{
    UI_VERIFY_PTR(m_helper);
    m_helper->backgroundEffect = be;
    if (be == BackgroundEffect::BlurWallpaper) {
        if (auto pm = GetManager()) {
            pm->AddMessageFilter(m_helper);
        }

        m_helper->GenerateWallpaperBlur();
    } else {
        if (auto pm = GetManager()) {
            pm->RemoveMessageFilter(m_helper);
        }
    }
}

void CControlUI::SetWave(bool _val)
{
    const int nElapse = 80;
    if (_val) {
        m_wave = true;
        SetTimer(TIMERID_UPDATE_WAVE, nElapse);
    } else {
        if (m_wave) {
            KillTimer(TIMERID_UPDATE_WAVE);
        }
        m_wave = false;
    }
    Invalidate();
}
void CControlUI::SetAttributeForMap(vector<CDuiString> vecDuiStr, LPCTSTR language, LPCTSTR attribute)
{
#ifdef SWITCH_LANGUAGE_TEST
    CDuiString translatorValue;
    for (int i = 0; i < vecDuiStr.size(); ++i) {
        DuiLib::CDuiString strVal = vecDuiStr[i];
        vector<DuiLib::CDuiString> endlist = strVal.Split(L"&UI&");
        if (endlist.size() == 2) {                     // 确认容器的大小
            if (_tcsicmp(language, endlist[0]) == 0) { // 获取容器第一个元素
                translatorValue = endlist[1];          // 获取容器的第二个元素
                break;
            }
            if (_tcscmp(L"EN-US", endlist[0]) == 0) {
                translatorValue = endlist[1]; // 获取容器的第二个元素
            }
        }
    }
    if (!translatorValue.IsEmpty()) {
        SetAttribute(attribute, translatorValue);
    }
#endif
}

void CControlUI::SetLanguageText(LPCTSTR language)
{
#ifdef SWITCH_LANGUAGE_TEST
    SetAttributeForMap(m_sTextAll.Split(L"$UI$"), language, L"text");
    SetAttributeForMap(m_sToolTipAll.Split(L"$UI$"), language, L"tooltip");
    SetAttributeForMap(m_sTipValueMap.Split(L"$UI$"), language, L"tipvalue");
    SetAttributeForMap(m_sDisableMap.Split(L"$UI$"), language, L"disabletext");
#endif
}
} // namespace DuiLib
