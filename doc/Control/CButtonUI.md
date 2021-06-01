# CButtonUI(按钮控件)

`CButtonUI` 继承了 `CLabelUI` 控件属性和方法，更多可用属性和方法请参考：`CLabelUI`控件

- [可用属性](#可用属性)
- [可用接口](#可用接口)



# 可用属性

| 属性名称 | 默认值 | 参数类型 | 用途 |
| --- | ---| --- | --- |
| normalimage | "" | STRING | 普通状态图 |
| hotimage | "" | STRING | 鼠标悬浮的状态图片 |
| pushedimage | "" | STRING | 鼠标按下的状态图片 |
| focusedimage | "" | STRING | 获得焦点时的状态图片 |
| disabledimage | "" | STRING | 禁用状态图片 |
| hotforeimage | "" | STRING | 鼠标悬浮状态的前置图片 |
| bindtabindex | -1 | INT | 点击后将触发绑定TabLayout控件的对应索引号 |
| bindtablayoutname | "" | STRING | 绑定TabLayout控件 |
| hotbkcolor | 0 | DWORD | 鼠标悬浮时的背景颜色 |
| normalbkcolor | 0 | DWORD | 鼠标悬浮时的背景颜色 |
| pushedbkcolor | 0 | DWORD | 鼠标按下时的背景颜色 |
| hottextcolor | 0 | DWORD | 鼠标悬浮字体颜色，0表示不使用此颜色,如(0xFFFF0000)" |
| pushedtextcolor | 0 | DWORD | 鼠标按下字体颜色，0表示不使用此颜色,如(0xFFFF0000)" |
| focusedtextcolor | 0 | DWORD | 获得焦点字体颜色，0表示不使用此颜色,如(0xFFFF0000) |
| hovertextshow | true | BOOL | 鼠标悬浮时是否显示问题，如(true) |
| roundenhance | false | BOOL | 自绘圆角时，是否使用圆角增强功能，尽可能消除毛刺，如(true) |


# 可用接口

| 接口名称              | 用途                         |
| --------------------- | ---------------------------- |
| [GetClass](#GetClass)          | 获取类名                     |
| [GetInterface](#GetInterface)      | 获取接口名                   |
| [GetControlFlags](#GetControlFlags) | 获取button控制状态           |
| [Activate](#Activate) | 激活button |
| [SetEnabled](#SetEnabled) | 设置使能开关状态             |
| [DoEvent](#DoEvent)   | 处理事件                     |
| [GetNormalImage](#GetNormalImage) | 获取普通状态图               |
| [SetNormalImage](#SetNormalImage) | 设置普通状态图               |
| [GetHotImage](#GetHotImage) | 获取热图（鼠标悬停的状态图） |
| [SetHotImage](#SetHotImage) | 设置热图（鼠标悬停的状态图） |
| [GetPushedImage](#GetPushedImage) | 获取鼠标按下状态的图片    |
| [SetPushedImage](#SetPushedImage) | 设置鼠标按下状态的图片    |
| [GetFocusedImage](#GetFocusedImage) | 获取焦点状态的图片      |
| [SetFocusedImage](#SetFocusedImage) | 设置焦点状态的图片      |
| [GetDisabledImage](#GetDisabledImage) | 获取禁用状态的图片        |
| [SetDisabledImage](#SetDisabledImage) | 设置禁用状态的图片        |
| [GetHotForeImage](#GetHotForeImage) | 获取热前状态的图片               |
| [SetHotForeImage](#SetHotForeImage) | 设置热前状态的图片              |
| [BindTabIndex](#BindTabIndex) | 绑定标签索引 |
| [BindTabLayoutName](#BindTabLayoutName) | 绑定标签布局名称 |
| [BindTriggerTabSel](#BindTriggerTabSel) | 绑定触发标签选择 |
| [RemoveBindTabIndex](#RemoveBindTabIndex) | 移除绑定标签索引 |
| [GetBindTabLayoutIndex](#GetBindTabLayoutIndex) | 获取绑定标签布局索引                 |
| [GetBindTabLayoutName](#GetBindTabLayoutName) | 获取绑定标签布局名称                 |
| [SetNormalBkColor](#SetNormalBkColor) | 设置普通状态下背景色          |
| [SetHotBkColor](#SetHotBkColor) | 设置鼠标悬停状态下背景色      |
| [GetHotBkColor](#GetHotBkColor) | 获取鼠标悬停状态下背景色      |
| [SetPushedBkColor](#SetPushedBkColor) | 设置鼠标按下状态下背景色      |
| [GetPushedBkColor](#GetPushedBkColor) | 获取鼠标按下状态下背景色 |
| [SetHotTextColor](#SetHotTextColor) | 设置热点状态下的文本颜色 |
| [GetHotTextColor](#GetHotTextColor) | 获取热点状态下的文本颜色 |
| [SetPushedTextColor](#SetPushedTextColor) | 设置鼠标按下状态的文本颜色 |
| [GetPushedTextColor](#GetPushedTextColor) | 获取鼠标按下状态的文本颜色 |
| [SetFocusedTextColor](#SetFocusedTextColor) | 设置焦点状态下的文本颜色 |
| [GetFocusedTextColor](#GetFocusedTextColor) | 获取焦点状态下的文本颜色 |
| [SetAttribute](#SetAttribute) | 设置属性 |
| [PaintText](#PaintText) | 书写文本 |
| [SubPaintText](#SubPaintText) | 书写文本子函数 |
| [PaintBkColor](#PaintBkColor) | 绘画背景颜色 |
| [PaintStatusImage](#PaintStatusImage) | 绘画状态图 |
| [PaintForeImage](#PaintForeImage) | 绘画前景图 |
| [GetButtonState](#GetButtonState) | 获取button状态 |
| [SetButtonState](#SetButtonState) | 设置button状态 |
| [SetVisible](#SetVisible) | 设置是否可见 |
| [SetHoverTextShow](#SetHoverTextShow) | 设置悬浮文本是否显示 |
| [GetHoverTextShow](#GetHoverTextShow) | 获取悬浮文本是否显示的逻辑值 |
| [SetRoundEnhance](#SetRoundEnhance) | 设置边缘增强效果 |
| [GetLinkContentIndex](#GetLinkContentIndex) | 获取链接目录索引 |
| [GetNcHit](#GetNcHit) | 获取非客户端是否能点击 |
| [GetToolTip](#GetToolTip) | 获取鼠标悬停显示提示语的字符串文本 |



### GetClass

获取类名

```cpp
LPCTSTR GetClass() const;
```

- 参数：无

- 返回值：类名字符串，如“CButtonUI”

## GetInterface

获取DuiLib项目下CButtonUI类中的接口。

```cpp
LPVOID GetInterface(LPCTSTR pstrName);
```
- 参数：`pstrName` 待查询指针类型字符串名

- 返回值：接口名字符串，如“GetHotImage”

## GetControlFlags

获取button控制状态

```cpp
UINT GetControlFlags() const;
```

- 参数：无

- 返回值：返回button的控制状态，如无控制返回0，键盘控制约束返回0x00000001，其他控制约束返回0x00000002

## Activate

激活button

```cpp
bool Activate() override;
```

- 参数：无

- 返回值：true激活/false未激活

## SetEnabled

设置button使能开关状态

```cpp
void SetEnabled(bool bEnable = true) override;
```

- 参数：`bEnable`  button使能状rue态逻辑值，初始值为true

- 返回值：无

## DoEvent

事件处理

```cpp
void DoEvent(TEventUI &event) override;
```

- 参数：event结构体

- 返回值：无

## GetNormalImage

获取普通状态图路径字符串

```cpp
virtual LPCTSTR GetNormalImage();
```

- 参数：无

- 返回值：返回普通状态图路径字符串

## SetNormalImage

显示普通状态图

```cpp
virtual void SetNormalImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetHotImage

获取鼠标悬停状态图路径字符串

```cpp
virtual LPCTSTR GetHotImage();
```

- 参数：无

- 返回值：鼠标悬停状态图路径字符串

## SetHotImage

显示鼠标悬停状态图

```cpp
virtual void SetHotImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetPushedImage

获取鼠标按下的状态图路径字符串

```cpp
virtual LPCTSTR GetPushedImage();
```

- 参数：无

- 返回值：鼠标按下的状态图路径字符串

## SetPushedImage

显示鼠标按下的状态图

```cpp
virtual void SetPushedImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetFocusedImage

获取聚焦的状态图路径字符串

```cpp
virtual LPCTSTR GetFocusedImage();
```

- 参数：无

- 返回值：聚焦的状态图路径字符串

## SetFocusedImage

显示聚焦的状态图

```cpp
virtual void SetFocusedImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetDisabledImage

获取禁用的状态图路径字符串

```cpp
virtual LPCTSTR GetDisabledImage();
```

- 参数：无

- 返回值：禁用的状态图路径字符串

## SetDisabledImage

显示禁用的状态图

```cpp
virtual void SetDisabledImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetHotForeImage

获取热前状态图路径字符串

```cpp
virtual LPCTSTR GetHotForeImage();
```

- 参数：无

- 返回值：热前状态图路径字符串

## SetHotForeImage

显示热前状态图

```cpp
virtual void SetHotForeImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## BindTabIndex

绑定标签索引

```cpp
void BindTabIndex(int _BindTabIndex);
```

- 参数：`_BindTabIndex`待绑定标签索引

- 返回值：无

## BindTabLayoutName

绑定标签布局名称

```cpp
void BindTabLayoutName(LPCTSTR _TabLayoutName);
```

- 参数：`_TabLayoutName`待绑定标签名称

- 返回值：无

## BindTriggerTabSel

绑定触发标签选择

```cpp
void BindTriggerTabSel(int _SetSelectIndex = -1);
```

- 参数：`_SetSelectIndex` 待设置选择索引

- 返回值：无

## RemoveBindTabIndex

移除绑定标签索引

```cpp
void RemoveBindTabIndex();
```

- 参数：无

- 返回值：无

## GetBindTabLayoutIndex

获取绑定标签布局索引

```cpp
int GetBindTabLayoutIndex();
```

- 参数：无

- 返回值：绑定标签布局索引

## GetBindTabLayoutName

获取绑定标签布局名称

```cpp
LPCTSTR GetBindTabLayoutName();
```

- 参数：无

- 返回值：返回绑定标签布局名称字符串

## SetNormalBkColor

设置普通状态下的背景颜色

```cpp
void SetNormalBkColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## SetHotBkColor

设置热点状态下的背景颜色

```cpp
void SetHotBkColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## GetHotBkColor

获取热点状态下的背景颜色

```cpp
DWORD GetHotBkColor() const;
```

- 参数：无

- 返回值：热点状态下的背景颜色值

## SetPushedBkColor

设置鼠标按下时的背景颜色

```cpp
void SetPushedBkColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## GetPushedBkColor

获取鼠标按下时的背景颜色

```cpp
DWORD GetPushedBkColor() const;
```

- 参数：无

- 返回值：鼠标按下时的背景颜色值

## SetHotTextColor

设置热点文本颜色

```cpp
void SetHotTextColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## GetHotTextColor

获取热点文本颜色

```cpp
DWORD GetHotTextColor() const;
```

- 参数：无

- 返回值：热点文本颜色值

## SetPushedTextColor

设置鼠标按下时的文本颜色

```cpp
void SetPushedTextColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## GetPushedTextColor

获取鼠标按下时的文本颜色

```cpp
DWORD GetPushedTextColor() const;
```

- 参数：无

- 返回值：鼠标按下时的文本颜色值

## SetFocusedTextColor

设置焦点文本颜色

```cpp
void SetFocusedTextColor(DWORD dwColor);
```

- 参数：`dwColor`待设置颜色值

- 返回值：无

## GetFocusedTextColor

获取焦点文本颜色

```cpp
DWORD GetFocusedTextColor() const;
```

- 参数：无

- 返回值：焦点文本颜色值

## SetAttribute

设置属性

```cpp
void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
```

- 参数：`pstrName` 待设置属性名 、`pstrValue` 待设置属性值

- 返回值：无

## PaintText

添加书写文本

```cpp
void PaintText(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## SubPaintText

绘画text文本补充功能

```cpp
RECT SubPaintText();
```

- 参数：无

- 返回值：text文本的内边距

## PaintBkColor

绘画背景颜色

```cpp
void PaintBkColor(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## PaintStatusImage

绘画状态图

```cpp
void PaintStatusImage(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## PaintForeImage

绘画前置图

```cpp
void PaintForeImage(HDC hDC);
```

- 参数：`hDC`设备场景句柄

- 返回值：无

## GetButtonState

获取button状态值

```cpp
UINT GetButtonState();
```

- 参数：无

- 返回值：返回button状态值

## SetButtonState

设置按钮状态

```cpp
void SetButtonState(UINT state);
```

- 参数：`state`待设置button状态

- 返回值：无

## SetVisible

设置是否可见

```cpp
void SetVisible(bool bVisible = true) override;
```

- 参数：`bVisible` 待设置是否可见，初始化值为true

- 返回值：无

## SetHoverTextShow

设置是否显示悬浮文本

```cpp
void SetHoverTextShow(bool val);
```

- 参数：`val`待设置是否显示悬浮文本的逻辑值

- 返回值：无

## GetHoverTextShow

获取hover text悬浮文本是否显示的逻辑值

```cpp
bool GetHoverTextShow();
```

- 参数：无

- 返回值：hover text悬浮文本是否显示的逻辑值

## SetRoundEnhance

设置边缘增强效果

```cpp
void SetRoundEnhance(bool val);
```

- 参数：`val`待设置是否使用增强效果的逻辑值

- 返回值：无

## GetLinkContentIndex

获取链接目录索引

```cpp
int GetLinkContentIndex(const POINT &ptMouse);
```

- 参数：`ptMouse`待获取鼠标触发事件

- 返回值：链接目录索引

## GetNcHit

获取非客户端是否能点击

```cpp
bool GetNcHit() override;
```

- 参数：无

- 返回值：非客户端是否能点击的逻辑值

## GetToolTip

获取文本悬浮提示语

```cpp
CDuiString GetToolTip() const override;
```

- 参数：无

- 返回值：hover tips展示的提示语字符串
