/*
 *    创建日期: 2012/11/05 15:09:48
 *    作者:         daviyang35@gmail.com
 *    描述: FlashEventHandler
 */


/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef FLASHEVENTHANDLER_INCLUDE
#define FLASHEVENTHANDLER_INCLUDE

#pragma once
namespace DuiLib {
class CFlashEventHandler {
public:
    CFlashEventHandler() {}
    ~CFlashEventHandler() {}

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        return S_OK;
    }
    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return S_OK;
    }

    virtual HRESULT OnReadyStateChange(long newState)
    {
        return S_OK;
    }

    virtual HRESULT OnProgress(long percentDone)
    {
        return S_OK;
    }

    virtual HRESULT FSCommand(LPCTSTR command, LPCTSTR args)
    {
        return S_OK;
    }

    virtual HRESULT FlashCall(LPCTSTR request)
    {
        return S_OK;
    }
};
}

#endif // FLASHEVENTHANDLER_INCLUDE