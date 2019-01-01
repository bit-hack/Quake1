#include "api.h"
#include "quakedef.h"

static const cdaudio_api_t *api;

int CDAudio_Init(void) {
  extern const cdaudio_api_t *getCDAudioApi(const quake_api_t *);
  api = getCDAudioApi(GetQuakeAPI());
  if (!api) {
    Sys_Error("getCDAudioApi failed\n");
    abort();
  }
  assert(api && api->Init);
  return api->Init();
}

void CDAudio_Play(uint8_t track, bool looping) {
  assert(api && api->Play);
  api->Play(track, looping);
}

void CDAudio_Stop(void) {
  assert(api && api->Stop);
  api->Stop();
}

void CDAudio_Pause(void) {
  assert(api && api->Pause);
  api->Pause();
}

void CDAudio_Resume(void) {
  assert(api && api->Resume);
  api->Resume();
}

void CDAudio_Shutdown(void) {
  assert(api && api->Shutdown);
  api->Shutdown();
}

void CDAudio_Update(void) {
  assert(api && api->Update);
  api->Update();
}
