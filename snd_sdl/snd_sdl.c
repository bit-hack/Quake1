#include <SDL/SDL.h>

#include "../api.h"
#include "../quakedef.h"

const quake_api_t* api;

cvar_t bgmvolume = { "bgmvolume", "1", true };
cvar_t volume = { "volume", "0.7", true };
cvar_t loadas8bit = { "loadas8bit", "0" };

SDL_AudioSpec spec;
bool active;

static const size_t MAX_SFX = 512;
sfx_t *known_sfx;

void SDLCALL SndSDL_Callback(void *userdata, Uint8 *stream, int len)
{
}

static void SndSDL_AmbientOff(void)
{
}

static void SndSDL_AmbientOn(void)
{
}

static void SndSDL_Startup(void)
{
  assert(!active);

  SDL_AudioSpec in;
  memset(&in, 0, sizeof(SDL_AudioSpec));
  in.callback = SndSDL_Callback;
  in.channels = 2;
  in.freq = 22050;
  in.samples = 1024 * 2;
  in.format = AUDIO_S16LSB;

  if (SDL_OpenAudio(&in, &spec) != 0) {
    api->sys->Error("Unable to open audio device\n");
    return;
  }

  active = true;
}

static void SndSDL_Init(void)
{
    known_sfx = Hunk_AllocName(MAX_SFX * sizeof(sfx_t), "sfx_t");

    assert(api && api->cvar);
    api->cvar->RegisterVariable(&bgmvolume, NULL);
    api->cvar->RegisterVariable(&volume, NULL);
    api->cvar->RegisterVariable(&loadas8bit, NULL);

    active = false;
}

static void SndSDL_Shutdown(void)
{
    if (active) {
        SDL_CloseAudio();
        active = false;
    }
}

static sfx_t* SndSDL_FindName(char* name)
{
    api->con->Printf("%s: %s\n", __FUNCDNAME__, name);
    return NULL;
}

static void SndSDL_TouchSound(char* name)
{
    api->con->Printf("%s %s\n", __FUNCDNAME__, name);
}

static sfx_t* SndSDL_PrecacheSound(char* name)
{
    api->con->Printf("%s: %s\n", __FUNCDNAME__, name);

    char namebuffer[256];
    byte stackbuf[1024];

    Q_strcpy(namebuffer, "sound/");
    Q_strcat(namebuffer, name);

    const byte *data = api->com->LoadStackFile(namebuffer, stackbuf, sizeof(stackbuf));

    return NULL;
}

static void SndSDL_StartSound(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation)
{
    api->con->Printf("%s\n", __FUNCDNAME__);
}

static void SndSDL_StopSound(int entnum, int entchannel)
{
}

static void SndSDL_StopAllSounds(bool clear)
{
}

static void SndSDL_StopAllSoundsC(void)
{
}

static void SndSDL_ClearBuffer(void)
{
}

static void SndSDL_StaticSound(sfx_t* sfx, vec3_t origin, float vol, float attenuation)
{
    api->con->Printf("%s\n", __FUNCDNAME__);
}

static void SndSDL_UpdateAmbientSounds(void)
{
}

static void SndSDL_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
//    api->con->Printf("%s\n", __FUNCDNAME__);
}

static void GetSoundtime(void)
{
}

static void SndSDL_ExtraUpdate(void)
{
}

static void SndSDL_Play(void)
{
}

static void SndSDL_PlayVol(void)
{
}

static void SndSDL_SoundList(void)
{
}

static void SndSDL_LocalSound(char* sound)
{
    api->con->Printf("%s: %s\n", __FUNCDNAME__, sound);
}

static void SndSDL_BeginPrecaching(void)
{
    api->con->Printf("%s\n", __FUNCDNAME__);
}

static void SndSDL_EndPrecaching(void)
{
    api->con->Printf("%s\n", __FUNCDNAME__);
}

static void SndSDL_ClearPrecache(void)
{
}

static void SndSDL_BlockSound(void)
{
}

static void SndSDl_UnblockSound(void)
{
}

static int SndSDL_SampleRate(void)
{
    return spec.freq;
}

static const sound_api_t SndSDLAPI = {
    SndSDL_StartSound,
    SndSDL_StopSound,
    SndSDL_StopAllSounds,
    SndSDL_StaticSound,
    SndSDL_BeginPrecaching,
    SndSDL_TouchSound,
    SndSDL_PrecacheSound,
    SndSDL_EndPrecaching,
    SndSDL_LocalSound,
    SndSDL_ClearBuffer,
    SndSDL_Init,
    SndSDL_Shutdown,
    SndSDL_Update,
    SndSDL_ExtraUpdate,
    SndSDL_SampleRate
};

__declspec(dllexport) extern const sound_api_t* getSoundApi(const quake_api_t *quake_api)
{
    api = quake_api;
    return &SndSDLAPI;
}
