#pragma once
#include "quakedef.h"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
// Module -> Quake
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

typedef struct sound_api_t
{
    void (*StartSound)(int entnum, int entchannel, sfx_t* sfx, vec3_t origin, float fvol, float attenuation);
    void (*StopSound)(int entnum, int entchannel);
    void (*StopAllSounds)(bool clear);
    void (*StaticSound)(sfx_t* sfx, vec3_t origin, float vol, float attenuation);
    void (*BeginPrecaching)(void);
    void (*TouchSound)(char* sample);
    sfx_t* (*PrecacheSound)(char* sample);
    void (*EndPrecaching)(void);
    void (*LocalSound)(char* s);
    void (*ClearBuffer)(void);
    void (*Init)(void);
    void (*Shutdown)(void);
    void (*Update)(vec3_t origin, vec3_t v_forward, vec3_t v_right, vec3_t v_up);
    void (*ExtraUpdate)(void);
    int (*SampleRate)(void);
} sound_api_t;

typedef struct cdaudio_api_t
{
    int (*Init)(void);
    void (*Play)(byte track, bool looping);
    void (*Stop)(void);
    void (*Pause)(void);
    void (*Resume)(void);
    void (*Shutdown)(void);
    void (*Update)(void);
} cdaudio_api_t;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
// Quake -> Module
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

typedef struct cvar_api_t
{
    void (*RegisterVariable)(cvar_t* variable, void* function);
    void (*Set)(char* var_name, char* value);
    void (*SetValue)(char* var_name, float value);
    float (*VariableValue)(char* var_name);
    char* (*VariableString)(char* var_name);
    char* (*CompleteVariable)(char* partial);
    bool (*Command)(void);
    void (*WriteVariables)(FILE* f);
    cvar_t* (*FindVar)(char* var_name);
} cvar_api_t;

typedef struct quake_api_t
{
    const cvar_api_t* cvar;
} quake_api_t;

extern const cvar_api_t* GetAPICVar();
extern const quake_api_t* GetQuakeAPI();
