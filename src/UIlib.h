// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifdef UILIB_STATIC
#define UILIB_API
#else
#if defined(UILIB_EXPORTS)
#if defined(_MSC_VER)
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API
#endif
#else
#if defined(_MSC_VER)
#define UILIB_API __declspec(dllimport)
#else
#define UILIB_API
#endif
#endif
#endif
#define UILIB_COMDAT __declspec(selectany)

#pragma warning(disable : 4251)

#if defined _M_IX86
#pragma comment(linker, \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define NAMESPACE_DUILIB_BEGIN namespace DuiLib {
#define NAMESPACE_DUILIB_END }
#define USING_NAMESPACE_DUILIB using namespace DuiLib;

#include <string>
#ifdef _UNICODE
using StdString = std::wstring;
using StdOStringStream = std::wostringstream;
using StdStringStream = std::wstringstream;
#define ToStdString std::to_wstring
#define Sprintf_s swprintf_s
#define Strlen wcslen

#else
using StdString = std::string;
using StdOStringStream = std::ostringstream;
using StdStringStream = std::stringstream;
#define ToStdString std::to_string
#define Sprintf_s sprintf_s
#define Strlen strlen
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cstddef>
#include <richedit.h>
#include <tchar.h>
#include <cassert>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <thread>
#include <gdiplus.h>
#include "Utils/Utils.h"
#include "Core/UIMarkup.h"
#include "Utils/observer_impl_base.h"
#include "Utils/UIShadow.h"
#include "Utils/UIDelegate.h"
#include "Utils/DragDropImpl.h"
#include "Utils/TrayIcon.h"
#include "Core/UIDefine.h"
#include "Core/UIResourceManager.h"
#include "Core/UIManager.h"
#include "Core/UIBase.h"
#include "Core/ControlFactory.h"
#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UICulture.h"
#include "Core/UIMainQueue.h"

#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"
#include "Utils/WinImplBase.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITileLayoutX.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UIAnimation.h"
#include "Control/UIAnimPanel.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/UIGifAnim.h"
#include "Control/UIGifAnimEx.h"

#include "Layout/UIAnimationTabLayout.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"

#include "Control/UIProgress.h"
#include "Control/UICircleShareProgress.h"
#include "Control/UISlider.h"
#include "Control/UIControlEx.h"
#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"
#include "Control/UIIPAddress.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIMenu.h"
#include "Control/UIGroupBox.h"
#include "Control/UIRollText.h"
#include "Control/UIColorPalette.h"
#include "Control/UIListEx.h"
#include "Control/UIHotKey.h"
#include "Control/UIFadeButton.h"
#include "Control/UIIcon.h"

#include "Control/UIToggleSwitch.h"
#include "Control/UIRoundedRectButton.h"
#include "Control/UILoadingControl.h"
#include "Control/UIImageAnimationControl.h"
#include "Control/UIPointsWaitControl.h"
#include "Core/UIImage.h"

#include "Layout/UIAnimationLayout.h"
#include "Layout/UIFadeTabLayout.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "GdiPlus.lib")
#pragma comment(lib, "Imm32.lib")