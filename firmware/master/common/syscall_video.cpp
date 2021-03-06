/* -*- mode: C; c-basic-offset: 4; intent-tabs-mode: nil -*-
 *
 * Thundercracker firmware
 *
 * Copyright <c> 2012 Sifteo, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Syscalls for video buffer manipulation.
 */

#include <sifteo/abi.h>
#include "svmmemory.h"
#include "svmruntime.h"
#include "vram.h"

extern "C" {

void _SYS_vbuf_init(_SYSVideoBuffer *vbuf)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::init(*vbuf);
}

void _SYS_vbuf_lock(_SYSVideoBuffer *vbuf, uint16_t addr)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::truncateWordAddr(addr);
    VRAM::lock(*vbuf, addr);
}

void _SYS_vbuf_unlock(_SYSVideoBuffer *vbuf)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::unlock(*vbuf);
}

void _SYS_vbuf_poke(_SYSVideoBuffer *vbuf, uint16_t addr, uint16_t word)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::truncateWordAddr(addr);
    VRAM::poke(*vbuf, addr, word);
}

void _SYS_vbuf_pokeb(_SYSVideoBuffer *vbuf, uint16_t addr, uint8_t byte)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::truncateByteAddr(addr);
    VRAM::pokeb(*vbuf, addr, byte);
}

void _SYS_vbuf_xorb(_SYSVideoBuffer *vbuf, uint16_t addr, uint8_t byte)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    VRAM::truncateByteAddr(addr);
    VRAM::xorb(*vbuf, addr, byte);
}

uint32_t _SYS_vbuf_peek(const _SYSVideoBuffer *vbuf, uint16_t addr)
{
    if (!isAligned(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);
        return 0;
    }

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return 0;
    }

    VRAM::truncateWordAddr(addr);
    return VRAM::peek(*vbuf, addr);
}

uint32_t _SYS_vbuf_peekb(const _SYSVideoBuffer *vbuf, uint16_t addr)
{
    if (!isAligned(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);
        return 0;
    }

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return 0;
    }

    VRAM::truncateByteAddr(addr);
    return VRAM::peekb(*vbuf, addr);
}

void _SYS_vbuf_fill(struct _SYSVideoBuffer *vbuf, uint16_t addr,
                    uint16_t word, uint16_t count)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    while (count) {
        VRAM::truncateWordAddr(addr);
        VRAM::poke(*vbuf, addr, word);
        count--;
        addr++;
    }
}

void _SYS_vbuf_write(struct _SYSVideoBuffer *vbuf, uint16_t addr, const uint16_t *src, uint16_t count)
{
    if (!isAligned(vbuf) || !isAligned(src, 2))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    FlashBlockRef ref;
    uint32_t bytes = mulsat16x16(sizeof *src, count);
    SvmMemory::VirtAddr srcVA = reinterpret_cast<SvmMemory::VirtAddr>(src);
    SvmMemory::PhysAddr srcPA;

    while (bytes) {
        uint32_t chunk = bytes;
        if (!SvmMemory::mapROData(ref, srcVA, chunk, srcPA)) {
            SvmRuntime::fault(F_SYSCALL_ADDRESS);
            return;
        }

        srcVA += chunk;
        bytes -= chunk;

        while (chunk) {
            VRAM::truncateWordAddr(addr);
            VRAM::poke(*vbuf, addr, *reinterpret_cast<uint16_t*>(srcPA));
            addr++;

            chunk -= sizeof(uint16_t);
            srcPA += sizeof(uint16_t);
        }
    }
}

void _SYS_vbuf_writei(struct _SYSVideoBuffer *vbuf, uint16_t addr, const uint16_t *src,
                      uint16_t offset, uint16_t count)
{
    if (!isAligned(vbuf) || !isAligned(src, 2))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    FlashBlockRef ref;
    uint32_t bytes = mulsat16x16(sizeof *src, count);
    SvmMemory::VirtAddr srcVA = reinterpret_cast<SvmMemory::VirtAddr>(src);
    SvmMemory::PhysAddr srcPA;

    ASSERT((bytes & 1) == 0);
    while (bytes) {
        uint32_t chunk = bytes;
        if (!SvmMemory::mapROData(ref, srcVA, chunk, srcPA)) {
            SvmRuntime::fault(F_SYSCALL_ADDRESS);
            return;
        }

        ASSERT((chunk & 1) == 0);
        srcVA += chunk;
        bytes -= chunk;

        while (chunk) {
            uint16_t index = offset + *reinterpret_cast<uint16_t*>(srcPA);

            VRAM::truncateWordAddr(addr);
            VRAM::poke(*vbuf, addr, _SYS_TILE77(index));
            addr++;

            chunk -= sizeof(uint16_t);
            srcPA += sizeof(uint16_t);
        }
    }
}

void _SYS_vbuf_seqi(struct _SYSVideoBuffer *vbuf, uint16_t addr, uint16_t index, uint16_t count)
{
    if (!isAligned(vbuf))
        return SvmRuntime::fault(F_SYSCALL_ADDR_ALIGN);

    if (!SvmMemory::mapRAM(vbuf)) {
        SvmRuntime::fault(F_SYSCALL_ADDRESS);
        return;
    }

    while (count) {
        VRAM::truncateWordAddr(addr);
        VRAM::poke(*vbuf, addr, _SYS_TILE77(index));
        count--;
        addr++;
        index++;
    }
}

void _SYS_vbuf_wrect(struct _SYSVideoBuffer *vbuf, uint16_t addr, const uint16_t *src, uint16_t offset, uint16_t count,
                     uint16_t lines, uint16_t src_stride, uint16_t addr_stride)
{
    /*
     * Shortcut for a rectangular blit, comprised of multiple calls to SYS_vbuf_writei.
     * We save the pointer validation for writei, since we want to do it per-scanline anyway.
     */

    while (lines--) {
        _SYS_vbuf_writei(vbuf, addr, src, offset, count);
        src += src_stride;
        addr += addr_stride;
    }
}

void _SYS_vbuf_spr_resize(struct _SYSVideoBuffer *vbuf, unsigned id, unsigned width, unsigned height)
{
    // Address validation occurs after these calculations, in _SYS_vbuf_poke.
    // Sprite ID validation is implicit.

    uint8_t xb = -(int)width;
    uint8_t yb = -(int)height;
    uint16_t word = ((uint16_t)xb << 8) | yb;
    uint16_t addr = ( offsetof(_SYSVideoRAM, spr[0].mask_y)/2 +
                     sizeof(_SYSSpriteInfo)/2 * id );

    _SYS_vbuf_poke(vbuf, addr, word);
}

void _SYS_vbuf_spr_move(struct _SYSVideoBuffer *vbuf, unsigned id, int x, int y)
{
    // Address validation occurs after these calculations, in _SYS_vbuf_poke.
    // Sprite ID validation is implicit.

    uint8_t xb = -x;
    uint8_t yb = -y;
    uint16_t word = ((uint16_t)xb << 8) | yb;
    uint16_t addr = ( offsetof(_SYSVideoRAM, spr[0].pos_y)/2 +
                      sizeof(_SYSSpriteInfo)/2 * id );

    _SYS_vbuf_poke(vbuf, addr, word);
}

}  // extern "C"
