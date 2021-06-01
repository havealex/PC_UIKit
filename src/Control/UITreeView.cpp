/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#include "../StdAfx.h"
#include "UITreeView.h"

#pragma warning(disable : 4251)
namespace DuiLib {
const int RADIX_HEXADECIMAL = 16; // ʮ������

IMPLEMENT_DUICONTROL(CTreeNodeUI)

// ************************************
// ��������: CTreeNodeUI
// ��������:
// ������Ϣ: CTreeNodeUI * _ParentNode
// ����˵��:
// ************************************
CTreeNodeUI::CTreeNodeUI(CTreeNodeUI *_ParentNode)
{
    m_dwItemTextColor = 0x00000000;
    m_dwItemHotTextColor = 0;
    m_dwSelItemTextColor = 0;
    m_dwSelItemHotTextColor = 0;

    pTreeView = nullptr;
    m_iTreeLavel = 0;
    m_bIsVisable = true;
    m_bIsCheckBox = false;
    pParentTreeNode = nullptr;

    pHoriz = new (std::nothrow) CHorizontalLayoutUI();
    pFolderButton = new (std::nothrow) CCheckBoxUI();
    pDottedLine = new (std::nothrow) CLabelUI();
    pCheckBox = new (std::nothrow) CCheckBoxUI();
    pItemButton = new (std::nothrow) COptionUI();
    if (pHoriz == nullptr || pFolderButton == nullptr || pDottedLine == nullptr || pCheckBox == nullptr ||
        pItemButton == nullptr) {
        return;
    }
    const int defaultFixedHeight = 18;
    const int defaultFixedWidth = 250;
    const int dotFixedWidth = 2;
    this->SetFixedHeight(defaultFixedHeight);
    this->SetFixedWidth(defaultFixedWidth);
    pFolderButton->SetFixedWidth(GetFixedHeight());
    pDottedLine->SetFixedWidth(dotFixedWidth);
    pCheckBox->SetFixedWidth(GetFixedHeight());
    pItemButton->SetAttribute(_T("align"), _T("left"));
    pDottedLine->SetVisible(false);
    pCheckBox->SetVisible(false);
    pItemButton->SetMouseEnabled(false);

    const int widthFixedValue = 16;
    if (_ParentNode == nullptr || _ParentNode->GetClass() == nullptr) {
        return;
    }

    if (_tcsicmp(_ParentNode->GetClass(), _T("TreeNodeUI")) != 0) {
        return;
    }
    pDottedLine->SetVisible(_ParentNode->IsVisible());
    pDottedLine->SetFixedWidth(_ParentNode->GetDottedLine()->GetFixedWidth() + widthFixedValue);
    this->SetParentNode(_ParentNode);

    pHoriz->Add(pDottedLine);
    pHoriz->Add(pFolderButton);
    pHoriz->Add(pCheckBox);
    pHoriz->Add(pItemButton);
    Add(pHoriz);
}

// ************************************
// ��������: ~CTreeNodeUI
// ��������:
// ������Ϣ: void
// ����˵��:
// ************************************
CTreeNodeUI::~CTreeNodeUI(void) {}

// ************************************
// ��������: GetClass
// ��������: LPCTSTR
// ����˵��:
// ************************************
LPCTSTR CTreeNodeUI::GetClass() const
{
    return _T("TreeNodeUI");
}

// ************************************
// ��������: GetInterface
// ��������: LPVOID
// ������Ϣ: LPCTSTR pstrName
// ����˵��:
// ************************************
LPVOID CTreeNodeUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("TreeNode")) == 0) {
        return static_cast<CTreeNodeUI *>(this);
    }
    return CListContainerElementUI::GetInterface(pstrName);
}

// ************************************
// ��������: DoEvent
// ��������: void
// ������Ϣ: TEventUI & event
// ����˵��:
// ************************************
void CTreeNodeUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND) {
        if (m_pOwner != nullptr) {
            m_pOwner->DoEvent(event);
        } else {
            CContainerUI::DoEvent(event);
        }
        return;
    }
    CListContainerElementUI::DoEvent(event);
    if (event.Type == UIEVENT_DBLCLICK) {
        if (IsEnabled()) {
            if (m_pManager == nullptr) {
                return;
            }
            m_pManager->SendNotify(this, DUI_MSGTYPE_TREEITEMDBCLICK);
            Invalidate();
        }
        return;
    }
    if (pItemButton == nullptr) {
        return;
    }
    if (event.Type == UIEVENT_MOUSEENTER) {
        if (IsEnabled()) {
            if (m_bSelected && GetSelItemHotTextColor()) {
                pItemButton->SetTextColor(GetSelItemHotTextColor());
            } else {
                pItemButton->SetTextColor(GetItemHotTextColor());
            }
        } else {
            pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());
        }

        return;
    }
    if (event.Type == UIEVENT_MOUSELEAVE) {
        if (IsEnabled()) {
            if (m_bSelected && GetSelItemTextColor()) {
                pItemButton->SetTextColor(GetSelItemTextColor());
            } else if (!m_bSelected) {
                pItemButton->SetTextColor(GetItemTextColor());
            }
        } else {
            pItemButton->SetTextColor(pItemButton->GetDisabledTextColor());
        }

        return;
    }
}

// ************************************
// ��������: Invalidate
// ��������: void
// ����˵��:
// ************************************
void CTreeNodeUI::Invalidate()
{
    if (!IsVisible()) {
        return;
    }

    if (GetParent()) {
        CContainerUI *pParentContainer = static_cast<CContainerUI *>(GetParent()->GetInterface(_T("Container")));
        if (pParentContainer) {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetInset();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI *pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
            if (pVerticalScrollBar && pVerticalScrollBar->IsVisible())
                rc.right -= pVerticalScrollBar->GetFixedWidth();
            CScrollBarUI *pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
            if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) {
                rc.bottom -= pHorizontalScrollBar->GetFixedHeight();
            }

            RECT invalidateRc = m_rcItem;
            if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc)) {
                return;
            }

            CControlUI *pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;
            if (pParent == nullptr) {
                return;
            }
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
        } else {
            CContainerUI::Invalidate();
        }
    } else {
        CContainerUI::Invalidate();
    }
}

// ************************************
// ��������: Select
// ��������: bool
// ������Ϣ: bool bSelect
// ����˵��:
// ************************************
bool CTreeNodeUI::Select(bool bSelect)
{
    bool nRet = CListContainerElementUI::Select(bSelect);
    if (pItemButton == nullptr) {
        return false;
    }
    if (m_bSelected) {
        pItemButton->SetTextColor(GetSelItemTextColor());
    } else {
        pItemButton->SetTextColor(GetItemTextColor());
    }

    return nRet;
}

// ************************************
// ��������: Add
// ��������: bool
// ������Ϣ: CControlUI * _pTreeNodeUI
// ����˵��: ͨ���ڵ������ӽڵ�
// ************************************
bool CTreeNodeUI::Add(CControlUI *_pTreeNodeUI)
{
    if (_pTreeNodeUI == nullptr) {
        return false;
    }
    if (_pTreeNodeUI->GetClass() == nullptr) {
        return false;
    }
    if (_tcsicmp(_pTreeNodeUI->GetClass(), _T("TreeNodeUI")) == 0) {
        return AddChildNode(reinterpret_cast<CTreeNodeUI *>(_pTreeNodeUI));
    }

    return CListContainerElementUI::Add(_pTreeNodeUI);
}

// ************************************
// ��������: AddAt
// ��������: bool
// ������Ϣ: CControlUI * pControl
// ������Ϣ: int iIndex				�ò�������Ե�ǰ�ڵ��µ��ֵ������������б���ͼ����
// ����˵��:
// ************************************
bool CTreeNodeUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (pControl == nullptr || pTreeView == nullptr) {
        return false;
    }
    if (static_cast<CTreeNodeUI *>(pControl->GetInterface(_T("TreeNode"))) == nullptr) {
        return false;
    }

    CTreeNodeUI *pIndexNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(iIndex));
    if (!pIndexNode) {
        if (!mTreeNodes.Add(pControl)) {
            return false;
        }
    } else if (pIndexNode && !mTreeNodes.InsertAt(iIndex, pControl)) {
        return false;
    }

    if (!pIndexNode && pTreeView && pTreeView->GetItemAt(GetTreeIndex() + 1)) {
        pIndexNode = static_cast<CTreeNodeUI *>(pTreeView->GetItemAt(GetTreeIndex() + 1)->GetInterface(_T("TreeNode")));
    }
    pControl = CalLocation(reinterpret_cast<CTreeNodeUI *>(pControl));
    if (pTreeView && pIndexNode) {
        return pTreeView->AddAt(reinterpret_cast<CTreeNodeUI *>(pControl), pIndexNode);
    } else {
        return pTreeView->Add(reinterpret_cast<CTreeNodeUI *>(pControl));
    }
    return true;
}

// ************************************
// ��������: Remove
// ��������: bool
// ������Ϣ: CControlUI * pControl
// ����˵��:
// ************************************
bool CTreeNodeUI::Remove(CControlUI *pControl)
{
    return RemoveAt(reinterpret_cast<CTreeNodeUI *>(pControl));
}

// ************************************
// ��������: SetVisibleTag
// ��������: void
// ������Ϣ: bool _IsVisible
// ����˵��:
// ************************************
void CTreeNodeUI::SetVisibleTag(bool _IsVisible)
{
    m_bIsVisable = _IsVisible;
}

// ************************************
// ��������: GetVisibleTag
// ��������: bool
// ����˵��:
// ************************************
bool CTreeNodeUI::GetVisibleTag()
{
    return m_bIsVisable;
}

// ************************************
// ��������: SetItemText
// ��������: void
// ������Ϣ: LPCTSTR pstrValue
// ����˵��:
// ************************************
void CTreeNodeUI::SetItemText(LPCTSTR pstrValue)
{
    if (pItemButton == nullptr) {
        return;
    }
    pItemButton->SetText(pstrValue);
}

// ************************************
// ��������: GetItemText
// ��������: DuiLib::CDuiString
// ����˵��:
// ************************************
CDuiString CTreeNodeUI::GetItemText()
{
    if (pItemButton == nullptr) {
        return CDuiString();
    }
    return pItemButton->GetText();
}

// ************************************
// ��������: CheckBoxSelected
// ��������: void
// ������Ϣ: bool _Selected
// ����˵��:
// ************************************
void CTreeNodeUI::CheckBoxSelected(bool _Selected)
{
    if (pCheckBox == nullptr) {
        return;
    }
    pCheckBox->Selected(_Selected);
}

// ************************************
// ��������: IsCheckBoxSelected
// ��������: bool
// ����˵��:
// ************************************
bool CTreeNodeUI::IsCheckBoxSelected() const
{
    if (pCheckBox == nullptr) {
        return false;
    }
    return pCheckBox->IsSelected();
}

// ************************************
// ��������: IsHasChild
// ��������: bool
// ����˵��:
// ************************************
bool CTreeNodeUI::IsHasChild()
{
    return !mTreeNodes.IsEmpty();
}

// ************************************
// ��������: AddChildNode
// ��������: bool
// ������Ϣ: CTreeNodeUI * _pTreeNodeUI
// ����˵��:
// ************************************
bool CTreeNodeUI::AddChildNode(CTreeNodeUI *_pTreeNodeUI)
{
    if (_pTreeNodeUI == nullptr || _pTreeNodeUI->GetClass() == nullptr) {
        return false;
    }

    if (_tcsicmp(_pTreeNodeUI->GetClass(), _T("TreeNodeUI")) != 0) {
        return false;
    }

    _pTreeNodeUI = CalLocation(_pTreeNodeUI);

    bool nRet = true;

    if (pTreeView) {
        CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(mTreeNodes.GetSize() - 1));
        if (!pNode || !pNode->GetLastNode()) {
            nRet = pTreeView->AddAt(_pTreeNodeUI, GetTreeIndex() + 1) >= 0;
        } else {
            nRet = pTreeView->AddAt(_pTreeNodeUI, pNode->GetLastNode()->GetTreeIndex() + 1) >= 0;
        }
    }

    if (nRet) {
        mTreeNodes.Add(_pTreeNodeUI);
    }

    return nRet;
}

// ************************************
// ��������: RemoveAt
// ��������: bool
// ������Ϣ: CTreeNodeUI * _pTreeNodeUI
// ����˵��:
// ************************************
bool CTreeNodeUI::RemoveAt(CTreeNodeUI *_pTreeNodeUI)
{
    int nIndex = mTreeNodes.Find(_pTreeNodeUI);
    CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(nIndex));
    if (pNode && pNode == _pTreeNodeUI) {
        while (pNode->IsHasChild()) {
            pNode->RemoveAt(static_cast<CTreeNodeUI *>(pNode->mTreeNodes.GetAt(0)));
        }

        mTreeNodes.Remove(nIndex);

        if (pTreeView) {
            pTreeView->Remove(_pTreeNodeUI);
        }

        return true;
    }
    return false;
}

// ************************************
// ��������: SetParentNode
// ��������: void
// ������Ϣ: CTreeNodeUI * _pParentTreeNode
// ����˵��:
// ************************************
void CTreeNodeUI::SetParentNode(CTreeNodeUI *_pParentTreeNode)
{
    pParentTreeNode = _pParentTreeNode;
}

// ************************************
// ��������: GetParentNode
// ��������: CTreeNodeUI*
// ����˵��:
// ************************************
CTreeNodeUI *CTreeNodeUI::GetParentNode()
{
    return pParentTreeNode;
}

// ************************************
// ��������: GetCountChild
// ��������: long
// ����˵��:
// ************************************
long CTreeNodeUI::GetCountChild()
{
    return mTreeNodes.GetSize();
}

// ************************************
// ��������: SetTreeView
// ��������: void
// ������Ϣ: CTreeViewUI * _CTreeViewUI
// ����˵��:
// ************************************
void CTreeNodeUI::SetTreeView(CTreeViewUI *_CTreeViewUI)
{
    pTreeView = _CTreeViewUI;
}

// ************************************
// ��������: GetTreeView
// ��������: CTreeViewUI*
// ����˵��:
// ************************************
CTreeViewUI *CTreeNodeUI::GetTreeView()
{
    return pTreeView;
}

// ************************************
// ��������: SetAttribute
// ��������: void
// ������Ϣ: LPCTSTR pstrName
// ������Ϣ: LPCTSTR pstrValue
// ����˵��:
// ************************************
void CTreeNodeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrName == nullptr || pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("text")) == 0) {
        if (pItemButton != nullptr) {
            pItemButton->SetText(pstrValue);
        }
    } else if (_tcsicmp(pstrName, _T("horizattr")) == 0) {
        pHoriz->ApplyAttributeList(pstrValue);
    } else if (_tcsicmp(pstrName, _T("dotlineattr")) == 0) {
        pDottedLine->ApplyAttributeList(pstrValue);
    } else if (_tcsicmp(pstrName, _T("folderattr")) == 0) {
        pFolderButton->ApplyAttributeList(pstrValue);
    } else if (_tcsicmp(pstrName, _T("checkboxattr")) == 0) {
        pCheckBox->ApplyAttributeList(pstrValue);
    } else if (_tcsicmp(pstrName, _T("itemattr")) == 0) {
        if (pItemButton != nullptr) {
            pItemButton->ApplyAttributeList(pstrValue);
        }
    } else if (_tcsicmp(pstrName, _T("itemtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
            SetItemTextColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("itemhottextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
            SetItemHotTextColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("selitemtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
            SetSelItemTextColor(clrColor);
        }
    } else if (_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        if (pstrValue != nullptr) {
            DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
            SetSelItemHotTextColor(clrColor);
        }
    } else {
        CListContainerElementUI::SetAttribute(pstrName, pstrValue);
    }
}

// ************************************
// ��������: GetTreeNodes
// ��������: DuiLib::CStdPtrArray
// ����˵��:
// ************************************
CStdPtrArray CTreeNodeUI::GetTreeNodes()
{
    return mTreeNodes;
}

// ************************************
// ��������: GetChildNode
// ��������: CTreeNodeUI*
// ������Ϣ: int _nIndex
// ����˵��:
// ************************************
CTreeNodeUI *CTreeNodeUI::GetChildNode(int _nIndex)
{
    return static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(_nIndex));
}

// ************************************
// ��������: SetVisibleFolderBtn
// ��������: void
// ������Ϣ: bool _IsVisibled
// ����˵��:
// ************************************
void CTreeNodeUI::SetVisibleFolderBtn(bool _IsVisibled)
{
    if (pFolderButton == nullptr) {
        return;
    }
    pFolderButton->SetVisible(_IsVisibled);
}

// ************************************
// ��������: GetVisibleFolderBtn
// ��������: bool
// ����˵��:
// ************************************
bool CTreeNodeUI::GetVisibleFolderBtn()
{
    if (pFolderButton == nullptr) {
        return false;
    }
    return pFolderButton->IsVisible();
}

// ************************************
// ��������: SetVisibleCheckBtn
// ��������: void
// ������Ϣ: bool _IsVisibled
// ����˵��:
// ************************************
void CTreeNodeUI::SetVisibleCheckBtn(bool _IsVisibled)
{
    if (pCheckBox == nullptr) {
        return;
    }
    pCheckBox->SetVisible(_IsVisibled);
}

// ************************************
// ��������: GetVisibleCheckBtn
// ��������: bool
// ����˵��:
// ************************************
bool CTreeNodeUI::GetVisibleCheckBtn()
{
    if (pCheckBox == nullptr) {
        return false;
    }
    return pCheckBox->IsVisible();
}

// ************************************
// ��������: GetNodeIndex
// ��������: int
// ����˵��: ȡ��ȫ������ͼ������
// ************************************
int CTreeNodeUI::GetTreeIndex()
{
    if (!pTreeView) {
        return -1;
    }

    for (int nIndex = 0; nIndex < pTreeView->GetCount(); nIndex++) {
        if (pTreeView->GetItemAt(nIndex) == this) {
            return nIndex;
        }
    }

    return -1;
}

// ************************************
// ��������: GetNodeIndex
// ��������: int
// ����˵��: ȡ��������ֵܽڵ�ĵ�ǰ����
// ************************************
int CTreeNodeUI::GetNodeIndex()
{
    if (!GetParentNode() && !pTreeView) {
        return -1;
    }

    if (!GetParentNode() && pTreeView) {
        return GetTreeIndex();
    }

    return GetParentNode()->GetTreeNodes().Find(this);
}

// ************************************
// ��������: GetLastNode
// ��������: CTreeNodeUI*
// ����˵��:
// ************************************
CTreeNodeUI *CTreeNodeUI::GetLastNode()
{
    if (!IsHasChild()) {
        return this;
    }

    CTreeNodeUI *nRetNode = nullptr;

    for (int nIndex = 0; nIndex < GetTreeNodes().GetSize(); nIndex++) {
        CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(GetTreeNodes().GetAt(nIndex));
        if (!pNode) {
            continue;
        }

        CDuiString aa = pNode->GetItemText();

        if (pNode->IsHasChild()) {
            nRetNode = pNode->GetLastNode();
        } else {
            nRetNode = pNode;
        }
    }

    return nRetNode;
}

// ************************************
// ��������: CalLocation
// ��������: CTreeNodeUI*
// ������Ϣ: CTreeNodeUI * _pTreeNodeUI
// ����˵��: ��������
// ************************************
CTreeNodeUI *CTreeNodeUI::CalLocation(CTreeNodeUI *_pTreeNodeUI)
{
    const int widthFixedValue = 16;
    if (_pTreeNodeUI == nullptr || pDottedLine == nullptr || pItemButton == nullptr ||
        _pTreeNodeUI->GetDottedLine() == nullptr || _pTreeNodeUI->GetItemButton() == nullptr) {
        return false;
    }
    _pTreeNodeUI->GetDottedLine()->SetVisible(true);
    _pTreeNodeUI->GetDottedLine()->SetFixedWidth(pDottedLine->GetFixedWidth() + widthFixedValue);
    _pTreeNodeUI->SetParentNode(this);
    _pTreeNodeUI->GetItemButton()->SetGroup(pItemButton->GetGroup());
    _pTreeNodeUI->SetTreeView(pTreeView);

    return _pTreeNodeUI;
}

// ************************************
// ��������: SetTextColor
// ��������: void
// ������Ϣ: DWORD _dwTextColor
// ����˵��:
// ************************************
void CTreeNodeUI::SetItemTextColor(DWORD _dwItemTextColor)
{
    m_dwItemTextColor = _dwItemTextColor;
    if (pItemButton == nullptr) {
        return;
    }
    pItemButton->SetTextColor(m_dwItemTextColor);
}

// ************************************
// ��������: GetTextColor
// ��������: DWORD
// ����˵��:
// ************************************
DWORD CTreeNodeUI::GetItemTextColor() const
{
    return m_dwItemTextColor;
}

// ************************************
// ��������: SetTextHotColor
// ��������: void
// ������Ϣ: DWORD _dwTextHotColor
// ����˵��:
// ************************************
void CTreeNodeUI::SetItemHotTextColor(DWORD _dwItemHotTextColor)
{
    m_dwItemHotTextColor = _dwItemHotTextColor;
    Invalidate();
}

// ************************************
// ��������: GetTextHotColor
// ��������: DWORD
// ����˵��:
// ************************************
DWORD CTreeNodeUI::GetItemHotTextColor() const
{
    return m_dwItemHotTextColor;
}

// ************************************
// ��������: SetSelItemTextColor
// ��������: void
// ������Ϣ: DWORD _dwSelItemTextColor
// ����˵��:
// ************************************
void CTreeNodeUI::SetSelItemTextColor(DWORD _dwSelItemTextColor)
{
    m_dwSelItemTextColor = _dwSelItemTextColor;
    Invalidate();
}

// ************************************
// ��������: GetSelItemTextColor
// ��������: DWORD
// ����˵��:
// ************************************
DWORD CTreeNodeUI::GetSelItemTextColor() const
{
    return m_dwSelItemTextColor;
}

// ************************************
// ��������: SetSelHotItemTextColor
// ��������: void
// ������Ϣ: DWORD _dwSelHotItemTextColor
// ����˵��:
// ************************************
void CTreeNodeUI::SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor)
{
    m_dwSelItemHotTextColor = _dwSelHotItemTextColor;
    Invalidate();
}

// ************************************
// ��������: GetSelHotItemTextColor
// ��������: DWORD
// ����˵��:
// ************************************
DWORD CTreeNodeUI::GetSelItemHotTextColor() const
{
    return m_dwSelItemHotTextColor;
}

/* *************************************************************************** */
/* *************************************************************************** */
/* *************************************************************************** */
IMPLEMENT_DUICONTROL(CTreeViewUI)

// ************************************
// ��������: CTreeViewUI
// ��������:
// ������Ϣ: void
// ����˵��:
// ************************************
CTreeViewUI::CTreeViewUI(void) : m_bVisibleFolderBtn(true), m_bVisibleCheckBtn(false), m_uItemMinWidth(0)
{
    this->GetHeader()->SetVisible(false);
}

// ************************************
// ��������: ~CTreeViewUI
// ��������:
// ������Ϣ: void
// ����˵��:
// ************************************
CTreeViewUI::~CTreeViewUI(void) {}

// ************************************
// ��������: GetClass
// ��������: LPCTSTR
// ����˵��:
// ************************************
LPCTSTR CTreeViewUI::GetClass() const
{
    return _T("TreeViewUI");
}

// ************************************
// ��������: GetInterface
// ��������: LPVOID
// ������Ϣ: LPCTSTR pstrName
// ����˵��:
// ************************************
LPVOID CTreeViewUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("TreeView")) == 0) {
        return static_cast<CTreeViewUI *>(this);
    }
    return CListUI::GetInterface(pstrName);
}

// ************************************
// ��������: Add
// ��������: bool
// ������Ϣ: CTreeNodeUI * pControl
// ����˵��:
// ************************************
bool CTreeViewUI::Add(CTreeNodeUI *pControl)
{
    if (pControl == nullptr || pControl->GetClass() == nullptr) {
        return false;
    }
    if (_tcsicmp(pControl->GetClass(), _T("TreeNodeUI")) != 0) {
        return false;
    }

    pControl->OnNotify += MakeDelegate(this, &CTreeViewUI::OnDBClickItem);
    pControl->GetFolderButton()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnFolderChanged);
    pControl->GetCheckBox()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnCheckBoxChanged);

    pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
    pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);
    if (m_uItemMinWidth > 0) {
        pControl->SetMinWidth(m_uItemMinWidth);
    }

    CListUI::Add(pControl);

    if (pControl->GetCountChild() > 0) {
        int nCount = pControl->GetCountChild();
        for (int nIndex = 0; nIndex < nCount; nIndex++) {
            CTreeNodeUI *pNode = pControl->GetChildNode(nIndex);
            if (pNode) {
                Add(pNode);
            }
        }
    }

    pControl->SetTreeView(this);
    return true;
}

// ************************************
// ��������: AddAt
// ��������: long
// ������Ϣ: CTreeNodeUI * pControl
// ������Ϣ: int iIndex
// ����˵��: �÷������Ὣ������Ľڵ������λ�������������Ľڵ�Ϊ�Ǹ��ڵ�,
// ��ʹ��AddAt(CTreeNodeUI*pControl,CTreeNodeUI* _IndexNode) ����
// ************************************
long CTreeViewUI::AddAt(CTreeNodeUI *pControl, int iIndex)
{
    const int ret = -1;
    if (pControl == nullptr || pControl->GetClass() == nullptr) {
        return ret;
    }
    if (_tcsicmp(pControl->GetClass(), _T("TreeNodeUI")) != 0) {
        return ret;
    }
    pControl->OnNotify += MakeDelegate(this, &CTreeViewUI::OnDBClickItem);
    pControl->GetFolderButton()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnFolderChanged);
    pControl->GetCheckBox()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnCheckBoxChanged);
    pControl->SetVisibleFolderBtn(m_bVisibleFolderBtn);
    pControl->SetVisibleCheckBtn(m_bVisibleCheckBtn);

    if (m_uItemMinWidth > 0) {
        pControl->SetMinWidth(m_uItemMinWidth);
    }
    CListUI::AddAt(pControl, iIndex);
    if (pControl->GetCountChild() > 0) {
        int nCount = pControl->GetCountChild();
        for (int nIndex = 0; nIndex < nCount; nIndex++) {
            CTreeNodeUI *pNode = pControl->GetChildNode(nIndex);
            if (pNode) {
                return AddAt(pNode, iIndex + 1);
            }
        }
    } else {
        return iIndex + 1;
    }

    return ret;
}

// ************************************
// ��������: AddAt
// ��������: bool
// ������Ϣ: CTreeNodeUI * pControl
// ������Ϣ: CTreeNodeUI * _IndexNode
// ����˵��:
// ************************************
bool CTreeViewUI::AddAt(CTreeNodeUI *pControl, CTreeNodeUI *_IndexNode)
{
    if (!_IndexNode && !pControl) {
        return false;
    }

    int nItemIndex = -1;
    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        if (_IndexNode == GetItemAt(nIndex)) {
            nItemIndex = nIndex;
            break;
        }
    }

    if (nItemIndex == -1) {
        return false;
    }

    return AddAt(pControl, nItemIndex) >= 0;
}

// ************************************
// ��������: Remove
// ��������: bool
// ������Ϣ: CTreeNodeUI * pControl
// ����˵��: pControl �����Լ��µ����нڵ㽫��һ���Ƴ�
// ************************************
bool CTreeViewUI::Remove(CTreeNodeUI *pControl)
{
    if (pControl == nullptr) {
        return false;
    }
    if (pControl->GetCountChild() > 0) {
        int nCount = pControl->GetCountChild();
        for (int nIndex = 0; nIndex < nCount; nIndex++) {
            CTreeNodeUI *pNode = pControl->GetChildNode(nIndex);
            if (pNode) {
                pControl->Remove(pNode);
            }
        }
    }
    CListUI::Remove(pControl);
    return true;
}

// ************************************
// ��������: RemoveAt
// ��������: bool
// ������Ϣ: int iIndex
// ����˵��: iIndex �����Լ��µ����нڵ㽫��һ���Ƴ�
// ************************************
bool CTreeViewUI::RemoveAt(int iIndex)
{
    CTreeNodeUI *pItem = (reinterpret_cast<CTreeNodeUI *>(GetItemAt(iIndex)));
    if (pItem == nullptr) {
        return false;
    }
    if (pItem->GetCountChild()) {
        Remove(pItem);
    }
    return true;
}

void CTreeViewUI::RemoveAll()
{
    CListUI::RemoveAll();
}

// ************************************
// ��������: Notify
// ��������: void
// ������Ϣ: TNotifyUI & msg
// ����˵��:
// ************************************
void CTreeViewUI::Notify(TNotifyUI &msg) {}

// ************************************
// ��������: OnCheckBoxChanged
// ��������: bool
// ������Ϣ: void * param
// ����˵��:
// ************************************
bool CTreeViewUI::OnCheckBoxChanged(void *param)
{
    auto pMsg = reinterpret_cast<TNotifyUI *>(param);
    if (pMsg == nullptr || pMsg->pSender == nullptr) {
        return false;
    }
    if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED) {
        auto pCheckBox = reinterpret_cast<CCheckBoxUI *>(pMsg->pSender);
        if (pCheckBox == nullptr || pCheckBox->GetParent() == nullptr ||
            pCheckBox->GetParent()->GetParent() == nullptr) {
            return false;
        }
        auto pItem = reinterpret_cast<CTreeNodeUI *>(pCheckBox->GetParent()->GetParent());
        SetItemCheckBox(pCheckBox->GetCheck(), pItem);
        return true;
    }
    return true;
}

// ************************************
// ��������: OnFolderChanged
// ��������: bool
// ������Ϣ: void * param
// ����˵��:
// ************************************
bool CTreeViewUI::OnFolderChanged(void *param)
{
    auto pMsg = reinterpret_cast<TNotifyUI *>(param);
    if (pMsg == nullptr) {
        return false;
    }
    if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED) {
        auto pFolder = reinterpret_cast<CCheckBoxUI *>(pMsg->pSender);
        if (pFolder == nullptr || pFolder->GetParent() == nullptr || pFolder->GetParent()->GetParent() == nullptr) {
            return false;
        }
        auto pItem = reinterpret_cast<CTreeNodeUI *>(pFolder->GetParent()->GetParent());
        if (pItem == nullptr) {
            return false;
        }
        pItem->SetVisibleTag(!pFolder->GetCheck());
        SetItemExpand(!pFolder->GetCheck(), pItem);
        return true;
    }
    return true;
}

// ************************************
// ��������: OnDBClickItem
// ��������: bool
// ������Ϣ: void * param
// ����˵��:
// ************************************
bool CTreeViewUI::OnDBClickItem(void *param)
{
    auto pMsg = reinterpret_cast<TNotifyUI *>(param);
    if (pMsg == nullptr) {
        return false;
    }
    if (_tcsicmp(pMsg->sType, DUI_MSGTYPE_TREEITEMDBCLICK) == 0) {
        auto pItem = static_cast<CTreeNodeUI *>(pMsg->pSender);
        if (pItem == nullptr || pItem->GetFolderButton() == nullptr) {
            return false;
        }
        CCheckBoxUI *pFolder = pItem->GetFolderButton();
        if (pFolder == nullptr) {
            return false;
        }
        pFolder->Selected(!pFolder->IsSelected());
        pItem->SetVisibleTag(!pFolder->GetCheck());
        SetItemExpand(!pFolder->GetCheck(), pItem);
        return true;
    }
    return false;
}

// ************************************
// ��������: SetItemCheckBox
// ��������: bool
// ������Ϣ: bool _Selected
// ������Ϣ: CTreeNodeUI * _TreeNode
// ����˵��:
// ************************************
bool CTreeViewUI::SetItemCheckBox(bool _Selected, CTreeNodeUI *_TreeNode)
{
    if (_TreeNode) {
        if (_TreeNode->GetCountChild() > 0) {
            int nCount = _TreeNode->GetCountChild();
            for (int nIndex = 0; nIndex < nCount; nIndex++) {
                CTreeNodeUI *pItem = _TreeNode->GetChildNode(nIndex);
                if (pItem == nullptr) {
                    return false;
                }
                pItem->GetCheckBox()->Selected(_Selected);
                if (pItem->GetCountChild()) {
                    SetItemCheckBox(_Selected, pItem);
                }
            }
        }
        return true;
    } else {
        int nIndex = 0;
        int nCount = GetCount();
        while (nIndex < nCount) {
            CTreeNodeUI *pItem = reinterpret_cast<CTreeNodeUI *>(GetItemAt(nIndex));
            if (pItem == nullptr) {
                return false;
            }
            pItem->GetCheckBox()->Selected(_Selected);
            if (pItem->GetCountChild()) {
                SetItemCheckBox(_Selected, pItem);
            }

            nIndex++;
        }
        return true;
    }
    return false;
}

// ************************************
// ��������: SetItemExpand
// ��������: void
// ������Ϣ: bool _Expanded
// ������Ϣ: CTreeNodeUI * _TreeNode
// ����˵��:
// ************************************
void CTreeViewUI::SetItemExpand(bool _Expanded, CTreeNodeUI *_TreeNode)
{
    if (_TreeNode) {
        if (_TreeNode->GetCountChild() > 0) {
            int nCount = _TreeNode->GetCountChild();
            for (int nIndex = 0; nIndex < nCount; nIndex++) {
                CTreeNodeUI *pItem = _TreeNode->GetChildNode(nIndex);
                if (pItem == nullptr) {
                    return;
                }
                pItem->SetVisible(_Expanded);
                if (pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected()) {
                    SetItemExpand(_Expanded, pItem);
                }
            }
        }
    } else {
        int nIndex = 0;
        int nCount = GetCount();
        while (nIndex < nCount) {
            CTreeNodeUI *pItem = reinterpret_cast<CTreeNodeUI *>(GetItemAt(nIndex));
            if (pItem == nullptr) {
                return;
            }
            pItem->SetVisible(_Expanded);
            if (pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected()) {
                SetItemExpand(_Expanded, pItem);
            }
            nIndex++;
        }
    }
}

// ************************************
// ��������: SetVisibleFolderBtn
// ��������: void
// ������Ϣ: bool _IsVisibled
// ����˵��:
// ************************************
void CTreeViewUI::SetVisibleFolderBtn(bool _IsVisibled)
{
    m_bVisibleFolderBtn = _IsVisibled;
    int nCount = this->GetCount();
    for (int nIndex = 0; nIndex < nCount; nIndex++) {
        CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
        if (pItem == nullptr) {
            return;
        }
        pItem->GetFolderButton()->SetVisible(m_bVisibleFolderBtn);
    }
}

// ************************************
// ��������: GetVisibleFolderBtn
// ��������: bool
// ����˵��:
// ************************************
bool CTreeViewUI::GetVisibleFolderBtn()
{
    return m_bVisibleFolderBtn;
}

// ************************************
// ��������: SetVisibleCheckBtn
// ��������: void
// ������Ϣ: bool _IsVisibled
// ����˵��:
// ************************************
void CTreeViewUI::SetVisibleCheckBtn(bool _IsVisibled)
{
    m_bVisibleCheckBtn = _IsVisibled;
    int nCount = this->GetCount();
    for (int nIndex = 0; nIndex < nCount; nIndex++) {
        CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
        if (pItem == nullptr) {
            return;
        }
        pItem->GetCheckBox()->SetVisible(m_bVisibleCheckBtn);
    }
}

// ************************************
// ��������: GetVisibleCheckBtn
// ��������: bool
// ����˵��:
// ************************************
bool CTreeViewUI::GetVisibleCheckBtn()
{
    return m_bVisibleCheckBtn;
}

// ************************************
// ��������: SetItemMinWidth
// ��������: void
// ������Ϣ: UINT _ItemMinWidth
// ����˵��:
// ************************************
void CTreeViewUI::SetItemMinWidth(UINT _ItemMinWidth)
{
    m_uItemMinWidth = _ItemMinWidth;

    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));
        if (pTreeNode) {
            pTreeNode->SetMinWidth(GetItemMinWidth());
        }
    }
    Invalidate();
}

// ************************************
// ��������: GetItemMinWidth
// ��������: UINT
// ����˵��:
// ************************************
UINT CTreeViewUI::GetItemMinWidth()
{
    return m_uItemMinWidth;
}

// ************************************
// ��������: SetItemTextColor
// ��������: void
// ������Ϣ: DWORD _dwItemTextColor
// ����˵��:
// ************************************
void CTreeViewUI::SetItemTextColor(DWORD _dwItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));
        if (pTreeNode) {
            pTreeNode->SetItemTextColor(_dwItemTextColor);
        }
    }
}

// ************************************
// ��������: SetItemHotTextColor
// ��������: void
// ������Ϣ: DWORD _dwItemHotTextColor
// ����˵��:
// ************************************
void CTreeViewUI::SetItemHotTextColor(DWORD _dwItemHotTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));
        if (pTreeNode) {
            pTreeNode->SetItemHotTextColor(_dwItemHotTextColor);
        }
    }
}

// ************************************
// ��������: SetSelItemTextColor
// ��������: void
// ������Ϣ: DWORD _dwSelItemTextColor
// ����˵��:
// ************************************
void CTreeViewUI::SetSelItemTextColor(DWORD _dwSelItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));
        if (pTreeNode) {
            pTreeNode->SetSelItemTextColor(_dwSelItemTextColor);
        }
    }
}

// ************************************
// ��������: SetSelItemHotTextColor
// ��������: void
// ������Ϣ: DWORD _dwSelHotItemTextColor
// ����˵��:
// ************************************
void CTreeViewUI::SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++) {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));
        if (pTreeNode) {
            pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor);
        }
    }
}

// ************************************
// ��������: SetAttribute
// ��������: void
// ������Ϣ: LPCTSTR pstrName
// ������Ϣ: LPCTSTR pstrValue
// ����˵��:
// ************************************
void CTreeViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (pstrValue == nullptr) {
        return;
    }
    if (_tcsicmp(pstrName, _T("visiblefolderbtn")) == 0) {
        SetVisibleFolderBtn(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("visiblecheckbtn")) == 0) {
        SetVisibleCheckBtn(_tcsicmp(pstrValue, _T("true")) == 0);
    } else if (_tcsicmp(pstrName, _T("itemminwidth")) == 0) {
        SetItemMinWidth(_ttoi(pstrValue));
    } else if (_tcsicmp(pstrName, _T("itemtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
        SetItemTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("itemhottextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
        SetItemHotTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("selitemtextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
        SetSelItemTextColor(clrColor);
    } else if (_tcsicmp(pstrName, _T("selitemhottextcolor")) == 0) {
        if (*pstrValue == _T('#')) {
            pstrValue = ::CharNext(pstrValue);
        }
        LPTSTR pstr = nullptr;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, RADIX_HEXADECIMAL);
        SetSelItemHotTextColor(clrColor);
    } else {
        CListUI::SetAttribute(pstrName, pstrValue);
    }
}
}