/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

This program is free software {} you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation {} either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY {} without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program {} if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../api.h"
#include "../quakedef.h"
#include "../cdaudio.h"

const quake_api_t *api;

int  CDSDL_Init(void) { return 1; }
void CDSDL_Play(uint8_t track, bool looping) {}
void CDSDL_Stop(void) {}
void CDSDL_Pause(void) {}
void CDSDL_Resume(void) {}
void CDSDL_Shutdown(void) {}
void CDSDL_Update(void) {}

static const cdaudio_api_t CDSDLAPI = {
    CDSDL_Init,
    CDSDL_Play,
    CDSDL_Stop,
    CDSDL_Pause,
    CDSDL_Resume,
    CDSDL_Shutdown,
    CDSDL_Update
};

__declspec(dllexport)
const cdaudio_api_t* getCDAudioApi(const quake_api_t *quake_api)
{
    api = quake_api;
    return &CDSDLAPI;
}
