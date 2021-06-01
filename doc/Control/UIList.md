# CListUI（列表）

`CListUI` 继承了 `CVerticalLayoutUI` 和 `IListUI`控件属性和方法，更多可用属性和方法请参考：CVerticalLayoutUI 控件 和 IListUI 控件

  - [可用属性](#可用属性)
  - [可用接口](#可用接口)
## 可用属性
| 属性名称 | 默认值 | 参数类型 | 用途 |
| :--- | :--- | :--- | :--- |
| header | true | BOOL | 是否显示表头,如(true) |
| headerbkimage | "" | STRING | 表头背景图片 |
| scrollselect | false | BOOL | 是否随滚动改变选中项,如(false) |
| multiexpanding | false | BOOL | 是否支持多个item同时打开,如(false) |
| itemfont | -1 | INT | item的字体id,如(0) |
| itemalign | center | STRING | item对齐方式,取值left、right、center，如(center) |
| itemvalign | vcenter | STRING | item对齐方式,取值top、vcenter、bottom，如(vcenter) |
| itemendellipsis | false | BOOL | item句末显示不全是否使用...代替,如(true) |
| optimizescroll | true | BOOL | 优化滚动效果,如(true) |
| itemtextpadding | 0,0,0,0 | RECT | item文字显示的边距,如(2,2,2,2) |
| itemtextcolor | 0xFF000000 | DWORD | item字体颜色 |
| itemspecialtextcolor | 0x00000000 | DWORD | item特殊字体颜色 |
| itembkcolor | 0x00000000 | DWORD | item背景颜色 |
| itembkimage | "" | STRING | item背景图片 |
| itemaltbk | false | BOOL | item是否使用隔行交替背景 |
| itemselectedtextcolor | 0xFF000000 | DWORD | item被选中时的字体颜色 |
| itemselectedbkcolor | 0xFFC1E3FF | DWORD | item被选中时的背景颜色 |
| itemselectedimage | "" | STRING | item被选中时的背景图片 |
| itemhottextcolor | 0xFF000000 | DWORD | item鼠标悬浮时的字体颜色 |
| itemhotbkcolor | 0xFFE9F5FF | DWORD | item鼠标悬浮时的背景颜色 |
| itemhotimage | "" | STRING | item鼠标悬浮时的背景图片 |
| itemdisabledtextcolor | 0xFFCCCCCC | DWORD | item禁用时的字体颜色 |
| itemdisabledbkcolor | 0xFFFFFFFF | DWORD | item禁用时的背景颜色 |
| itemdisabledimage | "" | STRING | item禁用时的背景图片 |
| itemlinecolor | 0x00000000 | DWORD | item行分割线颜色 |
| itemlinepadding | 0,0 | DWORD,DWORD | item行分割线边距，左边距、右边距，如(2,2) |
| itemshowrowline | false | BOOL | 是否显示行线，如(true) |
| itemshowcolumnline | false | BOOL | 是否显示列线，如(true) |
| itemshowhtml | false | BOOL | item文字是否为html文本，如(true) |
| multiselect | false | BOOL | item是否支持多选，如(true) |


## 可用接口

| 接口名称 | 用途 |
| :--- | :--- |
| [GetClass](#GetClass) | 获取类名 |
| [GetInterface](#GetInterface) | 获取对象指定类型指针 |
| [GetControlFlags](#GetControlFlags) | 获取控件标识 |
| [GetCurSel](#GetCurSel) | 获取当前选择项 |
| [GetCurSelActivate](#GetCurSelActivate) | 获取双击选择项 |
| [SelectItem](#SelectItem) | 选中指定项 |
| [SelectItemActivate](#SelectItemActivate) | 双击选中指定项 |
| [SelectMultiItem](#SelectMultiItem) | 选中指定项-多选 |
| [UnSelectItem](#UnSelectItem) | 取消选中指定项 |
| [SelectAllItems](#SelectAllItems) | 选中所有项 |
| [UnSelectAllItems](#UnSelectAllItems) | 取消选中所有项 |
| [GetSelectItemCount](#GetSelectItemCount) | 获取选中所有项个数 |
| [GetNextSelItem](#GetNextSelItem) | 获取下一个选中项 |
| [GetItemAt](#GetItemAt) | 获取指定项指针 |
| [GetItemIndex](#GetItemIndex) | 查询指定控件索引 |
| [Add](#Add) | 添加列表控件 |
| [AddAt](#AddAt) | 在指定添加列表控件 |
| [Remove](#Remove) | 移除指定添加列表控件 |
| [RemoveAt](#RemoveAt) | 移除指定添加列表控件 |
| [RemoveAll](#RemoveAll) | 移除所有列表控件 |
| [EnsureVisible](#EnsureVisible) | 使指定项可见 |
| [Scroll](#Scroll) | 列表滚动指定偏移量 |
| [SetItemIndex](#SetItemIndex) | 将控件插入到指定位置 |
| [SetPos](#SetPos) | 设置位置 |
| [Move](#Move) | 按偏移量移动列表 |
| [DoEvent](#DoEvent) | 执行事件 |
| [SetAttribute](#SetAttribute) | 设置属性 |
| [LineUp](#LineUp) | 上一行 |
| [LineDown](#LineDown) | 下一行 |
| [SortItems](#SortItems) | 排序所有项 |
| [AddClipRect](#AddClipRect) | 添加矩形剪切区域 |
| [ClearClipRect](#ClearClipRect) | 清空矩形剪切区域 |


### GetClass

获取类名

```cpp
LPCTSTR GetClass() const;
```

 - 参&emsp;数：无
 - 返回值：类名字符串

### GetInterface

获取对象指定类型指针

```cpp
LPVOID GetInterface(LPCTSTR pstrName);
```

 - 参&emsp;数：
      - `pstrName` 待查询指针类型字符串名
 - 返回值：void* 对象指定类型指针

### GetControlFlags

获取控件标识

```cpp
UINT GetControlFlags() const;
```

 - 参&emsp;数：无
 - 返回值：标准控件标识

### GetCurSel

获取当前选择项

```cpp
int GetCurSel() override;
```

 - 参&emsp;数：无
 - 返回值：当前选择项index

### GetCurSelActivate

获取双击选择项

```cpp
int GetCurSelActivate() const;
```

 - 参&emsp;数：无
 - 返回值：双击选择项index

### SelectItem

选中指定项

```cpp
bool SelectItem(int iIndex, bool bTakeFocus) override;
```

 - 参&emsp;数：
      - `iIndex` 待选择项index
      - `bTakeFocus` 是否获取焦点
 - 返回值：执行是否成功，true:成功，false:失败

### SelectItemActivate

双击选中指定项

```cpp
bool SelectItemActivate(int iIndex);
```

 - 参&emsp;数：
      - `iIndex` 待选择项index
 - 返回值：执行是否成功，true:成功，false:失败

### SelectMultiItem

选中指定项-多选

```cpp
bool SelectMultiItem(int iIndex, bool bTakeFocus = false) override;
```

 - 参&emsp;数：
      - `iIndex` 待选中项index
      - `bTakeFocus` 是否获取焦点
 - 返回值：执行是否成功，true:成功，false:失败

### UnSelectItem

取消选中指定项

```cpp
bool UnSelectItem(int iIndex, bool bOthers = false) override;
```

 - 参&emsp;数：
      - `iIndex` 待取消选中项index
      - `bTakeFocus` 是否获取焦点
 - 返回值：执行是否成功，true:成功，false:失败

### SelectAllItems

选中所有项

```cpp
void SelectAllItems() override;
```

 - 参&emsp;数：无
 - 返回值：无

### UnSelectAllItems

取消选中所有项

```cpp
void UnSelectAllItems() override;
```

 - 参&emsp;数：无
 - 返回值：无

### GetSelectItemCount

获取选中所有项个数

```cpp
int GetSelectItemCount() override;
```

 - 参&emsp;数：无
 - 返回值：选中所有项个数

### GetNextSelItem

获取下一个选中项

```cpp
int GetNextSelItem(int nItem) override;
```

 - 参&emsp;数：
      - `nItem` 当前选中项
 - 返回值：成功：下一选中项index，失败：-1

### GetItemAt

获取指定项指针

```cpp
CControlUI *GetItemAt(int iIndex) override;
```

 - 参&emsp;数：
      - `iIndex` 指定项index
 - 返回值：指定项指针

### GetItemIndex

查询指定控件索引

```cpp
int GetItemIndex(CControlUI *pControl) override;
```

 - 参&emsp;数：
      - `pControl` 待查询控件指针
 - 返回值：成功：待查询控件索引，失败：-1

### Add

添加列表控件

```cpp
bool Add(CControlUI *pControl) override;
```

 - 参&emsp;数：
      - `pControl` 待添加列表控件指针
 - 返回值：执行是否成功，true:成功，false:失败

### AddAt

在指定添加列表控件

```cpp
bool AddAt(CControlUI *pControl, int iIndex) override;
```

 - 参&emsp;数：
      - `pControl` 待添加列表控件指针
      - `iIndex` 指定位置索引
 - 返回值：执行是否成功，true:成功，false:失败

### Remove

移除指定添加列表控件

```cpp
bool Remove(CControlUI *pControl) override;
```

 - 参&emsp;数：
      - `pControl` 待移除列表控件指针
 - 返回值：执行是否成功，true:成功，false:失败

### RemoveAt

移除指定添加列表控件

```cpp
bool RemoveAt(int iIndex) override;
```

 - 参&emsp;数：
      - `iIndex` 待移除列表控件索引
 - 返回值：执行是否成功，true:成功，false:失败

### RemoveAll

移除所有列表控件

```cpp
void RemoveAll() override;
```

 - 参&emsp;数：无
 - 返回值：无

### EnsureVisible

使指定项可见

```cpp
void EnsureVisible(int iIndex);
```

 - 参&emsp;数：
      - `iIndex` 待可见项索引
 - 返回值：无

### Scroll

列表滚动指定偏移量

```cpp
void Scroll(int dx, int dy);
```

 - 参&emsp;数：
      - `dx` x方向偏移量
      - `dy` y方向偏移量
 - 返回值：无

### SetItemIndex

将控件插入到指定位置

```cpp
bool SetItemIndex(CControlUI *pControl, int iIndex) override;
```

 - 参&emsp;数：
      - `pControl` 控件指针
      - `iIndex` 位置index索引
 - 返回值：执行是否成功，true:成功，false:失败

### SetPos

设置位置

```cpp
void SetPos(RECT rc, bool bNeedInvalidate);
```

 - 参&emsp;数：
      - `rc` 位置区域
      - `bNeedInvalidate` 是否需要刷新界面
 - 返回值：无

### Move

按偏移量移动列表

```cpp
void Move(SIZE szOffset, bool bNeedInvalidate = true);
```

 - 参&emsp;数：
      - `szOffset` 偏移量
      - `bNeedInvalidate` 是否需要刷新界面
 - 返回值：无

### DoEvent

执行事件

```cpp
void DoEvent(TEventUI &event) override;
```

 - 参&emsp;数：
      - `event` 事件对象
 - 返回值：无

### SetAttribute

设置属性

```cpp
void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
```

 - 参&emsp;数：
      - `pstrName` 属性名
      - `pstrValue` 属性值
 - 返回值：无

### LineUp

上一行

```cpp
void LineUp() override;
```

 - 参&emsp;数：无
 - 返回值：无

### LineDown

下一行

```cpp
void LineDown() override;
```

 - 参&emsp;数：无
 - 返回值：无

### SortItems

排序所有项

```cpp
BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);
```

 - 参&emsp;数：
      - `pfnCompare` 对比函数
      - `dwData` 保留
 - 返回值：执行是否成功，TRUE:成功，FALSE:失败

### AddClipRect

添加矩形剪切区域

```cpp
void AddClipRect(const RECT &rc);
```

 - 参&emsp;数：
      - `rc` 矩形检测区域
 - 返回值：无

### ClearClipRect

清空矩形剪切区域

```cpp
void ClearClipRect();
```

 - 参&emsp;数：无
 - 返回值：无