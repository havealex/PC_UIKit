# WinImplBase (通用窗口类)

`WindowImplBase`继承自`CWindowWnd`,`CNotifyPump`,`INotifyUI`,`IMessageFilterUI`,`IDialogBuilderCallback`，`IQueryControlText`。

- [可用属性](#可用属性)
- [可用接口](#可用接口)

## 可用属性

| 属性名称 | 默认值  | 参数类型        | 用途                                 |
| :------- | :------ | :-------------- | :----------------------------------- |
| m_pm     | nullptr | CPaintManagerUI | 负责界面绘制工作、消息预处理、过滤。 |

## 可用接口

| 接口名称                                                  | 用途                                                     |
| :-------------------------------------------------------- | :------------------------------------------------------- |
| [InitWindow](#InitWindow)                                 | 窗口创建后，显示前的初始化操作                           |
| [OnFinalMessage](#OnFinalMessage)                         | 在窗口收到WM_NCDESTROY消息时会被调用                     |
| [Notify](#Notify)                                         | INotifyUI接口实现                                        |
| [CenterToControl](#CenterToControl)                       | 将窗口移动到某个控件的中心                               |
| [OnClick](#OnClick)                                       | 响应控件点击事件                                         |
| [CenterToParentWindow](#)                                 | 将窗口移动到父窗口中心                                   |
| [Move](#Move)                                             | 移动窗口                                                 |
| [AnimateMove](#AnimateMove)                               | 以动画形式移动窗口                                       |
| [Resize](#Resize)                                         | 改变窗口大小                                             |
| [AnimateResize](#AnimateResize)                           | 以动画形式改变窗口大小                                   |
| [GetRect](#GetRect)                                       | 获取窗口的屏幕坐标                                       |
| [EnsureVisible](#EnsureVisible)                           | 确保窗口在屏幕中可见                                     |
| [BringToForeground](#BringToForeground)                   | 置顶窗口                                                 |
| [GetClassStyle](#GetClassStyle)                           | 获取窗口类样式                                           |
| [MessageHandler](#MessageHandler)                         | 消息预处理                                               |
| [OnClose](#OnClose)                                       | 窗口关闭时的处理                                         |
| [OnDestroy](#OnDestroy)                                   | 窗口销毁时的处理                                         |
| [OnActivate](#OnActivate)                                 | 窗口被激活或未激活时的处理                               |
| [OnDisplayResolutionChanged](#OnDisplayResolutionChanged) | 显示分辨率变更时的处理                                   |
| [OnSize](#OnSize)                                         | 窗口大小变更时的处理                                     |
| [OnChar](#OnChar)                                         | 键盘按下时的处理                                         |
| [OnSysCommand](#OnSysCommand)                             | 用户点击系统菜单(最大化、最小化、关闭或恢复)后的消息处理 |
| [OnCreate](#OnCreate)                                     | 窗口创建后、显示前的处理                                 |
| [OnKeyDown](#OnKeyDown)                                   | 非系统键按下后的处理                                     |
| [OnKillFocus](#OnKillFocus)                               | 窗口失去键盘焦点后的处理                                 |
| [OnSetFocus](#OnSetFocus)                                 | 窗口获得键盘焦点后的处理                                 |
| [OnLButtonDown](#OnLButtonDown)                           | 窗口客户区鼠标左键点击后的处理                           |
| [OnLButtonUp](#OnLButtonUp)                               | 窗口客户区鼠标左键释放后的处理                           |
| [OnMouseMove](#OnMouseMove)                               | 鼠标移动时的处理                                         |
| [HandleMessage](#HandleMessage)                           | 消息处理                                                 |
| [HandleCustomMessage](#HandleCustomMessage)               | 自定义消息处理                                           |
| [MessageSysTreat](#MessageSysTreat)                       | 系统消息处理                                             |
| [MessageDeviceTreat](#MessageDeviceTreat)                 | 设备消息处理                                             |
| [MessageNormalTreat](#MessageNormalTreat)                 | 常规消息处理                                             |
| [GetStyle](#GetStyle)                                     | 获取窗口样式                                             |
| [OnResize](#OnResize)                                     | 窗口大小变化后的处理                                     |
| [OnResizeAnimationEnd](#OnResizeAnimationEnd)             | 窗口大小变化动画结束处理                                 |
| [OnMoveAnimationEnd](#OnMoveAnimationEnd)                 | 窗口移动动画结束处理                                     |



### InitWindow

```cpp
virtual void InitWindow();
```

### OnFinalMessage

```cpp
virtual void OnFinalMessage(HWND hWnd);
```

 - 参&emsp;数：
   - `hWnd` 失效区域
 - 返回值：无

### Notify

```cpp
void Notify(TNotifyUI &msg) override;
```

 - 参&emsp;数：
   - `msg` 消息封装结构体
 - 返回值：无

### CenterToControl

```cpp
void CenterToControl(CControlUI *ctrl);
```

 - 参&emsp;数：
   - `ctrl` 目标控件指针
 - 返回值：无

### OnClick

```cpp
virtual void OnClick(TNotifyUI &msg);
```

 - 参&emsp;数：
   - `msg` 消息封装结构体
 - 返回值：无

### CenterToParentWindow

```cpp
void CenterToParentWindow();
```

### Move

```cpp
void Move(int x, int y);
```

 - 参&emsp;数：
   - `x` 目标横坐标
   - `y` 目标纵坐标
 - 返回值：无

### AnimateMove

```cpp
void AnimateMove(int x, int y, EasingCurve easingCurve = Linear, int timeElapse = 500);
```

 - 参&emsp;数：
   - `x` 目标横坐标
   - `y` 目标纵坐标
   - `easingCurve` 动画类型
   - `timeElapse` 动画时间
 - 返回值：无

### Resize

```cpp
void Resize(int w, int h, AnimateResizePolicy keepInScreen = FixToLeft);
```

 - 参&emsp;数：
   - `w` 目标宽度
   - `h` 目标高度
   - `keepInScreen` 固定四周类型
 - 返回值：无

### AnimateResize

```cpp
void AnimateResize(int w, int h, EasingCurve easingCurve = Linear, int timerElapse, AnimateResizePolicy keepInScreen = FixToLeft);
```

 - 参&emsp;数：
   - `w` 目标宽度
   - `h` 目标高度
   - `easingCurve` 动画类型
   - `keepInScreen` 固定四周类型
 - 返回值：无

### GetRect

```cpp
RECT GetRect() const;
```

 - 参&emsp;数：无
 - 返回值：窗口的屏幕坐标

### EnsureVisible

```cpp
void EnsureVisible();
```

### BringToForeground

```cpp
void BringToForeground();
```

### GetClassStyle

```cpp
virtual UNIT GetClassStyle() const;
```

 - 参&emsp;数：无
 - 返回值：窗口类样式

### MessageHandler

```cpp
virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnClose

```cpp
virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnDestroy

```cpp
virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnActivate

```cpp
virtual LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnDisplayResolutionChanged

```cpp
virtual void OnDisplayResolutionChanged(UINT width, UINT height);
```

 - 参&emsp;数：
   - `width` 目标宽度
   - `height` 目标高度
 - 返回值：long

### OnSize

```cpp
virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnChar

```cpp
virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnSysCommand

```cpp
virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnCreate

```cpp
virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnKeyDown

```cpp
virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnKillFocus

```cpp
virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnSetFocus

```cpp
virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnLButtonDown

```cpp
virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnLButtonUp

```cpp
virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### OnMouseMove

```cpp
virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### HandleMessage

```cpp
virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
 - 返回值：long

### HandleCustomMessage

```cpp
virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bHandled` 是否被处理
 - 返回值：long

### MessageSysTreat

```cpp
virtual bool MessageSysTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `lRes` 结果值
   - `bHandled` 是否被处理
 - 返回值：true/false

### MessageDeviceTreat

```cpp
virtual bool MessageDeviceTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `lRes` 结果值
   - `bHandled` 是否被处理
 - 返回值：true/false

### MessageNormalTreat

```cpp
virtual bool MessageNormalTreat(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes, bool &bHandled);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `lRes` 结果值
   - `bHandled` 是否被处理
 - 返回值：true/false

### GetStyle

```cpp
virtual LONG GetStyle();
```

 - 参&emsp;数：无
 - 返回值：窗口样式

### OnResize

```cpp
virtual void OnResize(const SIZE &sz);
```

 - 参&emsp;数：
   - `sz` 目标窗口尺寸
 - 返回值：无

### OnResizeAnimationEnd

```cpp
virtual void OnResizeAnimationEnd();
```

### OnMoveAnimationEnd

```cpp
virtual void OnMoveAnimationEnd();
```
