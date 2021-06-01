# UICulture（多语言类）

`UICulture` 类封装了icu4c接口，对外提供多语言能力

## 可用接口

| 接口名称 | 用途 |
| :--- | :--- |
| [GetInstance](#GetInstance) | 获取UICulture单例对象指针 |
| [setResPath](#setResPath) | 设置文字资源路径 |
| [setLocale](#setLocale) | 设置当前语言 |
| [getDirection](#getDirection) | 获取icu文字方向 |
| [ParseBind](#ParseBind) | 解析绑定 IDS(字串key) 对应的字符串 |
| [GetString](#GetString) | 获取IDS(字串key)对应的字符串 |
| [GetLocalString](#GetLocalString) | 获取IDS(字串key)当前语言字符串 |
| [GetLocaleLanguage](#GetLocaleLanguage) | 获取当前语言 |
| [SetCustomFont](#SetCustomFont) | 设置默认字体 |
| [GetCustomFont](#GetCustomFont) | 查询默认字体 |
| [GetMultiVarStringForDotnet](#GetMultiVarStringForDotnet) | 返回整数向量依次填入占位符后的字符串(支持数字本地化显示，C#导入接口) |
| [GetLocalMultiVarStr](#GetLocalMultiVarStr) | 查找指定IDS当前语言下的字符串，并将count个vecNum中的整数依次填入占位符(支持数字本地化) |
| [GetMultiVarStr](#GetMultiVarStr) | 查找指定IDS当前语言下的字符串，并将count个vecNum中的整数依次填入占位符(支持数字本地化);SWITCH_LANGUAGE_TEST宏定义时，包含当前IDS所有支持语言的字符串 |
| [GetTextFromHtmlText](#GetTextFromHtmlText) | 从Html文本中提取文本，可用于Html文本宽高计算 |
| [GetMultiVarString](#GetMultiVarString) | 查找指定IDS当前语言下的字符串，并将指定数目字符串依次填入占位符 |
| [UnicodeFormat](#UnicodeFormat) | 提供Unicode编码格式化字符串 |
| [GetPercentageStr](#GetPercentageStr) | 查询百分比字符串 |
| [ToUpper](#ToUpper) | 转换为全大写字符串-支持多语言 |


### GetInstance

获取UICulture单例对象指针

```cpp
static UICulture *GetInstance();
```

 - 参&emsp;数：无
 - 返回值：UICulture单例对象指针

### setResPath

设置文字资源路径

```cpp
void setResPath(LPCTSTR path);
```

 - 参&emsp;数：
      - `path` 文字资源路径
 - 返回值：无

### setLocale

设置当前语言

```cpp
void setLocale(LPCTSTR locale, bool loadAll = true);
```

 - 参&emsp;数：
      - `locale` 宽字符格式语言，如(en-US)
      - `loadAll` 是否加载所有单复数形式
 - 返回值：无

### getDirection

获取icu文字方向

```cpp
LayoutDirection getDirection();
```

 - 参&emsp;数：无
 - 返回值：文字方向枚举类型，DIRECTION_LTR、DIRECTION_RTL、DIRECTION_MIXED，如(DIRECTION_LTR从左到右)

### ParseBind

解析绑定 IDS(字串key) 对应的字符串

```cpp
CDuiString ParseBind(LPCTSTR pstrValue);
```

 - 参&emsp;数：
      - `pstrValue` bind字符串，多用于解析xml文件中 bind属性，如_T("bind='IDS_TEST'")
 - 返回值：指定IDS当前语言下的字符串

### GetString

获取IDS(字串key)对应的字符串

```cpp
CDuiString GetString(LPCTSTR key);
```

 - 参&emsp;数：
      - `key` key字符串，如_T("IDS_TEST")
 - 返回值：指定IDS当前语言下的字符串；SWITCH_LANGUAGE_TEST宏定义时，包含当前IDS所有支持语言的字符串

### GetLocalString

获取IDS(字串key)当前语言字符串

```cpp
CDuiString GetLocalString(LPCTSTR key);
```

 - 参&emsp;数：
      - `key` key字符串，如_T("IDS_TEST")
 - 返回值：指定IDS当前语言字符串

### GetLocaleLanguage

获取当前语言

```cpp
std::wstring GetLocaleLanguage();
```

 - 参&emsp;数：无
 - 返回值：当前语言字符串，如L"en_us"

### SetCustomFont

设置默认字体

```cpp
void SetCustomFont(wstring _fontstr)
```

 - 参&emsp;数：
      - `_fontstr` 默认字体字符串，如(L"japan)
 - 返回值：无

### GetCustomFont

查询默认字体

```cpp
wstring GetCustomFont()
```

 - 参&emsp;数：无
 - 返回值：默认字体字符串，如(L"japan)

### GetMultiVarStringForDotnet

返回整数向量依次填入占位符后的字符串(支持数字本地化显示，C#导入接口)

```cpp
std::wstring GetMultiVarStringForDotnet(const wstring& localString,
     const vector<int>& nums, const string& language);
```

 - 参&emsp;数：
      - `localString` 带占位符字符串
      - `nums` 整数向量
      - `language` 语言，如"en-US"
 - 返回值：整数向量依次填入占位符后的字符串

### GetLocalMultiVarStr

查找指定IDS当前语言下的字符串，并将count个vecNum中的整数依次填入占位符(支持数字本地化)

```cpp
std::wstring GetLocalMultiVarStr(const wstring &strIDS, const vector<int> &vecNum, int count = 0);
```

 - 参&emsp;数：
      - `strIDS` key字符串，如_T("IDS_TEST")
      - `vecNum` 整数向量
      - `count` 整数个数
 - 返回值：整数向量依次填入IDS映射字符串占位符后的字符串

### GetMultiVarStr

查找指定IDS当前语言下的字符串，并将count个vecNum中的整数依次填入占位符(支持数字本地化);SWITCH_LANGUAGE_TEST宏定义时，包含当前IDS所有支持语言的字符串

```cpp
std::wstring GetMultiVarStr(wstring strIDS, vector<int> vecNum, int count = 0);
```

 - 参&emsp;数：
      - `strIDS` key字符串，如_T("IDS_TEST")
      - `vecNum` 整数向量
      - `count` 整数个数
 - 返回值：整数向量依次填入IDS映射字符串占位符后的字符串

### GetTextFromHtmlText

从Html文本中提取文本，可用于Html文本宽高计算

```cpp
wstring GetTextFromHtmlText(CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont);
```

 - 参&emsp;数：
      - `pManager` 绘图管理器指针
      - `pstrText` html文本
      - `iFont` 字体id，如(106)
 - 返回值：去除html标签后的文本

### GetMultiVarString

查找指定IDS当前语言下的字符串，并将指定数目字符串依次填入占位符

```cpp
CDuiString GetMultiVarString(LPCTSTR originalString, ...);
```

 - 参&emsp;数：
      - `originalString` key字符串，如_T("IDS_TEST")
      - `...` 依次为参数个数，需要填入到占位符的字符串依次排列
 - 返回值：参数字符串依次填入IDS映射字符串占位符后的字符串

### UnicodeFormat

提供Unicode编码格式化字符串

```cpp
int32_t UnicodeFormat(const string &pstrFormat, wstring &unicodeChar, va_list vaList);
```

 - 参&emsp;数：
      - `pstrFormat` 格式化字符串
      - `unicodeChar` 输出Unicode编码字符串
      - `vaList` 参数列表
 - 返回值：成功字符个数，失败负数

### GetPercentageStr

查询百分比字符串

```cpp
std::wstring GetPercentageStr(const int percentage);
```

 - 参&emsp;数：
      - `percentage` 百分比数字
 - 返回值：本地化百分比字符串

### ToUpper

转换为全大写字符串-支持多语言

```cpp
std::wstring ToUpper(const wstring &src);
```

 - 参&emsp;数：
      - `src` 源字符串
 - 返回值：全大写字符串转换后的全大写字符串