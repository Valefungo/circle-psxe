
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "noSDL.h"
#include "kernel.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#undef LOGG_C
#define LOGG_C(...) /* nothing */

#define LOG_MALLOC(sz, p)  malloc(sz); LOGG_C("Malloc of %d -> %08X", sz, p);
#define LOG_FREE(p)  LOGG_C("Free of %08X", p); free(p);

#define DEBF(f) (f == SDL_PIXELFORMAT_RGBA32 ? "RGBA32" : (f == SDL_PIXELFORMAT_BGR555 ? "BGR555" : "BOH"))

#include "noSDL_blitstretch.c"


void *noSDL_fakemalloc(int sz)
{
}

void noSDL_fakefree(void *p)
{
}

int SDL_Init(Uint32 flags)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // unused, nothing real to initialize
    return SDL_FALSE;
}

void SDL_Quit(void)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // absolutely not
}

SDL_AudioDeviceID SDL_OpenAudioDevice( const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    memcpy(obtained, desired, sizeof(SDL_AudioSpec));
    return 1;
}

void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
}

int SDL_ShowCursor(int toggle)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // unused
    return toggle;
}

void SDL_Delay(Uint32 ms)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // delay is actually unused but let's take the opportunity to update the USB status
    this_kernel->MsPause(ms);
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // absolutely not
}

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // it's always grabbed
    return mode;
}

void noSDL_UpdateUSB()
{
    this_kernel->UpdateKeyboardAndMouse();
}

void noSDL_Kernel_Log(const char *line)
{
    // easy logging
    LOGG_C( "KLOG %s %s %s : %s", __FUNCTION__, __LINE__, __FILE__, line);
}

void *noSDL_HighMem_Alloc(long size)
{
    return this_kernel->HighMem_Alloc(size);
}

void noSDL_HighMem_Delete(void *p)
{
    return this_kernel->HighMem_Delete(p);
}

uint64_t noSDL_fileGetSize(char *fname)
{
    return this_kernel->fileGetSize(fname);
}

uint64_t noSDL_fileFullRead(char *fname, void *buffer, uint64_t size)
{
    return this_kernel->fileFullRead(fname, buffer, size);
}


SDL_Event static_event[6];
SDL_Event static_mod_event[8];
SDL_Event static_mouse_move_event;
SDL_Event static_mouse_button_event[3];
int lastmousex=0;
int lastmousey=0;

void noSDL_addModDown(int slot, int mod)
{
    static_mod_event[slot].type = SDL_MOD_KEYDOWN;
    static_mod_event[slot].key_keysym_mod = mod;
}

void noSDL_addModUp(int slot, int mod)
{
    static_mod_event[slot].type = SDL_MOD_KEYUP;
    static_mod_event[slot].key_keysym_mod = mod;
}

int SDL_PollEvent(SDL_Event *event)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    noSDL_UpdateUSB();

    if (static_mouse_move_event.type != 0)
    {
        // LOGG_C( "MPOLL %d x%d y%d\n", static_mouse_move_event.type, static_mouse_move_event.motion_xrel, static_mouse_move_event.motion_yrel);

        memcpy(event, &static_mouse_move_event, sizeof(SDL_Event));
        static_mouse_move_event.type = 0;
        static_mouse_move_event.motion_xrel = 0;
        static_mouse_move_event.motion_yrel = 0;

        return SDL_TRUE;
    }

    for (int i=0; i<8; i++)
    {
        if (static_mod_event[i].type != 0)
        {
            memcpy(event, &static_mod_event[i], sizeof(SDL_Event));
            static_mod_event[i].type = 0;
            return SDL_TRUE;
        }
    }

    for (int i=0; i<6; i++)
    {
        if (static_event[i].type != 0)
        {
            // LOGG_C( "KPOLL[%d] %d %04X %02X", i, static_event[i].type, static_event[i].key_keysym_sym, static_event[i].key_keysym_mod);

            memcpy(event, &(static_event[i]), sizeof(SDL_Event));
            static_event[i].type = 0;
            return SDL_TRUE;
        }
    }

    for (int i=0; i<3; i++)
    {
        if (static_mouse_button_event[i].type != 0)
        {
            memcpy(event, &static_mouse_button_event[i], sizeof(SDL_Event));
            static_mouse_button_event[i].type = 0;
            return SDL_TRUE;
        }
    }

    return SDL_FALSE;
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    LOG_FREE(surface->pixels);
    LOG_FREE(surface);
}

int SDL_Flip(SDL_Surface *screen)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);

    // Nothing to explicitly flip
    noSDL_BlitSurface(screen, NULL, NULL, NULL);

    return 0;
}

#ifndef TRUE_RASPI_4
// the max display size, we just need a buffer
static TScreenColor pix[1920*1080];
static unsigned int temp;
#endif

int noSDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // LOG_C( "SDL_BlitSurface SRC %d %d %d, %d   DST %d %d %d %d\n", srcrect->x, srcrect->y, srcrect->w, srcrect->h, dstrect->x, dstrect->y, dstrect->w, dstrect->h);
    // LOG_C( "SDL_BlitSurface %d, %d\n", src->w, src->h);

#ifdef TRUE_RASPI_4
    // no, let it stay dirty to be able to see any log
    // this_kernel->wrapClearScreen(BLACK_COLOR);

    this_kernel->wrapDrawImage(0, 0, src->w, src->h, (TScreenColor *)src->pixels);
#else
    TScreenColor *scr = (TScreenColor *)src->pixels;

    // emulated raspi 3
    this_kernel->wrapClearScreen(BLACK_COLOR);

    // C2DGraphics needs the final uint as ARGB, we have xBGR
    for (int y = 0; y < src->w * src->h; y++) {
        temp   = scr[y];
        unsigned char b = (temp >> 16) & 0xFF;
        unsigned char g = (temp >> 8) & 0xFF;
        unsigned char r = (temp >> 0) & 0xFF;
        pix[y] = COLOR32(r, g, b, 255);
    }

    this_kernel->wrapDrawImage(0, 0, src->w, src->h, pix);

#endif

    if (do_screenshot)
    {
        char fn[200] = "";
        struct stat sb;

        while (screenshot_count < 10000)
        {
            sprintf(fn, "screenshot-%04d.png", screenshot_count);
            if (stat(fn, &sb) == -1) {
                screenshot(fn, (TScreenColor *)src->pixels, src->w, src->h);
                break;
            }
            else
                screenshot_count++;
        }

        do_screenshot = 0;
    }

    return 0;
}

SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    LOGG_C( "KLOG %s %d %s - %d %d %d %d", __FUNCTION__, __LINE__, __FILE__, width, height, bpp, flags);
    // LOG_C( "SDL_SetVideoMode %d, %d, %d, %d\n", width, height, bpp, DEPTH);

    if (width != 0 && height != 0)
        this_kernel->wrapResize(width, height);

    SDL_Surface *su = (SDL_Surface *)LOG_MALLOC(sizeof(SDL_Surface), su);
    su->w = width;
    su->h = height;
    su->pitch = width * 4;
    su->pixels = LOG_MALLOC(width*height*4, su->pixels);

    return su;
}

SDL_Surface * SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // LOG_C( "SDL_CreateRGBSurfaceFrom %d, %d, %d, %d\n", width, height, depth, pitch);

    SDL_Surface *su = (SDL_Surface *)LOG_MALLOC(sizeof(SDL_Surface), su);
    su->w = width;
    su->h = height;
    su->pixels = pixels;

    return su;
}

SDL_Surface* SDL_CreateRGBSurfaceWithFormat (Uint32 flags, int width, int height, int depth, Uint32 format)
{
    LOGG_C( "KLOG %s %d %s - %d %d %d %d %s", __FUNCTION__, __LINE__, __FILE__, flags, width, height, depth, DEBF(format));
    SDL_Surface *su = (SDL_Surface *)LOG_MALLOC(sizeof(SDL_Surface), su);
    su->w = width;
    su->h = height;
    su->pitch = width * 4;
    su->pixels = LOG_MALLOC(width*height*4, su->pixels);

    return su;
}
SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_Renderer *su = (SDL_Renderer *)LOG_MALLOC(sizeof(SDL_Renderer), su);
    su->window = window;
    su->index = index;
    su->flags = flags;
    su->scale.x = 1;
    su->scale.y = 1;
    return su;
}
SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, Uint32 format, int access, int w, int h)
{
    LOGG_C( "KLOG %s %d %s - %s %d %d", __FUNCTION__, __LINE__, __FILE__, DEBF(format), w, h);
    SDL_Texture *su = (SDL_Texture *)LOG_MALLOC(sizeof(SDL_Texture), su);
    su->renderer = renderer;
    su->format = format;
    su->access = access;
    su->w = w;
    su->h = h;
    su->pitch = w;
    su->surface = SDL_CreateRGBSurfaceWithFormat (renderer->flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);
    return su;
}
SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    LOGG_C( "KLOG %s %d %s - %s %d %d %d %d %d", __FUNCTION__, __LINE__, __FILE__, title, x, y, w, h, flags);
    SDL_Window *su = (SDL_Window *)LOG_MALLOC(sizeof(SDL_Window), su);
    strncpy(su->title, title, 15);
    su->x = x;
    su->y = y;
    su->w = w;
    su->h = h;
    su->flags = flags;
    // su->surface = SDL_CreateRGBSurfaceWithFormat (flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);
    su->surface = SDL_SetVideoMode(w, h, 32, flags);

    return su;
}
void SDL_DestroyRenderer(SDL_Renderer * renderer)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    LOG_FREE(renderer);
}
void SDL_DestroyTexture(SDL_Texture * texture)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_FreeSurface(texture->surface);
    LOG_FREE(texture);
}
void SDL_DestroyWindow(SDL_Window * window)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_FreeSurface(window->surface);
    LOG_FREE(window);
}

int SDL_SetRenderDrawColor(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // ok ok
    renderer->clearcolor = COLOR32 (r, g, b, a);
    return SDL_TRUE;
}
int SDL_SetWindowFullscreen(SDL_Window * window, Uint32 flags)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // sure sure
    return SDL_TRUE;
}

void SDL_SetWindowSize(SDL_Window * window, int w, int h)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);

    // do this if it actually changed for some reason
    if (w != window->w || h != window->h)
    {
        if (window->surface != NULL)
        {
            SDL_FreeSurface(window->surface);
        }

        window->w = w;
        window->h = h;
        // window->surface = SDL_CreateRGBSurfaceWithFormat (window->flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);
        window->surface = SDL_SetVideoMode(w, h, 32, window->flags);
    }
}

int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void *pixels, int stride)
{
    LOGG_C( "KLOG %s %d %s - tex f %s stride %d", __FUNCTION__, __LINE__, __FILE__, DEBF(texture->format), stride);
    // this is the real copy of all pixels
    // https://wiki.libsdl.org/SDL2/SDL_UpdateTexture

    // we have to support both SDL_PIXELFORMAT_RGB24 and SDL_PIXELFORMAT_BGR555

    if (texture->format == SDL_PIXELFORMAT_RGB24)
    {
        uint8_t *tgt = (uint8_t *)texture->surface->pixels;
        uint8_t *src = (uint8_t *)pixels;

        for (int i=0; i<texture->h; i++)
        {
            memcpy(tgt + ((texture->w*4)*i), src+(stride*i), texture->w*4);
        }
    }
    else if (texture->format == SDL_PIXELFORMAT_BGR555)
    {
        uint32_t *tgt = (uint32_t *)texture->surface->pixels;
        uint16_t *src = (uint16_t *)pixels;

        for (int i=0; i<texture->h; i++)
        {
            for (int x=0; x<texture->w; x++)
            {
                uint16_t c = src[x];

                int r = (c >> 10) & 0x1F;
                int g = (c >> 5) & 0x1F;
                int b = c & 0x1F;

                tgt[x] = COLOR32(b*8, g*8, r*8, 255);
            }

            tgt += texture->w;
            src += stride / 2;
        }
    }
    else
    {
        this_kernel->p_mLogger->Write (this_kernel->GetKernelName(), LogNotice, "BAD FORMAT");
    }

    return SDL_TRUE;
}

int SDL_RenderClear(SDL_Renderer * renderer)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // here we should clear the screen with the render clearcolor
    // just memset to black
    memset(renderer->window->surface->pixels, 0, renderer->window->surface->w*renderer->window->surface->h*4);
    return SDL_TRUE;
}




void
SDL_RenderGetViewport(SDL_Renderer * renderer, SDL_Rect * rect)
{
    if (rect) {
        rect->x = (int)(renderer->window->x / renderer->scale.x);
        rect->y = (int)(renderer->window->y / renderer->scale.y);
        rect->w = (int)(renderer->window->w / renderer->scale.x);
        rect->h = (int)(renderer->window->h / renderer->scale.y);
    }
}


/**
 *  This is a semi-private blit function and it performs low-level surface
 *  scaled blitting only.
 */
int
SDL_LowerBlitScaled(SDL_Surface * src, SDL_Rect * srcrect,
                SDL_Surface * dst, SDL_Rect * dstrect)
{
    /* Save off the original dst width, height */
    int dstW = dstrect->w;
    int dstH = dstrect->h;
    SDL_Rect full_rect;
    SDL_Rect final_dst = *dstrect;
    SDL_Rect final_src = *srcrect;

    /* Clip the dst surface to the dstrect */
    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = dst->w;
    full_rect.h = dst->h;
    if (!SDL_IntersectRect(&final_dst, &full_rect, &final_dst)) {
        return 0;
    }

    /* Clip the src surface to the srcrect */
    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = src->w;
    full_rect.h = src->h;
    if (!SDL_IntersectRect(&final_src, &full_rect, &final_src)) {
        return 0;
    }

    // stretch ?
    return SDL_SoftStretch( src, &final_src, dst, &final_dst );

    // return SDL_LowerBlit( src, &final_src, dst, &final_dst );
}

/*
 * Set up a blit between two surfaces -- split into three parts:
 * The upper part, SDL_UpperBlit(), performs clipping and rectangle
 * verification.  The lower part is a pointer to a low level
 * accelerated blitting function.
 *
 * These parts are separated out and each used internally by this
 * library in the optimimum places.  They are exported so that if
 * you know exactly what you are doing, you can optimize your code
 * by calling the one(s) you need.
 */
int SDL_LowerBlit(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
    return SDL_LowerBlitScaled(src, srcrect, dst, dstrect);
}

int SDL_UpperBlit(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
    SDL_Rect fulldst;
    int srcx, srcy, w, h;

    /* Make sure the surfaces aren't locked */
    if (!src || !dst) {
        return SDL_FALSE;
    }

    /* If the destination rectangle is NULL, use the entire dest surface */
    if (dstrect == NULL) {
        fulldst.x = fulldst.y = 0;
        dstrect = &fulldst;
    }

    /* clip the source rectangle to the source surface */
    if (srcrect) {
        int maxw, maxh;

        srcx = srcrect->x;
        w = srcrect->w;
        if (srcx < 0) {
            w += srcx;
            dstrect->x -= srcx;
            srcx = 0;
        }
        maxw = src->w - srcx;
        if (maxw < w)
            w = maxw;

        srcy = srcrect->y;
        h = srcrect->h;
        if (srcy < 0) {
            h += srcy;
            dstrect->y -= srcy;
            srcy = 0;
        }
        maxh = src->h - srcy;
        if (maxh < h)
            h = maxh;

    } else {
        srcx = srcy = 0;
        w = src->w;
        h = src->h;
    }

    /* clip the destination rectangle against the clip rectangle */
    {
        SDL_Rect *clip = dstrect;
        int dx, dy;

        dx = clip->x - dstrect->x;
        if (dx > 0) {
            w -= dx;
            dstrect->x += dx;
            srcx += dx;
        }
        dx = dstrect->x + w - clip->x - clip->w;
        if (dx > 0)
            w -= dx;

        dy = clip->y - dstrect->y;
        if (dy > 0) {
            h -= dy;
            dstrect->y += dy;
            srcy += dy;
        }
        dy = dstrect->y + h - clip->y - clip->h;
        if (dy > 0)
            h -= dy;
    }

    if (w > 0 && h > 0) {
        SDL_Rect sr;
        sr.x = srcx;
        sr.y = srcy;
        sr.w = dstrect->w = w;
        sr.h = dstrect->h = h;
        return SDL_LowerBlit(src, &sr, dst, dstrect);
    }
    dstrect->w = dstrect->h = 0;
    return 0;
}

int
SDL_UpperBlitScaled(SDL_Surface * src, const SDL_Rect * srcrect,
              SDL_Surface * dst, SDL_Rect * dstrect)
{
    SDL_Rect final_src, final_dst, fulldst;

    /* Make sure the surfaces aren't locked */
    if (!src || !dst) {
        return SDL_FALSE;
    }

    /* If the destination rectangle is NULL, use the entire dest surface */
    if (dstrect == NULL) {
        fulldst.x = fulldst.y = 0;
        dstrect = &fulldst;
    }

    /* clip the source rectangle to the source surface */
    if (srcrect) {
        int maxw, maxh;

        final_src.x = srcrect->x;
        final_src.w = srcrect->w;
        if (final_src.x < 0) {
            final_src.w += final_src.x;
            final_src.x = 0;
        }
        maxw = src->w - final_src.x;
        if (maxw < final_src.w)
            final_src.w = maxw;

        final_src.y = srcrect->y;
        final_src.h = srcrect->h;
        if (final_src.y < 0) {
            final_src.h += final_src.y;
            final_src.y = 0;
        }
        maxh = src->h - final_src.y;
        if (maxh < final_src.h)
            final_src.h = maxh;

    } else {
        final_src.x = final_src.y = 0;
        final_src.w = src->w;
        final_src.h = src->h;
    }

    /* clip the destination rectangle against the clip rectangle */
    if (dstrect) {
        int maxw, maxh;

        final_dst.x = dstrect->x;
        final_dst.w = dstrect->w;
        if (final_dst.x < 0) {
            final_dst.w += final_dst.x;
            final_dst.x = 0;
        }
        maxw = dst->w - final_dst.x;
        if (maxw < final_dst.w)
            final_dst.w = maxw;

        final_dst.y = dstrect->y;
        final_dst.h = dstrect->h;
        if (final_dst.y < 0) {
            final_dst.h += final_dst.y;
            final_dst.y = 0;
        }
        maxh = dst->h - final_dst.y;
        if (maxh < final_dst.h)
            final_dst.h = maxh;
    } else {
        final_dst.x = final_dst.y = 0;
        final_dst.w = dst->w;
        final_dst.h = dst->h;
    }

    if (final_dst.w > 0 && final_dst.h > 0) {
        return SDL_LowerBlitScaled(src, &final_src, dst, &final_dst);
    }

    return 0;
}

SDL_bool
SDL_IntersectRect(const SDL_Rect * A, const SDL_Rect * B, SDL_Rect * result)
{
    int Amin, Amax, Bmin, Bmax;

    if (!A || !B || !result) {
        // TODO error message
        return SDL_FALSE;
    }

    /* Special cases for empty rects */
    if (SDL_RectEmpty(A) || SDL_RectEmpty(B)) {
        return SDL_FALSE;
    }

    /* Horizontal intersection */
    Amin = A->x;
    Amax = Amin + A->w;
    Bmin = B->x;
    Bmax = Bmin + B->w;
    if (Bmin > Amin)
        Amin = Bmin;
    result->x = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->w = Amax - Amin;

    /* Vertical intersection */
    Amin = A->y;
    Amax = Amin + A->h;
    Bmin = B->y;
    Bmax = Bmin + B->h;
    if (Bmin > Amin)
        Amin = Bmin;
    result->y = Amin;
    if (Bmax < Amax)
        Amax = Bmax;
    result->h = Amax - Amin;

    return !SDL_RectEmpty(result);
}


static int renderer_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
              const SDL_Rect * srcrect, const SDL_FRect * dstrect)
{
    SDL_Surface *surface = renderer->window->surface;
    SDL_Surface *src = (SDL_Surface *) texture->surface;
    SDL_Rect final_rect;

    if (!surface) {
        return -1;
    }

    if (renderer->window->x || renderer->window->y) {
        final_rect.x = (int)(renderer->window->x + dstrect->x);
        final_rect.y = (int)(renderer->window->y + dstrect->y);
    } else {
        final_rect.x = (int)dstrect->x;
        final_rect.y = (int)dstrect->y;
    }
    final_rect.w = (int)dstrect->w;
    final_rect.h = (int)dstrect->h;

    if ( srcrect->w == final_rect.w && srcrect->h == final_rect.h ) {
        return SDL_BlitSurface(src, srcrect, surface, &final_rect);
    } else {
        return SDL_BlitScaled(src, srcrect, surface, &final_rect);
    }
}

static int GetScaleQuality(void)
{
    // nearest
    return 0;

    // bilinear ?
    // return 1;
}

static int renderer_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture,
                const SDL_Rect * srcrect, const SDL_FRect * dstrect,
                const double angle, const SDL_FPoint * center, const SDL_RendererFlip flip)
{
    return renderer_RenderCopy(renderer, texture, srcrect, dstrect);
}

/* FULL:
static int renderer_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture,
                const SDL_Rect * srcrect, const SDL_FRect * dstrect,
                const double angle, const SDL_FPoint * center, const SDL_RendererFlip flip)
{
    SDL_Surface *surface = renderer->window->surface;
    SDL_Surface *src = (SDL_Surface *) texture->surface;
    SDL_Rect final_rect, tmp_rect;
    SDL_Surface *surface_rotated, *surface_scaled;
    Uint32 colorkey;
    int retval, dstwidth, dstheight, abscenterx, abscentery;
    double cangle, sangle, px, py, p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y;

    if (!surface) {
        return -1;
    }

    if (renderer->window->x || renderer->window->y) {
        final_rect.x = (int)(renderer->window->x + dstrect->x);
        final_rect.y = (int)(renderer->window->y + dstrect->y);
    } else {
        final_rect.x = (int)dstrect->x;
        final_rect.y = (int)dstrect->y;
    }
    final_rect.w = (int)dstrect->w;
    final_rect.h = (int)dstrect->h;

    surface_scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, final_rect.w, final_rect.h, src->format->BitsPerPixel,
                                          src->format->Rmask, src->format->Gmask,
                                          src->format->Bmask, src->format->Amask );
    if (surface_scaled) {
        SDL_GetColorKey(src, &colorkey);
        SDL_SetColorKey(surface_scaled, SDL_TRUE, colorkey);
        tmp_rect = final_rect;
        tmp_rect.x = 0;
        tmp_rect.y = 0;

        retval = SDL_BlitScaled(src, srcrect, surface_scaled, &tmp_rect);
        if (!retval) {
            _rotozoomSurfaceSizeTrig(tmp_rect.w, tmp_rect.h, -angle, &dstwidth, &dstheight, &cangle, &sangle);
            surface_rotated = _rotateSurface(surface_scaled, -angle, dstwidth/2, dstheight/2, GetScaleQuality(), flip & SDL_FLIP_HORIZONTAL, flip & SDL_FLIP_VERTICAL, dstwidth, dstheight, cangle, sangle);
            if(surface_rotated) {
                // Find out where the new origin is by rotating the four final_rect points around the center and then taking the extremes
                abscenterx = final_rect.x + (int)center->x;
                abscentery = final_rect.y + (int)center->y;
                // Compensate the angle inversion to match the behaviour of the other backends
                sangle = -sangle;

                // Top Left
                px = final_rect.x - abscenterx;
                py = final_rect.y - abscentery;
                p1x = px * cangle - py * sangle + abscenterx;
                p1y = px * sangle + py * cangle + abscentery;

                // Top Right
                px = final_rect.x + final_rect.w - abscenterx;
                py = final_rect.y - abscentery;
                p2x = px * cangle - py * sangle + abscenterx;
                p2y = px * sangle + py * cangle + abscentery;

                // Bottom Left
                px = final_rect.x - abscenterx;
                py = final_rect.y + final_rect.h - abscentery;
                p3x = px * cangle - py * sangle + abscenterx;
                p3y = px * sangle + py * cangle + abscentery;

                // Bottom Right
                px = final_rect.x + final_rect.w - abscenterx;
                py = final_rect.y + final_rect.h - abscentery;
                p4x = px * cangle - py * sangle + abscenterx;
                p4y = px * sangle + py * cangle + abscentery;

                tmp_rect.x = (int)MIN(MIN(p1x, p2x), MIN(p3x, p4x));
                tmp_rect.y = (int)MIN(MIN(p1y, p2y), MIN(p3y, p4y));
                tmp_rect.w = dstwidth;
                tmp_rect.h = dstheight;

                retval = SDL_BlitSurface(surface_rotated, NULL, surface, &tmp_rect);
                SDL_FreeSurface(surface_scaled);
                SDL_FreeSurface(surface_rotated);
                return retval;
            }
        }
        return retval;
    }

    return -1;
}
*/

int
SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
               const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    SDL_Rect real_srcrect = { 0, 0, 0, 0 };
    SDL_Rect real_dstrect = { 0, 0, 0, 0 };
    SDL_FRect frect;

    real_srcrect.x = 0;
    real_srcrect.y = 0;
    real_srcrect.w = texture->w;
    real_srcrect.h = texture->h;
    if (srcrect) {
        if (!SDL_IntersectRect(srcrect, &real_srcrect, &real_srcrect)) {
            return 0;
        }
    }

    SDL_RenderGetViewport(renderer, &real_dstrect);
    real_dstrect.x = 0;
    real_dstrect.y = 0;
    if (dstrect) {
        if (!SDL_IntersectRect(dstrect, &real_dstrect, &real_dstrect)) {
            return 0;
        }
        /* Clip srcrect by the same amount as dstrect was clipped */
        if (dstrect->w != real_dstrect.w) {
            int deltax = (real_dstrect.x - dstrect->x);
            int deltaw = (real_dstrect.w - dstrect->w);
            real_srcrect.x += (deltax * real_srcrect.w) / dstrect->w;
            real_srcrect.w += (deltaw * real_srcrect.w) / dstrect->w;
        }
        if (dstrect->h != real_dstrect.h) {
            int deltay = (real_dstrect.y - dstrect->y);
            int deltah = (real_dstrect.h - dstrect->h);
            real_srcrect.y += (deltay * real_srcrect.h) / dstrect->h;
            real_srcrect.h += (deltah * real_srcrect.h) / dstrect->h;
        }
    }

    frect.x = real_dstrect.x * renderer->scale.x;
    frect.y = real_dstrect.y * renderer->scale.y;
    frect.w = real_dstrect.w * renderer->scale.x;
    frect.h = real_dstrect.h * renderer->scale.y;

    return renderer_RenderCopy(renderer, texture, &real_srcrect, &frect);
}



int
SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture,
               const SDL_Rect * srcrect, const SDL_Rect * dstrect,
               const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
{
    SDL_Rect real_srcrect = { 0, 0, 0, 0 };
    SDL_Rect real_dstrect = { 0, 0, 0, 0 };
    SDL_Point real_center;
    SDL_FRect frect;
    SDL_FPoint fcenter;

    real_srcrect.x = 0;
    real_srcrect.y = 0;
    real_srcrect.w = texture->w;
    real_srcrect.h = texture->h;
    if (srcrect) {
        if (!SDL_IntersectRect(srcrect, &real_srcrect, &real_srcrect)) {
            return 0;
        }
    }

    /* We don't intersect the dstrect with the viewport as RenderCopy does because of potential rotation clipping issues... TODO: should we? */
    if (dstrect) {
        real_dstrect = *dstrect;
    } else {
        SDL_RenderGetViewport(renderer, &real_dstrect);
        real_dstrect.x = 0;
        real_dstrect.y = 0;
    }


    if(center) real_center = *center;
    else {
        real_center.x = real_dstrect.w/2;
        real_center.y = real_dstrect.h/2;
    }

    frect.x = real_dstrect.x * renderer->scale.x;
    frect.y = real_dstrect.y * renderer->scale.y;
    frect.w = real_dstrect.w * renderer->scale.x;
    frect.h = real_dstrect.h * renderer->scale.y;

    fcenter.x = real_center.x * renderer->scale.x;
    fcenter.y = real_center.y * renderer->scale.y;

    return renderer_RenderCopyEx(renderer, texture, &real_srcrect, &frect, angle, &fcenter, flip);
}














int SDL_RenderCopy_val(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    if (renderer != NULL && renderer->window != NULL && renderer->window->surface != NULL &&
        texture != NULL && texture->surface != NULL)
    {
        /**/
        printf("T: %d %d   S: %d %d    DST:   %d %d    %d %d\n",
               renderer->window->surface->w, renderer->window->surface->h,
               texture->surface->w, texture->surface->h,
               dstrect->x, dstrect->y, dstrect->w, dstrect->h);
        /**/

        // TODO: must move/scale texture to render as dstrect says

        uint32_t *t = (uint32_t *)renderer->window->surface->pixels;
        uint32_t *s = (uint32_t *)texture->surface->pixels;

        if (renderer->window->surface->w == texture->surface->w && renderer->window->surface->h == texture->surface->h &&
            renderer->window->surface->w == dstrect->w && renderer->window->surface->h == dstrect->h)
        {
            // they all matches, fast memcpy
            memcpy(t, s, renderer->window->surface->w*renderer->window->surface->h*4);
        }
        else
        {
            // hard and horrible stretch...

            /*
                for (int y=0; y<renderer->window->surface->h; y++)
                {
                    for (int x=0; x<renderer->window->surface->w; x++)
                    {
                        t[ (y*renderer->window->surface->w) + x ] = s[(y*texture->surface->w) + x];
                    }
                }
            */
        }

    }
    return SDL_TRUE;
}

int SDL_RenderCopyEx_val(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect,
                   const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    return SDL_RenderCopy_val(renderer, texture, srcrect, dstrect);
}

int SDL_RenderCopy_sus(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    if (renderer == NULL || texture == NULL || renderer->window->surface->pixels == NULL || texture->surface->pixels == NULL) {
        return SDL_FALSE; // Errore: parametri non validi
    }

    // Determina l'area di origine e destinazione
    int src_x = srcrect ? srcrect->x : 0;
    int src_y = srcrect ? srcrect->y : 0;
    int src_w = srcrect ? srcrect->w : texture->w;
    int src_h = srcrect ? srcrect->h : texture->h;

    int dst_x = dstrect ? dstrect->x : 0;
    int dst_y = dstrect ? dstrect->y : 0;
    int dst_w = dstrect ? dstrect->w : renderer->window->w;
    int dst_h = dstrect ? dstrect->h : renderer->window->h;

    // Copia i pixel dalla texture al renderer
    for (int y = 0; y < src_h; ++y) {
        for (int x = 0; x < src_w; ++x) {
            // Calcola l'indice del pixel nella texture
            uint32_t *src_pixel = (uint32_t *)((uint8_t *)texture->surface->pixels + (y + src_y) * texture->pitch + (x + src_x) * sizeof(uint32_t));

            // Calcola l'indice del pixel nel renderer
            uint32_t *dst_pixel = (uint32_t *)((uint8_t *)renderer->window->surface->pixels + (y + dst_y) * renderer->window->surface->pitch + (x + dst_x) * sizeof(uint32_t));

            // Copia il pixel
            *dst_pixel = *src_pixel;
        }
    }

    return SDL_TRUE; // Successo
}

int SDL_RenderCopyEx_sus(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    if (renderer == NULL || texture == NULL || renderer->window == NULL || renderer->window->surface == NULL
        || texture->surface == NULL || renderer->window->surface->pixels == NULL || texture->surface->pixels == NULL) {
        return -1; // Errore: parametri non validi
    }

    // Calcolo dei parametri di origine e destinazione
    int src_x = srcrect ? srcrect->x : 0;
    int src_y = srcrect ? srcrect->y : 0;
    int src_w = srcrect ? srcrect->w : texture->w;
    int src_h = srcrect ? srcrect->h : texture->h;

    int dst_x = dstrect ? dstrect->x : 0;
    int dst_y = dstrect ? dstrect->y : 0;
    int dst_w = dstrect ? dstrect->w : src_w;
    int dst_h = dstrect ? dstrect->h : src_h;

    SDL_Point rotation_center = center ? *center : (SDL_Point){dst_w / 2, dst_h / 2};

    // Calcolo dei coseni e dei seni per la rotazione
    double radians = angle * M_PI / 180.0;
    double cos_angle = cos(radians);
    double sin_angle = sin(radians);

    // Iterazione sui pixel della destinazione
    for (int y = 0; y < dst_h; ++y) {
        for (int x = 0; x < dst_w; ++x) {
            // Calcolo della posizione relativa al centro di rotazione
            int rel_x = x - rotation_center.x;
            int rel_y = y - rotation_center.y;

            // Trasformazione inversa per trovare il pixel di origine
            int src_rel_x = (int)(rel_x * cos_angle + rel_y * sin_angle);
            int src_rel_y = (int)(-rel_x * sin_angle + rel_y * cos_angle);

            int src_coord_x = src_rel_x + src_w / 2;
            int src_coord_y = src_rel_y + src_h / 2;

            // Flip se necessario
            if (flip & SDL_FLIP_HORIZONTAL) {
                src_coord_x = src_w - 1 - src_coord_x;
            }
            if (flip & SDL_FLIP_VERTICAL) {
                src_coord_y = src_h - 1 - src_coord_y;
            }

            // Verifica che i pixel di origine siano all'interno dell'area valida
            if (src_coord_x >= 0 && src_coord_x < src_w && src_coord_y >= 0 && src_coord_y < src_h) {
                // Calcolo dell'indirizzo del pixel di origine
                uint32_t *src_pixel = (uint32_t *)((uint8_t *)texture->surface->pixels + (src_coord_y + src_y) * texture->surface->pitch + (src_coord_x + src_x) * sizeof(uint32_t));

                // Calcolo dell'indirizzo del pixel di destinazione
                uint32_t *dst_pixel = (uint32_t *)((uint8_t *)renderer->window->surface->pixels + (y + dst_y) * renderer->window->surface->pitch + (x + dst_x) * sizeof(uint32_t));

                // Copia del pixel
                *dst_pixel = *src_pixel;
            }
        }
    }

    return 0; // Successo
}




void SDL_RenderPresent(SDL_Renderer * renderer)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_Flip(renderer->window->surface);
}
int SDL_RenderReadPixels(SDL_Renderer * renderer, const SDL_Rect * rect, Uint32 format, void *pixels, int pitch)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // nope, this is used to make screenshot
    return SDL_TRUE;
}

int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // oook...
    return SDL_TRUE;
}



void SDL_GameControllerClose(SDL_GameController *gamecontroller)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    LOG_FREE(gamecontroller);
}
SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController *gamecontroller)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_Joystick *su = (SDL_Joystick *)LOG_MALLOC(sizeof(SDL_Joystick), su);
    su->gamecontroller = gamecontroller;
    return su;
}
SDL_GameController* SDL_GameControllerOpen(int joystick_index)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    SDL_GameController *su = (SDL_GameController *)LOG_MALLOC(sizeof(SDL_GameController), su);
    su->joystick_index = joystick_index;
    return su;
}

int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode * mode)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // LIE HARD
    mode->format = SDL_PIXELFORMAT_RGB24;              /**< pixel format */
    mode->w = virtual_screen_width;                      /**< width, in screen coordinates */
    mode->h = virtual_screen_height;                      /**< height, in screen coordinates */
    mode->refresh_rate = 0;           /**< refresh rate (or zero for unspecified) */
    mode->driverdata = NULL;           /**< driver-specific data, initialize to 0 */
    return SDL_TRUE;
}

int SDL_GetRendererOutputSize(SDL_Renderer * renderer, int *w, int *h)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    *w = renderer->window->w;
    *h = renderer->window->h;
    return SDL_TRUE;
}
SDL_bool SDL_IsGameController(int joystick_index)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // obviously it is...
    return SDL_TRUE;
}
SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *joystick)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    return joystick->gamecontroller->joystick_index;
}
int SDL_NumJoysticks(void)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // obviously we have one...
    return 1;
}


SDL_RWops* SDL_RWFromFile(const char *file, const char *mode)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // sure sure
    return NULL;
}
int SDL_SaveBMP_RW(SDL_Surface * surface, SDL_RWops * dst, int freedst)
{
    LOGG_C( "KLOG %s %d %s", __FUNCTION__, __LINE__, __FILE__);
    // just flag it to be done later
    do_screenshot = 1;
    return SDL_TRUE;
}

void noSDL_wrapStartTimer()
{
    this_kernel->StartTimer();
}

unsigned noSDL_wrapCheckTimer()
{
    return this_kernel->CheckTimer();
}

unsigned noSDL_wrapCheckTimerMs()
{
    return this_kernel->CheckTimerMs();
}

void noSDL_wrapScreenLogAt(char *line, unsigned x, unsigned y)
{
    this_kernel->DrawColorRect (x, y, 800, 16, BLACK_COLOR);
    this_kernel->DrawText (x, y, BRIGHT_WHITE_COLOR, line, CKernel::TTextAlign::AlignLeft);
}

void noSDL_Speaker_Enable(int enable)
{
    if (enable)
        noSDL_wrapScreenLogAt((char*)"S:ON", 10, 10);
    else
        noSDL_wrapScreenLogAt((char*)"S:OFF", 10, 10);
}

void noSDL_Speaker_Update(int mode, int count)
{
    char deb[100];
    sprintf(deb, "S: M:%d, C:%d", mode, count);
    noSDL_wrapScreenLogAt(deb, 10, 30);
}

