#include <assert.h>

#include <SDL.h>

#include "api.h"
#include "quakedef.h"
#include "sound.h"

static const sound_api_t* api;

void S_Init(void)
{
#if 1
    extern const sound_api_t *getSoundApi();
#else
    const char* dllpath = "sdl_null.dll";
    void* object = SDL_LoadObject(dllpath);
    if (!object)
    {
        Sys_Error("Unable to open api '%s'.\n", dllpath);
        abort();
    }
    typedef const sound_api_t* (*getSoundAPI_t)(void);
    getSoundAPI_t getSoundAPI = (getSoundAPI_t)SDL_LoadFunction(object, "getSoundApi");
    if (!getSoundAPI)
    {
        Sys_Error("%s returned NULL for getSoundAPI\n", dllpath);
        abort();
    }
#endif
    api = getSoundApi();
    if (!api)
    {
        Sys_Error("call to GetSoundApi() failed\n");
        abort();
    }
    assert(api->Init);
    api->Init();
}

void S_StartSound(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation)
{
    assert(api && api->StartSound);
    api->StartSound(entnum, entchannel, sfx, origin, fvol, attenuation);
}

void S_StopSound(int entnum, int entchannel)
{
    assert(api && api->StopSound);
    api->StopSound(entnum, entchannel);
}

void S_StopAllSounds(bool clear)
{
    assert(api && api->StopAllSounds);
    api->StopAllSounds(clear);
}

void S_StaticSound(sfx_t* sfx, vec3_t origin, float vol, float attenuation)
{
    assert(api && api->StaticSound);
    api->StaticSound(sfx, origin, vol, attenuation);
}

void S_BeginPrecaching(void)
{
    assert(api && api->BeginPrecaching);
    api->BeginPrecaching();
}

void S_TouchSound(char* sample)
{
    assert(api && api->TouchSound);
    api->TouchSound(sample);
}

sfx_t* S_PrecacheSound(char* sample)
{
    assert(api && api->PrecacheSound);
    return api->PrecacheSound(sample);
}

void S_EndPrecaching(void)
{
    assert(api && api->EndPrecaching);
    api->EndPrecaching();
}

void S_LocalSound(char* s)
{
    assert(api && api->LocalSound);
    api->LocalSound(s);
}

void S_ClearBuffer(void)
{
    assert(api && api->ClearBuffer);
    api->ClearBuffer();
}

void S_Shutdown(void)
{
    assert(api && api->Shutdown);
    api->Shutdown();
}

void S_Update(vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up)
{
    assert(api && api->Update);
    api->Update(origin, v_forward, v_right, v_up);
}

void S_ExtraUpdate(void)
{
    assert(api && api->ExtraUpdate);
    api->ExtraUpdate();
}

int S_SampleRate(void)
{
    assert(api && api->SampleRate);
    return api->SampleRate();
}
