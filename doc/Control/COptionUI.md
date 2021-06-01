# COptionUI（可选控件）

`COptionUI` 继承了 `CButtonUI` 控件属性和方法，更多可用属性和方法请参考：`CButtonUI`控件

- [可用属性](#可用属性)
- [可用接口](#可用接口)



# 可用属性

| 属性名称              | 默认值 | 参数：类型 | 用途                              |
| --------------------- | ------ | ---------- | --------------------------------- |
| group                 | ""     | STRING     | 群组名                            |
| selected              | false  | BOOL       | 是否被选择                        |
| selectedimage         | ""     | STRING     | 处于已被选择状态下的图片          |
| selectedhotimage      | ""     | STRING     | 处于已被选择且为热点状态下的图片  |
| selectedpushedimage   | ""     | STRING     | 处于已被选择且为按下状态下的图片  |
| selectedforeimage     | ""     | STRING     | 处于已被选择且为热前状态下的图片  |
| selecteddisabledimage | ""     | STRING     | 处于已被选择且为禁用状态下的图片  |
| selectedbkcolor       | 0      | DWORD      | 处于已被选择状态下的背景颜色      |
| selectedbkcolor2      | 0      | DWORD      | 处于已被选择状态下的背景渐变颜色2 |
| selectedbkcolor4      | 0      | DWORD      | 处于已被选择状态下的背景渐变颜色4 |
| selectedtextcolor     | 0      | DWORD      | 处于已被选择状态下的文本颜色      |
| selectedfont          | -1     | INT        | 处于已被选择状态下的字体          |



# 可用接口int

| 接口名称              | 用途                         |
| --------------------- | ---------------------------- |
| [GetClass](#GetClass)                                  | 获取类名                         |
| [GetInterface](#GetInterface)                          | 获取接口名                       |
| [SetManager](#SetManager)                              | 设置管理 |
| [Activate](#Activate)                                  | 激活button                       |
| [SetEnabled](#SetEnabled)                              | 设置button使能开关状态           |
| [GetSelectedImage](#GetSelectedImage)                  | 获取已选择状态下的图片           |
| [SetSelectedImage](#SetSelectedImage)                  | 设置已选择的图片                 |
| [GetSelectedHotImage](#GetSelectedHotImage)            | 获取已选择且处于热状态下的图片   |
| [SetSelectedHotImage](#SetSelectedHotImage)            | 设置已选择且处于热状态下的图片   |
| [GetSelectedPushedImage](#GetSelectedPushedImage)      | 获取已选择且处于按下状态的图片   |
| [SetSelectedPushedImage](#SetSelectedPushedImage)      | 设置已选择且处于按下状态的图片   |
| [SetSelectedTextColor](#SetSelectedTextColor)          | 设置已选择状态下的文本颜色       |
| [GetSelectedTextColor](#GetSelectedTextColor)          | 获取已选择状态下的文本颜色       |
| [SetSelectedBkColor](#SetSelectedBkColor)              | 设置已选择状态下的背景颜色       |
| [GetSelectBkColor](#GetSelectBkColor)                  | 获取已选择状态下的背景颜色       |
| [SetSelectedBkColor2](#SetSelectedBkColor2)            | 设置已选择状态下的背景渐变颜色2 |
| [GetSelectBkColor2](#GetSelectBkColor2)                | 获取已选择状态下的背景渐变颜色2 |
| [SetSelectedBkColor4](#SetSelectedBkColor4)            | 设置已选择状态下的背景渐变颜色4 |
| [GetSelectBkColor4](#GetSelectBkColor4)                | 获取已选择状态下的背景渐变颜色4 |
| [GetSelectedForedImage](#GetSelectedForedImage)        | 获取已选择且处于前置状态下的图片 |
| [SetSelectedForedImage](#SetSelectedForedImage)        | 设置已选择且处于前置状态下的图片 |
| [SetSelectedDisabledImage#](#SetSelectedDisabledImage) | 设置已选择且处于禁用状态下图片   |
| [GetGroup](#GetGroup)                                  | 获取群组名                       |
| [SetGroup](#SetGroup)                                  | 设置群组名                       |
| [IsSelected](#IsSelected)                              | 是否被选择                       |
| [Selected](#Selected)                                  | 设置选择状态                     |
| [SetAttribute](#SetAttribute)                          | 设置属性                         |
| [PaintBkColor](#PaintBkColor)                          | 绘画背景颜色                     |
| [PaintStatusImage](#PaintStatusImage)                  | 绘画状态图                       |
| [PaintForeImage](#PaintForeImage)                      | 绘画热前图                       |
| [PaintText](#PaintText)                                | 添加文字                         |
| [SetSelected](#SetSelected)                            | 设置是否处于被选择状态           |
| [GetToolTip](#GetToolTip)                              | 获取文本悬浮提示语               |



### GetClass

获取类名

```c++
LPCTSTR GetClass() const;
```

- 参数：无

- 返回值：类名字符串，如“CButtonUI”

## GetInterface

获取DuiLib项目下CButtonUI类中的接口。

```c++
LPVOID GetInterface(LPCTSTR pstrName);
```

- 参数：`pstrName` 待查询指针类型字符串名

- 返回值：接口名字符串，如“GetHotImage”

## SetManager

设置管理

```c++
void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true) override;
```

- 参数：`pManager` 、`pParent`、`bInit` 待设置是否初始化的逻辑值

- 返回值：无

## Activate

激活button

```c++
bool Activate() override;
```

- 参数：无

- 返回值：激活状态的逻辑值

## SetEnabled

设置button使能开关状态

```c++
void SetEnabled(bool bEnable = true) override;
```

- 参数：`bEnable` 是button使能状态

- 返回值：无

## GetSelectedImage

获取已选择状态下的图片

```c++
LPCTSTR GetSelectedImage();
```

- 参数：无

- 返回值：已选择状态下的图片路径信息

## SetSelectedImage

设置已选择的图片

```c++
void SetSelectedImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage`已选择状态下的图片路径信息

- 返回值：无

## GetSelectedHotImage

获取已选择且处于热状态下的图片

```c++
LPCTSTR GetSelectedHotImage();
```

- 参数：无
- 返回值：已选择且处于热状态下的图片的路径信息

## SetSelectedHotImage

设置已选择且处于热状态下的图片

```c++
void SetSelectedHotImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetSelectedPushedImage

获取已选择且处于按下状态的图片

```c++
LPCTSTR GetSelectedPushedImage();
```

- 参数：无

- 返回值：已选择且处于按下状态的图片路径字符串

## SetSelectedPushedImage

设置已选择且处于按下状态的图片

```c++
void SetSelectedPushedImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage` 待查询指针类型图片路径字符串

- 返回值：无

## GetSelectedTextColor

获取已选择状态下的文本颜色

```c++
DWORD GetSelectedTextColor();
```

- 参数：无

- 返回值：已选择状态下的文本颜色值

## SetSelectedTextColor

设置已选择状态下的文本颜色

```c++
void SetSelectedTextColor(DWORD dwTextColor);
```

- 参数：`dwTextColor` 待查询颜色值

- 返回值：无

## GetSelectBkColor

获取已选择状态下的背景颜色

```c++
DWORD GetSelectBkColor();
```

- 参数：无

- 返回值：已选择状态下的背景颜色值

## SetSelectedBkColor

设置已选择状态下的背景颜色

```c++
void SetSelectedBkColor(DWORD dwBkColor);
```

- 参数：`dwTextColor` 待查询颜色值

- 返回值：无

## GetSelectBkColor2

获取已选择状态下的背景颜色

```c++
DWORD GetSelectBkColor2();
```

- 参数：无

- 返回值：已选择状态下的背景颜色值

## SetSelectedBkColor2

设置已选择状态下的背景颜色

```c++
void SetSelectedBkColor2(DWORD dwBkColor);
```

- 参数：`dwTextColor` 待查询颜色值

- 返回值：无

## GetSelectBkColor4

获取已选择状态下的背景颜色

```c++
DWORD GetSelectBkColor4();
```

- 参数：无

- 返回值：

## SetSelectedBkColor4

设置已选择状态下的背景颜色

```c++
void SetSelectedBkColor4(DWORD dwBkColor);
```

- 参数：`dwTextColor` 待查询颜色值

- 返回值：无

## GetSelectedForedImage

获取已选择且处于前置状态下的图片

```c++
LPCTSTR GetSelectedForedImage();
```

- 参数：无

- 返回值：已选择且处于前置状态下的图片路径

## SetSelectedForedImage

设置已选择且处于前置状态下的图片

```c++
void SetSelectedForedImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage`待查询的指针类型图片路径字符串

- 返回值：无

## SetSelectedDisabledImage

设置已选择且处于禁用状态下图片

```c++
void SetSelectedDisabledImage(LPCTSTR pStrImage);
```

- 参数：`pStrImage`待查询的指针类型图片路径字符串

- 返回值：无

## GetGroup

获取群组名

```c++
LPCTSTR GetGroup() const;
```

- 参数：无

- 返回值：群组名字符串

## SetGroup

设置群组名

```c++
void SetGroup(LPCTSTR pStrGroupName = nullptr);
```

- 参数：`pStrGroupName` 待设置的指针类型群组名字符串

- 返回值：无

## IsSelected

是否处于已选择状态

```c++
bool IsSelected() const;
```

- 参数：无

- 返回值：true已选择/false未选择

## Selected

设置选择状态

```c++
virtual void `Selected`(bool bSelected, bool bforce = true);
```

- 参数：`bSelected`待设置是否为被选择状态的逻辑值、`bforce`待设置是否为强制类型的逻辑值

- 返回值：无

## SetAttribute

设置属性

```c++
void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
```

- 参数：`pstrName` 属性名 、`pstrValue` 属性值

- 返回值：无

## PaintBkColor

绘画背景颜色

```c++
void PaintBkColor(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## PaintStatusImage

绘画状态图

```c++
void PaintStatusImage(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## PaintForeImage

绘画前置图

```c++
void PaintForeImage(HDC hDC);
```

- 参数：``hDc`设备场景句柄`

- 返回值：无

## PaintText

添加文字

```c++
void PaintText(HDC hDC);
```

- 参数：`hDc`设备场景句柄

- 返回值：无

## SetSelected

设置是否被选择

```c++
void SetSelected(bool bSelected);
```

- 参数：`bSelected`待判断是否被选择的逻辑值

- 返回值：无

## GetToolTip

获取文本悬浮提示语

```c++
CDuiString GetToolTip() const override;
```

- 参数：无

- 返回值：hover tips展示的提示语字符串

