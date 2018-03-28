#pragma once
#include "quakedef.h"

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// Module -> Quake
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

// sound device interface
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

// cd audio interface
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

// console variable api
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

// command api
typedef struct cmd_api_t
{
    void (*AddCommand)(char* cmd_name, xcommand_t function);
    bool (*Exists)(char* cmd_name);
    char* (*CompleteCommand)(char* partial);
    int (*Argc)(void);
    char* (*Argv)(int arg);
    char* (*Args)(void);
    int (*CheckParm)(char* parm);
    void (*TokenizeString)(char* text);
    void (*ExecuteString)(char* text, cmd_source_t src);
    void (*ForwardToServer)(void);
} cmd_api_t;

// console api
typedef struct con_api_t
{
    void (*CheckResize)(void);
    void (*Init)(void);
    void (*DrawConsole)(int lines, bool drawinput);
    void (*Print)(char* txt);
    void (*Printf)(char* fmt, ...);
    void (*Warning)(char* fmt, ...);
    void (*DPrintf)(char* fmt, ...);
    void (*DPrintf2)(char* fmt, ...);
    void (*SafePrintf)(char* fmt, ...);
    void (*Clear_f)(void);
    void (*DrawNotify)(void);
    void (*ClearNotify)(void);
    void (*ToggleConsole_f)(void);
    void (*NotifyBox)(char* text);
} con_api_t;

// system api
typedef struct sys_api_t
{
    int (*FileOpenRead)(char* path, int* hndl);
    int (*FileOpenWrite)(char* path);
    void (*FileClose)(int handle);
    void (*FileSeek)(int handle, int position);
    int (*FileRead)(int handle, void* dest, int count);
    int (*FileWrite)(int handle, void* data, int count);
    int (*FileTime)(char* path);
    void (*mkdir)(char* path);
    void (*Error)(char* error, ...);
    void (*Printf)(char* fmt, ...);
    void (*Quit)(void);
    double (*FloatTime)(void);
    char* (*ConsoleInput)(void);
    void (*Sleep)(void);

} sys_api_t;

// api agregator
typedef struct quake_api_t
{
    const cvar_api_t* cvar;
    const cmd_api_t* cmd;
    const con_api_t* con;
    const sys_api_t* sys;
} quake_api_t;

extern const quake_api_t* GetQuakeAPI();
