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

#include "../StdAfx.h"
#include "UIAnimation.h"
#include <vector>
#include <algorithm>

namespace DuiLib {
struct CUIAnimation::Imp {
    std::vector<CAnimationData *> m_arAnimations;
};

CUIAnimation::CUIAnimation(CControlUI *pOwner) : m_pImp(new CUIAnimation::Imp())
{
    ASSERT(pOwner != nullptr);
    m_pControl = pOwner;
}
CUIAnimation::~CUIAnimation()
{
    if (m_pImp) {
        delete m_pImp;
        m_pImp = nullptr;
    }
}
BOOL CUIAnimation::StartAnimation(int nElapse, int nTotalFrame, int nAnimationID, BOOL bLoop)
{
    CAnimationData *pData = GetAnimationDataByID(nAnimationID);
    if (pData != nullptr || nElapse <= 0 || nTotalFrame <= 0 || m_pControl == nullptr) {
        ASSERT(FALSE);
        return FALSE;
    }

    CAnimationData *pAnimation = new CAnimationData(nElapse, nTotalFrame, nAnimationID, bLoop);
    if (pAnimation == nullptr) {
        return FALSE;
    }

    if (m_pImp == nullptr) {
        delete pAnimation;
        return FALSE;
    }
    if (m_pControl->GetManager()->SetTimer(m_pControl, nAnimationID, nElapse)) {
        m_pImp->m_arAnimations.push_back(pAnimation);
        return TRUE;
    }
    delete pAnimation;
    pAnimation = nullptr;
    return FALSE;
}

void CUIAnimation::StopAnimation(int nAnimationID)
{
    if (m_pControl == nullptr || m_pImp == nullptr) {
        return;
    }

    if (nAnimationID != 0) {
        CAnimationData *pData = GetAnimationDataByID(nAnimationID);
        if (pData != nullptr) {
            m_pControl->GetManager()->KillTimer(m_pControl, nAnimationID);
            m_pImp->m_arAnimations.erase(
                std::remove(m_pImp->m_arAnimations.begin(), m_pImp->m_arAnimations.end(), pData),
                m_pImp->m_arAnimations.end());
            if (pData != nullptr) {
                delete pData;
                pData = nullptr;
            }
            return;
        }
    } else {
        size_t nCount = m_pImp->m_arAnimations.size();
        for (size_t i = 0; i < nCount; ++i) {
            CAnimationData *pData = m_pImp->m_arAnimations[i];
            if (pData) {
                m_pControl->GetManager()->KillTimer(m_pControl, pData->m_nAnimationID);
                if (pData != nullptr) {
                    delete pData;
                    pData = nullptr;
                }
            }
        }
        m_pImp->m_arAnimations.clear();
    }
}

BOOL CUIAnimation::IsAnimationRunning(int nAnimationID)
{
    CAnimationData *pData = GetAnimationDataByID(nAnimationID);
    return pData != nullptr;
}

int CUIAnimation::GetCurrentFrame(int nAnimationID)
{
    CAnimationData *pData = GetAnimationDataByID(nAnimationID);
    if (pData == nullptr) {
        ASSERT(FALSE);
        return -1;
    }
    return pData->m_nCurFrame;
}

BOOL CUIAnimation::SetCurrentFrame(int nFrame, int nAnimationID)
{
    CAnimationData *pData = GetAnimationDataByID(nAnimationID);
    if (pData == nullptr) {
        ASSERT(FALSE);
        return FALSE;
    }

    if (nFrame >= 0 && nFrame <= pData->m_nTotalFrame) {
        pData->m_nCurFrame = nFrame;
        return TRUE;
    } else {
        ASSERT(FALSE);
    }
    return FALSE;
}

void CUIAnimation::OnAnimationElapse(int nAnimationID)
{
    if (m_pControl == nullptr) {
        return;
    }

    CAnimationData *pData = GetAnimationDataByID(nAnimationID);
    if (pData == nullptr) {
        return;
    }

    int nCurFrame = pData->m_nCurFrame;
    if (nCurFrame == 0) {
        OnAnimationStart(nAnimationID, pData->m_bFirstLoop);
        pData->m_bFirstLoop = FALSE;
    }

    OnAnimationStep(pData->m_nTotalFrame, nCurFrame, nAnimationID);
    if (!GetAnimationDataByID(nAnimationID)) {
        return;
    }

    if (nCurFrame >= pData->m_nTotalFrame - 1) {
        OnAnimationStop(nAnimationID);
        if (pData->m_bLoop) {
            pData->m_nCurFrame = 0;
        } else {
            m_pControl->GetManager()->KillTimer(m_pControl, nAnimationID);
            if (m_pImp == nullptr) {
                return;
            }
            m_pImp->m_arAnimations.erase(
                std::remove(m_pImp->m_arAnimations.begin(), m_pImp->m_arAnimations.end(), pData),
                m_pImp->m_arAnimations.end());
            delete pData;
            pData = nullptr;
        }
    }

    if (pData != nullptr) {
        ++(pData->m_nCurFrame);
    }
}

CAnimationData *CUIAnimation::GetAnimationDataByID(int nAnimationID)
{
    CAnimationData *pRet = nullptr;
    if (m_pImp == nullptr) {
        return pRet;
    }
    size_t nCount = m_pImp->m_arAnimations.size();
    for (size_t i = 0; i < nCount; ++i) {
        if (m_pImp->m_arAnimations[i]->m_nAnimationID == nAnimationID) {
            pRet = m_pImp->m_arAnimations[i];
            break;
        }
    }

    return pRet;
}
} // namespace DuiLib