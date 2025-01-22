

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

int SDL_Init(Uint32 flags)
{
    // unused, nothing real to initialize
    return SDL_FALSE;
}

void SDL_Quit(void)
{
    // absolutely not
}

SDL_AudioDeviceID SDL_OpenAudioDevice( const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes)
{
    memcpy(obtained, desired, sizeof(SDL_AudioSpec));
    return 1;
}

void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on)
{
}

int SDL_ShowCursor(int toggle)
{
    // unused
    return toggle;
}

void SDL_Delay(Uint32 ms)
{
    // delay is actually unused but let's take the opportunity to update the USB status
    this_kernel->MsPause(ms);
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
    // absolutely not
}

SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode)
{
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
    LOGG_C( "KLOG %s", line);
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
    free(surface);
}

int SDL_Flip(SDL_Surface *screen)
{
    // Nothing to explicitly flip
    return 0;
}

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
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

    TScreenColor pix[src->w * src->h];
    unsigned int temp;

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
    // LOG_C( "SDL_SetVideoMode %d, %d, %d, %d\n", width, height, bpp, DEPTH);

    if (height != 0)
        this_kernel->wrapResize(width, height);

    SDL_Surface *su = (SDL_Surface *)malloc(sizeof(SDL_Surface));
    su->w = width;
    su->h = height;
    su->pixels = NULL;

    return su;
}

SDL_Surface * SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    // LOG_C( "SDL_CreateRGBSurfaceFrom %d, %d, %d, %d\n", width, height, depth, pitch);

    SDL_Surface *su = (SDL_Surface *)malloc(sizeof(SDL_Surface));
    su->w = width;
    su->h = height;
    su->pixels = pixels;

    return su;
}

SDL_Surface* SDL_CreateRGBSurfaceWithFormat (Uint32 flags, int width, int height, int depth, Uint32 format)
{
    SDL_Surface *su = (SDL_Surface *)malloc(sizeof(SDL_Surface));
    su->w = width;
    su->h = height;
    su->pixels = malloc(width*height*4);

    return su;
}
SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags)
{
    SDL_Renderer *su = (SDL_Renderer *)malloc(sizeof(SDL_Renderer));
    su->window = window;
    su->index = index;
    su->flags = flags;
    return su;
}
SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, Uint32 format, int access, int w, int h)
{
    SDL_Texture *su = (SDL_Texture *)malloc(sizeof(SDL_Texture));
    su->renderer = renderer;
    su->format = format;
    su->access = access;
    su->w = w;
    su->h = h;
    su->pitch = h;
    su->surface = SDL_CreateRGBSurfaceWithFormat (renderer->flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);
    return su;
}
SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags)
{
    SDL_Window *su = (SDL_Window *)malloc(sizeof(SDL_Window));
    strncpy(su->title, title, 15);
    su->x = x;
    su->y = y;
    su->w = w;
    su->h = h;
    su->flags = flags;
    su->surface = SDL_CreateRGBSurfaceWithFormat (flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);

    // Surprise...
    SDL_SetVideoMode(w, h, 32, flags);

    return su;
}
void SDL_DestroyRenderer(SDL_Renderer * renderer)
{
    free(renderer);
}
void SDL_DestroyTexture(SDL_Texture * texture)
{
    free(texture->surface->pixels);
    free(texture->surface);
    free(texture);
}
void SDL_DestroyWindow(SDL_Window * window)
{
    free(window->surface->pixels);
    free(window->surface);
    free(window->title);
    free(window);
}

int SDL_SetRenderDrawColor(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // ok ok
    renderer->clearcolor = COLOR32 (r, g, b, a);
    return SDL_TRUE;
}
int SDL_SetWindowFullscreen(SDL_Window * window, Uint32 flags)
{
    // sure sure
    return SDL_TRUE;
}

void SDL_SetWindowSize(SDL_Window * window, int w, int h)
{
    if (window->surface != NULL)
        free(window->surface);

    window->w = w;
    window->h = h;
    window->surface = SDL_CreateRGBSurfaceWithFormat (window->flags, w, h, 8, SDL_PIXELFORMAT_RGBA32);

    // Surprise...
    SDL_SetVideoMode(w, h, 32, window->flags);
}

int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void *pixels, int pitch)
{
    // this is the real copy of all pixels
    // https://wiki.libsdl.org/SDL2/SDL_UpdateTexture

    // we have to support both SDL_PIXELFORMAT_RGB24 and SDL_PIXELFORMAT_BGR555
    // TODO
    return SDL_TRUE;
}
int SDL_RenderClear(SDL_Renderer * renderer)
{
    // here we should clear the screen with the render clearcolor
    // just memset to black
    memset(renderer->window->surface->pixels, 0, renderer->window->surface->w*renderer->window->surface->h*4);
    return SDL_TRUE;
}

int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect,
                   const double angle, const SDL_Point *center, const SDL_RendererFlip flip);

int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{
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
    int dst_w = dstrect ? dstrect->w : src_w;
    int dst_h = dstrect ? dstrect->h : src_h;

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

int SDL_RenderCopyEx(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip)
{
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
    SDL_Flip(renderer->window->surface);
}
int SDL_RenderReadPixels(SDL_Renderer * renderer, const SDL_Rect * rect, Uint32 format, void *pixels, int pitch)
{
    // nope, this is used to make screenshot
    return SDL_TRUE;
}

int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY)
{
    // oook...
    return SDL_TRUE;
}



void SDL_GameControllerClose(SDL_GameController *gamecontroller)
{
    free(gamecontroller);
}
SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController *gamecontroller)
{
    SDL_Joystick *su = (SDL_Joystick *)malloc(sizeof(SDL_Joystick));
    su->gamecontroller = gamecontroller;
    return su;
}
SDL_GameController* SDL_GameControllerOpen(int joystick_index)
{
    SDL_GameController *su = (SDL_GameController *)malloc(sizeof(SDL_GameController));
    su->joystick_index = joystick_index;
    return su;
}

int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode * mode)
{
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
    *w = renderer->window->w;
    *h = renderer->window->h;
    return SDL_TRUE;
}
SDL_bool SDL_IsGameController(int joystick_index)
{
    // obviously it is...
    return SDL_TRUE;
}
SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *joystick)
{
    return joystick->gamecontroller->joystick_index;
}
int SDL_NumJoysticks(void)
{
    // obviously we have one...
    return 1;
}


SDL_RWops* SDL_RWFromFile(const char *file, const char *mode)
{
    // sure sure
    return NULL;
}
int SDL_SaveBMP_RW(SDL_Surface * surface, SDL_RWops * dst, int freedst)
{
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

