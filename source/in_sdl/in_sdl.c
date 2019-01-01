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

#include <SDL/SDL.h>

#include "../quakedef.h"
#include "../winquake.h"

// mouse variables
cvar_t m_filter = { "m_filter", "0" };

//johnfitz -- compatibility with old Quake -- setting to 0 disables KP_* codes
cvar_t cl_keypad = { "cl_keypad", "1" };

bool mouseactive;

static bool mouse_avail;
static int mouse_buttons;
static int mouse_oldbuttonstate;
static POINT current_pos;
static int mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, my_accum;

static bool restore_spi;
static int originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };

static unsigned int uiWheelMessage;
static bool mouseinitialized;
static bool mouseparmsvalid, mouseactivatetoggle;
static bool mouseshowtoggle = 1;

/* Map from SDL to quake keynums */
static int MapKey(int key)
{
    switch (key)
    {
    case SDLK_TAB:
        return K_TAB;
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
    SDL_ShowCursor(1);
}

void IN_HideMouse(void)
{
    SDL_ShowCursor(0);
}

void IN_ActivateMouse(void)
{
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(0);
}

void IN_SetQuakeMouseState(void)
{
}

void IN_DeactivateMouse(void)
{
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor(1);
}

void IN_RestoreOriginalMouseState(void)
{
}

void IN_StartupMouse(void)
{
    if (COM_CheckParm("-nomouse"))
        return;

    SDL_WM_GrabInput(SDL_GRAB_ON);

    mouse_x = mouse_y = 0.0;
    mouse_avail = 1;
    mouseinitialized = true;
    mouseactive = true;

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

void IN_ClearStates(void)
{
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

    Cmd_AddCommand("force_centerview", Force_CenterView_f);

    uiWheelMessage = RegisterWindowMessage("MSWHEEL_ROLLMSG");

    IN_StartupMouse();
}

void IN_Shutdown(void)
{
    mouse_avail = 0;
}

void IN_Commands(void)
{
    int i;
    int mouse_buttonstate;

    if (!mouse_avail)
        return;

    i = SDL_GetMouseState(NULL, NULL);
    /* Quake swaps the second and third buttons */
    mouse_buttonstate = (i & ~0x06) | ((i & 0x02) << 1) | ((i & 0x04) >> 1);
    for (i = 0; i < 3; i++)
    {
        if ((mouse_buttonstate & (1 << i)) && !(mouse_oldbuttonstate & (1 << i)))
            Key_Event(K_MOUSE1 + i, true);

        if (!(mouse_buttonstate & (1 << i)) && (mouse_oldbuttonstate & (1 << i)))
            Key_Event(K_MOUSE1 + i, false);
    }
    mouse_oldbuttonstate = mouse_buttonstate;
}

void IN_Move(usercmd_t* cmd)
{
    if (!mouse_avail)
        return;

    mouse_x *= sensitivity.value;
    mouse_y *= sensitivity.value;

    if ((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
        cmd->sidemove += m_side.value * mouse_x;
    else
        cl.viewangles[YAW] -= m_yaw.value * mouse_x;
    if (in_mlook.state & 1)
        V_StopPitchDrift();

    if ((in_mlook.state & 1) && !(in_strafe.state & 1))
    {
        cl.viewangles[PITCH] += m_pitch.value * mouse_y;
        // clamp viewing angles
        if (cl.viewangles[PITCH] > 80)
            cl.viewangles[PITCH] = 80;
        if (cl.viewangles[PITCH] < -70)
            cl.viewangles[PITCH] = -70;
    }
    else
    {
        if ((in_strafe.state & 1) && noclip_anglehack)
            cmd->upmove -= m_forward.value * mouse_y;
        else
            cmd->forwardmove -= m_forward.value * mouse_y;
    }
    mouse_x = mouse_y = 0.0f;
}

// process SDL input related events
void IN_SDLEvent(const SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        Key_Event(MapKey(event->key.keysym.sym), true);
        break;
    case SDL_KEYUP:
        Key_Event(MapKey(event->key.keysym.sym), false);
        break;
    case SDL_MOUSEMOTION:
        if ((event->motion.x != (vid.width / 2)) || (event->motion.y != (vid.height / 2)))
        {
            mouse_x = event->motion.xrel * 10;
            mouse_y = event->motion.yrel * 10;
        }
        break;
    case SDL_QUIT:
        CL_Disconnect();
        Host_ShutdownServer(false);
        Sys_Quit();
        break;
    }
}
