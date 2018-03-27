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

#include "../api.h"
#include "../quakedef.h"

const quake_api_t* api;

cvar_t bgmvolume = { "bgmvolume", "1", true };
cvar_t volume = { "volume", "0.7", true };
cvar_t loadas8bit = { "loadas8bit", "0" };

void SndNull_AmbientOff(void)
{
}

void SndNull_AmbientOn(void)
{
}

void SndNull_SoundInfo_f(void)
{
}

void SndNull_Startup(void)
{
}

void SndNull_Init(void)
{
    assert(api && api->cvar);
    api->cvar->RegisterVariable(&bgmvolume, NULL);
    api->cvar->RegisterVariable(&volume, NULL);
    api->cvar->RegisterVariable(&loadas8bit, NULL);
}

void SndNull_Shutdown(void)
{
}

sfx_t* SndNull_FindName(char* name)
{
    return NULL;
}

void SndNull_TouchSound(char* name)
{
}

sfx_t* SndNull_PrecacheSound(char* name)
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

void SndNull_StartSound(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation)
{
}

void SndNull_StopSound(int entnum, int entchannel)
{
}

void SndNull_StopAllSounds(bool clear)
{
}

void SndNull_StopAllSoundsC(void)
{
}

void SndNull_ClearBuffer(void)
{
}

void SndNull_StaticSound(sfx_t* sfx, vec3_t origin, float vol, float attenuation)
{
}

void SndNull_UpdateAmbientSounds(void)
{
}

void SndNull_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
}

void GetSoundtime(void)
{
}

void SndNull_ExtraUpdate(void)
{
}

void SndNull_Play(void)
{
}

void SndNull_PlayVol(void)
{
}

void SndNull_SoundList(void)
{
}

void SndNull_LocalSound(char* sound)
{
}

void SndNull_BeginPrecaching(void)
{
}

void SndNull_EndPrecaching(void)
{
}

void SndNull_ClearPrecache(void)
{
}

void SndNull_BlockSound(void)
{
}

void SndNull_UnblockSound(void)
{
}

int SndNull_SampleRate(void)
{
    return 22050;
}

static const sound_api_t SndNullAPI = {
    SndNull_StartSound,
    SndNull_StopSound,
    SndNull_StopAllSounds,
    SndNull_StaticSound,
    SndNull_BeginPrecaching,
    SndNull_TouchSound,
    SndNull_PrecacheSound,
    SndNull_EndPrecaching,
    SndNull_LocalSound,
    SndNull_ClearBuffer,
    SndNull_Init,
    SndNull_Shutdown,
    SndNull_Update,
    SndNull_ExtraUpdate,
    SndNull_SampleRate
};

__declspec(dllexport) extern const sound_api_t* getSoundApi(const quake_api_t *quake_api)
{
    api = quake_api;
    return &SndNullAPI;
}
