/*
 * Thundercracker Firmware -- Confidential, not for redistribution.
 * Copyright <c> 2012 Sifteo, Inc. All rights reserved.
 */

#include <stdio.h>
#include "svm.h"
#include "svmdebugger.h"
#include "svmdebugpipe.h"
#include "svmcpu.h"
#include "svmruntime.h"
#include "radio.h"
#include "macros.h"

using namespace Svm;
using namespace Svm::Debugger;

#define LOG_PREFIX "SvmDebugger: "

SvmDebugger SvmDebugger::instance;


void SvmDebugger::handleBreakpoint(void *param)
{
    LOG((LOG_PREFIX "Hit breakpoint\n"));

    do {
        SvmDebugPipe::DebuggerMsg msg;
        
        while (!SvmDebugPipe::debuggerMsgAccept(msg))
            Radio::halt();

        instance.handleMessage(msg);
        SvmDebugPipe::debuggerMsgFinish(msg);

    } while (instance.stopped);

    LOG((LOG_PREFIX "Resuming\n"));
}

void SvmDebugger::handleMessage(SvmDebugPipe::DebuggerMsg &msg)
{
    /*
     * Handle a single debugger message, with a command and a reply packet.
     * Returns a boolean indicating whether the target should remain stopped.
     */

    // Must be initialized by debuggerMsgAccept().
    ASSERT(msg.replyWords == 0);

    if (msg.cmdWords < 1) {
        LOG((LOG_PREFIX "Empty debug command?\n"));
        return;
    }

    switch (msg.cmd[0] & M_TYPE_MASK) {
        case M_READ_REGISTERS:      return readRegisters(msg);
        case M_WRITE_REGISTERS:     return writeRegisters(msg);
        case M_READ_RAM:            return readRAM(msg);
        case M_WRITE_RAM:           return writeRAM(msg);
    }

    LOG((LOG_PREFIX "Unhandled command 0x%08x\n", msg.cmd[0]));
}

void SvmDebugger::readRegisters(SvmDebugPipe::DebuggerMsg &msg)
{
    msg.replyWords = 14;

    for (unsigned i = 0; i < 10; i++)
        msg.reply[i] = SvmCpu::reg(i);

    msg.reply[10] = SvmMemory::physToVirtRAM(SvmCpu::reg(REG_FP));
    msg.reply[11] = SvmMemory::physToVirtRAM(SvmCpu::reg(REG_SP));
    msg.reply[12] = SvmRuntime::reconstructCodeAddr(SvmCpu::reg(REG_PC));;
    msg.reply[13] = SvmCpu::reg(REG_CPSR);
}

void SvmDebugger::writeRegisters(SvmDebugPipe::DebuggerMsg &msg)
{
}

void SvmDebugger::readRAM(SvmDebugPipe::DebuggerMsg &msg)
{
}

void SvmDebugger::writeRAM(SvmDebugPipe::DebuggerMsg &msg)
{
}
