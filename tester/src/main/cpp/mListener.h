//
// Created by rose on 2025/1/30.
//

#ifndef LSPOSED_TOOLS_MLISTENER_H
#define LSPOSED_TOOLS_MLISTENER_H

#include "utils.h"

enum InstrumentationEvent {
    kMethodEntered = 0x1,
    kMethodExited = 0x2,
    kMethodUnwind = 0x4,
    kDexPcMoved = 0x8,
    kFieldRead = 0x10,
    kFieldWritten = 0x20,
    kExceptionThrown = 0x40,
    kBranch = 0x80,
    kWatchedFramePop = 0x200,
    kExceptionHandled = 0x400,
};

#endif //LSPOSED_TOOLS_MLISTENER_H
