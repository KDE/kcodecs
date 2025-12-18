/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsCodingStateMachine_h__
#define nsCodingStateMachine_h__

#include "../kencodingprober_p.h"

#include "kcodecs_export.h"

#include "nsPkgInt.h"
namespace kencodingprober
{
enum {
    eStart = 0,
    eError = 1,
    eItsMe = 2,
};
using nsSMState = int;

// state machine model
typedef struct {
    nsPackedTable classTable;
    unsigned int classFactor;
    nsPackedTable stateTable;
    const unsigned int *charLenTable;
    const char *name;
} SMModel;

class KCODECS_NO_EXPORT nsCodingStateMachine
{
public:
    nsCodingStateMachine(const SMModel *sm)
        : mModel(sm)
    {
    }
    nsSMState NextState(char c)
    {
        // for each byte we get its class, if it is first byte, we also get byte length
        unsigned int byteCls = GETFROMPCK(static_cast<uint8_t>(c), mModel->classTable);
        if (mCurrentState == eStart) {
            mCurrentCharLen = mModel->charLenTable[byteCls];
        }
        // from byte's class and stateTable, we get its next state
        mCurrentState = GETFROMPCK(mCurrentState * (mModel->classFactor) + byteCls, mModel->stateTable);
        return mCurrentState;
    }
    unsigned int GetCurrentCharLen(void)
    {
        return mCurrentCharLen;
    }
    void Reset(void)
    {
        mCurrentState = eStart;
    }
    const char *GetCodingStateMachine()
    {
        return mModel->name;
    }
#ifdef DEBUG_PROBE
    const char *DumpCurrentState()
    {
        switch (mCurrentState) {
        case eStart:
            return "eStart";
        case eError:
            return "eError";
        case eItsMe:
            return "eItsMe";
        default:
            return "OK";
        }
    }
#endif

protected:
    int mCurrentState = eStart;
    unsigned int mCurrentCharLen = 0;

    const SMModel *mModel = nullptr;
};

extern KCODECS_NO_EXPORT const SMModel UTF8SMModel;
extern KCODECS_NO_EXPORT const SMModel Big5SMModel;
extern KCODECS_NO_EXPORT const SMModel EUCJPSMModel;
extern KCODECS_NO_EXPORT const SMModel EUCKRSMModel;
extern KCODECS_NO_EXPORT const SMModel GB18030SMModel;
extern KCODECS_NO_EXPORT const SMModel SJISSMModel;
extern KCODECS_NO_EXPORT const SMModel UCS2LESMModel;
extern KCODECS_NO_EXPORT const SMModel UCS2BESMModel;

extern KCODECS_NO_EXPORT const SMModel HZSMModel;
extern KCODECS_NO_EXPORT const SMModel ISO2022JPSMModel;
}
#endif /* nsCodingStateMachine_h__ */
