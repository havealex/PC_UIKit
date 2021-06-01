/*
 * 2021.03.27-Code rectification and function enhancement.
 *            Huawei Device Co., Ltd. <mobile@huawei.com>
 */

#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once

namespace DuiLib {
class UILIB_API CDelegateBase {
public:
    CDelegateBase(void *pObject, const void *pFn);
    CDelegateBase(const CDelegateBase &rhs);
    CDelegateBase & operator=(const CDelegateBase &rhs) = default;
    virtual ~CDelegateBase();
    bool Equals(const CDelegateBase &rhs) const;
    bool operator () (void *param);
    virtual CDelegateBase *Copy() const = 0; // add const for gcc

protected:
    void *GetFn();
    void *GetObject();
    virtual bool Invoke(void *param) = 0;

private:
    void *m_pObject;
    void *m_pFn;
};

class CDelegateStatic : public CDelegateBase {
    using FO = std::function<bool(void *)>;

public:
    explicit CDelegateStatic(FO pFo) : CDelegateBase(nullptr, nullptr), m_pFo(pFo) {}
    ~CDelegateStatic() = default;
    CDelegateStatic(const CDelegateStatic &rhs) : CDelegateBase(rhs), m_pFo(rhs.m_pFo) {}
    CDelegateStatic& operator=(const CDelegateStatic &rhs) = default;
    CDelegateBase *Copy() const override
    {
        return new CDelegateStatic(*this);
    }

protected:
    bool Invoke(void *param) override
    {
        return m_pFo(param);
    }

private:
    FO m_pFo;
};

template <class O, class T>
class CDelegate : public CDelegateBase {
    using Fn = bool (T::*)(void *);

public:
    ~CDelegate() = default;
    CDelegate(O *pObj, Fn pFn) : CDelegateBase(pObj, &pFn), m_pFn(pFn) {}
    CDelegate(const CDelegate &rhs) : CDelegateBase(rhs)
    {
        m_pFn = rhs.m_pFn;
    }
    CDelegate &operator=(const CDelegate &rhs) = default;
    CDelegateBase *Copy() const override
    {
        return new CDelegate(*this);
    }

protected:
    bool Invoke(void *param) override
    {
        if (this->m_pFn == nullptr || param == nullptr) {
            return false;
        }
        O *pObject = reinterpret_cast<O *>(GetObject());
        return (pObject->*m_pFn)(param);
    }

private:
    Fn m_pFn;
};

template <class O, class T>
CDelegate<O, T> MakeDelegate(O *pObject, bool (T::*pFn)(void *))
{
    return CDelegate<O, T>(pObject, pFn);
}

inline CDelegateStatic MakeDelegate(const std::function<bool(void *)> &fo)
{
    return CDelegateStatic(fo);
}

class UILIB_API CEventSource {
    using FnType = bool (*)(void *);

public:
    ~CEventSource();
    operator bool();
    void operator += (const CDelegateBase &d); // add const for gcc
    void operator += (FnType pFn);
    void operator += (const std::function<bool(void *)> &fo);
    void operator -= (const CDelegateBase &d);
    void operator -= (FnType pFn);
    bool operator () (void *param);
    void Clear();

protected:
    CStdPtrArray m_aDelegates;
};
} // namespace DuiLib

#endif // __UIDELEGATE_H__