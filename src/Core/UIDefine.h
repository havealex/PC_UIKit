﻿/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#pragma once

#ifndef _UIDEFINE_INCLUDE__
#define _UIDEFINE_INCLUDE__
namespace DuiLib {
#define MAX_FONT_ID 30000
#define CARET_TIMERID 0x1999

// 鼠标光标定义
#define DUI_ARROW 32512
#define DUI_IBEAM 32513
#define DUI_WAIT 32514
#define DUI_CROSS 32515
#define DUI_UPARROW 32516
#define DUI_SIZE 32640
#define DUI_ICON 32641
#define DUI_SIZENWSE 32642
#define DUI_SIZENESW 32643
#define DUI_SIZEWE 32644
#define DUI_SIZENS 32645
#define DUI_SIZEALL 32646
#define DUI_NO 32648
#define DUI_HAND 32649

// 消息类型
enum DuiSig {
    DuiSig_end = 0, // [marks end of message map]
    DuiSig_lwl,     // LRESULT (WPARAM, LPARAM)
    DuiSig_vn,      // void (TNotifyUI)
};

// 核心控件
class CControlUI;

// Structure for notifications to the outside world
struct tagTNotifyUI {
    CDuiString sType;
    CDuiString sVirtualWnd;
    CControlUI *pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    WPARAM wParam;
    LPARAM lParam;
};
using TNotifyUI = tagTNotifyUI;

class CNotifyPump;
using DUI_PMSG = void (CNotifyPump::*)(TNotifyUI &msg); // 指针类型

union DuiMessageMapFunctions {
    DUI_PMSG pfn; // generic member function pointer
    LRESULT (CNotifyPump::*pfn_Notify_lwl)(WPARAM, LPARAM);
    void (CNotifyPump::*pfn_Notify_vn)(TNotifyUI &);
};

// 定义所有消息类型
// ////////////////////////////////////////////////////////////////////////
#define DUI_MSGTYPE_MENU (_T("menu"))
#define DUI_MSGTYPE_LINK (_T("link"))

#define DUI_MSGTYPE_TIMER (_T("timer"))
#define DUI_MSGTYPE_CLICK (_T("click"))
#define DUI_MSGTYPE_DBCLICK (_T("dbclick"))
#define DUI_MSGTYPE_LBTN_UP (_T("lbtn_up"))

#define DUI_MSGTYPE_EDIT_KILLFOCUS (_T("edit_killfocus"))
#define DUI_MSGTYPE_ESCAPE (_T("escape"))
#define DUI_MSGTYPE_RETURN (_T("return"))
#define DUI_MSGTYPE_SCROLL (_T("scroll"))
#define DUI_MSGTYPE_VK_TAB (_T("vk_tab"))
#define DUI_MSGTYPE_VK_BACK (_T("vk_back"))

#define DUI_MSGTYPE_DROPDOWN (_T("dropdown"))
#define DUI_MSGTYPE_SETFOCUS (_T("setfocus"))

#define DUI_MSGTYPE_KILLFOCUS (_T("killfocus"))
#define DUI_MSGTYPE_ITEMCLICK (_T("itemclick"))
#define DUI_MSGTYPE_ITEMRCLICK (_T("itemrclick"))
#define DUI_MSGTYPE_TABSELECT (_T("tabselect"))
#define DUI_MSGTYPE_TABSELECT_IN (_T("tabselectin"))
#define DUI_MSGTYPE_TABSELECT_IN_PRE (_T("tabselectin_pre"))
#define DUI_MSGTYPE_TABSELECT_OUT (_T("tabselectout"))

#define DUI_MSGTYPE_ITEMSELECT (_T("itemselect"))
#define DUI_MSGTYPE_ITEMEXPAND (_T("itemexpand"))
#define DUI_MSGTYPE_WINDOWINIT (_T("windowinit"))
#define DUI_MSGTYPE_WINDOWSIZE (_T("windowsize"))
#define DUI_MSGTYPE_BUTTONDOWN (_T("buttondown"))
#define DUI_MSGTYPE_MOUSEMOVE (_T("mousemove"))
#define DUI_MSGTYPE_MOUSEENTER (_T("mouseenter"))
#define DUI_MSGTYPE_MOUSELEAVE (_T("mouseleave"))

#define DUI_MSGTYPE_TEXTCHANGED (_T("textchanged"))
#define DUI_MSGTYPE_HEADERCLICK (_T("headerclick"))
#define DUI_MSGTYPE_ITEMDBCLICK (_T("itemdbclick"))
#define DUI_MSGTYPE_SHOWACTIVEX (_T("showactivex"))

#define DUI_MSGTYPE_ITEMCOLLAPSE (_T("itemcollapse"))
#define DUI_MSGTYPE_ITEMACTIVATE (_T("itemactivate"))
#define DUI_MSGTYPE_VALUECHANGED (_T("valuechanged"))
#define DUI_MSGTYPE_VALUECHANGED_MOVE (_T("movevaluechanged"))

#define DUI_MSGTYPE_SELECTCHANGED (_T("selectchanged"))
#define DUI_MSGTYPE_UNSELECTED (_T("unselected"))

#define DUI_MSGTYPE_TREEITEMSELECT (_T("treeitemselect"))
#define DUI_MSGTYPE_TREEITEMEXPAND (_T("treeitemexpand"))
#define DUI_MSGTYPE_TREEITEMACTIVATE (_T("treeitemactivate"))
#define DUI_MSGTYPE_TREEITEMDBCLICK (_T("treeitemdbclick"))
#define DUI_MSGTYPE_CHECKCLICK (_T("checkclick"))
#define DUI_MSGTYPE_TEXTROLLEND (_T("textrollend"))
#define DUI_MSGTYPE_COLORCHANGED (_T("colorchanged"))

#define DUI_MSGTYPE_LISTITEMSELECT (_T("listitemselect"))
#define DUI_MSGTYPE_LISTITEMCHECKED (_T("listitemchecked"))
#define DUI_MSGTYPE_COMBOITEMSELECT (_T("comboitemselect"))
#define DUI_MSGTYPE_LISTHEADERCLICK (_T("listheaderclick"))
#define DUI_MSGTYPE_LISTHEADITEMCHECKED (_T("listheaditemchecked"))
#define DUI_MSGTYPE_LISTPAGECHANGED (_T("listpagechanged"))
#define DUI_MSGTYPE_ONPAINT (_T("onpaint"))
#define DUI_MSGTYPE_ANIMATION_STOP (_T("animationstop"))
#define DUI_MSGTYPE_DRAGDROP (_T("dragdrop"))


struct DUI_MSGMAP_ENTRY;
struct DUI_MSGMAP {
#ifndef UILIB_STATIC
    const DUI_MSGMAP *(PASCAL *pfnGetBaseMap)();
#else
    const DUI_MSGMAP *pBaseMap;
#endif
    const DUI_MSGMAP_ENTRY *lpEntries;
};

// 结构定义
struct DUI_MSGMAP_ENTRY { // 定义一个结构体，来存放消息信息
    CDuiString sMsgType;  // DUI消息类型
    CDuiString sCtrlName; // 控件名称
    UINT nSig;            // 标记函数指针类型
    DUI_PMSG pfn;         // 指向函数的指针
};

// 定义
#ifndef UILIB_STATIC
#define DUI_DECLARE_MESSAGE_MAP()                         \
private:                                                  \
    static const DUI_MSGMAP_ENTRY _messageEntries[];      \
protected:                                                \
    static const DUI_MSGMAP messageMap;                   \
    static const DUI_MSGMAP *PASCAL _GetBaseMessageMap(); \
    virtual const DUI_MSGMAP *GetMessageMap() const;

#else
#define DUI_DECLARE_MESSAGE_MAP()                    \
private:                                             \
    static const DUI_MSGMAP_ENTRY _messageEntries[]; \
protected:                                           \
    static const DUI_MSGMAP messageMap;              \
    virtual const DUI_MSGMAP *GetMessageMap() const;

#endif


// 基类声明开始
#ifndef UILIB_STATIC
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                              \
    const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()               \
    { return nullptr; }                                                  \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                 \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
    {  &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };\
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
    {                                                                     \

#else
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                              \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                 \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
    {  nullptr, &theClass::_messageEntries[0] };                         \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
    {                                                                     \

#endif


    // 子类声明开始
#ifndef UILIB_STATIC
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
    const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()               \
    { return &baseClass::messageMap; }                                \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                 \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
    { &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
    {                                                                     \

#else
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                 \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
    { &baseClass::messageMap, &theClass::_messageEntries[0] };        \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
    {                                                                     \

#endif


    // 声明结束
#define DUI_END_MESSAGE_MAP()                                             \
    { _T(""), _T(""), DuiSig_end, (DUI_PMSG)0 }                           \
    };                                                                        \


    // 定义消息类型--执行函数宏
#define DUI_ON_MSGTYPE(msgtype, memberFxn)                                \
    { msgtype, _T(""), DuiSig_vn, (DUI_PMSG)&memberFxn},                  \


    // 定义消息类型--控件名称--执行函数宏
#define DUI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                 \
    { msgtype, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },                \


    // 定义click消息的控件名称--执行函数宏
#define DUI_ON_CLICK_CTRNAME(ctrname,memberFxn)                           \
    { DUI_MSGTYPE_CLICK, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },      \


    // 定义selectchanged消息的控件名称--执行函数宏
#define DUI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                   \
    { DUI_MSGTYPE_SELECTCHANGED,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn }, \


    // 定义killfocus消息的控件名称--执行函数宏
#define DUI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                       \
    { DUI_MSGTYPE_KILLFOCUS,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },     \


    // 定义menu消息的控件名称--执行函数宏
#define DUI_ON_MENU_CTRNAME(ctrname,memberFxn)                            \
    { DUI_MSGTYPE_MENU,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },          \


    // 定义与控件名称无关的消息宏
    // 定义timer消息--执行函数宏
#define DUI_ON_TIMER()                                                    \
    { DUI_MSGTYPE_TIMER, _T(""), DuiSig_vn,(DUI_PMSG)&OnTimer },          \


// ////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
// /

#define DUI_CTR_EDIT (_T("Edit"))
#define DUI_CTR_LIST (_T("List"))
#define DUI_CTR_TEXT (_T("Text"))

#define DUI_CTR_COMBO (_T("Combo"))
#define DUI_CTR_LABEL (_T("Label"))
#define DUI_CTR_FLASH (_T("Flash"))
#define DUI_CTR_TOGGLESWITCH (_T("ToggleSwitch"))
#define DUI_CTR_ROUNDEDRECTBUTTON (_T("RoundedRectButton"))
#define DUI_CTR_LOADINGCONTROL (_T("LoadingControl"))
#define DUI_CTR_POINTSWAITCONTROL (_T("PointsWaitControl"))

#define DUI_CTR_BUTTON (_T("Button"))
#define DUI_CTR_OPTION (_T("Option"))
#define DUI_CTR_SLIDER (_T("Slider"))
#define DUI_CTR_SLIDESHOW (_T("SlideShow"))

#define DUI_CTR_CONTROL (_T("Control"))
#define DUI_CTR_ACTIVEX (_T("ActiveX"))
#define DUI_CTR_GIFANIM (_T("GifAnim"))

#define DUI_CTR_LISTITEM (_T("ListItem"))
#define DUI_CTR_PROGRESS (_T("Progress"))
#define DUI_CTR_RICHEDIT (_T("RichEdit"))
#define DUI_CTR_CHECKBOX (_T("CheckBox"))
#define DUI_CTR_COMBOBOX (_T("ComboBox"))
#define DUI_CTR_DATETIME (_T("DateTime"))
#define DUI_CTR_TREEVIEW (_T("TreeView"))
#define DUI_CTR_TREENODE (_T("TreeNode"))

#define DUI_CTR_CONTAINER (_T("Container"))
#define DUI_CTR_TABLAYOUT (_T("TabLayout"))
#define DUI_CTR_SCROLLBAR (_T("ScrollBar"))
#define DUI_CTR_IPADDRESS (_T("IPAddress"))

#define DUI_CTR_LISTHEADER (_T("ListHeader"))
#define DUI_CTR_LISTFOOTER (_T("ListFooter"))
#define DUI_CTR_TILELAYOUT (_T("TileLayout"))
#define DUI_CTR_WEBBROWSER (_T("WebBrowser"))

#define DUI_CTR_CHILDLAYOUT (_T("ChildLayout"))
#define DUI_CTR_LISTELEMENT (_T("ListElement"))

#define DUI_CTR_VERTICALLAYOUT (_T("VerticalLayout"))
#define DUI_CTR_LISTHEADERITEM (_T("ListHeaderItem"))

#define DUI_CTR_LISTTEXTELEMENT (_T("ListTextElement"))

#define DUI_CTR_HORIZONTALLAYOUT (_T("HorizontalLayout"))
#define DUI_CTR_LISTLABELELEMENT (_T("ListLabelElement"))

#define DUI_CTR_ANIMATIONTABLAYOUT (_T("AnimationTabLayout"))

#define DUI_CTR_LISTCONTAINERELEMENT (_T("ListContainerElement"))

#define DUI_CTR_TEXTSCROLL (_T("TextScroll"))

#define DUI_CTR_COLORPALETTE (_T("ColorPalette"))


#define DUI_CTR_ICON (_T("Icon"))

#define DUI_CTR_ANIMPANEL (_T("AnimPanel"))

#define DUI_CTR_CIRCLE_SHARE_PROGRES (_T("CircleShareProgress"))
// /
// //////////// END控件名称宏定义//////////////////////////////////////////////////
#define FONT_ID_TEXT_SIZE_ 0x2328           // 9000
#define FONT_ID_TEXT_BOLD_SIZE_ 0x238c      // 9100
#define FONT_ID_TEXT_UNDERLINE_SIZE_ 0x23f0 // 9200

#define FONT_ID_VALUE_SIZE_ 0x2454           // 9300
#define FONT_ID_VALUE_BOLD_SIZE_ 0x24B8      // 9400
#define FONT_ID_VALUE_UNDERLINE_SIZE_ 0x251C // 9500
} // namespace DuiLib


#endif // _UIDEFINE_INCLUDE__
