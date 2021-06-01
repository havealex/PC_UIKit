# CLabelUI（标签控件）

`CLabelUI` 继承了 `CAnimPanelUI` 控件属性和方法，更多可用属性和方法请参考：`CAnimPanelUI`控件

- [可用属性](#可用属性)
- [可用接口](#可用接口)



# 可用属性

| 属性名称               | 默认值    | 参数：类型 | 用途                                                         |
| ---------------------- | --------- | ---------- | ------------------------------------------------------------ |
| align                  | "left"    | STRING     | 水平布局，left左对齐，right右对齐，center居中。              |
| valign                 | "vcenter" | STRING     | 垂直布局，top顶部对齐，bottom底部对齐，vcenter居中。         |
| endellipsis            | false     | BOOL       | 句末显示不全是否使用...代替                                  |
| wordellipsis           | false     | BOOL       | 字符显示不全是否使用...代替                                  |
| pathellipsis           | false     | BOOL       | 路径显示不全是否使用...代替                                  |
| wordbreak              | false     | BOOL       | 文本是否换行<br />true表示当文本换行时将该文本以下的布局往下挤（如button被挤下移）<br />false表示文本换行时换行的文本不会显示 |
| ellipsemask            | false     | BOOL       | 椭圆遮罩效果                                                 |
| noprefix               | false     | BOOL       | 是否支持'&'字符转义为下划线                                  |
| font                   | -1        | INT        | 字体号                                                       |
| textcolor              | 0         | DWORD      | 文本颜色                                                     |
| disabledtextcolor      | 0         | DWORD      | 禁用状态文本颜色                                             |
| textpadding            | "0,0,0,0" | RECT       | 文字显示的外边距                                             |
| showhtml               | false     | BOOL       | Html标签是否显示                                             |
| autocalcwidth          | false     | BOOL       | 自适应宽度                                                   |
| autocalcheight         | false     | BOOL       | 自适应高度                                                   |
| autochangeparentheight | false     | BOOL       | 当文本换行时，会将该文本下方的所有布局往下挤压               |
| autoheightbytextsingle | false     | BOOL       | 当文本换行时，会将该文本下方的所有布局按单倍行距往下挤压     |
| autoheightbytextmulti  | false     | BOOL       | 当文本换行时，会将该文本下方的所有布局按多倍行距往下挤压     |



# 可用接口

| 接口名称              | 用途                         |
| --------------------- | ---------------------------- |
| [GetClass](#GetClass) | 获取类名 |
| [GetInterface](#GetInterface) | 获取接口名 |
| [GetControlFlags](#GetControlFlags) | 获取控制标志位 |
| [SetTextStyle](#SetTextStyle) | 设置文字风格 |
| [GetTextStyle](#GetTextStyle) | 获取文字风格 |
| [SetTextColor](#SetTextColor) | 设置文本颜色 |
| [GetTextColor](#GetTextColor) | 获取文本颜色 |
| [SetDisabledTextColor](#SetDisabledTextColor) | 设置禁用状态下的文本颜色 |
| [GetDisabledTextColor](#GetDisabledTextColor) | 获取禁用状态下的文本颜色 |
| [GetFont](#GetFont) | 获取字体 |
| [GetTextPadding](#GetTextPadding) | 获取文字显示的边距 |
| [SetTextPadding](#SetTextPadding) | 设置文字显示的边距 |
| [IsShowHtml](#IsShowHtml) | 获取是否使用类html富文本绘制的逻辑值 |
| [SetShowHtml](#SetShowHtml) | 设置是否使用类html富文本绘制 |
| [EstimateSize](#EstimateSize) | 尺寸评估 |
| [DoEvent](#DoEvent) | 处理事件 |
| [SetAttribute](#SetAttribute) | 设置属性 |
| [JudgeDirection](#JudgeDirection) | 水平排版的对齐方向判断 |
| [PaintText](#PaintText) | 书写文本 |
| [GetAutoCalcWidth](#GetAutoCalcWidth) | 获取是否自动计算宽度的逻辑值 |
| [SetAutoCalcWidth](#SetAutoCalcWidth) | 设置自动计算宽度 |
| [SetText](#SetText) | 设置文本 |
| [GetTextRect](#GetTextRect) | 获取文本填充内边距 |
| [GetAutoCalcHeight](#GetAutoCalcHeight) | 获取是否自动计算高度的逻辑值 |
| [SetAutoCalcHeight](#SetAutoCalcHeight) | 设置自动计算高度 |
| [GetAutoHeightByTextSingle](#GetAutoHeightByTextSingle) | 获取是否根据单行文本自适应高度的逻辑值 |
| [SetAutoHeightByTextSingle](#SetAutoHeightByTextSingle) | 设置根据单行文本自适应高度 |
| [GetAutoHeightByTextMulti](#GetAutoHeightByTextMulti) | 获取是否根据多行文本自适应高度的逻辑值 |
| [SetAutoHeightByTextMulti](#SetAutoHeightByTextMulti) | 设置根据多行文本自适应高度 |
| [EstimateSizeYX](#EstimateSizeYX) | 估计尺寸X和Y的值 |
| [SetEllipseMask](#SetEllipseMask) | 设置椭圆遮罩效果 |
| [SetFixedHeight](#SetFixedHeight) | 设置固定高度（缩放比改变前的高度） |
| [SetFixedHeightEx](#SetFixedHeightEx) | 设置固定高度（随缩放比改变） |
| [SetFixedWidth](#SetFixedWidth) | 设置固定宽度（缩放比改变前的宽度） |
| [SetFixedWidthEx](#SetFixedWidthEx) | 设置固定高度（随缩放比改变） |
| [SetFixedXY](#SetFixedXY) | 设置固定X、Y值 |
| [SetMinWidth](#SetMinWidth) | 设置最新宽度 |
| [SetMaxWidth](#SetMaxWidth) | 设置最大宽度 |
| [SetMinHeight](#SetMinHeight) | 设置最小高度 |
| [SetMaxHeight](#SetMaxHeight) | 设置最大高度 |
| [SetVisible](#SetVisible) | 设置是否可见 |
| [SetPos](#SetPos) | 设置位置坐标 |
| [SetDirectionPos](#SetDirectionPos) | 设置方位 |
| [SetPadding](#SetPadding) | 设置文本内边距 |
| [SetFloat](#SetFloat) | 设置是否使用绝对定位 |
| [GetToolTip](#GetToolTip) | 获取文本悬浮提示语 |



### GetClass

获取类名

```c++
LPCTSTR GetClass() const;
```

- 参数：无

- 返回值：类名字符串，如“CLabelUI”

## GetInterface

获取DuiLib项目下CButtonUI类中的接口。

```c++
LPVOID GetInterface(LPCTSTR pstrName);
```

- 参数：`pstrName` 待查询指针类型字符串名

- 返回值：接口名字符串，如“GetHotImage”

## GetControlFlags

获取button控制状态

```c++
UINT GetControlFlags() const;
```

- 参数：无

- 返回值：返回button的控制状态，如无控制返回0，键盘控制约束返回0x00000001，其他控制约束返回0x00000002

## SetTextStyle

设置字体风格

```c++
void SetTextStyle(UINT uStyle);
```

- 参数：`uStyle`字体风格

- 返回值：无

## GetTextStyle

获取字体风格

```c++
UINT GetTextStyle() const;
```

- 参数：无

- 返回值：返回字体风格

## SetTextColor

设置字体颜色

```c++
void SetTextColor(DWORD dwTextColor);
```

- 参数：`dwTextColor`字体颜色值

- 返回值：无

## GetTextColor

获取字体颜色

```c++
DWORD GetTextColor() const;
```

- 参数：无

- 返回值：返回字体颜色值

## SetDisabledTextColor

设置禁用状态字体颜色

```c++
cvoid SetDisabledTextColor(DWORD dwTextColor);
```

- 参数：`dwTextColor` 字体颜色值

- 返回值：无

## GetDisabledTextColor

获取禁用状态字体颜色

```c++
DWORD GetDisabledTextColor() const;
```

- 参数：无

- 返回值：返回禁用状态字体颜色值

## GetFont

获取字体

```c++
int GetFont() const;
```

- 参数：无

- 返回值：字体编号

## GetTextPadding

获取文字显示的边距

```c++
RECT GetTextPadding() const;
```

- 参数：无

- 返回值：返回文字显示的边距

## SetTextPadding

设置文字显示的边距

```c++
void SetTextPadding(RECT rc);
```

- 参数：`rc`文字显示的边距

- 返回值：无

## IsShowHtml

获取是否使用类html富文本绘制的逻辑值

```c++
bool IsShowHtml();
```

- 参数：无

- 返回值：是否显示的逻辑值

## SetShowHtml

设置是否使用类html富文本绘制

```c++
void SetShowHtml(bool bShowHtml = true);
```

- 参数：`bShowHtml` 是否显示的逻辑值

- 返回值：无

## EstimateSize

字体尺寸评估

```c++
SIZE EstimateSize(SIZE szAvailable);
```

- 参数：`szAvailable`预估尺寸值

- 返回值：最终估计的尺寸值

## DoEvent

处理事件

```c++
void DoEvent(TEventUI &event) override;
```

- 参数：`event`

- 返回值：无

## SetAttribute

设置属性

```c++
void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
```

- 参数：`pstrName`属性名、`pstrValue`属性值

- 返回值：无

## JudgeDirection

文本排版的对齐方向判断

```c++
void JudgeDirection(LPCTSTR pstrName, LPCTSTR pstrValue);
```

- 参数：`pstrName`属性名、`pstrValue`属性值

- 返回值：无

## PaintText

书写文本

```c++
void PaintText(HDC hDC);
```

- 参数：`hDC`设备场景句柄

- 返回值：无

## GetAutoCalcWidth

是否获取自动计算宽度

```c++
virtual bool GetAutoCalcWidth() const;
```

- 参数：无

- 返回值：是否执行获取自动计算宽度的逻辑值

## SetAutoCalcWidth

设置自动计算宽度

```c++
virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
```

- 参数：`bAutoCalcWidth`是否执行获取自动计算宽度的逻辑值

- 返回值：无

## SetText

设置文本

```c++
virtual void SetText(LPCTSTR pstrText);
```

- 参数：`pstrText`需要显示的文本字符串

- 返回值：无

## GetTextRect

获取文本填充内间距，包括左、上、右、下4个方向

```c++
CDuiRect GetTextRect(SIZE szAvailable);
```

- 参数：`szAvailable`预估尺寸值

- 返回值：文本填充内间距

## GetAutoCalcHeight

是否获取自动计算高度

```c++
bool GetAutoCalcHeight() const;
```

- 参数：无

- 返回值：返回是否执行获取自动计算高度的逻辑值

## SetAutoCalcHeight

设置自动计算高度

```c++
void SetAutoCalcHeight(bool bAutoCalcHeight);
```

- 参数：`bAutoCalcHeight`是否执行获取自动计算高度的逻辑值

- 返回值：无

## GetAutoHeightByTextSingle

是否根据单行文本自适应高度

```c++
bool GetAutoHeightByTextSingle() const;
```

- 参数：无

- 返回值：是否执行根据单行文本自适应高度的逻辑值

## SetAutoHeightByTextSingle

设置根据单行文本自适应高度

```c++
void SetAutoHeightByTextSingle(bool bAutoHeightByTextSingle);
```

- 参数：`bAutoHeightByTextSingle`是否执行根据单行文本自适应高度的逻辑值

- 返回值：无

## GetAutoHeightByTextMulti

是否根据多行文本自适应高度

```c++
bool GetAutoHeightByTextMulti() const;
```

- 参数：无

- 返回值：是否执行根据多行文本自适应高度的逻辑值

## SetAutoHeightByTextMulti

设置根据多行文本自适应高度

```c++
void SetAutoHeightByTextMulti(bool bAutoHeightByTextMulti);
```

- 参数：`bAutoHeightByTextMulti`是否执行根据多行文本自适应高度的逻辑值

- 返回值：无

## EstimateSizeYX

是否评估文本尺寸的Y、X值

```c++
bool EstimateSizeYX(SIZE &_size);
```

- 参数：`_size`文本尺寸

- 返回值：是否评估文本尺寸的Y、X值的逻辑值

## DoPaint

绘画执行

```c++
virtual void DoPaint(HDC hDC, const RECT &rcPaint);
```

- 参数：`hDc`设备场景句柄、rcPaint

- 返回值：无

## SetEllipseMask

设置椭圆遮罩效果

```c++
void SetEllipseMask(bool b);
```

- 参数：`b`是否设置椭圆掩码的逻辑值

- 返回值：无

## SetFixedHeight

设置固定高度（缩放比改变前的高度）

```c++
void SetFixedHeight(int cy) override;
```

- 参数：`cy`高度值

- 返回值：无

## SetFixedHeightEx

设置固定高度（随缩放比改变）

```c++
void SetFixedHeightEx(int cy) override;
```

- 参数：`cy`高度值

- 返回值：无

## SetFixedWidth

设置固定宽度（缩放比改变前的高度）

```c++
void SetFixedWidth(int cx) override;
```

- 参数：`cx`宽度值

- 返回值：无

## SetFixedWidthEx

设置固定宽度度（随缩放比改变）

```c++
void SetFixedWidthEx(int cx) override;
```

- 参数：`cx`宽度值

- 返回值：无

## SetFixedXY

设置固定X、Y值

```c++
void SetFixedXY(SIZE szXY);
```

- 参数：`szXY`

- 返回值：无

## SetMinWidth

设置最小宽度

```c++
void SetMinWidth(int cx) override;
```

- 参数：`cx`宽度值

- 返回值：无

## SetMaxWidth

设置最大宽度

```c++
void SetMaxWidth(int cx) override;
```

- 参数：`cx`宽度值

- 返回值：无

## SetMinHeight

设置最小高度

```c++
void SetMinHeight(int cy) override;
```

- 参数：`cy`高度值

- 返回值：无

## SetMaxHeight

设置最大高度

```c++
void SetMaxHeight(int cy) override;
```

- 参数：`cy`高度值

- 返回值：无

## SetVisible

设置文本可见

```c++
void SetVisible(bool bVisible = true) override;
```

- 参数：`bVisible` 是否可见的逻辑值，初始化值为true

- 返回值：无

## SetPos

设置位置坐标

```c++
void SetPos(RECT rc, bool bNeedInvalidate = true);
```

- 参数：`rc`位置坐标、`bNeedInvalidate` 是否需要初始化的逻辑值

- 返回值：无

## SetDirectionPos

设置文本方向坐标

```c++
RECT SetDirectionPos(RECT rc);
```

- 参数：`rc`位置坐标

- 返回值：文本方向坐标

## SetPadding

设置文本内边距

```c++
void SetPadding(RECT rcPadding);
```

- 参数：`rcPadding`是否显示悬浮文本的逻辑值

- 返回值：无

## SetFloat

设置是否使用绝对定位

```c++
void SetFloat(bool bFloat = true) override;
```

- 参数：`bFloat` 待设置是否生效的逻辑值

- 返回值：无

## GetToolTip

获取文本悬浮提示语

```c++
CDuiString GetToolTip() const override;
```

- 参数：无

- 返回值：hover tips展示的提示语字符串
