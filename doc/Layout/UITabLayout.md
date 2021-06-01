# UITabLayout（页签布局）

`CTabLayoutUI` 继承了 `CContainerUI` 控件属性和方法，更多可用属性和方法请参考：CContainerUI 控件

  - [可用属性](#可用属性)
  - [可用接口](#可用接口)
## 可用属性
| 属性名称 | 默认值 | 参数类型 | 用途 |
| :--- | :--- | :--- | :--- |
| selectedid | -1 | int | 当前选中页签index |
## 可用接口

| 接口名称 | 用途 |
| :--- | :--- |
| [GetClass](#GetClass) | 获取类名 |
| [GetInterface](#GetInterface) | 获取指定类型指针 |
| [Add](#Add) | 添加页签 |
| [AddAt](#AddAt) | 添加页签到指定位置 |
| [Remove](#Remove) | 移除页签 |
| [RemoveAll](#RemoveAll) | 移除所有页签 |
| [GetCurSel](#GetCurSel) | 查询当前选中页签索引 |
| [SelectItem](#SelectItem) | 按index选中指定页签 |
| [SelectItem](#SelectItemA) | 按页签对象指针选中指定页签 |
| [SetPos](#SetPos) | 设置位置 |
| [SetAttribute](#SetAttribute) | 设置属性 |
| [AddSubPageWithName](#AddSubPageWithName) | 添加命名子页签 |
| [SelectPageByName](#SelectPageByName) | 按名称选择页签 |
| [GetCurrentPageName](#GetCurrentPageName) | 查询当前页签名称 |
| [SelectItemByName](#SelectItemByName) | 按控件名称选择页签 |


### GetClass

获取类名

```cpp
LPCTSTR GetClass() const;
```

 - 参&emsp;数：无
 - 返回值：类名字符串

### GetInterface

获取指定类型指针

```cpp
LPVOID GetInterface(LPCTSTR pstrName);
```

 - 参&emsp;数：
      - `pstrName` 待查询指针类型字符串名
 - 返回值：void* 对象指定类型指针

### Add

添加页签

```cpp
bool Add(CControlUI *pControl) override;
```

 - 参&emsp;数：
      - `pControl` 页签对象指针
 - 返回值：是否添加成功，true:成功，false:失败

### AddAt

添加页签到指定位置

```cpp
bool AddAt(CControlUI *pControl, int iIndex) override;
```

 - 参&emsp;数：
      - `pControl` 页签对象指针
      - `iIndex` 页签添加位置，从0开始
 - 返回值：是否添加成功，true:成功，false:失败

### Remove

移除页签

```cpp
bool Remove(CControlUI *pControl) override;
```

 - 参&emsp;数：
      - `pControl` 待移除页签对象指针
 - 返回值：是否移除成功，true:成功，false:失败

### RemoveAll

移除所有页签

```cpp
void RemoveAll() override;
```

 - 参&emsp;数：无
 - 返回值：无

### GetCurSel

查询当前选中页签索引

```cpp
int GetCurSel();
```

 - 参&emsp;数：无
 - 返回值：当前选中页签索引

### SelectItem

按index选中指定页签

```cpp
virtual bool SelectItem(int iIndex);
```

 - 参&emsp;数：
      - `iIndex` 待选中页签索引
 - 返回值：是否选中成功，true:成功，false:失败

### SelectItem<a name="SelectItemA"></a>

按页签对象指针选中指定页签

```cpp
virtual bool SelectItem(CControlUI *pControl);
```

 - 参&emsp;数：
      - `pControl` 待选中页签对象指针
 - 返回值：是否选中成功，true:成功，false:失败

### SetPos

设置位置

```cpp
void SetPos(RECT rc, bool bNeedInvalidate);
```

 - 参&emsp;数：
      - `rc` 位置区域
      - `bNeedInvalidate` 是否需要刷新界面
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

### AddSubPageWithName

添加命名子页签

```cpp
bool AddSubPageWithName(CControlUI *subpage, const wchar_t *page_name);
```

 - 参&emsp;数：
      - `subpage` 页签对象指针
      - `page_name` 页签名称
 - 返回值：是否添加成功，true:成功，false:失败

### SelectPageByName

按名称选择页签

```cpp
bool SelectPageByName(const wchar_t *page_name);
```

 - 参&emsp;数：
      - `page_name` 页签名称
 - 返回值：是否选中成功，true:成功，false:失败

### GetCurrentPageName

查询当前页签名称

```cpp
std::wstring GetCurrentPageName();
```

 - 参&emsp;数：无
 - 返回值：当前页签名称

### SelectItemByName

按控件名称选择页签

```cpp
bool SelectItemByName(CDuiString _controlName);
```

 - 参&emsp;数：
      - `_controlName` 页签控件名称
 - 返回值：是否选中成功，true:成功，false:失败