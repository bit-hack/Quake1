/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// gl_vidnt.c -- NT GL vid component

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>

#include "quakedef.h"
#include "winquake.h"
#include "resource.h"

#define MAX_MODE_LIST 600 //johnfitz -- was 30
#define VID_ROW_SIZE 3
#define WARP_WIDTH 320
#define WARP_HEIGHT 200
#define MAXWIDTH 10000
#define MAXHEIGHT 10000
#define BASEWIDTH 320
#define BASEHEIGHT 200

#define MODE_WINDOWED 0
#define NO_MODE (MODE_WINDOWED - 1)
#define MODE_FULLSCREEN_DEFAULT (MODE_WINDOWED + 1)

typedef struct
{
    modestate_t type;
    int width;
    int height;
    int modenum;
    int dib;
    int fullscreen;
    int bpp;
    int refreshrate; //johnfitz
    int halfscreen;
    char modedesc[17];
} vmode_t;

typedef struct
{
    int width;
    int height;
} lmode_t;

lmode_t lowresmodes[] = {
    { 320, 200 },
    { 320, 240 },
    { 400, 300 },
    { 512, 384 },
};

const char* gl_vendor;
const char* gl_renderer;
const char* gl_version;
const char* gl_extensions = "";
const char* wgl_extensions = "";

qboolean DDActive;
qboolean scr_skipupdate;

// XXX: default init this list
static vmode_t modelist[MAX_MODE_LIST];

static int nummodes;
static vmode_t* pcurrentmode;
static vmode_t badmode;

static DEVMODE gdevmode;
static qboolean vid_initialized = false;
static qboolean windowed, leavecurrentmode;
static qboolean vid_canalttab = false;
static qboolean vid_wassuspended = false;
static int windowed_mouse;
extern qboolean mouseactive; // from in_win.c

int DIBWidth, DIBHeight;
RECT WindowRect;

// HWND mainwindow, dibwindow;

int vid_modenum = NO_MODE;
int vid_realmode;
int vid_default = MODE_WINDOWED;
static int windowed_default;
unsigned char vid_curpal[256 * 3];
static qboolean fullsbardraw = false;

glvert_t glv;

viddef_t vid; // global video state

//unsigned short	d_8to16table[256]; //johnfitz -- never used
//unsigned char		d_15to8table[65536]; //johnfitz -- never used

modestate_t modestate = MS_UNINIT;

void VID_Menu_Init(void); //johnfitz
void VID_Menu_f(void); //johnfitz
void VID_MenuDraw(void);
void VID_MenuKey(int key);

char* VID_GetModeDescription(int mode);
void ClearAllStates(void);
void VID_UpdateWindowStatus(void);
void GL_Init(void);

PROC glArrayElementEXT;
PROC glColorPointerEXT;
PROC glTexCoordPointerEXT;
PROC glVertexPointerEXT;

typedef void(APIENTRY* lp3DFXFUNC)(int, int, int, int, int, const void*);

extern MTEXCOORDFUNC GL_MTexCoord2fFunc = NULL; //johnfitz
extern SELECTTEXFUNC GL_SelectTextureFunc = NULL; //johnfitz

typedef BOOL(APIENTRY* SETSWAPFUNC)(int); //johnfitz
typedef int(APIENTRY* GETSWAPFUNC)(void); //johnfitz
SETSWAPFUNC wglSwapIntervalEXT = NULL; //johnfitz
GETSWAPFUNC wglGetSwapIntervalEXT = NULL; //johnfitz

qboolean isPermedia = false;
qboolean isIntelVideo = false; //johnfitz -- intel video workarounds from Baker
qboolean gl_mtexable = false;
qboolean gl_texture_env_combine = false; //johnfitz
qboolean gl_texture_env_add = false; //johnfitz
qboolean gl_swap_control = false; //johnfitz
qboolean gl_anisotropy_able = false; //johnfitz
float gl_max_anisotropy; //johnfitz

int gl_stencilbits; //johnfitz

qboolean vid_locked = false; //johnfitz

void GL_SetupState(void); //johnfitz

//====================================

//johnfitz -- new cvars
cvar_t vid_fullscreen = { "vid_fullscreen", "1", true };
cvar_t vid_width = { "vid_width", "640", true };
cvar_t vid_height = { "vid_height", "480", true };
cvar_t vid_bpp = { "vid_bpp", "16", true };
cvar_t vid_refreshrate = { "vid_refreshrate", "60", true };
cvar_t vid_vsync = { "vid_vsync", "0", true };
//johnfitz

cvar_t _windowed_mouse = { "_windowed_mouse", "1", true };
cvar_t vid_gamma = { "gamma", "1", true }; //johnfitz -- moved here from view.c

int window_x, window_y, window_width, window_height;
RECT window_rect;

//==========================================================================
//
//  HARDWARE GAMMA -- johnfitz
//
//==========================================================================

typedef int(WINAPI* RAMPFUNC)();
#if 0
static RAMPFUNC wglGetDeviceGammaRamp3DFX;
static RAMPFUNC wglSetDeviceGammaRamp3DFX;
#endif

static unsigned short vid_gammaramp[768];
static unsigned short vid_systemgammaramp[768]; //to restore gamma on exit
static unsigned short vid_3dfxgammaramp[768]; //to restore gamma on exit
static int vid_gammaworks, vid_3dfxgamma;

/*
================
VID_Gamma_SetGamma -- apply gamma correction
================
*/
void VID_Gamma_SetGamma(void)
{
#if 0
    if (maindc)
    {
        if (vid_gammaworks)
            if (SetDeviceGammaRamp(maindc, vid_gammaramp) == false)
                Con_Printf("VID_Gamma_SetGamma: failed on SetDeviceGammaRamp\n");

        if (vid_3dfxgamma)
            if (wglSetDeviceGammaRamp3DFX(maindc, vid_gammaramp) == false)
                Con_Printf("VID_Gamma_SetGamma: failed on wglSetDeviceGammaRamp3DFX\n");
    }
#endif
}

/*
================
VID_Gamma_Restore -- restore system gamma
================
*/
void VID_Gamma_Restore(void)
{
#if 0
    if (maindc)
    {
        if (vid_gammaworks)
            if (SetDeviceGammaRamp(maindc, vid_systemgammaramp) == false)
                Con_Printf("VID_Gamma_Restore: failed on SetDeviceGammaRamp\n");

        if (vid_3dfxgamma)
            if (wglSetDeviceGammaRamp3DFX(maindc, vid_3dfxgammaramp) == false)
                Con_Printf("VID_Gamma_Restore: failed on wglSetDeviceGammaRamp3DFX\n");
    }
#endif
}

/*
================
VID_Gamma_Shutdown -- called on exit
================
*/
void VID_Gamma_Shutdown(void)
{
    VID_Gamma_Restore();
}

/*
================
VID_Gamma_f -- callback when the cvar changes
================
*/
void VID_Gamma_f(void)
{
    static float oldgamma;
    int i;

    if (vid_gamma.value == oldgamma)
        return;

    oldgamma = vid_gamma.value;

    for (i = 0; i < 256; i++)
        vid_gammaramp[i] = vid_gammaramp[i + 256] = vid_gammaramp[i + 512] = CLAMP(0, (int)(255 * pow((i + 0.5) / 255.5, vid_gamma.value) + 0.5), 255) << 8;

    VID_Gamma_SetGamma();
}

/*
================
VID_Gamma_Init -- call on init
================
*/
void VID_Gamma_Init(void)
{
#if 0
    vid_gammaworks = vid_3dfxgamma = false;

    if (strstr(gl_extensions, "WGL_3DFX_gamma_control"))
    {
        wglSetDeviceGammaRamp3DFX = (RAMPFUNC)wglGetProcAddress("wglSetDeviceGammaRamp3DFX");
        wglGetDeviceGammaRamp3DFX = (RAMPFUNC)wglGetProcAddress("wglGetDeviceGammaRamp3DFX");

        if (wglGetDeviceGammaRamp3DFX(maindc, vid_3dfxgammaramp))
            vid_3dfxgamma = true;

        Con_Printf("WGL_3DFX_gamma_control found\n");
    }

    if (GetDeviceGammaRamp(maindc, vid_systemgammaramp))
        vid_gammaworks = true;

    Cvar_RegisterVariable(&vid_gamma, VID_Gamma_f);
#endif
}

//==========================================================================

// direct draw software compatability stuff

void VID_HandlePause(qboolean pause)
{
}

void VID_ForceLockState(int lk)
{
}

void VID_LockBuffer(void)
{
}

void VID_UnlockBuffer(void)
{
}

int VID_ForceUnlockedAndReturnState(void)
{
    return 0;
}

void D_BeginDirectRect(int x, int y, byte* pbitmap, int width, int height)
{
}

void D_EndDirectRect(int x, int y, int width, int height)
{
}

static qboolean CreateSDLWindow(int modenum, qboolean fullscreen)
{
    // make sure that sdl was initalized
    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            Sys_Error("Unable to initalize SDL, SDL_Init failed");
        }
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // window creation flags
    const uint32_t flags = SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0);

    // create an opengl window
    SDL_Surface* surface = SDL_SetVideoMode(
        modelist[modenum].width,
        modelist[modenum].height,
        32, flags);
    if (surface == NULL)
    {
        Sys_Error("SDL_SetVideoMode failed");
    }

    //johnfitz -- stuff
    vid.width = modelist[modenum].width;
    vid.height = modelist[modenum].height;
    vid.conwidth = vid.width & 0xFFFFFFF8;
    vid.conheight = vid.conwidth * vid.height / vid.width;
    //johnfitz

    // whats this used for?
    vid.numpages = 2;

    // set the window rect
    WindowRect.top = 0;
    WindowRect.left = 0;
    WindowRect.right = modelist[modenum].width;
    WindowRect.bottom = modelist[modenum].height;

    // set the DIB size
    DIBWidth = modelist[modenum].width;
    DIBHeight = modelist[modenum].height;

    return true;
}

/*
================
VID_SetWindowedMode
================
*/
qboolean VID_SetWindowedMode(int modenum)
{
    modestate = MS_WINDOWED;
    return CreateSDLWindow(modenum, false);
}

/*
================
VID_SetFullDIBMode
================
*/
qboolean VID_SetFullDIBMode(int modenum)
{
    modestate = MS_FULLDIB;
    return CreateSDLWindow(modenum, true);
}

/*
================
VID_SetMode
================
*/
int VID_SetMode(int modenum)
{
    int original_mode, temp;
    qboolean stat;

    // so Con_Printfs don't mess us up by forcing vid and snd updates
    temp = scr_disabled_for_loading;
    scr_disabled_for_loading = true;

    CDAudio_Pause();

    if (vid_modenum == NO_MODE)
        original_mode = windowed_default;
    else
        original_mode = vid_modenum;

    // Set either the fullscreen or windowed mode
    if (modelist[modenum].type == MS_WINDOWED)
    {
        if (_windowed_mouse.value && key_dest == key_game)
        {
            stat = VID_SetWindowedMode(modenum);
            IN_ActivateMouse();
            IN_HideMouse();
        }
        else
        {
            IN_DeactivateMouse();
            IN_ShowMouse();
            stat = VID_SetWindowedMode(modenum);
        }
    }
    else if (modelist[modenum].type == MS_FULLDIB)
    {
        stat = VID_SetFullDIBMode(modenum);
        IN_ActivateMouse();
        IN_HideMouse();
    }
    else
    {
        Sys_Error("VID_SetMode: Bad mode type in modelist");
    }

    window_width = DIBWidth;
    window_height = DIBHeight;
    VID_UpdateWindowStatus();

    CDAudio_Resume();
    scr_disabled_for_loading = temp;

    if (!stat)
    {
        Sys_Error("Couldn't set video mode");
    }

    // now we try to make sure we get the focus on the mode switch, because
    // sometimes in some systems we don't.  We grab the foreground, then
    // finish setting up, pump all our messages, and sleep for a little while
    // to let messages finish bouncing around the system, then we put
    // ourselves at the top of the z order, then grab the foreground again,
    // Who knows if it helps, but it probably doesn't hurt
    vid_modenum = modenum;

    // fix the leftover Alt from any Alt-Tab or the like that switched us away
    ClearAllStates();

    if (!msg_suppress_1)
        Con_SafePrintf("Video mode %s initialized\n", VID_GetModeDescription(vid_modenum));

    vid.recalc_refdef = 1;

    return true;
}

/*
===============
VID_Vsync_f -- johnfitz
===============
*/
void VID_Vsync_f(void)
{
    if (gl_swap_control)
    {
        if (vid_vsync.value)
        {
            if (!wglSwapIntervalEXT(1))
                Con_Printf("VID_Vsync_f: failed on wglSwapIntervalEXT\n");
        }
        else
        {
            if (!wglSwapIntervalEXT(0))
                Con_Printf("VID_Vsync_f: failed on wglSwapIntervalEXT\n");
        }
    }
}

/*
===================
VID_Restart -- johnfitz -- change video modes on the fly
===================
*/
void VID_Restart(void)
{
    // TODO
}

/*
================
VID_Test -- johnfitz -- like vid_restart, but asks for confirmation after switching modes
================
*/
void VID_Test(void)
{
    vmode_t oldmode;
    qboolean mode_changed = false;

    if (vid_locked)
        return;
    //
    // check cvars against current mode
    //
    if (vid_fullscreen.value)
    {
        if (modelist[vid_default].type == MS_WINDOWED)
            mode_changed = true;
        else if (modelist[vid_default].bpp != (int)vid_bpp.value)
            mode_changed = true;
        else if (modelist[vid_default].refreshrate != (int)vid_refreshrate.value)
            mode_changed = true;
    }
    else if (modelist[vid_default].type != MS_WINDOWED)
        mode_changed = true;

    if (modelist[vid_default].width != (int)vid_width.value || modelist[vid_default].height != (int)vid_height.value)
        mode_changed = true;

    if (!mode_changed)
        return;
    //
    // now try the switch
    //
    oldmode = modelist[vid_default];

    VID_Restart();

    //pop up confirmation dialoge
    if (!SCR_ModalMessage("Would you like to keep this\nvideo mode? (y/n)\n", 5.0f))
    {
        //revert cvars and mode
        Cvar_Set("vid_width", va("%i", oldmode.width));
        Cvar_Set("vid_height", va("%i", oldmode.height));
        Cvar_Set("vid_bpp", va("%i", oldmode.bpp));
        Cvar_Set("vid_refreshrate", va("%i", oldmode.refreshrate));
        Cvar_Set("vid_fullscreen", (oldmode.type == MS_WINDOWED) ? "0" : "1");
        VID_Restart();
    }
}

/*
================
VID_Unlock -- johnfitz
================
*/
void VID_Unlock(void)
{
    vid_locked = false;
    //sync up cvars in case they were changed during the lock
    Cvar_Set("vid_width", va("%i", modelist[vid_default].width));
    Cvar_Set("vid_height", va("%i", modelist[vid_default].height));
    Cvar_Set("vid_bpp", va("%i", modelist[vid_default].bpp));
    Cvar_Set("vid_refreshrate", va("%i", modelist[vid_default].refreshrate));
    Cvar_Set("vid_fullscreen", (windowed) ? "0" : "1");
}

/*
================
VID_UpdateWindowStatus
================
*/
void VID_UpdateWindowStatus(void)
{
    window_rect.left = window_x;
    window_rect.top = window_y;
    window_rect.right = window_x + window_width;
    window_rect.bottom = window_y + window_height;
    IN_UpdateClipCursor();
}

//==============================================================================
//
//	OPENGL STUFF
//
//==============================================================================

/*
===============
GL_MakeNiceExtensionsList -- johnfitz
===============
*/
static char* GL_MakeNiceExtensionsList(const char* in)
{
    char *copy, *token, *out;
    int i, count;

    //each space will be replaced by 4 chars, so count the spaces before we malloc
    for (i = 0, count = 1; i < strlen(in); i++)
        if (in[i] == ' ')
            count++;
    out = Z_Malloc(strlen(in) + count * 3 + 1); //usually about 1-2k
    out[0] = 0;

    copy = Z_Malloc(strlen(in) + 1);
    strcpy(copy, in);

    for (token = strtok(copy, " "); token; token = strtok(NULL, " "))
    {
        strcat(out, "\n   ");
        strcat(out, token);
    }

    Z_Free(copy);
    return out;
}

/*
===============
GL_Info_f -- johnfitz
===============
*/
static void GL_Info_f(void)
{
    static char* gl_extensions_nice = NULL;
    static char* wgl_extensions_nice = NULL;

    if (!gl_extensions_nice)
        gl_extensions_nice = GL_MakeNiceExtensionsList(gl_extensions);

    if (!wgl_extensions_nice)
        wgl_extensions_nice = GL_MakeNiceExtensionsList(wgl_extensions);

    Con_SafePrintf("GL_VENDOR: %s\n", gl_vendor);
    Con_SafePrintf("GL_RENDERER: %s\n", gl_renderer);
    Con_SafePrintf("GL_VERSION: %s\n", gl_version);
    Con_Printf("GL_EXTENSIONS: %s\n", gl_extensions_nice);
    Con_Printf("WGL_EXTENSIONS: %s\n", wgl_extensions_nice);
}

/*
===============
CheckArrayExtensions
===============
*/
static void CheckArrayExtensions(void)
{
    char* tmp;

    /* check for texture extension */
    tmp = (unsigned char*)glGetString(GL_EXTENSIONS);
    while (*tmp)
    {
        if (strncmp((const char*)tmp, "GL_EXT_vertex_array", strlen("GL_EXT_vertex_array")) == 0)
        {
            if (
                ((glArrayElementEXT = wglGetProcAddress("glArrayElementEXT")) == NULL) || ((glColorPointerEXT = wglGetProcAddress("glColorPointerEXT")) == NULL) || ((glTexCoordPointerEXT = wglGetProcAddress("glTexCoordPointerEXT")) == NULL) || ((glVertexPointerEXT = wglGetProcAddress("glVertexPointerEXT")) == NULL))
            {
                Sys_Error("GetProcAddress for vertex extension failed");
                return;
            }
            return;
        }
        tmp++;
    }

    Sys_Error("Vertex array extension not present");
}

/*
===============
GL_CheckExtensions -- johnfitz
===============
*/
static void GL_CheckExtensions(void)
{
    //
    // multitexture
    //
    if (COM_CheckParm("-nomtex"))
        Con_Warning("Mutitexture disabled at command line\n");
    else if (strstr(gl_extensions, "GL_ARB_multitexture"))
    {
        GL_MTexCoord2fFunc = (void*)wglGetProcAddress("glMultiTexCoord2fARB");
        GL_SelectTextureFunc = (void*)wglGetProcAddress("glActiveTextureARB");
        if (GL_MTexCoord2fFunc && GL_SelectTextureFunc)
        {
            Con_Printf("FOUND: ARB_multitexture\n");
            TEXTURE0 = GL_TEXTURE0_ARB;
            TEXTURE1 = GL_TEXTURE1_ARB;
            gl_mtexable = true;
        }
        else
            Con_Warning("multitexture not supported (wglGetProcAddress failed)\n");
    }
    else if (strstr(gl_extensions, "GL_SGIS_multitexture"))
    {
        GL_MTexCoord2fFunc = (void*)wglGetProcAddress("glMTexCoord2fSGIS");
        GL_SelectTextureFunc = (void*)wglGetProcAddress("glSelectTextureSGIS");
        if (GL_MTexCoord2fFunc && GL_SelectTextureFunc)
        {
            Con_Printf("FOUND: SGIS_multitexture\n");
            TEXTURE0 = TEXTURE0_SGIS;
            TEXTURE1 = TEXTURE1_SGIS;
            gl_mtexable = true;
        }
        else
            Con_Warning("multitexture not supported (wglGetProcAddress failed)\n");
    }
    else
        Con_Warning("multitexture not supported (extension not found)\n");
    //
    // texture_env_combine
    //
    if (COM_CheckParm("-nocombine"))
        Con_Warning("texture_env_combine disabled at command line\n");
    else if (strstr(gl_extensions, "GL_ARB_texture_env_combine"))
    {
        Con_Printf("FOUND: ARB_texture_env_combine\n");
        gl_texture_env_combine = true;
    }
    else if (strstr(gl_extensions, "GL_EXT_texture_env_combine"))
    {
        Con_Printf("FOUND: EXT_texture_env_combine\n");
        gl_texture_env_combine = true;
    }
    else
        Con_Warning("texture_env_combine not supported\n");
    //
    // texture_env_add
    //
    if (COM_CheckParm("-noadd"))
        Con_Warning("texture_env_add disabled at command line\n");
    else if (strstr(gl_extensions, "GL_ARB_texture_env_add"))
    {
        Con_Printf("FOUND: ARB_texture_env_add\n");
        gl_texture_env_add = true;
    }
    else if (strstr(gl_extensions, "GL_EXT_texture_env_add"))
    {
        Con_Printf("FOUND: EXT_texture_env_add\n");
        gl_texture_env_add = true;
    }
    else
        Con_Warning("texture_env_add not supported\n");

    //
    // swap control
    //
    // XXX: Sort this mess out
    if (!wglSwapIntervalEXT || !wglGetSwapIntervalEXT)
    {
        wglSwapIntervalEXT = (SETSWAPFUNC)wglGetProcAddress("wglSwapIntervalEXT");
        wglGetSwapIntervalEXT = (GETSWAPFUNC)wglGetProcAddress("wglGetSwapIntervalEXT");

        if (wglSwapIntervalEXT && wglGetSwapIntervalEXT)
        {
            if (!wglSwapIntervalEXT(0))
                Con_Warning("vertical sync not supported (wglSwapIntervalEXT failed)\n");
            else if (wglGetSwapIntervalEXT() == -1)
                Con_Warning("vertical sync not supported (swap interval is -1.) Make sure you don't have vertical sync disabled in your driver settings.\n");
            else
            {
                Con_Printf("FOUND: WGL_EXT_swap_control\n");
                gl_swap_control = true;
            }
        }
        else
            Con_Warning("vertical sync not supported (wglGetProcAddress failed)\n");
    }

    //
    // anisotropic filtering
    //
    if (strstr(gl_extensions, "GL_EXT_texture_filter_anisotropic"))
    {
        float test1, test2;
        int tex;

        // test to make sure we really have control over it
        // 1.0 and 2.0 should always be legal values
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
        glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &test1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0f);
        glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &test2);
        glDeleteTextures(1, &tex);

        if (test1 == 1 && test2 == 2)
        {
            Con_Printf("FOUND: EXT_texture_filter_anisotropic\n");
            gl_anisotropy_able = true;
        }
        else
            Con_Warning("anisotropic filtering locked by driver. Current driver setting is %f\n", test1);

        //get max value either way, so the menu and stuff know it
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_anisotropy);
    }
    else
        Con_Warning("texture_filter_anisotropic not supported\n");
}

static void GetWGLExtensions(void)
{
    typedef const char*(__stdcall * wglGetExtensionsStringARB_t)(HDC hdc);
    typedef const char*(__stdcall * wglGetExtensionsStringEXT_t)();
    wglGetExtensionsStringARB_t wglGetExtensionsStringARB = (wglGetExtensionsStringARB_t)wglGetProcAddress("wglGetExtensionsStringARB");
    wglGetExtensionsStringEXT_t wglGetExtensionsStringEXT = (wglGetExtensionsStringEXT_t)wglGetProcAddress("wglGetExtensionsStringEXT");
    wgl_extensions = "";
    if (wglGetExtensionsStringARB)
    {
        HDC dc = wglGetCurrentDC();
        wgl_extensions = wglGetExtensionsStringARB(dc);
    }
    else if (wglGetExtensionsStringEXT)
    {
        wgl_extensions = wglGetExtensionsStringEXT();
    }
}

/*
===============
GL_SetupState -- johnfitz

does all the stuff from GL_Init that needs to be done every time a new GL render context is created
GL_Init will still do the stuff that only needs to be done once
===============
*/
void GL_SetupState(void)
{
    glClearColor(0.15, 0.15, 0.15, 0); //johnfitz -- originally 1,0,0,0
    glCullFace(GL_BACK); //johnfitz -- glquake used CCW with backwards culling -- let's do it right
    glFrontFace(GL_CW); //johnfitz -- glquake used CCW with backwards culling -- let's do it right
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.666);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_FLAT);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //johnfitz
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDepthRange(0, 1); //johnfitz -- moved here becuase gl_ztrick is gone.
    glDepthFunc(GL_LEQUAL); //johnfitz -- moved here becuase gl_ztrick is gone.
}

/*
===============
GL_Init
===============
*/
void GL_Init(void)
{
    gl_vendor = glGetString(GL_VENDOR);
    gl_renderer = glGetString(GL_RENDERER);
    gl_version = glGetString(GL_VERSION);
    gl_extensions = glGetString(GL_EXTENSIONS);

    GetWGLExtensions(); //johnfitz
    GL_CheckExtensions(); //johnfitz

    Cmd_AddCommand("gl_info", GL_Info_f); //johnfitz

    Cvar_RegisterVariable(&vid_vsync, VID_Vsync_f); //johnfitz

    if (strnicmp(gl_renderer, "PowerVR", 7) == 0)
        fullsbardraw = true;

    if (strnicmp(gl_renderer, "Permedia", 8) == 0)
        isPermedia = true;

    //johnfitz -- intel video workarounds from Baker
    if (!strcmp(gl_vendor, "Intel"))
    {
        Con_Printf("Intel Display Adapter detected\n");
        isIntelVideo = true;
    }
    //johnfitz

#if 0
	//johnfitz -- confirm presence of stencil buffer
	glGetIntegerv(GL_STENCIL_BITS, &gl_stencilbits);
	if(!gl_stencilbits)
		Con_Warning ("Could not create stencil buffer\n");
	else
		Con_Printf ("%i bit stencil buffer\n", gl_stencilbits);
#endif

    GL_SetupState(); //johnfitz
}

/*
=================
GL_BeginRendering -- sets values of glx, gly, glwidth, glheight
=================
*/
void GL_BeginRendering(int* x, int* y, int* width, int* height)
{
    *x = *y = 0;
    *width = WindowRect.right - WindowRect.left;
    *height = WindowRect.bottom - WindowRect.top;
}

/*
=================
GL_EndRendering
=================
*/
void GL_EndRendering(void)
{
    if (!scr_skipupdate || block_drawing)
    {
#if 0
        SwapBuffers(maindc);
#else
        SDL_GL_SwapBuffers();
#endif
    }

    // handle the mouse state when windowed if that's changed
    if (modestate == MS_WINDOWED)
    {
        if (!_windowed_mouse.value)
        {
            if (windowed_mouse)
            {
                IN_DeactivateMouse();
                IN_ShowMouse();
                windowed_mouse = false;
            }
        }
        else
        {
            windowed_mouse = true;
            if (key_dest == key_game && !mouseactive && ActiveApp)
            {
                IN_ActivateMouse();
                IN_HideMouse();
            }
            else if (mouseactive && key_dest != key_game)
            {
                IN_DeactivateMouse();
                IN_ShowMouse();
            }
        }
    }
    if (fullsbardraw)
        Sbar_Changed();
}

void VID_SetDefaultMode(void)
{
    IN_DeactivateMouse();
}

void VID_Shutdown(void)
{
    if (vid_initialized)
    {
        vid_canalttab = false;
    }
}

/*
===================================================================

MAIN WINDOW

===================================================================
*/

/*
================
ClearAllStates
================
*/
static void ClearAllStates(void)
{
    //johnfitz -- moved some code into Key_ClearStates
    Key_ClearStates();
    IN_ClearStates();
}

//==========================================================================
//
//  COMMANDS
//
//==========================================================================

/*
=================
VID_NumModes
=================
*/
int VID_NumModes(void)
{
    return nummodes;
}

/*
=================
VID_GetModePtr
=================
*/
vmode_t* VID_GetModePtr(int modenum)
{
    if ((modenum >= 0) && (modenum < nummodes))
        return &modelist[modenum];
    else
        return &badmode;
}

/*
=================
VID_GetModeDescription
=================
*/
char* VID_GetModeDescription(int mode)
{
    char* pinfo = "";
    vmode_t* pv;
    static char temp[100];

    if ((mode < 0) || (mode >= nummodes))
        return NULL;

    if (!leavecurrentmode)
    {
        pv = VID_GetModePtr(mode);
        pinfo = pv->modedesc;
    }
    else
    {
#if 0
        sprintf(temp, "Desktop resolution (%ix%ix%i)", //johnfitz -- added bpp
            modelist[MODE_FULLSCREEN_DEFAULT].width,
            modelist[MODE_FULLSCREEN_DEFAULT].height,
            modelist[MODE_FULLSCREEN_DEFAULT].bpp); //johnfitz -- added bpp
        pinfo = temp;
#endif
    }

    return pinfo;
}

// KJB: Added this to return the mode driver name in description for console
/*
=================
VID_GetExtModeDescription
=================
*/
char* VID_GetExtModeDescription(int mode)
{
    static char pinfo[40];
    vmode_t* pv;

    if ((mode < 0) || (mode >= nummodes))
        return NULL;

    pv = VID_GetModePtr(mode);
    if (modelist[mode].type == MS_FULLDIB)
    {
        if (!leavecurrentmode)
        {
            sprintf(pinfo, "%s fullscreen", pv->modedesc);
        }
        else
        {
            sprintf(pinfo, "Desktop resolution (%ix%ix%i)", //johnfitz -- added bpp
                modelist[MODE_FULLSCREEN_DEFAULT].width,
                modelist[MODE_FULLSCREEN_DEFAULT].height,
                modelist[MODE_FULLSCREEN_DEFAULT].bpp); //johnfitz -- added bpp
        }
    }
    else
    {
        if (modestate == MS_WINDOWED)
            sprintf(pinfo, "%s windowed", pv->modedesc);
        else
            sprintf(pinfo, "windowed");
    }

    return pinfo;
}

// command: "vid_describecurrentmode"
static void VID_DescribeCurrentMode_f(void)
{
    Con_Printf("%s\n", VID_GetExtModeDescription(vid_modenum));
}

// command: "vid_describemodes"
static void VID_DescribeModes_f(void)
{
    int i, lnummodes, t;
    vmode_t* pv;
    int lastwidth = 0, lastheight = 0, lastbpp = 0, count = 0;

    lnummodes = VID_NumModes();

    t = leavecurrentmode;
    leavecurrentmode = 0;

    for (i = 1; i < lnummodes; i++)
    {
        pv = VID_GetModePtr(i);
        if (lastwidth == pv->width && lastheight == pv->height && lastbpp == pv->bpp)
        {
            Con_SafePrintf(",%i", pv->refreshrate);
        }
        else
        {
            if (count > 0)
                Con_SafePrintf("\n");
            Con_SafePrintf("   %4i x %4i x %i : %i", pv->width, pv->height, pv->bpp, pv->refreshrate);
            lastwidth = pv->width;
            lastheight = pv->height;
            lastbpp = pv->bpp;
            count++;
        }
    }
    Con_Printf("\n%i modes\n", count);

    leavecurrentmode = t;
}

//==========================================================================
//
//  INIT
//
//==========================================================================

/*
=================
VID_InitDIB
=================
*/
static void VID_InitDIB()
{
    modelist[0].type = MS_WINDOWED;

    if (COM_CheckParm("-width"))
        modelist[0].width = Q_atoi(com_argv[COM_CheckParm("-width") + 1]);
    else
        modelist[0].width = 640;

    if (modelist[0].width < 320)
        modelist[0].width = 320;

    if (COM_CheckParm("-height"))
        modelist[0].height = Q_atoi(com_argv[COM_CheckParm("-height") + 1]);
    else
        modelist[0].height = modelist[0].width * 240 / 320;

    if (modelist[0].height < 200) //johnfitz -- was 240
        modelist[0].height = 200; //johnfitz -- was 240

    modelist[0].bpp = 32;
    modelist[0].refreshrate = 0;

    sprintf(modelist[0].modedesc, "%dx%dx%d %dHz", //johnfitz -- added bpp, refreshrate
        modelist[0].width,
        modelist[0].height,
        modelist[0].bpp, //johnfitz -- added bpp
        modelist[0].refreshrate); //johnfitz -- added refreshrate

    modelist[0].modenum = MODE_WINDOWED;
    modelist[0].dib = 1;
    modelist[0].fullscreen = 0;
    modelist[0].halfscreen = 0;

    nummodes = 1;
}

/*
===================
VID_Init
===================
*/
void VID_Init(void)
{
    //johnfitz -- clean up init readouts
    //Con_Printf("------------- Init Video -------------\n");
    //Con_Printf("%cVideo Init\n", 2);
    //johnfitz

    Cvar_RegisterVariable(&vid_fullscreen, NULL); //johnfitz
    Cvar_RegisterVariable(&vid_width, NULL); //johnfitz
    Cvar_RegisterVariable(&vid_height, NULL); //johnfitz
    Cvar_RegisterVariable(&vid_bpp, NULL); //johnfitz
    Cvar_RegisterVariable(&vid_refreshrate, NULL); //johnfitz
    Cvar_RegisterVariable(&_windowed_mouse, NULL);

    Cmd_AddCommand("vid_unlock", VID_Unlock); //johnfitz
    Cmd_AddCommand("vid_restart", VID_Restart); //johnfitz
    Cmd_AddCommand("vid_test", VID_Test); //johnfitz
    Cmd_AddCommand("vid_describecurrentmode", VID_DescribeCurrentMode_f);
    Cmd_AddCommand("vid_describemodes", VID_DescribeModes_f);

    // Enumerate display modes
    VID_InitDIB();

    vid_initialized = true;

    vid.maxwarpwidth = WARP_WIDTH;
    vid.maxwarpheight = WARP_HEIGHT;
    vid.colormap = host_colormap;
    vid.fullbright = 256 - LittleLong(*((int*)vid.colormap + 2048));

    //    DestroyWindow(hwnd_dialog);

    VID_SetMode(vid_default); // create our SDL window

    GL_Init();

    //johnfitz -- removed code to create "glquake" subdirectory

    vid_realmode = vid_modenum;
    vid_menucmdfn = VID_Menu_f; //johnfitz
    vid_menudrawfn = VID_MenuDraw;
    vid_menukeyfn = VID_MenuKey;

    strcpy(badmode.modedesc, "Bad mode");
    vid_canalttab = true;

    if (COM_CheckParm("-fullsbar"))
        fullsbardraw = true;

    VID_Gamma_Init(); //johnfitz
    VID_Menu_Init(); //johnfitz

    //johnfitz -- command line vid settings should override config file settings.
    //so we have to lock the vid mode from now until after all config files are read.
    if (COM_CheckParm("-width") || COM_CheckParm("-height") || COM_CheckParm("-bpp") || COM_CheckParm("-window"))
    {
        vid_locked = true;
    }
    //johnfitz
}

/*
================
VID_SyncCvars -- johnfitz -- set vid cvars to match current video mode
================
*/
void VID_SyncCvars(void)
{
    Cvar_Set("vid_width", va("%i", modelist[vid_default].width));
    Cvar_Set("vid_height", va("%i", modelist[vid_default].height));
    Cvar_Set("vid_bpp", va("%i", modelist[vid_default].bpp));
    Cvar_Set("vid_refreshrate", va("%i", modelist[vid_default].refreshrate));
    Cvar_Set("vid_fullscreen", (windowed) ? "0" : "1");
}

//==========================================================================
//
//  NEW VIDEO MENU -- johnfitz
//
//==========================================================================

extern void M_Menu_Options_f(void);
extern void M_Print(int cx, int cy, char* str);
extern void M_PrintWhite(int cx, int cy, char* str);
extern void M_DrawCharacter(int cx, int line, int num);
extern void M_DrawTransPic(int x, int y, qpic_t* pic);
extern void M_DrawPic(int x, int y, qpic_t* pic);
extern void M_DrawCheckbox(int x, int y, int on);

extern qboolean m_entersound;

enum
{
    m_none,
    m_main,
    m_singleplayer,
    m_load,
    m_save,
    m_multiplayer,
    m_setup,
    m_net,
    m_options,
    m_video,
    m_keys,
    m_help,
    m_quit,
    m_serialconfig,
    m_modemconfig,
    m_lanconfig,
    m_gameoptions,
    m_search,
    m_slist
} m_state;

#define VIDEO_OPTIONS_ITEMS 6
int video_cursor_table[] = { 48, 56, 64, 72, 88, 96 };
int video_options_cursor;

typedef struct
{
    int width, height;
} vid_menu_mode;

//TODO: replace these fixed-length arrays with hunk_allocated buffers

vid_menu_mode vid_menu_modes[MAX_MODE_LIST];
int vid_menu_nummodes;
int vid_menu_bpps[4];
int vid_menu_numbpps;
int vid_menu_rates[20];
int vid_menu_numrates;
int vid_menu_rwidth;
int vid_menu_rheight;

void VID_Menu_Init(void)
{
    int i, j, h, w;

    for (i = 1; i < nummodes; i++) //start i at mode 1 because 0 is windowed mode
    {
        w = modelist[i].width;
        h = modelist[i].height;

        for (j = 0; j < vid_menu_nummodes; j++)
        {
            if (vid_menu_modes[j].width == w && vid_menu_modes[j].height == h)
                break;
        }

        if (j == vid_menu_nummodes)
        {
            vid_menu_modes[j].width = w;
            vid_menu_modes[j].height = h;
            vid_menu_nummodes++;
        }
    }
}

/*
================
VID_Menu_RebuildBppList

regenerates bpp list based on current vid_width and vid_height
================
*/
void VID_Menu_RebuildBppList(void)
{
    int i, j, b;

    vid_menu_numbpps = 0;

    for (i = 1; i < nummodes; i++) //start i at mode 1 because 0 is windowed mode
    {
        //bpp list is limited to bpps available with current width/height
        if (modelist[i].width != vid_width.value || modelist[i].height != vid_height.value)
            continue;

        b = modelist[i].bpp;

        for (j = 0; j < vid_menu_numbpps; j++)
        {
            if (vid_menu_bpps[j] == b)
                break;
        }

        if (j == vid_menu_numbpps)
        {
            vid_menu_bpps[j] = b;
            vid_menu_numbpps++;
        }
    }

    //if there are no valid fullscreen bpps for this width/height, just pick one
    if (vid_menu_numbpps == 0)
    {
        Cvar_SetValue("vid_bpp", (float)modelist[0].bpp);
        return;
    }

    //if vid_bpp is not in the new list, change vid_bpp
    for (i = 0; i < vid_menu_numbpps; i++)
        if (vid_menu_bpps[i] == (int)(vid_bpp.value))
            break;

    if (i == vid_menu_numbpps)
        Cvar_SetValue("vid_bpp", (float)vid_menu_bpps[0]);
}

/*
================
VID_Menu_RebuildRateList

regenerates rate list based on current vid_width, vid_height and vid_bpp
================
*/
void VID_Menu_RebuildRateList(void)
{
    int i, j, r;

    vid_menu_numrates = 0;

    for (i = 1; i < nummodes; i++) //start i at mode 1 because 0 is windowed mode
    {
        //rate list is limited to rates available with current width/height/bpp
        if (modelist[i].width != vid_width.value || modelist[i].height != vid_height.value || modelist[i].bpp != vid_bpp.value)
            continue;

        r = modelist[i].refreshrate;

        for (j = 0; j < vid_menu_numrates; j++)
        {
            if (vid_menu_rates[j] == r)
                break;
        }

        if (j == vid_menu_numrates)
        {
            vid_menu_rates[j] = r;
            vid_menu_numrates++;
        }
    }

    //if there are no valid fullscreen refreshrates for this width/height, just pick one
    if (vid_menu_numrates == 0)
    {
        Cvar_SetValue("vid_refreshrate", (float)modelist[0].refreshrate);
        return;
    }

    //if vid_refreshrate is not in the new list, change vid_refreshrate
    for (i = 0; i < vid_menu_numrates; i++)
        if (vid_menu_rates[i] == (int)(vid_refreshrate.value))
            break;

    if (i == vid_menu_numrates)
        Cvar_SetValue("vid_refreshrate", (float)vid_menu_rates[0]);
}

/*
================
VID_Menu_CalcAspectRatio

calculates aspect ratio for current vid_width/vid_height
================
*/
void VID_Menu_CalcAspectRatio(void)
{
    int w, h, f;
    w = vid_width.value;
    h = vid_height.value;
    f = 2;
    while (f < w && f < h)
    {
        if ((w / f) * f == w && (h / f) * f == h)
        {
            w /= f;
            h /= f;
            f = 2;
        }
        else
            f++;
    }
    vid_menu_rwidth = w;
    vid_menu_rheight = h;
}

/*
================
VID_Menu_ChooseNextMode

chooses next resolution in order, then updates vid_width and
vid_height cvars, then updates bpp and refreshrate lists
================
*/
void VID_Menu_ChooseNextMode(int dir)
{
    int i;

    for (i = 0; i < vid_menu_nummodes; i++)
    {
        if (vid_menu_modes[i].width == vid_width.value && vid_menu_modes[i].height == vid_height.value)
            break;
    }

    if (i == vid_menu_nummodes) //can't find it in list, so it must be a custom windowed res
    {
        i = 0;
    }
    else
    {
        i += dir;
        if (i >= vid_menu_nummodes)
            i = 0;
        else if (i < 0)
            i = vid_menu_nummodes - 1;
    }

    Cvar_SetValue("vid_width", (float)vid_menu_modes[i].width);
    Cvar_SetValue("vid_height", (float)vid_menu_modes[i].height);
    VID_Menu_RebuildBppList();
    VID_Menu_RebuildRateList();
    VID_Menu_CalcAspectRatio();
}

/*
================
VID_Menu_ChooseNextBpp

chooses next bpp in order, then updates vid_bpp cvar, then updates refreshrate list
================
*/
void VID_Menu_ChooseNextBpp(int dir)
{
    int i;

    for (i = 0; i < vid_menu_numbpps; i++)
    {
        if (vid_menu_bpps[i] == vid_bpp.value)
            break;
    }

    if (i == vid_menu_numbpps) //can't find it in list
    {
        i = 0;
    }
    else
    {
        i += dir;
        if (i >= vid_menu_numbpps)
            i = 0;
        else if (i < 0)
            i = vid_menu_numbpps - 1;
    }

    Cvar_SetValue("vid_bpp", (float)vid_menu_bpps[i]);
    VID_Menu_RebuildRateList();
}

/*
================
VID_Menu_ChooseNextRate

chooses next refresh rate in order, then updates vid_refreshrate cvar
================
*/
void VID_Menu_ChooseNextRate(int dir)
{
    int i;

    for (i = 0; i < vid_menu_numrates; i++)
    {
        if (vid_menu_rates[i] == vid_refreshrate.value)
            break;
    }

    if (i == vid_menu_numrates) //can't find it in list
    {
        i = 0;
    }
    else
    {
        i += dir;
        if (i >= vid_menu_numrates)
            i = 0;
        else if (i < 0)
            i = vid_menu_numrates - 1;
    }

    Cvar_SetValue("vid_refreshrate", (float)vid_menu_rates[i]);
}

void VID_MenuKey(int key)
{
    switch (key)
    {
    case K_ESCAPE:
        VID_SyncCvars(); //sync cvars before leaving menu. FIXME: there are other ways to leave menu
        S_LocalSound("misc/menu1.wav");
        M_Menu_Options_f();
        break;

    case K_UPARROW:
        S_LocalSound("misc/menu1.wav");
        video_options_cursor--;
        if (video_options_cursor < 0)
            video_options_cursor = VIDEO_OPTIONS_ITEMS - 1;
        break;

    case K_DOWNARROW:
        S_LocalSound("misc/menu1.wav");
        video_options_cursor++;
        if (video_options_cursor >= VIDEO_OPTIONS_ITEMS)
            video_options_cursor = 0;
        break;

    case K_LEFTARROW:
        S_LocalSound("misc/menu3.wav");
        switch (video_options_cursor)
        {
        case 0:
            VID_Menu_ChooseNextMode(-1);
            break;
        case 1:
            VID_Menu_ChooseNextBpp(-1);
            break;
        case 2:
            VID_Menu_ChooseNextRate(-1);
            break;
        case 3:
            Cbuf_AddText("toggle vid_fullscreen\n");
            break;
        case 4:
        case 5:
        default:
            break;
        }
        break;

    case K_RIGHTARROW:
        S_LocalSound("misc/menu3.wav");
        switch (video_options_cursor)
        {
        case 0:
            VID_Menu_ChooseNextMode(1);
            break;
        case 1:
            VID_Menu_ChooseNextBpp(1);
            break;
        case 2:
            VID_Menu_ChooseNextRate(1);
            break;
        case 3:
            Cbuf_AddText("toggle vid_fullscreen\n");
            break;
        case 4:
        case 5:
        default:
            break;
        }
        break;

    case K_ENTER:
        m_entersound = true;
        switch (video_options_cursor)
        {
        case 0:
            VID_Menu_ChooseNextMode(1);
            break;
        case 1:
            VID_Menu_ChooseNextBpp(1);
            break;
        case 2:
            VID_Menu_ChooseNextRate(1);
            break;
        case 3:
            Cbuf_AddText("toggle vid_fullscreen\n");
            break;
        case 4:
            Cbuf_AddText("vid_test\n");
            break;
        case 5:
            Cbuf_AddText("vid_restart\n");
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
}

void VID_MenuDraw(void)
{
    int i = 0;
    qpic_t* p;
    char* title;

    M_DrawTransPic(16, 4, Draw_CachePic("gfx/qplaque.lmp"));

    //p = Draw_CachePic ("gfx/vidmodes.lmp");
    p = Draw_CachePic("gfx/p_option.lmp");
    M_DrawPic((320 - p->width) / 2, 4, p);

    // title
    title = "Video Options";
    M_PrintWhite((320 - 8 * strlen(title)) / 2, 32, title);

    // options
    M_Print(16, video_cursor_table[i], "        Video mode");
    M_Print(184, video_cursor_table[i], va("%ix%i (%i:%i)", (int)vid_width.value, (int)vid_height.value, vid_menu_rwidth, vid_menu_rheight));
    i++;

    M_Print(16, video_cursor_table[i], "       Color depth");
    M_Print(184, video_cursor_table[i], va("%i", (int)vid_bpp.value));
    i++;

    M_Print(16, video_cursor_table[i], "      Refresh rate");
    M_Print(184, video_cursor_table[i], va("%i Hz", (int)vid_refreshrate.value));
    i++;

    M_Print(16, video_cursor_table[i], "        Fullscreen");
    M_DrawCheckbox(184, video_cursor_table[i], (int)vid_fullscreen.value);
    i++;

    M_Print(16, video_cursor_table[i], "      Test changes");
    i++;

    M_Print(16, video_cursor_table[i], "     Apply changes");

    // cursor
    M_DrawCharacter(168, video_cursor_table[video_options_cursor], 12 + ((int)(realtime * 4) & 1));

    // notes          "345678901234567890123456789012345678"
    //	M_Print (16, 172, "Windowed modes always use the desk- ");
    //	M_Print (16, 180, "top color depth, and can never be   ");
    //	M_Print (16, 188, "larger than the desktop resolution. ");
}

void VID_Menu_f(void)
{
    key_dest = key_menu;
    m_state = m_video;
    m_entersound = true;

    //set all the cvars to match the current mode when entering the menu
    VID_SyncCvars();

    //set up bpp and rate lists based on current cvars
    VID_Menu_RebuildBppList();
    VID_Menu_RebuildRateList();

    //aspect ratio
    VID_Menu_CalcAspectRatio();
}
