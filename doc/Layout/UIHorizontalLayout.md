# CHorizontalLayoutUI（水平布局）

`CHorizontalLayoutUI` 继承了 `CContainerUI` 控件属性和方法，更多可用属性和方法请参考：CContainerUI 控件

  - [可用属性](#可用属性)
  - [可用接口](#可用接口)
## 可用属性
| 属性名称 | 默认值 | 参数类型 | 用途 |
| :--- | :--- | :--- | :--- |
| sepwidth | 0 | INT | 分隔符宽,正负表示分隔符在左边还是右边,如(-4) |
| sepimm | false | BOOL | 拖动分隔符是否立即改变大小,如(false) |
## 可用接口

| 接口名称 | 用途 |
| :--- | :--- |
| [GetClass](#GetClass) | 获取类名 |
| [GetInterface](#GetInterface) | 获取对象指定类型指针 |
| [GetControlFlags](#GetControlFlags) | 获取控件标识 |
| [SetSepWidth](#SetSepWidth) | 设置分隔符宽度 |
| [GetSepWidth](#GetSepWidth) | 获取分隔符宽度 |
| [SetSepImmMode](#SetSepImmMode) | 设置拖动分隔符是否立即改变大小 |
| [IsSepImmMode](#IsSepImmMode) | 查询拖动分隔符是否立即改变大小 |
| [SetAttribute](#SetAttribute) | 设置属性 |
| [DoEvent](#DoEvent) | 处理事件 |
| [SetPos](#SetPos) | 设置位置 |
| [DoPostPaint](#DoPostPaint) | 执行最上层绘制 |
| [GetThumbRect](#GetThumbRect) | 获取分割区域 |
| [EstimateSize](#EstimateSize) | 计算控件的宽高 |


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

### SetSepWidth

设置分隔符宽度

```cpp
void SetSepWidth(int iWidth);
```

 - 参&emsp;数：
      - `iWidth` 分隔符宽,正负表示分隔符在左边还是右边,如(-4)
 - 返回值：无

### GetSepWidth

获取分隔符宽度

```cpp
int GetSepWidth() const;
```

 - 参&emsp;数：无
 - 返回值：分隔符宽度

### SetSepImmMode

设置拖动分隔符是否立即改变大小

```cpp
void SetSepImmMode(bool bImmediately);
```

 - 参&emsp;数：
      - `bImmediately` 设置拖动分隔符是否立即改变大小，true:是，false:否
 - 返回值：无

### IsSepImmMode

查询拖动分隔符是否立即改变大小

```cpp
bool IsSepImmMode() const;
```

 - 参&emsp;数：无
 - 返回值：true/false

### SetAttribute

设置属性

```cpp
void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
```

 - 参&emsp;数：
      - `pstrName` 属性名
      - `pstrValue` 属性值
 - 返回值：无

### DoEvent

处理事件

```cpp
void DoEvent(TEventUI &event) override;
```

 - 参&emsp;数：
      - `event` TEventUI类型引用对象
 - 返回值：无

### SetPos

设置位置

```cpp
void SetPos(RECT rc, bool bNeedInvalidate);
```

 - 参&emsp;数：
      - `rc` 位置区域
      - `bNeedInvalidate` 是否需要刷新界面
 - 返回值：无

### DoPostPaint

执行最上层绘制

```cpp
void DoPostPaint(HDC hDC, const RECT &rcPaint);
```

 - 参&emsp;数：
      - `hDC` 特定窗口上下文句柄
      - `rcPaint` 绘制区域
 - 返回值：无

### GetThumbRect

获取分割区域

```cpp
RECT GetThumbRect(bool bUseNew = false) const;
```

 - 参&emsp;数：
      - `bUseNew` 是否使用新区域进行计算
 - 返回值：分割区域

### EstimateSize

计算控件的宽高

```cpp
SIZE EstimateSize(SIZE szAvailable);
```

 - 参&emsp;数：
      - `szAvailable` 可用区域
 - 返回值：计算所得控件的宽高