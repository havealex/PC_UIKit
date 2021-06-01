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

#include "../stdafx.h"
#include "variantanimation.h"

NAMESPACE_DUILIB_BEGIN

const int VARIANT_ANI_TIMER_ID = 0x4396;
const int TIMER_PRECISION = 10;
#define M_PI 3.14159265358979323846

namespace EasingTools {
static double EaseInQuad(double t)
{
    return t * t;
}

static double EaseOutQuad(double t)
{
    return -t * (t - 2);
}

static double EaseInOutElastic(double t, double a, double p)
{
    if (t == 0) {
        return 0.0;
    }
    t *= 2.0;
    if (t == 2) {
        return 1.0;
    }

    double s;
    if (a < 1.0) {
        a = 1.0;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * ::asin(1.0 / a);
    }

    if (t < 1) {
        return -.5 * (a * ::pow(2.0f, 10 * (t - 1)) * ::sin((t - 1 - s) * (2 * M_PI) / p));
    }
    return a * ::pow(2.0f, -10 * (t - 1)) * ::sin((t - 1 - s) * (2 * M_PI) / p) * .5 + 1.0;
}

static double EaseInElastic_helper(double t, double b, double c, double d, double a, double p)
{
    if (t == 0) {
        return b;
    }
    double tAdj = static_cast<double>(t) / static_cast<double>(d);
    if (tAdj == 1) {
        return b + c;
    }

    double s;
    if (a < ::fabs(c)) {
        a = c;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * ::asin(c / a);
    }

    tAdj -= 1.0f;
    return -(a * ::pow(2.0f, 10 * tAdj) * ::sin((tAdj * d - s) * (2 * M_PI) / p)) + b;
}

static double EaseInBack(double t, double s)
{
    return t * t * ((s + 1) * t - s);
}

static double EaseOutBack(double t, double s)
{
    t -= double(1.0);
    return t * t * ((s + 1) * t + s) + 1;
}

static double EaseInOutBack(double t, double s)
{
    t *= 2.0;
    if (t < 1) {
        s *= 1.525f;
        return 0.5 * (t * t * ((s + 1) * t - s));
    } else {
        t -= 2;
        s *= 1.525f;
        return 0.5 * (t * t * ((s + 1) * t + s) + 2);
    }
}

static double EaseOutInBack(double t, double s)
{
    if (t < 0.5) {
        return EaseOutBack(2 * t, s) / 2;
    }
    return EaseInBack(2 * t - 1, s) / 2 + 0.5;
}

static double EaseInElastic(double t, double a, double p)
{
    return EaseInElastic_helper(t, 0, 1, 1, a, p);
}

static double EaseOutElastic_helper(double t, double, double c, double, double a, double p)
{
    if (t == 0) {
        return 0;
    }
    if (t == 1) {
        return c;
    }

    double s;
    if (a < c) {
        a = c;
        s = p / 4.0f;
    } else {
        s = p / (2 * M_PI) * ::asin(c / a);
    }

    return (a * ::pow(2.0f, -10 * t) * ::sin((t - s) * (2 * M_PI) / p) + c);
}

static double EaseOutElastic(double t, double a, double p)
{
    return EaseOutElastic_helper(t, 0, 1, 1, a, p);
}

static double EaseOutInElastic(double t, double a, double p)
{
    if (t < 0.5) {
        return EaseOutElastic_helper(t * 2, 0, 0.5, 1.0, a, p);
    }
    return EaseInElastic_helper(2 * t - 1.0, 0.5, 0.5, 1.0, a, p);
}

static double EaseInQuart(double t)
{
    return t * t * t * t;
}

static double EaseOutQuart(double t)
{
    t -= double(1.0);
    return -(t * t * t * t - 1);
}

static double EaseInCubic(double t)
{
    return t * t * t;
}

static double EaseOutCubic(double t)
{
    t -= 1.0;
    return t * t * t + 1;
}

static double EaseOutBounce_helper(double t, double c, double a)
{
    if (t == 1.0) {
        return c;
    }
    if (t < (4 / 11.0)) {
        return c * (7.5625 * t * t);
    } else if (t < (8 / 11.0)) {
        t -= (6 / 11.0);
        return -a * (1. - (7.5625 * t * t + .75)) + c;
    } else if (t < (10 / 11.0)) {
        t -= (9 / 11.0);
        return -a * (1. - (7.5625 * t * t + .9375)) + c;
    } else {
        t -= (21 / 22.0);
        return -a * (1. - (7.5625 * t * t + .984375)) + c;
    }
}

static double EaseOutBounce(double t, double a)
{
    return EaseOutBounce_helper(t, 1, a);
}

static double EaseInBounce(double t, double a)
{
    return 1.0 - EaseOutBounce_helper(1.0 - t, 1.0, a);
}

static double EaseInOutBounce(double t, double a)
{
    if (t < 0.5) {
        return EaseInBounce(2 * t, a) / 2;
    } else {
        return (t == 1.0) ? 1.0 : EaseOutBounce(2 * t - 1, a) / 2 + 0.5;
    }
}

static double EaseOutInBounce(double t, double a)
{
    if (t < 0.5) {
        return EaseOutBounce_helper(t * 2, 0.5, a);
    }
    return 1.0 - EaseOutBounce_helper(2.0 - 2 * t, 0.5, a);
}

class EasingCurveFunction {
public:
    enum Type {
        In,
        Out,
        InOut,
        OutIn
    };

    explicit EasingCurveFunction(EasingCurveFunction::Type type = In,
        double period = 0.3, double amplitude = 1.0, double overshoot = 1.70158)
        : _t(type), _p(period), _a(amplitude), _o(overshoot)
    {}
    virtual ~EasingCurveFunction() {}
    virtual double Value(double t)
    {
        return t;
    }

    Type _t;
    double _p;
    double _a;
    double _o;
};

struct ElasticEase : public EasingCurveFunction {
    explicit ElasticEase(Type type) : EasingCurveFunction(type, double(0.3), double(1.0)) {}

    double Value(double t) override
    {
        double p = (_p < 0) ? double(0.3) : _p;
        double a = (_a < 0) ? double(1.0) : _a;
        switch (_t) {
            case In:
                return EaseInElastic(t, a, p);
            case Out:
                return EaseOutElastic(t, a, p);
            case InOut:
                return EaseInOutElastic(t, a, p);
            case OutIn:
                return EaseOutInElastic(t, a, p);
            default:
                return t;
        }
    }
};

struct BounceEase : public EasingCurveFunction {
    explicit BounceEase(Type type) : EasingCurveFunction(type, double(0.3), double(1.0)) {}

    double Value(double t) override
    {
        double a = (_a < 0) ? double(1.0) : _a;
        switch (_t) {
            case In:
                return EaseInBounce(t, a);
            case Out:
                return EaseOutBounce(t, a);
            case InOut:
                return EaseInOutBounce(t, a);
            case OutIn:
                return EaseOutInBounce(t, a);
            default:
                return t;
        }
    }
};

struct BackEase : public EasingCurveFunction {
    explicit BackEase(Type type) : EasingCurveFunction(
        type, double(0.3), double(1.0), double(1.70158)) {}

    double Value(double t) override
    {
        double o = (_o < 0) ? double(1.70158) : _o;
        switch (_t) {
            case In:
                return EaseInBack(t, o);
            case Out:
                return EaseOutBack(t, o);
            case InOut:
                return EaseInOutBack(t, o);
            case OutIn:
                return EaseOutInBack(t, o);
            default:
                return t;
        }
    }
};
}

static std::vector<UINT> g_timerIDList;
static UINT g_globalTimerID = VARIANT_ANI_TIMER_ID;

static bool FoundTimerID(UINT timerID)
{
    std::vector<UINT>::iterator it = g_timerIDList.begin();

    for (; it != g_timerIDList.end(); ++it) {
        if (*it == timerID) {
            return true;
        }
    }

    return false;
}

static UINT GetAvailableTimerID()
{
    return g_globalTimerID++;
}

static void RemoveTimerID(UINT timerID)
{
    std::vector<UINT>::iterator it = g_timerIDList.begin();

    for (; it != g_timerIDList.end(); ++it) {
        if (*it == timerID) {
            g_timerIDList.erase(it);
            return;
        }
    }
}

using EasingFunction = std::function<double(double)>;

class VariantAnimationHelper {
public:
    VariantAnimationHelper() : easingValue() {}

    ~VariantAnimationHelper()
    {
        if (ctrl) {
            ctrl->OnNotify -= DuiLib::MakeDelegate(this, &VariantAnimationHelper::OnDoNofify);
        }

        if (startValue) {
            delete startValue;
            startValue = nullptr;
        }

        if (endValue) {
            delete endValue;
            endValue = nullptr;
        }
    }

    DuiLib::CControlUI *ctrl = 0;
    IAnimationCallback *callback = 0;
    VariantAnimation *animation = nullptr;
    unsigned int during = 1000;
    unsigned int frameCount = 0;
    unsigned int currentFrame = 0;
    unsigned int currentCount = 0;
    unsigned int loopCount = 1;
    unsigned int timerInterval = TIMER_PRECISION;
    bool running = false;
    bool loopback = false;
    bool loopbackForward = true;
    UINT timerID = GetAvailableTimerID();
    Variable *startValue = 0;
    Variable *endValue = 0;
    VariantAnimation::EasingCurve easingCurve = VariantAnimation::Linear;

    EasingValue easingValue;

    void SetStartValue(const Variable &value)
    {
        if (startValue) {
            delete startValue;
        }

        startValue = value.Clone();
    }

    void SetEndValue(const Variable &value)
    {
        if (endValue) {
            delete endValue;
        }

        endValue = value.Clone();
    }

    void SetControl(DuiLib::CControlUI *ctrl)
    {
        this->ctrl = ctrl;
    }

    void UpdateValue()
    {
        if (startValue && endValue && callback) {
            double d = currentFrame / static_cast<double>(frameCount);
            d = easingValue.GetEasingValue(easingCurve, d);
            Variable *var = startValue->GetValue(*endValue, d);
            callback->OnValueChanged(*var);
            delete var;
        }
    }

    bool OnDoNofify(void *param)
    {
        DuiLib::TNotifyUI *noti = (DuiLib::TNotifyUI *)(param);
        if (this->ctrl == nullptr || noti == nullptr) {
            return false;
        }
        if (noti->sType == _T("timer") && noti->pSender == ctrl && noti->wParam == timerID && running) {
            if (loopback) {
                if (loopbackForward) {
                    currentFrame++;
                    if (currentFrame >= frameCount) {
                        currentFrame = frameCount;
                        running = ctrl->GetManager()->SetTimer(ctrl, timerID, timerInterval);
                        loopbackForward = false;
                    } else {
                        UpdateValue();
                    }
                } else {
                    currentFrame--;
                    UpdateValue();
                    if (currentFrame <= 0) {
                        currentFrame = 0;
                        loopbackForward = true;
                        currentCount++;
                        if (loopCount != 0 && currentCount == loopCount) {
                            ctrl->OnNotify -= DuiLib::MakeDelegate(this, &VariantAnimationHelper::OnDoNofify);
                            ctrl->GetManager()->KillTimer(ctrl, timerID);
                            if (callback) {
                                running = false;
                                callback->OnAnimationEnd();
                                if (callback) {
                                    callback->OnAnimationEnd(animation);
                                }
                            }
                        }
                    }
                }
            } else {
                currentFrame++;
                UpdateValue();
                if (currentFrame >= frameCount) {
                    currentCount++;
                    currentFrame = 0;
                    if (loopCount != 0 && currentCount == loopCount) {
                        ctrl->OnNotify -= DuiLib::MakeDelegate(this, &VariantAnimationHelper::OnDoNofify);
                        ctrl->GetManager()->KillTimer(ctrl, timerID);
                        if (callback) {
                            running = false;
                            callback->OnAnimationEnd();
                            if (callback) {
                                callback->OnAnimationEnd(animation);
                            }
                        }
                    }
                }
            }
        }
        return true;
    }
};

VariantAnimation::VariantAnimation(DuiLib::CControlUI *ctrl) : helper(new VariantAnimationHelper)
{
    helper->SetControl(ctrl);
    helper->animation = this;
    if (!CPaintManagerUI::IsMainThread()) {
        OutputDebugString(_T("Create VariantAnimation in non-mainthread.\n"));
    }
}

VariantAnimation::~VariantAnimation()
{
    if (IsRunning()) {
        Stop();
    }
}

void VariantAnimation::SetCallback(IAnimationCallback *callback)
{
    if (callback == nullptr || this->helper == nullptr) {
        return;
    }
    helper->callback = callback;
}

bool VariantAnimation::Start()
{
    if (this->helper == nullptr) {
        return false;
    }
    helper->frameCount = helper->during / helper->timerInterval;
    helper->currentFrame = 0;
    helper->currentCount = 0;
    helper->loopbackForward = true;
    if (helper->ctrl) {
        helper->ctrl->OnNotify += DuiLib::MakeDelegate(helper.get(),
            &VariantAnimationHelper::OnDoNofify);
        helper->running = helper->ctrl->GetManager()->SetTimer(helper->ctrl, helper->timerID,
            helper->timerInterval);
        return helper->running;
    } else {
        return false;
    }
}

void VariantAnimation::Stop()
{
    if (helper->ctrl == nullptr) {
        return;
    }

    helper->ctrl->OnNotify -= DuiLib::MakeDelegate(helper.get(), &VariantAnimationHelper::OnDoNofify);
    if (CPaintManagerUI *pm = helper->ctrl->GetManager()) {
        helper->ctrl->GetManager()->KillTimer(helper->ctrl, helper->timerID);
    }

    helper->running = false;
}

void VariantAnimation::SetStartValue(const Variable &startValue)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->SetStartValue(startValue);
}

void VariantAnimation::SetEndValue(const Variable &endValue)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->SetEndValue(endValue);
}

void VariantAnimation::SetDuration(unsigned int msecs)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->during = msecs;
}

void DuiLib::VariantAnimation::SetLoopBack(bool loopback)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->loopback = loopback;
}

void VariantAnimation::SetLoopCount(unsigned int count)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->loopCount = count;
}

void VariantAnimation::SetEasingCurve(EasingCurve easingCurve)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->easingCurve = easingCurve;
}

bool DuiLib::VariantAnimation::IsRunning() const
{
    if (this->helper == nullptr) {
        return false;
    }
    return helper->running;
}

void DuiLib::VariantAnimation::SetTimerInterval(int interval)
{
    if (this->helper == nullptr) {
        return;
    }
    helper->timerInterval = interval;
}

class EasingValueHelper {
public:
    std::map<VariantAnimation::EasingCurve, EasingFunction> easingFunctions = {};

    EasingValueHelper()
    {
        easingFunctions[VariantAnimation::Linear] = [](double d) { return d; };

        easingFunctions[VariantAnimation::InQuad] = [](double d) { return d * d; };

        easingFunctions[VariantAnimation::OutQuad] = [](double d) { return -d * (d - 2); };

        easingFunctions[VariantAnimation::InOutQuad] = [](double d) {
            d *= 2.0;
            if (d < 1) {
                return d * d / double(2);
            } else {
                --d;
                return -0.5 * (d * (d - 2) - 1);
            }
        };

        easingFunctions[VariantAnimation::OutInQuad] = [](double d) {
            if (d < 0.5)
                return EasingTools::EaseOutQuad(d * 2) / 2;
            return EasingTools::EaseInQuad((2 * d) - 1) / 2 + 0.5;
        };

        easingFunctions[VariantAnimation::InOutQuart] = [](double d) {
            if (d < 0.5)
                return EasingTools::EaseInQuart(d * 2) / 2;
            return EasingTools::EaseOutQuart((2 * d) - 1) / 2 + 0.5;
        };

        easingFunctions[VariantAnimation::OutInQuart] = [](double d) {
            if (d < 0.5)
                return EasingTools::EaseOutQuart(d * 2) / 2;
            return EasingTools::EaseInQuart((2 * d) - 1) / 2 + 0.5;
        };

        easingFunctions[VariantAnimation::InOutCubic] = [](double d) {
            d *= 2.0;
            if (d < 1) {
                return 0.5 * d * d * d;
            } else {
                d -= double(2.0);
                return 0.5 * (d * d * d + 2);
            }
        };

        easingFunctions[VariantAnimation::InBounce] = [](double d) {
            EasingTools::BounceEase e(EasingTools::ElasticEase::In);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::OutBounce] = [](double d) {
            EasingTools::BounceEase e(EasingTools::ElasticEase::Out);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::OutInCubic] = [](double d) {
            if (d < 0.5)
                return EasingTools::EaseOutCubic(2 * d) / 2;
            return EasingTools::EaseInCubic(2 * d - 1) / 2 + 0.5;
        };

        easingFunctions[VariantAnimation::OutElastic] = [](double d) {
            EasingTools::ElasticEase e(EasingTools::ElasticEase::Out);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::InElastic] = [](double d) {
            EasingTools::ElasticEase e(EasingTools::ElasticEase::In);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::InBack] = [](double d) {
            EasingTools::BackEase e(EasingTools::ElasticEase::In);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::OutBack] = [](double d) {
            EasingTools::BackEase e(EasingTools::ElasticEase::Out);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::InOutBack] = [](double d) {
            EasingTools::BackEase e(EasingTools::ElasticEase::InOut);
            return e.Value(d);
        };

        easingFunctions[VariantAnimation::OutInBack] = [](double d) {
            EasingTools::BackEase e(EasingTools::ElasticEase::OutIn);
            return e.Value(d);
        };
    }

    ~EasingValueHelper() {}
};

EasingValue::EasingValue() : helper(new EasingValueHelper) {}

EasingValue::~EasingValue()
{
    if (helper) {
        delete helper;
        helper = nullptr;
    }
}

double EasingValue::GetEasingValue(VariantAnimation::EasingCurve ec, double v)
{
    if (this->helper == nullptr) {
        return 0;
    }
    return helper->easingFunctions[ec](v);
}

NAMESPACE_DUILIB_END
