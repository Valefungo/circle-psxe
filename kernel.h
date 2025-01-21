//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2020  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _kernel_h
#define _kernel_h

#include <string>
#include <circle/new.h>
#include <circle_stdlib_app.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/2dgraphics.h>
#include <circle/types.h>
#include <circle/input/mouse.h>
#include <circle/sound/soundbasedevice.h>
#include <circle/sound/hdmisoundbasedevice.h>
#include <circle/multicore.h>
#include <circle/memory.h>

// if building for raspi 4, define this
#if RASPPI == 4
#define TRUE_RASPI_4
#else
#undef TRUE_RASPI_4
#endif

// sound config
#define SAMPLE_RATE     48000           // overall system clock
#define WRITE_FORMAT    1               // 0: 8-bit unsigned, 1: 16-bit signed, 2: 24-bit signed
#define WRITE_CHANNELS  2               // 1: Mono, 2: Stereo
#define VOLUME          0.5             // [0.0, 1.0]
#define QUEUE_SIZE_MSECS 100            // size of the sound queue in milliseconds duration
#define CHUNK_SIZE      (384 * 10)      // number of samples, written to sound device at once
#define DAC_I2C_ADDRESS 0               // I2C slave address of the DAC (0 for auto probing)

#if WRITE_FORMAT == 0
        #define FORMAT          SoundFormatUnsigned8
        #define TYPE            u8
        #define TYPE_SIZE       sizeof (u8)
        #define FACTOR          ((1 << 7)-1)
        #define NULL_LEVEL      (1 << 7)
#elif WRITE_FORMAT == 1
        #define FORMAT          SoundFormatSigned16
        #define TYPE            s16
        #define TYPE_SIZE       sizeof (s16)
        #define FACTOR          ((1 << 15)-1)
        #define NULL_LEVEL      0
#elif WRITE_FORMAT == 2
        #define FORMAT          SoundFormatSigned24
        #define TYPE            s32
        #define TYPE_SIZE       (sizeof (u8)*3)
        #define FACTOR          ((1 << 23)-1)
        #define NULL_LEVEL      0
#endif


/**
 * Multicore App
 */
class CStdlibAppMultiCore: public CMultiCoreSupport
{
public:
        CStdlibAppMultiCore (CMemorySystem *pMemorySystem): CMultiCoreSupport (pMemorySystem)
        {
        }

        void Run (unsigned nCore);
};

class CKernel : public CStdlibAppStdio
{
public:
    enum TTextAlign
    {
            AlignLeft,
            AlignRight,
            AlignCenter
    };

    CKernel (void);
    virtual bool Initialize(void);
    void *HighMem_Alloc(long size);
    void HighMem_Delete(void *p);

    uint64_t fileGetSize(char *fname);
    uint64_t fileFullRead(char *fname, void *buffer, uint64_t size);

    void MsPause(int ms);
    void StartTimer();
    unsigned CheckTimer();
    unsigned CheckTimerMs();

    unsigned short kmap_usb_to_ps2[256];

    void DrawColorRect (unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight, TScreenColor Color,
                        unsigned nTargetWidth, unsigned nTargetHeight, TScreenColor *targetPixelBuffer);
    void DrawColorRect (unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight, TScreenColor Color);
    void DrawImageRect (unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight,
                        unsigned nSourceX, unsigned nSourceY, TScreenColor *sourcePixelBuffer,
                        unsigned nTargetWidth, unsigned nTargetHeight, TScreenColor *targetPixelBuffer);
    void DrawImageRect (unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight,
                        unsigned nSourceX, unsigned nSourceY, TScreenColor *sourcePixelBuffer);
    void DrawText (unsigned nX, unsigned nY, TScreenColor Color, const char *pText, TTextAlign Align,
                   unsigned nTargetWidth, unsigned nTargetHeight, TScreenColor *targetPixelBuffer);
    void DrawText (unsigned nX, unsigned nY, TScreenColor Color, const char *pText, TTextAlign Align);

    void wrapClearScreen(TScreenColor color);
    void wrapDrawImage(unsigned nX, unsigned nY, unsigned nWidth, unsigned nHeight, TScreenColor *sourcePixelBuffer);
    void wrapResize(unsigned nWidth, unsigned nHeight);

    void ConfigureMouse(boolean init, unsigned nScreenWidth, unsigned nScreenHeight);

    CUSBKeyboardDevice * volatile m_pre_pKeyboard;
    CUSBKeyboardDevice * volatile m_pKeyboard;

    CMouseDevice * volatile m_pre_pMouse;
    CMouseDevice * volatile m_pMouse;

    CLogger * p_mLogger;
    CCharGenerator m_Font;

    CSoundBaseDevice        *m_pSound;

    boolean bRunningMulticore;
    CStdlibAppMultiCore mStdlibAppMultiCore;

    void UpdateKeyboardAndMouse();
    static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
    static void KeyboardRemovedHandler (CDevice *pDevice, void *pContext);

    void MouseEventHandler (TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove);
    static void MouseEventStub (TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove);
    static void MouseRemovedHandler (CDevice *pDevice, void *pContext);

    TShutdownMode Run (void);
};

/**
 * noSDL needs these thing
 */
extern CKernel *this_kernel;

extern int screenshot_count;
extern int do_screenshot;
void screenshot(const char *fn, TScreenColor *pixels, int width, int height);

// current very virtual video mode
extern int virtual_screen_width;
extern int virtual_screen_height;

#define LOGG_C(...) this_kernel->p_mLogger->Write (this_kernel->GetKernelName(), LogNotice, __VA_ARGS__)
#define LOGG_K(...) this_kernel->mLogger.Write (this_kernel->GetKernelName(), LogNotice, __VA_ARGS__)

#endif
