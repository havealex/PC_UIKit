/*
 * Copyright (C) 2021. Huawei Device Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 */

/*
 Add by zhaoboen.
 For no-linear animation.
*/
#pragma once
#ifndef _VARIANTANIMATION_INCLUDE__
#define _VARIANTANIMATION_INCLUDE__

#include "../UIlib.h"

NAMESPACE_DUILIB_BEGIN

class UILIB_API Variable {
public:
    virtual ~Variable() {}
    virtual Variable *GetValue(const Variable &endValue, double percent) = 0;
    virtual Variable *Clone() const = 0;
};

template <typename T>
class Variant : public Variable {
public:
    explicit Variant(const T &val)
    {
        value = val;
    }
    ~Variant() {}

    Variable *GetValue(const Variable &endValue, double percent) override
    {
        auto endVal = (Variant<T> &)endValue;
        T newVal = value + (T)((endVal.value - value) * percent);
        auto val = new Variant<T>(newVal);
        return val;
    }

    Variable *Clone() const override
    {
        auto var = new Variant<T>(value);
        return var;
    }

    T value;
};

class _Point {
public:
    _Point()
    {
        x = 0;
        y = 0;
    }

    explicit _Point(const POINT &pt)
    {
        x = pt.x;
        y = pt.y;
    }

    _Point(const _Point &pt)
    {
        x = pt.x;
        y = pt.y;
    }

    _Point& operator=(const _Point &pt) = default;

    _Point operator + (const _Point &other)
    {
        _Point pt;
        pt.x = x + other.x;
        pt.y = y + other.y;
        return pt;
    }

    _Point operator - (const _Point &other)
    {
        _Point pt;
        pt.x = x - other.x;
        pt.y = y - other.y;
        return pt;
    }

    _Point operator*(double d)
    {
        _Point pt;
        pt.x = static_cast<int>(x * d);
        pt.y = static_cast<int>(y * d);
        return pt;
    }

    int x = 0;
    int y = 0;
};

using UInt32Variant = Variant<unsigned int>;
using Int32Variant = Variant<int>;
using DoubleVariant = Variant<double>;
using PointVariant = Variant<_Point>;

class VariantAnimation;
class UILIB_API IAnimationCallback {
public:
    virtual ~IAnimationCallback() {}
    virtual void OnValueChanged(const Variable &value) {}
    virtual void OnAnimationEnd() {}
    virtual void OnAnimationEnd(VariantAnimation *) {}
};

class VariantAnimationHelper;
class UILIB_API VariantAnimation {
public:
    enum EasingCurve {
        Linear, // 线性增长。

        InQuad,  // 从t^2到线性。
        OutQuad, // 从线性到t^2。

        InOutQuad, // t^2
        OutInQuad, // t^2

        InOutQuart, // t^4
        OutInQuart, // t^4

        InOutCubic, // t^3
        OutInCubic, // t^3

        InBounce, // 弹球。
        OutBounce,

        InElastic, // 弹簧。
        OutElastic,

        InBack,
        OutBack,

        InOutBack,
        OutInBack
    };

    explicit VariantAnimation(DuiLib::CControlUI *ctrl);
    ~VariantAnimation();

    void SetCallback(IAnimationCallback *callback);

    bool Start();

    void Stop();

    void SetStartValue(const Variable &startValue);

    void SetEndValue(const Variable &endValue);

    void SetDuration(unsigned int msecs);

    void SetLoopBack(bool loopback);

    // 0 - 无限循环。
    void SetLoopCount(unsigned int count);

    void SetEasingCurve(EasingCurve easingCurve);

    void SetTimerInterval(int interval);

    bool IsRunning() const;

private:
    std::unique_ptr<VariantAnimationHelper> helper;
};

class EasingValueHelper;
class UILIB_API EasingValue {
public:
    EasingValue();
    ~EasingValue();

    double GetEasingValue(VariantAnimation::EasingCurve ec, double v);

    EasingValueHelper *helper = nullptr;
};

NAMESPACE_DUILIB_END

#endif // _VARIANTANIMATION_INCLUDE__
