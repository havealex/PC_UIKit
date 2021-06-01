/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "../Debug/LogOut.h"

namespace DuiLib {
constexpr int CHAR_LENGTH = 500; // 用于字符长度
constexpr int RADIX_TYPE_ONE_VALUE = 10; // set radix type one value 10
constexpr int RADIX_TYPE_TWO_VALUE = 16; // set radix type two value 16

CDuiString ParseUserValue(LPCTSTR pstrValue);

CDialogBuilder::CDialogBuilder() : m_pCallback(nullptr), m_pstrtype(nullptr)
{
    m_instance = nullptr;
}

CControlUI *CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback *pCallback,
    CPaintManagerUI *pManager, CControlUI *pParent)
{
    // 资源ID为0-65535，两个字节；字符串指针为4个字节
    // 字符串以<开头认为是XML字符串，否则认为是XML文件
    if (((DWORD_PTR(xml.m_lpstr)) > 0xFFFF) && *(xml.m_lpstr) != _T('<')) {
        LPCTSTR lpstrXML = CResourceManager::GetInstance()->GetXmlPath(xml.m_lpstr);
        if (lpstrXML != nullptr) {
            xml = lpstrXML;
        }
    }

    if ((DWORD_PTR(xml.m_lpstr)) > 0xFFFF) {
        if (*(xml.m_lpstr) == _T('<')) {
            if (!m_xml.Load(xml.m_lpstr)) {
                LOGGER_ERROR(L"Load xml failed " << xml.m_lpstr);
                return nullptr;
            }
        } else {
            if (!m_xml.LoadFromFile(xml.m_lpstr)) {
                LOGGER_ERROR(L"Load xml failed " << xml.m_lpstr);
                return nullptr;
            }
        }
    } else {
        HINSTANCE dllInstence = nullptr;
        if (m_instance) {
            dllInstence = m_instance;
        } else {
            dllInstence = CPaintManagerUI::GetResourceDll();
        }
        HRSRC hResource = ::FindResource(dllInstence, xml.m_lpstr, type);
        if (hResource == nullptr) {
            LOGGER_ERROR("hResource is nullptr!");
            return nullptr;
        }
        HGLOBAL hGlobal = ::LoadResource(dllInstence, hResource);
        if (hGlobal == nullptr) {
            FreeResource(hResource);
            LOGGER_ERROR("hGlobal is nullptr!");
            return nullptr;
        }

        m_pCallback = pCallback;
        if (!m_xml.LoadFromMem(reinterpret_cast<BYTE *>(::LockResource(hGlobal)),
            ::SizeofResource(dllInstence, hResource))) {
            LOGGER_ERROR("LoadFromMem failed!");
            return nullptr;
        }
        ::FreeResource(hResource);
        m_pstrtype = type;
    }
    return Create(pCallback, pManager, pParent);
}

CControlUI *CDialogBuilder::Create(IDialogBuilderCallback *pCallback, CPaintManagerUI *pManager, CControlUI *pParent)
{
    m_pCallback = pCallback;
    CMarkupNode root = m_xml.GetRoot();
    if (!root.IsValid()) {
        return nullptr;
    }

    if (pManager) {
        LPCTSTR pstrClass = nullptr;
        int nAttributes = 0;
        LPCTSTR pstrName = nullptr;
        LPCTSTR pstrValue = nullptr;
        LPTSTR pstr = nullptr;
        for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling()) {
            pstrClass = node.GetName();
            if (_tcsicmp(pstrClass, _T("Image")) == 0) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pImageName = nullptr;
                LPCTSTR pImageResType = nullptr;
                bool shared = false;
                DWORD mask = 0;
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                    pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                    if (_tcsicmp(pstrName, _T("name")) == 0) {
                        pImageName = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("restype")) == 0) {
                        pImageResType = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("mask")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        mask = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                    } else if (_tcsicmp(pstrName, _T("shared")) == 0) {
                        shared = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                }
                if (pImageName) {
                    pManager->AddImage(pImageName, pImageResType, mask, false, shared);
                }
            } else if (_tcsicmp(pstrClass, _T("UserValueList")) == 0) {
                nAttributes = node.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    CPaintManagerUI::AddUserValue(pstrName, pstrValue);
                }
            } else if (_tcsicmp(pstrClass, _T("Font")) == 0) {
                nAttributes = node.GetAttributeCount();
                int id = -1;
                LPCTSTR pFontName = nullptr;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;
                bool shared = false;
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                    pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                    if (_tcsicmp(pstrName, _T("id")) == 0) {
                        id = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                    } else if (_tcsicmp(pstrName, _T("name")) == 0) {
                        pFontName = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("size")) == 0) {
                        size = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                    } else if (_tcsicmp(pstrName, _T("bold")) == 0) {
                        bold = (_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("underline")) == 0) {
                        underline = (_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("italic")) == 0) {
                        italic = (_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("default")) == 0) {
                        defaultfont = (_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("shared")) == 0) {
                        shared = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                }
                if (id >= 0) {
                    if (!pFontName) {
                        return nullptr;
                    }
                    pManager->AddFont(id, pFontName, size, bold, underline, italic, shared);
                    if (defaultfont) {
                        pManager->SetDefaultFont(pFontName, CResourceManager::GetInstance()->Scale(size), bold,
                            underline, italic, shared);
                    }
                }
            } else if (_tcsicmp(pstrClass, _T("Default")) == 0) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pControlName = nullptr;
                LPCTSTR pControlValue = nullptr;
                bool shared = false;
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                    pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                    if (_tcsicmp(pstrName, _T("name")) == 0) {
                        pControlName = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("value")) == 0) {
                        pControlValue = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("shared")) == 0) {
                        shared = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                }
                if (pControlName) {
                    pManager->AddDefaultAttributeList(pControlName, pControlValue, shared);
                }
            } else if (_tcsicmp(pstrClass, _T("Style")) == 0) {
                nAttributes = node.GetAttributeCount();
                LPCTSTR pName = nullptr;
                LPCTSTR pStyle = nullptr;
                bool shared = false;
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                    pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                    if (_tcsicmp(pstrName, _T("name")) == 0) {
                        pName = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("value")) == 0) {
                        pStyle = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("shared")) == 0) {
                        shared = (_tcsicmp(pstrValue, _T("true")) == 0);
                    }
                }
                if (pName) {
                    pManager->AddStyle(pName, pStyle, shared);
                }
            }
        }

        pstrClass = root.GetName();
        if (_tcsicmp(pstrClass, _T("Window")) == 0) {
            if (pManager->GetPaintWindow()) {
                int nAttributes = root.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                    pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                    if (_tcsicmp(pstrName, _T("size")) == 0) {
                        LPTSTR pstr = nullptr;
                        int cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetInitSize(CResourceManager::GetInstance()->Scale(cx),
                            CResourceManager::GetInstance()->Scale(cy));
                        pManager->SetOrgiSize(cx, cy);
                    } else if (_tcsicmp(pstrName, _T("sizebox")) == 0) {
                        RECT rcSizeBox = { 0 };
                        LPTSTR pstr = nullptr;
                        rcSizeBox.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcSizeBox.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcSizeBox.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetSizeBox(rcSizeBox);
                    } else if (_tcsicmp(pstrName, _T("caption")) == 0) {
                        RECT rcCaption = { 0 };
                        LPTSTR pstr = nullptr;
                        rcCaption.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCaption.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCaption.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCaption.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetCaptionRect(rcCaption);
                    } else if (_tcsicmp(pstrName, _T("roundcorner")) == 0) {
                        LPTSTR pstr = nullptr;
                        int cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetRoundCorner(cx, cy);
                    } else if (_tcsicmp(pstrName, _T("mininfo")) == 0) {
                        LPTSTR pstr = nullptr;
                        int cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetMinInfo(cx, cy);
                    } else if (_tcsicmp(pstrName, _T("maxinfo")) == 0) {
                        LPTSTR pstr = nullptr;
                        int cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->SetMaxInfo(cx, cy);
                    } else if (_tcsicmp(pstrName, _T("showdirty")) == 0) {
                        pManager->SetShowUpdateRect(_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("opacity")) == 0 || _tcsicmp(pstrName, _T("alpha")) == 0) {
                        pManager->SetOpacity(_ttoi(pstrValue));
                    } else if (_tcscmp(pstrName, _T("layeredopacity")) == 0) {
                        pManager->SetLayeredOpacity(_ttoi(pstrValue));
                    } else if (_tcscmp(pstrName, _T("layered")) == 0 || _tcscmp(pstrName, _T("bktrans")) == 0) {
                        pManager->SetLayered(_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("disabledfontcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->SetDefaultDisabledColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("defaultfontcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->SetDefaultFontColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("linkfontcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->SetDefaultLinkFontColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("linkhoverfontcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->SetDefaultLinkHoverFontColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("selectedcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->SetDefaultSelectedBkColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("shadowsize")) == 0) {
                        pManager->GetShadow()->SetSize(_ttoi(pstrValue));
                    } else if (_tcsicmp(pstrName, _T("shadowsharpness")) == 0) {
                        pManager->GetShadow()->SetSharpness(_ttoi(pstrValue));
                    } else if (_tcsicmp(pstrName, _T("shadowdarkness")) == 0) {
                        pManager->GetShadow()->SetDarkness(_ttoi(pstrValue));
                    } else if (_tcsicmp(pstrName, _T("shadowposition")) == 0) {
                        LPTSTR pstr = nullptr;
                        int cx = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        int cy = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->GetShadow()->SetPosition(cx, cy);
                    } else if (_tcsicmp(pstrName, _T("shadowcolor")) == 0) {
                        if (*pstrValue == _T('#')) {
                            pstrValue = ::CharNext(pstrValue);
                        }
                        LPTSTR pstr = nullptr;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_TYPE_TWO_VALUE);
                        pManager->GetShadow()->SetColor(clrColor);
                    } else if (_tcsicmp(pstrName, _T("shadowcorner")) == 0) {
                        RECT rcCorner = { 0 };
                        LPTSTR pstr = nullptr;
                        rcCorner.left = _tcstol(pstrValue, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCorner.top = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCorner.right = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        rcCorner.bottom = _tcstol(pstr + 1, &pstr, RADIX_TYPE_ONE_VALUE);
                        ASSERT(pstr);
                        pManager->GetShadow()->SetShadowCorner(rcCorner);
                    } else if (_tcsicmp(pstrName, _T("shadowimage")) == 0) {
                        pManager->GetShadow()->SetImage(pstrValue);
                    } else if (_tcsicmp(pstrName, _T("showshadow")) == 0) {
                        pManager->GetShadow()->ShowShadow(_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("gdiplustext")) == 0) {
                        pManager->SetUseGdiplusText(_tcsicmp(pstrValue, _T("true")) == 0);
                    } else if (_tcsicmp(pstrName, _T("textrenderinghint")) == 0) {
                        pManager->SetGdiplusTextRenderingHint(_ttoi(pstrValue));
                    } else if (_tcsicmp(pstrName, _T("blur")) == 0) {
                        if (_tcsicmp(pstrValue, _T("true")) == 0) {
                            pManager->SetLayered(true);
                            pManager->EnableBlurRect();
                        }
                    } else if (_tcsicmp(pstrName, _T("acrylikblur")) == 0) {
                        if (_tcsicmp(pstrValue, _T("true")) == 0) {
                            pManager->SetBlurMode(pManager->GetPaintWindow(), ACCENT_ENABLE_ACRYLICBLURBEHIND,
                                0xCCFFFFFF);
                        }
                    }
                }
            }
        }
    }
    return _Parse(&root, pParent, pManager);
}

CMarkup *CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

CControlUI *CDialogBuilder::_Parse(CMarkupNode *pRoot, CControlUI *pParent, CPaintManagerUI *pManager)
{
    IContainerUI *pContainer = nullptr;
    CControlUI *pReturn = nullptr;
    if (pRoot == nullptr) {
        return pReturn;
    }
    for (CMarkupNode node = pRoot->GetChild(); node.IsValid(); node = node.GetSibling()) {
        LPCTSTR pstrClass = node.GetName();
        if (_tcsicmp(pstrClass, _T("Image")) == 0 || _tcsicmp(pstrClass, _T("Font")) == 0 ||
            _tcsicmp(pstrClass, _T("Default")) == 0 || _tcsicmp(pstrClass, _T("Style")) == 0) {
            continue;
        }

        CControlUI *pControl = nullptr;
        if (_tcsicmp(pstrClass, _T("Include")) == 0) {
            if (!node.HasAttributes()) {
                continue;
            }
            int count = 1;
            LPTSTR pstr = nullptr;
            TCHAR szValue[CHAR_LENGTH] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            if (node.GetAttributeValue(_T("count"), szValue, cchLen)) {
                count = _tcstol(szValue, &pstr, RADIX_TYPE_ONE_VALUE);
            }
            cchLen = lengthof(szValue) - 1;
            if (!node.GetAttributeValue(_T("source"), szValue, cchLen)) {
                continue;
            }
            for (int i = 0; i < count; i++) {
                CDialogBuilder builder;
                if (m_pstrtype != nullptr) { // 使用资源dll，从资源中读取
                    WORD id = (WORD)_tcstol(szValue, &pstr, RADIX_TYPE_ONE_VALUE);
                    pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
                } else {
                    pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
                }
            }
            continue;
        } else {
            CDuiString strClass = _T("C");
            strClass = strClass + pstrClass + _T("UI");
            pControl = dynamic_cast<CControlUI *>(CControlFactory::GetInstance()->CreateControl(strClass));

            // 检查插件
            if (pControl == nullptr) {
                CStdPtrArray *pPlugins = CPaintManagerUI::GetPlugins();
                LPCREATECONTROL lpCreateControl = nullptr;
                for (int i = 0; i < pPlugins->GetSize(); ++i) {
                    lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
                    if (lpCreateControl != nullptr) {
                        pControl = lpCreateControl(pstrClass);
                        if (pControl != nullptr) {
                            break;
                        }
                    }
                }
            }
            // 回掉创建
            if (pControl == nullptr && m_pCallback != nullptr) {
                pControl = m_pCallback->CreateControl(pstrClass);
            }
        }

        if (pControl == nullptr) {
            DUITRACE(_T("未知控件:%s"), pstrClass);
            continue;
        }

        // Add children
        if (node.HasChildren()) {
            _Parse(&node, pControl, pManager);
        }
        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
        CTreeViewUI *pTreeView = nullptr;
        if (pParent != nullptr && pControl != nullptr) {
            CTreeNodeUI *pParentTreeNode = static_cast<CTreeNodeUI *>(pParent->GetInterface(_T("TreeNode")));
            CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(_T("TreeNode")));
            pTreeView = static_cast<CTreeViewUI *>(pParent->GetInterface(_T("TreeView")));
            // TreeNode子节点
            if (pTreeNode != nullptr) {
                if (pParentTreeNode) {
                    pTreeView = pParentTreeNode->GetTreeView();
                    if (!pParentTreeNode->Add(pTreeNode)) {
                        delete pTreeNode;
                        pTreeNode = nullptr;
                        continue;
                    }
                } else {
                    if (pTreeView != nullptr) {
                        if (!pTreeView->Add(pTreeNode)) {
                            delete pTreeNode;
                            pTreeNode = nullptr;
                            continue;
                        }
                    }
                }
            } else if (pParentTreeNode != nullptr) { // TreeNode子控件
                pParentTreeNode->GetTreeNodeHoriznotal()->Add(pControl);
            } else { // 普通控件
                if (pContainer == nullptr) {
                    pContainer = static_cast<IContainerUI *>(pParent->GetInterface(_T("IContainer")));
                }
                ASSERT(pContainer);
                if (pContainer == nullptr) {
                    return nullptr;
                }
                if (!pContainer->Add(pControl)) {
                    delete pControl;
                    continue;
                }
            }
        }
        // Init default attributes
        if (pManager) {
            if (pTreeView != nullptr) {
                pControl->SetManager(pManager, pTreeView, true);
            } else {
                pControl->SetManager(pManager, nullptr, false);
            }
            LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
            if (pDefaultAttributes) {
                pControl->ApplyAttributeList(pDefaultAttributes);
            }
        }
        // Process attributes
        if (node.HasAttributes()) {
            TCHAR szValue[CHAR_LENGTH] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            // Set ordinary attributes
            int nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++) {
                auto pstrValue = node.GetAttributeValue(i);
                auto userValue = ParseUserValue(pstrValue);
                CDuiString userValueGet = CPaintManagerUI::GetUserValue(decltype(pstrValue)(userValue));
                pstrValue = !userValue.IsEmpty() ? userValueGet.GetData() : pstrValue;
                auto bindValue = UICulture::GetInstance()->ParseBind(pstrValue);
                pstrValue = !bindValue.IsEmpty() ? decltype(pstrValue)(bindValue) : pstrValue;
                pControl->SetAttribute(node.GetAttributeName(i), pstrValue);
            }
        }
        if (pManager) {
            if (pTreeView == nullptr) {
                pControl->SetManager(nullptr, nullptr, false);
            }
        }
        // Return first item
        if (pReturn == nullptr) {
            pReturn = pControl;
        }
    }
    return pReturn;
}

static CDuiString ParseUserValue(LPCTSTR pstrValue)
{
    const auto key = _T("uservalue");
    const auto len = _tcslen(key);
    auto pcur = pstrValue;
    SkipWhitespace(pcur);
    if (_tcsncicmp(pcur, key, len) == 0) {
        pcur += len;
        SkipWhitespace(pcur);
        if (*pcur++ != _T('=')) {
            return pstrValue;
        }
        SkipWhitespace(pcur);
        if (*pcur++ != _T('\'')) {
            return pstrValue;
        }

        auto count = 0;
        auto pstrTemp = pcur;
        while (*pstrTemp != _T('\0') && *pstrTemp != _T('\'')) {
            pstrTemp = ::CharNext(pstrTemp);
            ++count;
        }

        // get locale name by icu4c
        *const_cast<LPTSTR>(pstrTemp) = '\0';
        return pcur;
    }

    return CDuiString();
}
} // namespace DuiLib
