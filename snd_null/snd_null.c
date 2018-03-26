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
// snd_dma.c -- main control for any streaming sound output device

#include "../quakedef.h"

#ifdef _WIN32
#include "../winquake.h"
#endif

cvar_t bgmvolume = { "bgmvolume", "1", true };
cvar_t volume = { "volume", "0.7", true };
cvar_t loadas8bit = { "loadas8bit", "0" };

void S_AmbientOff(void)
{
}

void S_AmbientOn(void)
{
}

void S_SoundInfo_f(void)
{
}

void S_Startup(void)
{
}

void S_Init(void)
{
    Cvar_RegisterVariable(&bgmvolume, NULL);
    Cvar_RegisterVariable(&volume, NULL);
    Cvar_RegisterVariable(&loadas8bit, NULL);
}

void S_Shutdown(void)
{
}

sfx_t* S_FindName(char* name)
{
    return NULL;
}

void S_TouchSound(char* name)
{
}

sfx_t* S_PrecacheSound(char* name)
{
    return NULL;
}

channel_t* SND_PickChannel(int entnum, int entchannel)
{
    return NULL;
}

void SND_Spatialize(channel_t* ch)
{
}

void S_StartSound(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation)
{
}

void S_StopSound(int entnum, int entchannel)
{
}

void S_StopAllSounds(qboolean clear)
{
}

void S_StopAllSoundsC(void)
{
}

void S_ClearBuffer(void)
{
}

void S_StaticSound(sfx_t* sfx, vec3_t origin, float vol, float attenuation)
{
}

void S_UpdateAmbientSounds(void)
{
}

void S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
}

void GetSoundtime(void)
{
}

void S_ExtraUpdate(void)
{
}

void S_Play(void)
{
}

void S_PlayVol(void)
{
}

void S_SoundList(void)
{
}

void S_LocalSound(char* sound)
{
}

void S_BeginPrecaching(void)
{
}

void S_EndPrecaching(void)
{
}

void S_ClearPrecache(void)
{
}

void S_BlockSound(void)
{
}

void S_UnblockSound(void)
{
}

int S_SampleRate(void) {
  return 22050;
}
