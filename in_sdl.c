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

#include <SDL.h>

#include "quakedef.h"
#include "winquake.h"

#define DINPUT_BUFFERSIZE 16
#define iDirectInputCreate(a, b, c, d) pDirectInputCreate(a, b, c, d)

// mouse variables
cvar_t m_filter = { "m_filter", "0" };

//johnfitz -- compatibility with old Quake -- setting to 0 disables KP_* codes
cvar_t cl_keypad = { "cl_keypad", "1" };

int mouse_buttons;
int mouse_oldbuttonstate;
POINT current_pos;
int mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, my_accum;

static qboolean restore_spi;
static int originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };

unsigned int uiWheelMessage;
qboolean mouseactive;
qboolean mouseinitialized;
static qboolean mouseparmsvalid, mouseactivatetoggle;
static qboolean mouseshowtoggle = 1;

static unsigned int mstate_di;

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS 0x00000000 // control like a joystick
#define JOY_RELATIVE_AXIS 0x00000010 // control like a mouse, spinner, trackball
#define JOY_MAX_AXES 6 // X, Y, Z, R, U, V
#define JOY_AXIS_X 0
#define JOY_AXIS_Y 1
#define JOY_AXIS_Z 2
#define JOY_AXIS_R 3
#define JOY_AXIS_U 4
#define JOY_AXIS_V 5

enum _ControlList
{
    AxisNada = 0,
    AxisForward,
    AxisLook,
    AxisSide,
    AxisTurn
};

DWORD dwAxisFlags[JOY_MAX_AXES] = {
    JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ, JOY_RETURNR, JOY_RETURNU, JOY_RETURNV
};

DWORD dwAxisMap[JOY_MAX_AXES];
DWORD dwControlMap[JOY_MAX_AXES];
PDWORD pdwRawValue[JOY_MAX_AXES];

// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
cvar_t in_joystick = { "joystick", "0", true };
cvar_t joy_name = { "joyname", "joystick" };
cvar_t joy_advanced = { "joyadvanced", "0" };
cvar_t joy_advaxisx = { "joyadvaxisx", "0" };
cvar_t joy_advaxisy = { "joyadvaxisy", "0" };
cvar_t joy_advaxisz = { "joyadvaxisz", "0" };
cvar_t joy_advaxisr = { "joyadvaxisr", "0" };
cvar_t joy_advaxisu = { "joyadvaxisu", "0" };
cvar_t joy_advaxisv = { "joyadvaxisv", "0" };
cvar_t joy_forwardthreshold = { "joyforwardthreshold", "0.15" };
cvar_t joy_sidethreshold = { "joysidethreshold", "0.15" };
cvar_t joy_pitchthreshold = { "joypitchthreshold", "0.15" };
cvar_t joy_yawthreshold = { "joyyawthreshold", "0.15" };
cvar_t joy_forwardsensitivity = { "joyforwardsensitivity", "-1.0" };
cvar_t joy_sidesensitivity = { "joysidesensitivity", "-1.0" };
cvar_t joy_pitchsensitivity = { "joypitchsensitivity", "1.0" };
cvar_t joy_yawsensitivity = { "joyyawsensitivity", "-1.0" };
cvar_t joy_wwhack1 = { "joywwhack1", "0.0" };
cvar_t joy_wwhack2 = { "joywwhack2", "0.0" };

qboolean joy_avail, joy_advancedinit, joy_haspov;
DWORD joy_oldbuttonstate, joy_oldpovstate;

int joy_id;
DWORD joy_flags;
DWORD joy_numbuttons;

static JOYINFOEX ji;

static HINSTANCE hInstDI;

void IN_StartupJoystick(void);
void Joy_AdvancedUpdate_f(void);
void IN_JoyMove(usercmd_t* cmd);

/* Map from SDL to quake keynums */
static int MapKey(int key)
{
    switch (key)
    {
    case SDLK_RETURN:
        return K_ENTER;
    case SDLK_HOME:
        return K_HOME;
    case SDLK_UP:
        return K_UPARROW;
    case SDLK_PAGEUP:
        return K_PGUP;
    case SDLK_LEFT:
        return K_LEFTARROW;
    case SDLK_RIGHT:
        return K_RIGHTARROW;
    case SDLK_END:
        return K_END;
    case SDLK_DOWN:
        return K_DOWNARROW;
    case SDLK_PAGEDOWN:
        return K_PGDN;
    case SDLK_INSERT:
        return K_INS;
    case SDLK_DELETE:
        return K_DEL;
    case SDLK_ESCAPE:
        return K_ESCAPE;
    case SDLK_ASTERISK:
        return '*';
    case SDLK_MINUS:
        return '-';
    case SDLK_5:
        return '5';
    case SDLK_PLUS:
        return '+';
    case SDLK_LCTRL:
    case SDLK_RCTRL:
        return K_CTRL;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
        return K_SHIFT;
    default:
        return key;
    }
}

void Force_CenterView_f(void)
{
    cl.viewangles[PITCH] = 0;
}

void IN_UpdateClipCursor(void)
{
}

void IN_ShowMouse(void)
{
}

void IN_HideMouse(void)
{
}

void IN_ActivateMouse(void)
{
}

void IN_SetQuakeMouseState(void)
{
}

void IN_DeactivateMouse(void)
{
}

void IN_RestoreOriginalMouseState(void)
{
}

qboolean IN_InitDInput(void)
{
    return false;
}

void IN_StartupMouse(void)
{
    if (COM_CheckParm("-nomouse"))
        return;

    mouseinitialized = true;

    mouseparmsvalid = SystemParametersInfo(SPI_GETMOUSE, 0, originalmouseparms, 0);

    if (mouseparmsvalid)
    {
        if (COM_CheckParm("-noforcemspd"))
            newmouseparms[2] = originalmouseparms[2];

        if (COM_CheckParm("-noforcemaccel"))
        {
            newmouseparms[0] = originalmouseparms[0];
            newmouseparms[1] = originalmouseparms[1];
        }

        if (COM_CheckParm("-noforcemparms"))
        {
            newmouseparms[0] = originalmouseparms[0];
            newmouseparms[1] = originalmouseparms[1];
            newmouseparms[2] = originalmouseparms[2];
        }
    }

    mouse_buttons = 3;

    // if a fullscreen video mode was set before the mouse was initialized,
    // set the mouse state appropriately
    if (mouseactivatetoggle)
        IN_ActivateMouse();
}

void IN_Init(void)
{
    //johnfitz -- clean up init readouts
    //Con_Printf("------------- Init Input -------------\n");
    //Con_Printf("%cInput Init\n", 2);
    //johnfitz

    // mouse variables
    Cvar_RegisterVariable(&m_filter, NULL);

    //johnfitz
    Cvar_RegisterVariable(&cl_keypad, NULL);

    // joystick variables
    Cvar_RegisterVariable(&in_joystick, NULL);
    Cvar_RegisterVariable(&joy_name, NULL);
    Cvar_RegisterVariable(&joy_advanced, NULL);
    Cvar_RegisterVariable(&joy_advaxisx, NULL);
    Cvar_RegisterVariable(&joy_advaxisy, NULL);
    Cvar_RegisterVariable(&joy_advaxisz, NULL);
    Cvar_RegisterVariable(&joy_advaxisr, NULL);
    Cvar_RegisterVariable(&joy_advaxisu, NULL);
    Cvar_RegisterVariable(&joy_advaxisv, NULL);
    Cvar_RegisterVariable(&joy_forwardthreshold, NULL);
    Cvar_RegisterVariable(&joy_sidethreshold, NULL);
    Cvar_RegisterVariable(&joy_pitchthreshold, NULL);
    Cvar_RegisterVariable(&joy_yawthreshold, NULL);
    Cvar_RegisterVariable(&joy_forwardsensitivity, NULL);
    Cvar_RegisterVariable(&joy_sidesensitivity, NULL);
    Cvar_RegisterVariable(&joy_pitchsensitivity, NULL);
    Cvar_RegisterVariable(&joy_yawsensitivity, NULL);
    Cvar_RegisterVariable(&joy_wwhack1, NULL);
    Cvar_RegisterVariable(&joy_wwhack2, NULL);

    Cmd_AddCommand("force_centerview", Force_CenterView_f);
    Cmd_AddCommand("joyadvancedupdate", Joy_AdvancedUpdate_f);

    uiWheelMessage = RegisterWindowMessage("MSWHEEL_ROLLMSG");

    IN_StartupMouse();
    IN_StartupJoystick();
}

void IN_Shutdown(void)
{
}

void IN_MouseEvent(int mstate)
{
}

void IN_MouseMove(usercmd_t* cmd)
{
}

void IN_Move(usercmd_t* cmd)
{
}

void IN_Accumulate(void)
{
}

void IN_ClearStates(void)
{
}

void IN_StartupJoystick(void)
{
}

void Joy_AdvancedUpdate_f(void)
{
}

void IN_Commands(void)
{
}

qboolean IN_ReadJoystick(void)
{
  return false;
}

void IN_JoyMove(usercmd_t* cmd)
{
}

// process SDL input related events
void IN_SDLEvent(const SDL_Event *event) {
  switch (event->type) {
  case SDL_KEYDOWN:
    Key_Event(MapKey(event->key.keysym.sym), true);
    break;
  case SDL_KEYUP:
    Key_Event(MapKey(event->key.keysym.sym), false);
    break;
  }
}
