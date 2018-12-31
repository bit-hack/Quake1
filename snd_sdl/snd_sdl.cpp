#include <map>
#include <string>

#include <SDL/SDL.h>

#include "../api.h"
#include "../quakedef.h"

const quake_api_t* api;

cvar_t volume = { "volume", "0.7", true };
cvar_t bgmvolume = { "bgmvolume", "1", true };
cvar_t loadas8bit = { "loadas8bit", "0" };

bool active;
SDL_AudioSpec spec;
SDL_mutex *mutex = NULL;

std::map<std::string, sfx_t> sfx_map;

struct foo_t {
  sfx_t *sfx;
};

std::map<std::pair<int, int>, foo_t> channel_map;

struct listener_t {
  vec3_t origin;
  vec3_t forward;
  vec3_t right;
  vec3_t up;
};

listener_t listener;


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

  mutex = SDL_CreateMutex();
  if (!mutex) {
    api->sys->Error("Unable to create audio mutex\n");
    return;
  }

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
    assert(api && api->cvar);
    api->cvar->RegisterVariable(&bgmvolume, NULL);
    api->cvar->RegisterVariable(&volume, NULL);
    api->cvar->RegisterVariable(&loadas8bit, NULL);

    active = false;
}

static void SndSDL_Shutdown(void)
{
    if (active)
    {
        SDL_CloseAudio();
        active = false;
    }
    if (mutex)
    {
        SDL_DestroyMutex(mutex);
        mutex = NULL;
    }
}

static sfx_t* SndSDL_FindName(char* name)
{
    api->con->Printf("%s: %s\n", __FUNCDNAME__, name);
    auto itt = sfx_map.find(std::string(name));
    if (itt == sfx_map.end()) {
      return NULL;
    }
    return &(itt->second);
}

static void SndSDL_TouchSound(char* name)
{
    api->con->Printf("%s %s\n", __FUNCDNAME__, name);
}

static sfx_t* SndSDL_PrecacheSound(char* name)
{
    api->con->Printf("%s: %s\n", __FUNCDNAME__, name);

    // check if its already cached
    {
      auto itt = sfx_map.find(std::string(name));
      if (itt != sfx_map.end()) {
        return &(itt->second);
      }
    }

    char namebuffer[256];
    api->str->Q_strcpy(namebuffer, "sound/");
    api->str->Q_strcat(namebuffer, name);

    // load the data from disk
    byte *data = api->com->LoadHunkFile(namebuffer);

    sfx_map.emplace(std::string(name), sfx_t{0});
    sfx_t &sfx = sfx_map[std::string(name)];
    strncpy(sfx.name, name, sizeof(sfx.name));



    sfx.cache.data = (void*)data;
    return &sfx;
}

static void SndSDL_StartSound(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation)
{
    api->con->Printf("%s\n", __FUNCDNAME__);

    if (!SDL_mutexP(mutex)) {

      foo_t chan;
      chan.sfx = sfx;
      channel_map[std::make_pair(entnum, entchannel)] = chan;

      SDL_mutexV(mutex);
    }
}

static void SndSDL_StopSound(int entnum, int entchannel)
{
    if (!SDL_mutexP(mutex)) {

      auto itt = channel_map.find(std::make_pair(entnum, entchannel));
      if (itt != channel_map.end()) {

        channel_map.erase(itt);
      }

      SDL_mutexV(mutex);
    }
}

static void SndSDL_StopAllSounds(bool clear)
{
    if (!SDL_mutexP(mutex))
    {
        channel_map.clear();
        SDL_mutexV(mutex);
    }
}

static void SndSDL_StopAllSoundsC(void)
{
    if (!SDL_mutexP(mutex))
    {
        channel_map.clear();
        SDL_mutexV(mutex);
    }
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
    api->math->_VectorCopy(origin, listener.origin);
    api->math->_VectorCopy(forward, listener.forward);
    api->math->_VectorCopy(right, listener.right);
    api->math->_VectorCopy(up, listener.up);
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

extern "C" {
__declspec(dllexport) extern const sound_api_t* getSoundApi(const quake_api_t* quake_api)
{
    api = quake_api;
    return &SndSDLAPI;
}
} // extern "C"
