#ifndef NOSDL_H_INCLUDED
#define NOSDL_H_INCLUDED

#include "noSDL_scancode.h"
#include "noSDL_keycode.h"
#include "noSDL_keysym.h"
#include "noSDL_pixels.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDL_INIT_TIMER          0x00000001u
#define SDL_INIT_AUDIO          0x00000010u
#define SDL_INIT_VIDEO          0x00000020u  /**< SDL_INIT_VIDEO implies SDL_INIT_EVENTS */
#define SDL_INIT_JOYSTICK       0x00000200u  /**< SDL_INIT_JOYSTICK implies SDL_INIT_EVENTS */
#define SDL_INIT_HAPTIC         0x00001000u
#define SDL_INIT_GAMECONTROLLER 0x00002000u  /**< SDL_INIT_GAMECONTROLLER implies SDL_INIT_JOYSTICK */
#define SDL_INIT_EVENTS         0x00004000u
#define SDL_INIT_SENSOR         0x00008000u
#define SDL_INIT_NOPARACHUTE    0x00100000u  /**< compatibility; this flag is ignored. */
#define SDL_INIT_EVERYTHING ( \
                SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | \
                SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR \
            )

#define SDL_SWSURFACE           1
#define SDL_WINDOWPOS_CENTERED  1
#define SDL_WINDOW_FULLSCREEN_DESKTOP   1
#define AUDIO_S16SYS 1

#define SDL_QUIT            1

// Events
#define SDL_KEYDOWN         2
#define SDL_MOUSEBUTTONDOWN 3
#define SDL_MOUSEBUTTONUP   4
#define SDL_MOUSEMOTION     5
#define SDL_KEYUP           6
#define SDL_MOD_KEYDOWN     7
#define SDL_MOD_KEYUP       8
#define SDL_CONTROLLERBUTTONDOWN	9
#define SDL_CONTROLLERBUTTONUP		10
#define SDL_CONTROLLERAXISMOTION	11
#define SDL_CONTROLLERDEVICEADDED	12
#define SDL_CONTROLLERDEVICEREMOVED	13
#define SDL_TEXTINPUT			14

#define SDL_BUTTON_LEFT     1
#define SDL_BUTTON_MIDDLE   2
#define SDL_BUTTON_RIGHT    3

typedef enum
{
    SDL_CONTROLLER_BUTTON_INVALID = -1,
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_GUIDE,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    SDL_CONTROLLER_BUTTON_MISC1,    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
    SDL_CONTROLLER_BUTTON_PADDLE1,  /* Xbox Elite paddle P1 */
    SDL_CONTROLLER_BUTTON_PADDLE2,  /* Xbox Elite paddle P3 */
    SDL_CONTROLLER_BUTTON_PADDLE3,  /* Xbox Elite paddle P2 */
    SDL_CONTROLLER_BUTTON_PADDLE4,  /* Xbox Elite paddle P4 */
    SDL_CONTROLLER_BUTTON_TOUCHPAD, /* PS4/PS5 touchpad button */
    SDL_CONTROLLER_BUTTON_MAX
} SDL_GameControllerButton;

typedef enum
{
    SDL_CONTROLLER_AXIS_INVALID = -1,
    SDL_CONTROLLER_AXIS_LEFTX,
    SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_RIGHTX,
    SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
    SDL_CONTROLLER_AXIS_MAX
} SDL_GameControllerAxis;

typedef enum
{
    SDL_TEXTUREACCESS_STATIC,    /**< Changes rarely, not lockable */
    SDL_TEXTUREACCESS_STREAMING, /**< Changes frequently, lockable */
    SDL_TEXTUREACCESS_TARGET     /**< Texture can be used as a render target */
} SDL_TextureAccess;

/**
 * Flip constants for SDL_RenderCopyEx
 */
typedef enum
{
    SDL_FLIP_NONE = 0x00000000,     /**< Do not flip */
    SDL_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
    SDL_FLIP_VERTICAL = 0x00000002     /**< flip vertically */
} SDL_RendererFlip;

typedef enum
{
    SDL_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
    SDL_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware acceleration */
    SDL_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized with the refresh rate */
    SDL_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports rendering to texture */
} SDL_RendererFlags;


typedef unsigned char SDL_bool;

typedef int SDL_RWops;

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned int SDL_GrabMode;

typedef struct SDL_button {
    int button;
} SDL_button;

typedef struct SDL_axis {
    int axis;
    int value;
} SDL_axis;

typedef struct SDL_device {
    int which;
} SDL_device;

typedef struct SDL_text {
    char text[16];
} SDL_text;

typedef struct SDL_Event {
    int type;

    SDL_button button;
    SDL_button cbutton;
    SDL_axis caxis;
    SDL_device cdevice;
    SDL_text text;

    int key_keysym_sym;
    int key_keysym_mod;

    int motion_xrel;
    int motion_yrel;
} SDL_Event;

typedef struct SDL_Surface {
    void *pixels;
    int w;
    int h;
    int pitch;
} SDL_Surface;

typedef struct SDL_Rect {
    int x;
    int y;
    int w;
    int h;
} SDL_Rect;

typedef struct SDL_FRect {
    double x;
    double y;
    double w;
    double h;
} SDL_FRect;

typedef struct SDL_DisplayMode
{
    uint32_t format;              /**< pixel format */
    int w;                      /**< width, in screen coordinates */
    int h;                      /**< height, in screen coordinates */
    int refresh_rate;           /**< refresh rate (or zero for unspecified) */
    void *driverdata;           /**< driver-specific data, initialize to 0 */
} SDL_DisplayMode;

typedef struct SDL_Point {
    int x;
    int y;
} SDL_Point;

typedef struct SDL_FPoint {
    double x;
    double y;
} SDL_FPoint;

typedef struct SDL_Window {
    char title[128];
    int x;
    int y;
    int w;
    int h;
    Uint32 flags;
    SDL_Surface *surface;
} SDL_Window;
typedef struct SDL_Renderer {
    SDL_Window * window;
    SDL_Point scale;
    int index;
    Uint32 flags;
    Uint32 clearcolor;
} SDL_Renderer;
typedef struct SDL_Texture {
    SDL_Renderer * renderer;
    Uint32 format;
    int access;
    int w;
    int h;
    int pitch;
    SDL_Surface *surface;
} SDL_Texture;

typedef struct SDL_GameController {
    int joystick_index;
} SDL_GameController;
typedef struct SDL_Joystick {
    SDL_GameController *gamecontroller;
} SDL_Joystick;

typedef int SDL_GameControllerID;
typedef int SDL_JoystickID;

typedef Uint16 SDL_AudioFormat;
typedef Uint32 SDL_AudioDeviceID;
typedef void (*SDL_AudioCallback) (void *userdata, Uint8 * stream, int len);

typedef struct SDL_AudioSpec
{
    int freq;                   /**< DSP frequency -- samples per second */
    SDL_AudioFormat format;     /**< Audio data format */
    Uint8 channels;             /**< Number of channels: 1 mono, 2 stereo */
    Uint8 silence;              /**< Audio buffer silence value (calculated) */
    Uint16 samples;             /**< Audio buffer size in sample FRAMES (total samples divided by channel count) */
    Uint16 padding;             /**< Necessary for some compile environments */
    Uint32 size;                /**< Audio buffer size in bytes (calculated) */
    SDL_AudioCallback callback; /**< Callback that feeds the audio device (NULL to use SDL_QueueAudio()). */
    void *userdata;             /**< Userdata passed to callback (ignored for NULL callbacks). */
} SDL_AudioSpec;

#define SDL_RectEmpty(X)    ((!(X)) || ((X)->w <= 0) || ((X)->h <= 0))

#define SDL_BlitSurface SDL_UpperBlit
#define SDL_BlitScaled SDL_UpperBlitScaled

extern SDL_Event static_event[6];
extern SDL_Event static_mod_event[8];
extern SDL_Event static_mouse_move_event;
extern SDL_Event static_mouse_button_event[3];

extern int lastmousex;
extern int lastmousey;

#define SDL_TRUE 1
#define SDL_FALSE 0

/***
  SDL super weak emulation
**/

void noSDL_addModDown(int slot, int mod);
void noSDL_addModUp(int slot, int mod);
int SDL_PollEvent(SDL_Event *event);

int SDL_Init(Uint32 flags);

int SDL_ShowCursor(int toggle);
void SDL_Delay(Uint32 ms);

int SDL_Flip(SDL_Surface *screen);
// this is the true blit to circle kernel
int noSDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);

void SDL_FreeSurface(SDL_Surface *surface);
SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
SDL_Surface * SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

void SDL_WM_SetCaption(const char *title, const char *icon);
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);

SDL_AudioDeviceID SDL_OpenAudioDevice( const char *device, int iscapture, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int allowed_changes);
void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on);

SDL_Surface* SDL_CreateRGBSurfaceWithFormat (Uint32 flags, int width, int height, int depth, Uint32 format);
SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags);
SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, Uint32 format, int access, int w, int h);
SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
void SDL_DestroyRenderer(SDL_Renderer * renderer);
void SDL_DestroyTexture(SDL_Texture * texture);
void SDL_DestroyWindow(SDL_Window * window);
void SDL_GameControllerClose(SDL_GameController *gamecontroller);
SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController *gamecontroller);
SDL_GameController* SDL_GameControllerOpen(int joystick_index);
int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode * mode);
int SDL_GetRendererOutputSize(SDL_Renderer * renderer, int *w, int *h);
SDL_bool SDL_IsGameController(int joystick_index);
SDL_JoystickID SDL_JoystickInstanceID(SDL_Joystick *joystick);
int SDL_NumJoysticks(void);
void SDL_Quit(void);
int SDL_RenderClear(SDL_Renderer * renderer);


void
SDL_RenderGetViewport(SDL_Renderer * renderer, SDL_Rect * rect);

SDL_bool SDL_IntersectRect(const SDL_Rect * A, const SDL_Rect * B, SDL_Rect * result);

int SDL_UpperBlitScaled(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);
int SDL_UpperBlit(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);
int SDL_LowerBlitScaled(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect);


int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect,
                   const double angle, const SDL_Point *center, const SDL_RendererFlip flip);
void SDL_RenderPresent(SDL_Renderer * renderer);
int SDL_RenderReadPixels(SDL_Renderer * renderer, const SDL_Rect * rect, Uint32 format, void *pixels, int pitch);
int SDL_RenderSetScale(SDL_Renderer * renderer, float scaleX, float scaleY);

SDL_RWops* SDL_RWFromFile(const char *file, const char *mode);
int SDL_SaveBMP_RW(SDL_Surface * surface, SDL_RWops * dst, int freedst);
#define SDL_SaveBMP(surface, file) \
        SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "wb"), 1)

int SDL_SetRenderDrawColor(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int SDL_SetWindowFullscreen(SDL_Window * window, Uint32 flags);
void SDL_SetWindowSize(SDL_Window * window, int w, int h);
int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect, const void *pixels, int pitch);

/***
  noSDL additions
**/

void *noSDL_HighMem_Alloc(long size);
void noSDL_HighMem_Delete(void *p);

void noSDL_UpdateUSB();
uint64_t noSDL_fileGetSize(char *fname);
uint64_t noSDL_fileFullRead(char *fname, void *buffer, uint64_t size);

void noSDL_Kernel_Log(const char *line);

void noSDL_Speaker_Enable(int enable);
void noSDL_Speaker_Update(int mode, int count);


void noSDL_wrapStartTimer();
unsigned noSDL_wrapCheckTimer();
unsigned noSDL_wrapCheckTimerMs();
void noSDL_wrapScreenLogAt(char *line, unsigned x, unsigned y);

#ifdef __cplusplus
}
#endif
#endif // NOSDL_H_INCLUDED
