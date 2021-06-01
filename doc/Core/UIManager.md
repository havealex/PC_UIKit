# CPaintManagerUI (绘制管理器)

`CPaintManagerUI`主要负责界面绘制工作、消息预处理、过滤。

- [可用接口](#可用接口)

## 可用接口
| 接口名称 | 用途 |
| :--- | :--- |
| [Init](#Init) | 初始化绘制管理器 |
| [IsUpdateNeeded](#IsUpdateNeeded) | 获取当前界面是否需要更新 |
| [Invalidate](#Invalidate) | 指定区域失效 |
| [GetPaintWindow](#GetPaintWindow) | 获取窗口句柄 |
| [GetClientSize](#GetClientSize) | 获取窗口客户区大小 |
| [GetInitSize](#GetInitSize) | 获取窗口初始化大小 |
| [SetInitSize](#SetInitSize) | 设置窗口初始化大小 |
| [GetRoundCorner](#GetRoundCorner) | 设置窗口四角的圆角弧度 |
| [SetRoundCorner](#SetRoundCorner)                 | 获取窗口四角的圆角弧度 |
| [IsLayered](#IsLayered) | 获取窗口是否支持透明 |
| [SetLayered](#SetLayered) | 设置窗口是否支持透明 |
| [SetInstance](#SetInstance) | 设置应用程序的实例句柄 |
| [SetResourcePath](#SetResourcePath) | 设置资源路径 |
| [AttachDialog](#AttachDialog) | 将控件附加到绘制管理器 |
| [GetFocus](#GetFocus) | 获取焦点状态的控件 |
| [SetFocus](#SetFocus) | 设置控件为获得焦点状态 |
| [SetFocusNeeded](#SetFocusNeeded) | 设置控件为需要绘制焦点 |
| [SetTimer](#SetTimer) | 为指定控件以及其子控件设置定时器 |
| [KillTimer](#KillTimer) | 移除指定控件上的指定编号的定时器 |
| [RemoveAllTimers](#RemoveAllTimers) | 清空所有的定时器 |
| [AddNotifier](#AddNotifier) | 添加控件到通知集合中 |
| [RemoveNotifier](#RemoveNotifier) | 将控件从通知集合中移除 |
| [SendNotify](#SendNotify1) | 发送同步/异步通知 |
| [SendNotify](#SendNotify2) | 构建同步/异步通知并发送 |
| [AddPreMessageFilter](#AddPreMessageFilter) | 向预处理消息过滤器链中添加消息过滤器 |
| [RemovePreMessageFilter](#RemovePreMessageFilter) | 从预处理消息过滤器链合中移除指定的消息过滤器 |
| [AddMessageFilter](#AddMessageFilter) | 向消息过滤器链中添加消息过滤器 |
| [RemoveMessageFilter](#RemoveMessageFilter) | RemoveMessageFilter |
| [MessageLoop](#MessageLoop) | 消息循环 |
| [TranslateMessage](#TranslateMessage) | 消息翻译，在Win32原有的消息转换基础上，将需要自己处理的消息转发给消息预处理器 |
| [PreMessageHandler](#PreMessageHandler) | 消息预处理器 |
| [SetBlurMode](#SetBlurMode) | 设置亚克力效果 |

### Init

```cpp
void Init(HWND hWnd, LPCTSTR pstrName = NULL);
```

 - 参&emsp;数：
        - `hwnd` 窗口句柄
        - `pstrName` 绘制管理器名称
 - 返回值：无

### IsUpdateNeeded

```cpp
bool IsUpdateNeeded() const;
```

 - 参&emsp;数：无
 - 返回值：true/false

### Invalidate

```cpp
void Invalidate(RECT &rcItem);
```

 - 参&emsp;数：
      - `rcItem` 失效区域
 - 返回值：无

### GetPaintWindow

```cpp
HWND GetPaintWindow() const;
```

 - 参&emsp;数：无
 - 返回值：窗口句柄

### GetClientSize

```cpp
SIZE GetClientSize() const;
```

 - 参&emsp;数：无
 - 返回值：窗口客户区大小

### GetInitSize

```cpp
SIZE GetInitSize() const;
```

 - 参&emsp;数：无
 - 返回值：窗口初始大小

### SetInitSize

```cpp
void SetInitSize(int cx, int cy);
```

 - 参&emsp;数：
      - `cx` 窗口宽度
      - `cy` 窗口高度
 - 返回值：无

### GetRoundCorner

```cpp
SIZE GetRoundCorner() const;
```

 - 参&emsp;数：无
 - 返回值：圆角弧度

### SetRoundCorner

```cpp
void SetRoundCorner(int cx, int cy);
```

 - 参&emsp;数：
      - `cx` 上下圆角弧度
      - `cy` 左右圆角弧度
 - 返回值：无

### IsLayered

```cpp
bool IsLayered();
```

 - 参&emsp;数：无
 - 返回值：true/false

### SetLayered

```cpp
void SetLayered(bool bLayered);
```

 - 参&emsp;数：
      - `bLayered` 是否支持透明
 - 返回值：无

### SetInstance

```cpp
static void SetInstance(HINSTANCE hInst);
```

 - 参&emsp;数：
      - `hInst` 应用程序实例句柄
 - 返回值：无

### SetResourcePath

```cpp
static void SetResourcePath(LPCTSTR pStrPath);
```

 - 参&emsp;数：
      - `pStrPath` 资源路径
 - 返回值：无

### AttachDialog

```cpp
bool AttachDialog(CControlUI *pControl);
```

 - 参&emsp;数：
   - `pControl` 控件指针
 - 返回值：true/false

### GetFocus

```cpp
CControlUI *GetFocus() const;
```

 - 参&emsp;数：无
 - 返回值：控件指针

### SetFocus

```cpp
void SetFocus(CControlUI *pControl);
```

 - 参&emsp;数：
   - `pControl` 控件指针
 - 返回值：无

### SetFocusNeeded

```cpp
void SetFocusNeeded(CControlUI *pControl);
```

 - 参&emsp;数：
   - `pControl` 控件指针
 - 返回值：无

### SetTimer

```cpp
bool SetTimer(CControlUI *pControl, UINT nTimerID, UINT uElapse);
```

 - 参&emsp;数：
   - `pControl` 控件指针
   - `nTimerID` 定时器编号
   - `uElapse` 时间间隔
 - 返回值：true/false

### KillTimer

```cpp
bool KillTimer(CControlUI *pControl, UNIT nTimerID);
```

 - 参&emsp;数：
   - `pControl` 控件指针
   - `nTimerID` 定时器编号
 - 返回值：true/false

### RemoveAllTimers

```cpp
void RemoveAllTimers();
```

### AddNotifier

```cpp
bool AddNotifier(INotifyUI *pControl);
```

 - 参&emsp;数：
   - `pControl` 实现了INotifyUI接口的对象指针
 - 返回值：true/false

### RemoveNotifier

```cpp
bool RemoveNotifier(INotifyUI *pControl);
```

 - 参&emsp;数：
   - `pControl` 实现了INotifyUI接口的对象指针
 - 返回值：true/false

### SendNotify<a name="SendNotify1"></a>

```cpp
void SendNotify(TNotifyUI &msg, bool bAsync = false);
```

 - 参&emsp;数：
   - `msg` 消息对象
   - `bAsync` 是否异步
 - 返回值：无

### SendNotify<a name="SendNotify2"></a>

```cpp
void SendNotify(CControlUI *pControl, LPCTSTR pStrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);
```

 - 参&emsp;数：
   - `pControl` 控件指针
   - `pStrMessage` 消息类型
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `bAsync` 是否异步
 - 返回值：无

### AddPreMessageFilter

```cpp
bool AddPreMessageFilter(IMessageFilterUI *pFilter);
```

 - 参&emsp;数：
   - `pFilter` 实现了IMessageFilterUI接口的对象指针
 - 返回值：true/false

### RemovePreMessageFilter

```cpp
bool RemovePreMessageFilter(IMessageFilterUI *pFilter);
```

 - 参&emsp;数：
   - `pFilter` 实现了IMessageFilterUI接口的对象指针
 - 返回值：true/false

### AddMessageFilter

```cpp
bool AddMessageFilter(IMessageFilterUI *pFilter);
```

 - 参&emsp;数：
   - `pFilter` 实现了IMessageFilterUI接口的对象指针
 - 返回值：true/false

### RemoveMessageFilter

```cpp
bool RemoveMessageFilter(IMessageFilterUI *pFilter);
```

 - 参&emsp;数：
   - `pFilter` 实现了IMessageFilterUI接口的对象指针
 - 返回值：true/false

### MessageLoop

```cpp
static void MessageLoop();
```

### TranslateMessage

```cpp
static bool TranslateMessage(const LPMSG pMsg);
```

 - 参&emsp;数：
   - `pMsg` 消息对象指针
 - 返回值：true/false

### PreMessageHandler

```cpp
bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
```

 - 参&emsp;数：
   - `uMsg` 消息编号
   - `wParam` 消息解析参数
   - `lParam` 消息解析参数
   - `lRes` 是否被处理
 - 返回值：true/false

### SetBlurMode

```cpp
static void SetBlurMode(HWND hwnd, ACCENT_STATE mode, DWORD color);
```

 - 参&emsp;数：
   - `hwnd` 窗口句柄
   - `mode` 特效模式
   - `color` 颜色
 - 返回值：无

