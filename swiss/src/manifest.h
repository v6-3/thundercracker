/* -*- mode: C; c-basic-offset: 4; intent-tabs-mode: nil -*-
 *
 * swiss - your Sifteo utility knife
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

#ifndef MANIFEST_H
#define MANIFEST_H

#include "iodevice.h"
#include "usbvolumemanager.h"
#include "bits.h"
#include "basedevice.h"

#include <string>

class Manifest
{
public:
    Manifest(IODevice &_dev, bool rpc=false);

    static int run(int argc, char **argv, IODevice &_dev);

private:
    int dumpBaseSysInfo();
    int dumpOverview();
    void dumpVolumes();

    const char *getVolumeTypeString(unsigned type);

    UsbVolumeManager::VolumeOverviewReply overview;
    IODevice &dev;
    BaseDevice base;
    char volTypeBuffer[16];
    bool isRPC;
};

#endif // INSTALLER_H
